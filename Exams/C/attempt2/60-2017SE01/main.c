#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

struct Pair {
	uint16_t offset;
	uint8_t old, new;
};

struct Pair p;

int openFile(char* file);
int openFile(char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(5, "ERROR: opening file for read: %s", file);

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(6, "ERROR: creating file for write: %s", file);

	return fd;
}

int myRead(int fd, uint8_t* buf);
int myRead(int fd, uint8_t* buf) {
	int bytes = 0;
	if((bytes = read(fd, buf, 1)) == -1)
		err(7, "ERROR: reading from file");

	return bytes;
}


int myWrite(int fd);
int myWrite(int fd) {
	int bytes = 0;
	if((bytes = write(fd, &p, sizeof(p))) != sizeof(p))
		err(8, "ERROR: writing to file");
	
	return bytes;

	return bytes;
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");

	char* f1 = argv[1];
	char* f2 = argv[2];
	char* patch = argv[3];

	struct stat st1;
	struct stat st2;
	if(stat(f1, &st1) == -1)
		err(2, "ERROR: running stat for f1");
	if(stat(f2, &st2) == -1)
		err(3, "ERROR: running stat for f2");

	if(st1.st_size != st2.st_size)
		errx(4, "ERROR: f1 and f2 are different sizes");

	int fd1 = openFile(f1);
	int fd2 = openFile(f2);
	int fd3 = createFile(patch);

	for(p.offset = 0; p.offset < st1.st_size; ++p.offset) {
		myRead(fd1, &p.old);		
		myRead(fd2, &p.new);

		if(p.old != p.new)
			myWrite(fd3);
	}

	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
