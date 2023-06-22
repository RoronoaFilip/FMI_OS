#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <sys/stat.h>

uint8_t elSize = 0;

struct Header {
	uint32_t magic;
	uint8_t headerVersion, type;
	uint16_t count;
	uint32_t r1, r2;
};

struct Header h;

int openPatch(char* file);
int openPatch(char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening patch file for read");
	
	if(read(fd, &h, sizeof(h)) == -1)
		err(3, "ERROR: reading header from patch file");

	if(h.magic != 0xEFBEADDE)
		errx(4, "ERROR: invalid magic value in header");

	if(h.type == 0x00)
		elSize = 1;
	else if(h.type == 0x01)
		elSize = 2;
	else
		errx(5, "ERROR: invalid type in header");

	struct stat st;
	if(stat(file, &st) == -1)
		err(6, "ERROR: running stat for patch file");

	uint16_t dataSize = ((elSize == 1) ? 4 : 8);

	if((st.st_size - sizeof(h)) / dataSize != h.count)
		errx(7, "ERROR: invalid data structure in patch file");

	return fd;
}

int openFile(char* file);
int openFile(char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(12, "ERROR: opening 2nd file for read");

	if(elSize == 1)
		return fd;

	struct stat st;
	if(stat(file, &st) == -1)
		err(13, "ERROR: running stat for 2nd file");

	if(st.st_size % elSize != 0)
		errx(14, "ERROR: 2nd file size not devisible by: %d", elSize);

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1)
		err(15, "ERROR: creating file for write");

	return fd;
}

void myLseek(int fd, uint32_t offset);
void myLseek(int fd, uint32_t offset) {
	if(lseek(fd, offset*elSize, SEEK_SET) == -1)
		err(8, "ERROR: running lseek in file");
}

int myRead(int fd, uint32_t* buf, int size);
int myRead(int fd, uint32_t* buf, int size) {
	int bytes = 0;
	if((fd = read(fd, buf, size)) == -1)
		err(9, "ERROR: reading number from file");

	return fd;
}

int myWrite(int fd, uint16_t* buf, int size);
int myWrite(int fd, uint16_t* buf, int size) {
	int bytes = 0;
	if((fd = write(fd, buf, size)) == size)
		err(9, "ERROR: writing number to file");

	return fd;
}

void myUnlink(char* file, int error, const char* message, uint32_t pos, uint16_t expected, uint16_t actual);
void myUnlink(char* file, int error, const char* message, uint32_t pos, uint16_t expected, uint16_t actual) {
	if(unlink(file) == -1)
		err(16, "ERROR: deliting file: %s", file);

	err(error, message, pos, expected, actual);
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: ");

	int patchFd = openPatch(argv[1]);
	int fd1 = openFile(argv[2]);
	int fd2 = createFile(argv[3]);

	uint16_t buf;
	while(myRead(fd1, (uint32_t*)&buf, elSize) > 0) {
		myWrite(fd2, &buf, elSize);
	}

	uint32_t offset;
	uint16_t old, new;
	int bytes = 0;
	while(1) {
		bytes = myRead(patchFd, &offset, elSize*2);
		if(bytes == 0)
			break;
		bytes = myRead(patchFd, (uint32_t*)&old, elSize);
		if(bytes == 0)
			break;
		bytes = myRead(patchFd, (uint32_t*)&new, elSize);
		if(bytes == 0)
			break;

		myLseek(fd1, offset);
		myRead(fd1, (uint32_t*)&buf, elSize);

		if(buf != old)
			myUnlink(argv[2], 20, "ERROR: on pos: %d, expected: %d, but found: %d", offset, old, buf);

		myLseek(fd2, offset);
		myWrite(fd2, &new, elSize);
	}

	close(patchFd);
	close(fd1);
	close(fd2);
	return 0;
}
