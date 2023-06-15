#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#define uint128_t __uint128_t

struct Header {
	uint64_t magic;
	uint32_t cfsb, cfsu, ofsb, ofsu, unused1, checksum;
	uint128_t sectionkey;
	uint32_t s[4];
};

struct Header h;
 
struct Section {
	uint64_t offset, len;
	uint128_t datakey;
};

struct Section s;

int openFile(const char* file);
int openFile(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file1 for read: %s", file);

	int bytes = 0;
	if((bytes = read(fd, &h, sizeof(h))) == -1)
		err(3, "ERROR: reading header from file1: %s", file);

	if(h.magic != 0x0000534f44614c47)
		errx(5, "ERROR: invalid magic number in header of file: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(4, "ERROR: running stat on file1");

	if(st.st_size != h.cfsb)
		errx(5, "ERROR: invalid crypted file size in bytes in header: %s", file);
	//if(st.st_size / 16 != h.cfsu)
	//	errx(6, "ERROR: invalid cryped file size in units in header: %s", file);

	return fd;
}

void decryptSection(void);
void decryptSection(void) {
	uint128_t* t1 = (uint128_t*)&s;
	uint128_t* t2 = t1 + 1;

	*t1 = *t1 ^ h.sectionkey;
	*t2 = *t2 ^ h.sectionkey;
}

void readSection(int fd);
void readSection(int fd) {
	if(read(fd, &s, sizeof(s)) != sizeof(s))
		err(9, "ERROR: reading section from crypted file");
}

void seekSet(int fd, uint64_t pos);
void seekSet(int fd, uint64_t pos) {
	if(lseek(fd, pos*16, SEEK_SET) == -1)
		err(11, "ERROR: running lseek()");
}

uint32_t findChecksum(int fd);
uint32_t findChecksum(int fd) {
	uint32_t checksum = 0;

	uint32_t buf;
	int bytes = 0;
	while((bytes = read(fd, &buf, sizeof(buf))) > 0) {
		checksum ^= buf;
	}
	if(bytes < 0)
		err(13, "ERROR: finding checksum of file2");

	return checksum;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	char* file1 = argv[1];
	char* file2 = argv[2];

	int fd1 = openFile(file1);
	int fd2;
	if((fd2 = open(file2, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
		err(7, "ERROR: creating file2 for read and write: %s", file2);

	for(int i = 0; i < 4; ++i) {
		if(h.s[i] == 0)
			continue;

		seekSet(fd1, h.s[i]);

		readSection(fd1);
		decryptSection();
		
		seekSet(fd1, (uint64_t)h.s[i] + s.offset);

		uint128_t cryptdata;
		for(uint64_t j = 0; j < s.len; ++j) {
			if(read(fd1, &cryptdata, sizeof(cryptdata)) != sizeof(cryptdata))
				err(12, "ERROR: reading cryptdata");

			cryptdata ^= s.datakey;

			if(write(fd2, &cryptdata, sizeof(cryptdata)) != sizeof(cryptdata))
				err(13, "ERROR; writing cryptdata to file 2");
		}	
	}	

	seekSet(fd2, 0);
	uint32_t checksum = findChecksum(fd2);
	seekSet(fd2, 0);

	if(checksum != h.checksum)
		errx(20, "ERROR: invalid checksum of decrypted file with padding");

	struct stat st;
	if(stat(file2, &st) == -1)
		err(14, "ERROR: running stat on file2 to check units count");

	//if(st.st_size != h.ofsb)
	//	errx(15, "ERROR: decrypted file invalid bytes count");

	//if(st.st_size / 16 != h.ofsu)
	//	errx(15, "ERROR: decrypted file invalid units count");

	uint32_t arr[h.ofsb / 4];
	uint32_t bytes = 0;
	if((bytes = read(fd2, arr, h.ofsb)) != h.ofsb)
		err(17, "ERROR: removing padding from crypted file");

	close(fd2);
	if((fd2 = open(file2, O_WRONLY | O_TRUNC)) == -1)
		err(16, "ERROR: truncating file2");

	if(write(fd2, arr, h.ofsb) != h.ofsb)
		err(18, "ERROR: writing file without padding");

	close(fd1);
	close(fd2);
	return 0;
}
