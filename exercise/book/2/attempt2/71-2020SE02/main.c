#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

void myLseek(int fd, int pos);
void myLseek(int fd, int pos) {
	if(lseek(fd, pos, SEEK_SET) == -1)
		err(6, "ERROR: running lseek. Position does not exist");
}

int openFile(char* file, bool isSdl);
int openFile(char* file, bool isSdl) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	if(!isSdl)
		return fd;

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat for file: %s", file);

	if(st.st_size % 2 != 0)
		errx(4, "ERROR: file size not devisible by 2: %s", file);

	return fd;
}

int createFile(const char* file);
int createFile(const char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(5, "ERROR: creating file for write: %s", file);

	return fd;
}

int myRead(int fd, uint16_t* buf, int size);
int myRead(int fd, uint16_t* buf, int size) {
	int bytes = 0;
	if((bytes = read(fd, buf, size)) == -1)
		err(7, "ERROR: reading from file");

	return bytes;
}

int myWrite(int fd, uint16_t* buf);
int myWrite(int fd, uint16_t* buf) {
	int bytes = 0;
	if((bytes = write(fd, buf, 2)) != 2)
		err(8, "ERROR: writing to file");

	return bytes;
}

bool getBit(uint8_t byte, int index);
bool getBit(uint8_t byte, int index) {
	return ((1 << index) & byte) != 0;
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");

	char* scl = argv[1];
	char* sdl = argv[2];
	char* newFile = argv[3];
	
	int fd1 = openFile(scl, 0);
	int fd2 = openFile(sdl, 1);
	int fd3 = createFile(newFile);

	uint16_t position = 0;
	while(myRead(fd2, &position, 2) > 0) {
		int pos = position / 8;
		int index = 8 - (position % 8) - 1;

		myLseek(fd1, pos);

		uint8_t byte;
		myRead(fd1, (uint16_t*)&byte, 1);

		bool bit = getBit(byte, index);

		if(bit)
			myWrite(fd3, &position);
	}

	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
