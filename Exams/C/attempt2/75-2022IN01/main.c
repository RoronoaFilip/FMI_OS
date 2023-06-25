#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

uint16_t magic = 0x5A4D;
uint16_t fileSize = 0;

struct Header {
	uint16_t magic, type;
	uint32_t len;
};

struct Header h;

struct Pair {
	uint16_t old, new;
};

struct Pair p;

void myLseek(int fd, int elSize, uint16_t pos);
void myLseek(int fd, int elSize, uint16_t pos) {
	if(lseek(fd, (elSize * pos) + 8, SEEK_SET) == -1)
		err(3, "ERROR: running lseek");
}

int myRead(int fd, uint32_t* buf);
int myRead(int fd, uint32_t* buf) {
	int bytes = 0;
	if((bytes = read(fd, buf, 4)) == -1)
		err(10, "ERROR: reading from file");

	return bytes;
}


int myWrite(int fd, uint64_t* buf);
int myWrite(int fd, uint64_t* buf) {
	int bytes = 0;
	if((bytes = write(fd, buf, 8)) == -1)
		err(10, "ERROR: writing to file");

	return bytes;
}

int openFile(char* file, uint16_t type);
int openFile(char* file, uint16_t type) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	if(read(fd, &h, sizeof(h)) == -1)
		err(4, "ERROR: reading header from file: %s", file);

	if(h.magic != magic)
		errx(5, "ERROR: invalid magic value in file: %s", file);
	
	if(h.type != type)
		errx(5, "ERROR: invalid type in file header: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(6, "ERROR: running stat for file: %s", file);

	if((st.st_size - sizeof(h)) % 4 != 0)
		errx(7, "ERROR: invalid file structure: %s", file);
	
	if((st.st_size - sizeof(h)) / 4 != h.len)
		errx(8, "ERROR: invalid file elements count: %s", file);

	if(type == 2)
		fileSize = (uint16_t)st.st_size;

	return fd;
}

int createFile(char* file, uint32_t elementsCount);
int createFile(char* file, uint32_t elementsCount) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(9, "ERROR: creating file: %s", file);

	h.len = elementsCount;
	h.type = 3;
	h.magic = magic;

	if(write(fd, &h, sizeof(h)) != sizeof(h))
		err(10, "ERROR: writing header to file: %s", file);

	uint64_t buf = 0;
	for(uint16_t i = 0; i < elementsCount; ++i)
		myWrite(fd, &buf);	

	return fd;
}

uint16_t findMaxEl(int fd);
uint16_t findMaxEl(int fd) {
	uint16_t max = 0;
	while(myRead(fd, (uint32_t*)&p) > 0) {
		if(p.old > fileSize)
			errx(19, "ERROR: offset: %d does not exist in file 2", p.old);

		if(p.new > max)
			max = p.new;
	}

	myLseek(fd, 4, 0);

	return max;
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");

	char* file1 = argv[1];
	char* file2 = argv[2];
	char* file3 = argv[3];

	int fd1 = openFile(file1, 1);
	int fd2 = openFile(file2, 2);

	uint32_t max = (uint32_t)findMaxEl(fd1);

	int fd3 = createFile(file3, max);

	uint64_t buf = 0;
	while(myRead(fd1, (uint32_t*)&p) > 0) {
		myLseek(fd2, 4, p.old);	
		myLseek(fd3, 8, p.new);

		myRead(fd2, (uint32_t*)&buf);
		myWrite(fd3, &buf);
	}

	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
