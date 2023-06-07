#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>

const uint32_t MAGIC_DATA = 0x21796F4A;
const uint32_t MAGIC_COMP_1 = 0xAFBC7A37;
const uint16_t MAGIC_COMP_2 = 0x1C27;

struct HeaderData {
	uint32_t magic, count;
};

struct HeaderData hData;

struct HeaderComparator {
	uint32_t magic1;
	uint16_t magic2;
	uint16_t reserved;
	uint64_t count;
};

struct HeaderComparator hComp;

struct CompData {
	uint16_t type;
	uint16_t reserved1, reserved2, reserved3;
	uint32_t offset1, offset2;
};

struct CompData compData;

void setSeek(int fd, uint32_t num);
void setSeek(int fd, uint32_t num) {
	if(lseek(fd, num, SEEK_SET) == -1)
		err(5, "ERROR: running lseek()");
}

int openFile1(char* name);
int openFile1(char* name) {
	struct stat st;
	if(stat(name, &st) == -1)
		err(3, "ERROR: running stat() on file 1: %s", name);

	int fd;
	if((fd = open(name, O_RDWR)) == -1)
		err(2, "ERROR: error opening file 1 for read and write: %s", name);

	if(read(fd, &hData, sizeof(hData)) == -1)
		err(4, "ERRIR: reading header from file 1");

	if(st.st_size / 8 != hData.count + 1)
		errx(6, "ERROR: invalid number of elements in file 1: %s", name);

	if(hData.magic != MAGIC_DATA)
		errx(7, "ERROR: magic value in file 1 is invalid: %s", name);

	//setSeek(fd, 0);

	return fd;
}

int openFile2(char* name);
int openFile2(char* name) {
	struct stat st;
	if(stat(name, &st) == -1)
		err(3, "ERROR: running stat() on file 2: %s", name);

	int fd;
	if((fd = open(name, O_RDONLY)) == -1)
		err(2, "ERROR: opening file2 for read: %s", name);

	if(read(fd, &hComp, sizeof(hComp)) == -1)
		err(4, "ERROR: reading header from file 2");
	
	if((uint64_t) st.st_size / 16 != hComp.count + 1)
		errx(6, "ERROR: invalid number of elements in file 2: %s", name);

	if(hComp.magic1 != MAGIC_COMP_1)
		errx(7, "ERROR: invalid magic1 in file2: %s", name);

	if(hComp.magic2 != MAGIC_COMP_2)
		errx(7, "ERROR: invalid magic2 in file2: %s", name);

	//setSeek(fd, 0);

	return fd;
}

void readCompData(int fd);
void readCompData(int fd) {
	if(read(fd, &compData, sizeof(compData)) == -1)
		err(9, "ERROR: reading data from file 2");
}

uint64_t readDataFrom(int fd, uint32_t offset);
uint64_t readDataFrom(int fd, uint32_t offset) {
	setSeek(fd, offset * 8);

	uint64_t buf;
	if(read(fd, &buf, sizeof(buf)) == -1)
		err(10, "ERROR: reading data from file 1");

	return buf;
}

bool compare(uint64_t d1, uint64_t d2, uint16_t c);
bool compare(uint64_t d1, uint64_t d2, uint16_t c) {
	if(c == 1) {
		return d1 < d2;
	} else if (c == 0) {
		return d1 > d2;
	} else {
		errx(11, "ERROR: invalid comparing argument found in Comparator Data");
	}
}

void swap(int fd, uint64_t d1, uint32_t o1, uint64_t d2, uint32_t o2);
void swap(int fd, uint64_t d1, uint32_t o1, uint64_t d2, uint32_t o2) {
	setSeek(fd, o1);
	if(write(fd, &d2, sizeof(d2)) < 0)
		err(12, "ERROR: swapping data in file1 on offset1");

	setSeek(fd, o2);
	if(write(fd, &d1, sizeof(d1)) < 0)
		err(12, "ERROR: swapping data in file1 on offset2");
}
int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count");

	char* dataFile = argv[1];
	char* comparator = argv[2];

	int fd1 = openFile1(dataFile);
	int fd2 = openFile2(comparator);

	for(uint64_t i = 0; i < hComp.count; ++i) {
		readCompData(fd2);	
		
		uint64_t data1 = readDataFrom(fd1, compData.offset1);
		uint64_t data2 = readDataFrom(fd1, compData.offset2);

		bool toSwap = compare(data1, data2, compData.type);	

		if(toSwap) {
			swap(fd1, data1, compData.offset1, data2, compData.offset2);	
		}
	}

	close(fd1);
	close(fd2);
	return 0;
}
