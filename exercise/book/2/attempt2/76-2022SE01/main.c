#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>

struct DataHeader {
	uint32_t magic, count;
};

struct DataHeader dh;

struct CompHeader {
	uint32_t magic1;
	uint16_t magic2;
	uint16_t reserved;
	uint64_t count;
};

struct CompHeader ch;

struct Data {
	uint16_t type, r1, r2, r3;
	uint32_t offset1, offset2;
};

struct Data d;

void myLseek(int fd, uint32_t pos);
void myLseek(int fd, uint32_t pos) {
	if(lseek(fd, 8 + (pos*8), SEEK_SET) == -1)
		err(2, "ERROR: running lseek in file. Invalid position");
}

int myRead(int fd, uint64_t* buf);
int myRead(int fd, uint64_t* buf) {
	int bytes = 0;
	if((bytes = read(fd, buf, 8)) == -1)
		err(3, "ERROR: reading from file");

	return bytes;
}

int myWrite(int fd, uint64_t* buf);
int myWrite(int fd, uint64_t* buf) {
	int bytes = 0;
	if((bytes = write(fd, buf, 8)) == -1)
		err(4, "ERROR: writing to file");

	return bytes;
}

int openData(char* file);
int openData(char* file) {
	int fd;
	if((fd = open(file, O_RDWR)) == -1)
		err(5, "ERROR: opening file for read/write: %s", file);
	
	if(read(fd, &dh, sizeof(dh)) != sizeof(dh))
		err(6, "ERROR: reading header from data file: %s", file);

	if(dh.magic != 0x21796F4A)
		errx(8, "ERROR: invalid magic value in file");

	struct stat st;
	if(stat(file, &st) == -1)
		err(7, "ERROR: running stat for data file: %s", file);
	
	if((st.st_size - sizeof(dh)) % 8 != 0)
		errx(9, "ERROR: file size not devisible by 8: %s", file);
	
	if((st.st_size - sizeof(dh)) / 8 != dh.count)
		errx(10, "ERROR: file size is not the same as in the header");

	return fd;
}

int openComp(char* file);
int openComp(char* file) {
	int fd;
	if((fd = open(file, O_RDWR)) == -1)
		err(5, "ERROR: opening file for read/write: %s", file);
	
	if(read(fd, &ch, sizeof(ch)) != sizeof(ch))
		err(6, "ERROR: reading header from comp file: %s", file);

	if(ch.magic1 != 0xAFBC7A37)
		errx(8, "ERROR: invalid magic1 value in file");
	if(ch.magic2 != 0x1C27)
		errx(8, "ERROR: invalid magic2 value in file");

	struct stat st;
	if(stat(file, &st) == -1)
		err(7, "ERROR: running stat for comp file: %s", file);
	
	if((st.st_size - sizeof(ch)) % 16 != 0)
		errx(9, "ERROR: Comp file size not devisible by 16: %s", file);
	
	if((st.st_size - sizeof(ch)) / 16 != ch.count)
		errx(10, "ERROR: Comp file size is not the same as in the header");

	return fd;
}

bool compare(uint64_t* n1, uint64_t* n2, bool type);
bool compare(uint64_t* n1, uint64_t* n2, bool type) {
	if(type) {
		return *n1 < *n2;
	} else {
		return *n1 > *n2;
	}
}

void mySwap(int fd, uint64_t* n1, uint64_t* n2, uint32_t* pos1, uint32_t* pos2);
void mySwap(int fd, uint64_t* n1, uint64_t* n2, uint32_t* pos1, uint32_t* pos2) {
	myLseek(fd, *pos1);
	myWrite(fd, n2);

	myLseek(fd, *pos2);
	myWrite(fd, n1);
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int fd1 = openData(argv[1]);
	int fd2 = openData(argv[2]);
	
	int bytes = 0;
	bool swap;
	uint64_t n1;
	uint64_t n2;
	while((bytes = read(fd2, &d, sizeof(d))) > 0) {
		bool type = (bool)d.type;

		myLseek(fd1, d.offset1);
		myRead(fd1, &n1);

		myLseek(fd1, d.offset2);
		myRead(fd2, &n2);

		swap = compare(&n1, &n2, type);

		if(swap)
			mySwap(fd1, &n1, &n2, &d.offset1, &d.offset2);
	}
	if(bytes == -1)
		err(12, "ERROR: reading data from comp file");

	close(fd1);
	close(fd2);
	return 0;
}
