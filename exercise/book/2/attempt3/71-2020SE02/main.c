#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdbool.h>

int openFile(char* file, int isSecond);
int openFile(char* file, int isSecond) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);
	
	if(!isSecond)
		return fd;

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat on file: %s", file);

	if(st.st_size % 2 != 0)
		errx(4, "ERROR: file size not devisible by 2: %s", file);

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		err(5, "ERROR: creating file for read: %s", file);
	
	return fd;
}

void myLseek(int fd, int pos);
void myLseek(int fd, int pos) {
	if(lseek(fd, pos * 2, SEEK_SET) == -1)
		err(6, "ERROR: running lseek ");
}

int myRead(int fd, uint16_t* buf, int size);
int myRead(int fd, uint16_t* buf, int size) {
	int bytes = 0;
	if((bytes = read(fd, buf, size)) == -1)
		err(7, "ERROR: read %d bytes from file", size);

	return bytes;
}

int myWrite(int fd, uint16_t* buf, int size);
int myWrite(int fd, uint16_t* buf, int size) {
	int bytes = 0;
	if((bytes = write(fd, buf, size)) == -1)
		err(7, "ERROR: writing %d bytes to file", size);

	return bytes;
}

bool getBit(uint8_t byte, int index);
bool getBit(uint8_t byte, int index) {
	return ((128 >> index) & byte) != 0;
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");

	int fd1 = openFile(argv[1], 0);
	int fd2 = openFile(argv[2], 1);
	int fd3 = createFile(argv[3]);

	uint8_t byte;
	int position = 0;
	uint16_t num;
	while(myRead(fd1, (uint16_t*)&byte, 1) > 0) {
		for(int i = 0; i < 8; ++i, ++position) {
			bool bit = getBit(byte, i);

			if(!bit)
				continue;

			myLseek(fd2, position);
			myRead(fd2, &num, 2);
			myWrite(fd3, &num, 2);
		}
	}

	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
