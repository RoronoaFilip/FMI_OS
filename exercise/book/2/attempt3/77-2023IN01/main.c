#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#define uint128_t __uint128_t

struct Header {
	uint64_t magic;
	uint32_t cfsb, cfsu, ofsb, ofsu, u1, checksum;
	uint128_t sectionkey;
	uint32_t s[4];
};

struct Header h;

struct Section {
	uint64_t offset, len;
	uint128_t datakey;
};

struct Section s;

void myLseek(int fd, uint64_t pos);
void myLseek(int fd, uint64_t pos) {
	if(lseek(fd, pos*16, SEEK_SET) == -1)
		err(3, "ERROR: running lseek in crypted File");
}

int myRead(int fd, uint128_t* buf, int size);
int myWrite(int fd, uint128_t* buf, int size);
int myRead(int fd, uint128_t* buf, int size) {
	int bytes = 0;
	if((bytes = read(fd, buf, size)) == -1)
		err(4, "ERROR: reading from crypted file");
	
	return bytes;
}

int myWrite(int fd, uint128_t* buf, int size) {
	int bytes = 0;	
	if((bytes = write(fd, buf, size)) == -1)
		err(5, "ERROR: writing to file");

	return bytes;
} 

int openF1(const char* file);
int openF1(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening crypted file for read");

	if(read(fd, &h, sizeof(h)) != sizeof(h))
		err(6, "ERROR: could not read header from crypted file");

	if(h.magic != 0x0000534f44614c47)
		errx(6, "ERROR: invalid magic value in header");

	struct stat st;
	if(stat(file, &st) == -1)
		err(7, "ERROR: running stat on crypted file");

	if(st.st_size != h.cfsb)
		errx(8, "ERROR: file size in header");

	if(st.st_size % 16 != 0)
		errx(9, "ERROR: file structure not in units of 16 bytes");

	if(st.st_size / 16 != h.cfsu)
		errx(10, "ERROR: file size in units in header");

	return fd;
}

void readSection(int fd);
void readSection(int fd) {
	if(read(fd, &s, sizeof(s)) != sizeof(s))
		err(14, "ERROR: reading section from file");

	uint128_t* p1 = (uint128_t*)&s;
	uint128_t* p2 = p1 + 1;

	*p1 = *p1 ^ h.sectionkey;
	*p2 = *p2 ^ h.sectionkey;
}

uint32_t findChecksum(int fd);
uint32_t findChecksum(int fd) {
	uint32_t result = 0x00000000;
	uint32_t buf;
	while(myRead(fd, (uint128_t*)&buf, 4) > 0) {
		result ^= buf;
	}

	return result;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	char* file1 = argv[1];
	char* file2 = argv[2];

	int fd1 = openF1(file1);
	

	int fd2;
	if((fd2 = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
		err(12, "ERROR: creating tempfile for read/write");


	for(int i = 0; i < 4; ++i) {
		if(h.s[i] == 0)
			continue;

		myLseek(fd1, (uint64_t)h.s[i]);
		readSection(fd1);
		
		myLseek(fd1, (uint64_t)h.s[i] + (uint64_t)s.offset);

		uint128_t data;
		for(uint32_t j = 0; j < s.len; ++j) {
			myRead(fd1, &data, 16);
			data = data ^ s.datakey;
			myWrite(fd2, &data, 16);
		}
	}
	
	struct stat st;
	if(stat("temp", &st) == -1)
		err(15, "ERROR: running stat for new file");

	//myLseek(fd2, 0);
	if(st.st_size % 16 != 0)
		errx(16, "ERROR: decrypted file size not devisible by 16");
	//if(st.st_size / 16 != h.ofsu)
	//	errx(17, "ERROR: decrypted file size in unists not the same as in header");

	myLseek(fd2, 0);
	if(findChecksum(fd2) != h.checksum)
		errx(13, "ERROR: invalid checksum for decrypted file");
	myLseek(fd2, 0);

	int fd3;
	if((fd3 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(18, "ERROR: creating file2 for write");
	
	uint8_t buf;
	for(uint32_t i = 0; i < h.ofsb; ++i) {
		myRead(fd2, (uint128_t*)&buf, 1);
		myWrite(fd3, (uint128_t*)&buf, 1);
	}


	close(fd1);
	close(fd2);
	close(fd3);
	unlink("temp");
	return 0;
}
