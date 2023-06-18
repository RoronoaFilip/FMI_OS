#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

struct Interval {
	uint32_t offset, len;
};

struct Interval interval;

int openFile(char* file, int num);
int openFile(char* file, int num) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat for file: %s", file);

	int delim = ((num == 1) ? 8 : 4);

	if(st.st_size % delim != 0)
		errx(4, "ERROR: file size not devisible by %d: %s", delim, file);

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(5, "ERROR: creating file for write: %s", file);

	return fd;
}

int readInterval(int fd);
int readInterval(int fd) {
	int bytes = 0;
	if((bytes = read(fd, &interval, sizeof(interval))) < 0)
		err(6, "ERROR: reading interval from file");

	return bytes;
}

int myRead(int fd, uint32_t* buf);
int myRead(int fd, uint32_t* buf) {
	int bytes = 0;
	if((bytes = read(fd, buf, 4)) < 0)
		err(7, "ERROR: reading num from file");

	return bytes;
}

int myWrite(int fd, uint32_t* buf);
int myWrite(int fd, uint32_t* buf) {
	int bytes = 0;
	if((bytes = write(fd, buf, 4)) != 4)
		err(8, "ERROR: writing num to file");

	return fd;
}

void myLseek(int fd, int pos);
void myLseek(int fd, int pos) {
	if(lseek(fd, pos*4, SEEK_SET) == -1)
		err(9, "ERROR: running lseek on file for pos: %d", pos);
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");

	char* f1 = argv[1];
	char* f2 = argv[2];
	char* f3 = argv[3];

	int fd1 = openFile(f1, 1);
	int fd2 = openFile(f2, 0);
	int fd3 = createFile(f3);

	uint32_t buf;
	while(readInterval(fd1) > 0) {
		myLseek(fd2, interval.offset);

		for(uint32_t i = 0; i < interval.len; ++i) {
			myRead(fd2, &buf);
			myWrite(fd3, &buf);
		}
	}

	close(fd1);
	close(fd3);
	close(fd2);
	return 0;
}
