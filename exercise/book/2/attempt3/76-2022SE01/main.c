#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

struct DataHeader {
	uint32_t magic, count;
};

struct DataHeader dh;

struct CompHeader {
	uint32_t magic1;	
	uint16_t magic2, r1;
	uint64_t count;
};

struct CompHeader ch;

struct CompData {
	uint16_t type, r1, r2, r3;
	uint32_t offset1, offset2;
};

struct CompData d;

void myLseek(int fd, uint32_t pos);
void myLseek(int fd, uint32_t pos) {
	if(lseek(fd, pos*8, SEEK_SET) == -1)
		err(2, "ERROR: running lseek. Invalid position");
}

int myRead(int fd, uint64_t* buf, int size);
int myWrite(int fd, uint64_t* buf, int size);

int myRead(int fd, uint64_t* buf, int size) {
	int bytes = 0;
	if((bytes = read(fd, &buf, size)) == -1)
		err(3, "ERROR: reading from file");

	return bytes;
}

int myWrite(int fd, uint64_t* buf, int size) {
	int bytes = 0;
	if((bytes = write(fd, &buf, size)) != size)
		err(4, "ERROR: writing to file");

	return bytes;
}

int readData(int fd);
int readData(int fd) {
	int bytes = 0;
	if((bytes = read(fd, &d, sizeof(d))) == -1)
		err(15, "ERROR: reading data from file");

	return bytes;
}

int openFile(char* file);
int openFile(char* file) {
	int fd;
	if((fd = open(file, O_RDWR)) == -1)
		err(5, "ERROR: opening file for read: %s", file);
	
	if(read(fd, &dh, sizeof(dh)) != sizeof(dh))
		err(6, "ERROR: reading header from file: %s", file);

	if(dh.magic != 0x21796F4A)
		errx(8, "ERROR: invalid magic value found in fiule header: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(7, "ERROR: running stat on file: %s", file);

	if((st.st_size - sizeof(dh)) / 8 != dh.count)
		errx(8, "ERROR: invalid elements count in header: %s", file);

	return fd;
}

int openComp(char* file);
int openComp(char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(9, "ERROR: opening file for read: %s", file);
	
	if(read(fd, &ch, sizeof(ch)) != sizeof(ch))
		err(10, "ERROR: reading header from file: %s", file);

	if(ch.magic1 != 0xAFBC7A37)
		errx(11, "ERROR: invalid magic1 value fount in file header: %s", file);

	if(ch.magic2 != 0x1C27)
		errx(12, "ERROR: invalid magic2 value fount in file header: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(7, "ERROR: running stat on file: %s", file);

	if((st.st_size - sizeof(ch)) / 8 != ch.count)
		errx(8, "ERROR: invalid elements count in header: %s", file);

	return fd;
}

int compare(uint64_t n1, uint64_t n2, bool type);
int compare(uint64_t n1, uint64_t n2, bool type) {
	if(type) {
		return n1 < n2;
	} else {
		return n1 > n2;
	}
}

void mySwap(int fd, uint64_t n1, uint64_t n2, uint32_t offset1, uint32_t offset2);
void mySwap(int fd, uint64_t n1, uint64_t n2, uint32_t offset1, uint32_t offset2) {
	myLseek(fd, offset1);
	myWrite(fd, &n2, sizeof(n2));
	
	myLseek(fd, offset2);
	myWrite(fd, &n1, sizeof(n1));
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int fd1 = openFile(argv[1]);
	int fd2 = openComp(argv[2]);

	uint64_t n1, n2;
	while(readData(fd2) > 0) {
		myLseek(fd1, d.offset1);
		myRead(fd1, &n1, sizeof(n1));
		
		myLseek(fd1, d.offset2);
		myRead(fd1, &n2, sizeof(n2));

		bool swap = compare(n1, n2, d.type);

		if(swap)
			mySwap(fd1, n1, n2, d.offset1, d.offset2);
	}

	close(fd1);
	close(fd2);
	return 0;
}
