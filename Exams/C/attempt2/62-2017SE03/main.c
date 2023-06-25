#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

struct Data {
	uint16_t offset;
	uint8_t old, new; 
};


struct Data d;

void myLseek(int fd, int pos);
void myLseek(int fd, int pos) {
	if(lseek(fd, pos, SEEK_SET) == -1)
		err(2, "ERROR: running lseek in file");
}

int myRead(int fd, uint8_t* byte);
int myRead(int fd, uint8_t* byte) {
	int bytes = 0;
	if((bytes = read(fd, byte, 1)) == -1)
		err(3, "ERROR: reading from file");

	return bytes;
}

int myWrite(int fd, uint8_t* byte);
int myWrite(int fd, uint8_t* byte) {
	if(write(fd, byte, 1) != 1)
		err(4, "ERROR: writing to file");

	return 1;
}

int openPatch(const char* file);
int openPatch(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(5, "ERROR: opening patch file");

	struct stat st;
	if(stat(file, &st) == -1)
		err(6, "ERROR: running stat on patch file");

	if(st.st_size % sizeof(d) != 0)
		errx(7, "ERROR: structure in patch file is not devisible by 4");

	return fd;	
}

int openF1(const char* file);
int openF1(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(8, "ERROR: opening f1 for read");

	return fd;
}

int openF2(const char* file);
int openF2(const char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(9, "ERROR: craeting file 2 for write");

	return fd;
}

int compareAndWrite(int fd1, int fd2, uint8_t* buf);
int compareAndWrite(int fd1, int fd2, uint8_t* buf) {
	myLseek(fd1, d.offset);
	myLseek(fd2, d.offset);

	myRead(fd1, buf);

	if(*buf != d.old)
		return -1;

	myWrite(fd2, &d.new);

	return 1;
}

void myUnlink(const char* file, int error, const char* message, int pos, uint8_t old, uint8_t new);
void myUnlink(const char* file, int error, const char* message, int pos, uint8_t old, uint8_t new) {
	if(unlink(file) == -1)
		err(10, "ERROR: deleting file: %s", file);

	errx(error, message, pos, old, new);
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");
	
	char* patch = argv[1];
	char* f1 = argv[2];
	char* f2 = argv[3];

	int patchFd = openPatch(patch);
	int fd1 = openF1(f1);
	int fd2 = openF2(f2);

	int bytes = 0;
	uint8_t buf = 0;

	while((bytes = myRead(fd1, &buf)) > 0) {
		myWrite(fd2, &buf);
	}

	while((bytes = read(patchFd, &d, sizeof(d))) > 0) {
		int val = compareAndWrite(fd1, fd2, &buf);

		if(val == -1)
			myUnlink(f2, 12, "ERROR: writing byte to f2. On pos: %d found byte: %d, when expected byte: %d", d.offset, buf, d.old);
	}
	if(bytes < 0)
		err(11, "ERROR: reading data from patch file");

	close(patchFd);
	close(fd1);
	close(fd2);
	return 0;
}
