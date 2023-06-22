#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h> 

uint32_t fileSize = 0;

struct Header{
	uint16_t magic, type;
	uint32_t count;
};

struct Header h;

struct Pair {
	uint16_t offset1, offset2;
};

struct Pair p;

void myLseek(int fd, uint16_t pos, int elSize);
void myLseek(int fd, uint16_t pos, int elSize) {
	if(lseek(fd, 8 + (pos*elSize), SEEK_SET) == -1)
		err(2, "ERROR: running lseek for file with elSize: %d", elSize);
}

int myRead(int fd, uint32_t* buf, int size);
int myRead(int fd, uint32_t* buf, int size) {
	int bytes = 0;
	if((bytes = read(fd, buf, size)) == -1)
		err(3, "ERROR: reading from file");

	return bytes;
}

int myWrite(int fd, uint64_t* buf, int size);
int myWrite(int fd, uint64_t* buf, int size) {
	int bytes = 0;
	if((bytes = write(fd, buf, size)) != size)
		err(4, "ERROR: writing to file");

	return fd;
}

int openFile(char* file, uint16_t type);
int openFile(char* file, uint16_t type) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(5, "ERROR: opening file for read: %s", file);

	if(h.magic != 0x5A4D)
		errx(6, "ERROR: invalid magic value in header: %s", file);
	if(h.type != type)
		errx(7, "ERROR: invalid file type in header: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(8, "ERROR: runnign stat on file: %s", file);

	if((st.st_size-sizeof(h)) / 4 != h.count)
		errx(9, "ERROR: invalid elemetns count found in header");

	if(type == 2)
		fileSize = h.count;

	return fd;
}

int createFile(char* file, uint16_t size);
int createFile(char* file, uint16_t size) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(10, "ERROR: creating file for write: %s", file);

	h.count = size;
	h.type = 3;
	if(write(fd, &h, sizeof(h)) != sizeof(h))
		err(11, "ERROR: writing header to file: %s", file);

	uint64_t buf = 0x0000000000000000;
	for(uint16_t i = 0; i < size; ++i)
		myWrite(fd, &buf, 8);

	myLseek(fd, 0, 8);

	return fd;
}

uint16_t maxEl(int fd);
uint16_t maxEl(int fd) {
	uint16_t maxSize = (uint16_t)fileSize;

	uint16_t max = 0;
	while(myRead(fd, (uint32_t*)&p, 4) > 0) {
		if(p.offset1 > maxSize)
			errx(12, "ERROR: offset %d does not exist in file2", p.offset1);

		if(p.offset2 > max)
			max = p.offset2;
	}

	
	myLseek(fd, 0, 4);
	return max;
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected;");

	int fd1 = openFile(argv[1], 1);
	int fd2 = openFile(argv[2], 2);

	uint16_t max = maxEl(fd2);

	int fd3 = createFile(argv[3], max);

	uint32_t buf = 0;
	while(myRead(fd1, (uint32_t*)&p, 4) > 0) {
		myLseek(fd2, p.offset1, 4);
		myRead(fd2, &buf, 4);

		myLseek(fd3, p.offset2, 8);
		myWrite(fd3, (uint64_t*)&buf, 4);
	}

	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
