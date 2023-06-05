#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#define uint128_t __uint128_t

struct Header {
	uint64_t magic;
	uint32_t cryptedFileSizeBytes;
	uint32_t cryptedFileUnitsCount;
	uint32_t originalFileSizeBytes;
	uint32_t originalFileUnitsCount;
	uint32_t _;
	uint32_t checksum;
	uint128_t sectionKey;
	uint32_t s[4];
};

struct Section {
	uint64_t relativeOffset;
	uint64_t len;
	uint128_t datakey;
};

void parseHeader(int fd, struct Header* header);                                
void parseSection(int fd, struct Section* sec);                                 
void decryptSection(struct Section* sec, uint128_t key);
uint32_t findCheksumOfFile(int fd);
void resetLseek(int fd);

void parseHeader(int fd, struct Header* header) {
	int bytes;
	if((bytes = read(fd, header, sizeof(*header))) == -1)
		err(2, "ERROR: reading header from crypted file");
}

void parseSection(int fd, struct Section* sec) {
	int bytes;
	if((bytes = read(fd, sec, sizeof(*sec))) == -1)
		err(3, "ERROR: reading section from file");
}

void decryptSection(struct Section* sec, uint128_t key) {
	uint128_t* first = (uint128_t*)sec;
	uint128_t* second = first + 1;

	*first = *first ^ key;
	*second = *second ^ key;
}

uint32_t findCheksumOfFile(int fd) {
	uint32_t sum;
	uint32_t buf;
	int bytes;
	if((bytes = read(fd, &sum, sizeof(sum))) == -1)
		err(12, "ERROR: finding checksum of file: %d", fd);

	while((bytes = read(fd, &buf, sizeof(buf))) > 0) {
		sum = sum ^ buf;
	}
	if(bytes < 0)
		err(13, "ERROR: reading from file: %d", fd);

	resetLseek(fd);

	return sum;
}

void resetLseek(int fd) {
	if(lseek(fd, 0, SEEK_SET) == -1)
		err(11, "ERROR: lseek to 0 in file 2: %d", fd);
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count");

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);
	int fd2;
	if((fd2 = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
		err(3, "ERROR: opening file for write: %s", argv[2]);

	struct stat s;
	if(fstat(fd1, &s) == -1)
		err(4, "ERROR: running stat");

	if(s.st_size % 16 != 0)
		errx(5, "ERROR: file size not devisible by 16: %s", argv[1]);

	struct Header header;
	parseHeader(fd1, &header);

	for(int i = 0; i < 4; ++i) {
		if(header.s[i] == 0)
			continue;
		if(lseek(fd1, header.s[i] * 16, SEEK_SET) == -1)
			err(6, "ERROR: running lseek in file: %s", argv[1]);

		struct Section sec;
		parseSection(fd1, &sec);
		
		decryptSection(&sec, header.sectionKey);

		if(lseek(fd1, (sec.relativeOffset + header.s[i]) * 16, SEEK_SET) == -1)
			err(6, "ERROR: running lseek to find the first section");

		for(uint64_t j = 0; j < sec.len; ++j) {
			uint128_t cryptData;

			int bytes;
			if((bytes = read(fd1, &cryptData, sizeof(cryptData))) == -1)
				err(7, "ERROR: reading the %ld'th cryptdata in section: %d", j, header.s[i]);

			cryptData = cryptData ^ sec.datakey;

			int writtenBytes;
			if((writtenBytes = write(fd2, &cryptData, sizeof(cryptData))) == -1)
				err(9, "ERROR: writing section with index %ld, in section %d", j, header.s[i]);

			if(bytes != writtenBytes)
				err(10, "ERROR: not all bytes in cryptdata with index %ld int section %d were written", j, header.s[i]);
		}
	}

	resetLseek(fd2);

	uint32_t checksum = findCheksumOfFile(fd2);

	if(checksum != header.checksum)
		err(14, "ERROR: checksum of decrypted file not the same as checksum in header");

	resetLseek(fd2);

	uint32_t arr[header.originalFileSizeBytes / 4];

	if(read(fd2, arr, header.originalFileSizeBytes) < 0)
		err(15, "ERROR: reading file without padding");
	
	close(fd2);
	if((fd2 = open(argv[2], O_WRONLY | O_TRUNC)) == -1)
		err(16, "ERROR: Truncating 2nd file: %s", argv[2]);

	int writtenBytes;
	if((writtenBytes = write(fd2, arr, header.originalFileSizeBytes)) < 0)
		err(17, "ERROR: removing padding from second file: %s", argv[2]);

	close(fd1);
	close(fd2);
	return 0;
}
