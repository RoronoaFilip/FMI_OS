#include <err.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

char* file1;
char* file2;
int fileSize = 0;

int openFileForRead(const char* file);
int openFileForRead(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file1 for read: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(4, "ERROR: running stat on file1: %s", file);

	if(st.st_size % 2 != 0)
		err(5, "ERROR: file is not made of uint16_t nums");

	if(st.st_size / 2 > 524288)
		err(6, "ERROR: file size is more than 524288");

	fileSize = st.st_size / 2;

	return fd;
}

int createFileForWrite(const char* file);
int createFileForWrite(const char* file) {
	int fd;
	if((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		err(3, "ERROR: creating file for write: %s", file);

	return fd;
}

int myWrite(int fd, const char* arr);
int myWriteNum(int fd, uint8_t num);

int myWrite(int fd, const char* arr) {
	int len = strlen(arr);

	int bytes = 0;
	if((bytes = write(fd, arr, len)) != len)
		err(7, "ERROR: writing string: %s in file %s", arr, file2);

	return bytes;
}

int myWriteNum(int fd, uint8_t num) {
	int size = sizeof(num);

	int bytes = 0;
	if((bytes = write(fd, &num, size)) != size)
		err(8, "ERROR: writing %d to file: %s", num, file2);

	return bytes;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	file1 = argv[1];
	file2 = argv[2];

	int fd1 = openFileForRead(file1);
	int fd2 = createFileForWrite(file2);

	dup2(fd2, 1);
	setbuf(stdout, NULL);

	myWrite(fd2, "#include <stdio.h>\n\0");
	myWrite(fd2, "#include <stdint.h>\n\0");
	myWrite(fd2, "\n\0");
	myWrite(fd2, "uint32_t arrN = \0");
	dprintf(fd2, "%d;\n", fileSize);
	myWrite(fd2, "\n\0");
	myWrite(fd2, "uint16_t arr[] = { \0");

	uint8_t byte1;
	uint8_t byte2;
	int bytes = 0;
	for(int i = 0; i < fileSize - 1; ++i) {
		bytes = read(fd1, &byte1, sizeof(byte1));
		if(bytes <= 0)
			break;

		bytes = read(fd1, &byte2, sizeof(byte2));
		if(bytes <= 0)
			break;

		dprintf(fd2, "0x%02x%02x, ", byte2, byte1);
	}

	bytes = read(fd1, &byte1, sizeof(byte1));
	if(bytes <= 0)
		 err(10, "ERROR: reading from file1: %s", file1);
	bytes = read(fd1, &byte2, sizeof(byte2));
	if(bytes <= 0)
		err(10, "ERROR: reading from file1: %s", file1);
	dprintf(fd2, "0x%02x%02x ", byte2, byte1);

	if(bytes < 0)
		err(10, "ERROR: reading from file1: %s", file1);

	myWrite(fd2, "};\0");

	close(fd1);
	close(fd2);
	return 0;
}
