#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int openFile(char* file, uint32_t* size);
int openFile(char* file, uint32_t* size) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(6, "ERROR: opening file for read: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(7, "ERROR: running stat on file: %s", file);

	if(st.st_size % 2 != 0)
		errx(8, "ERROR: file size not devisible by 2");

	*size = st.st_size / 2;

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(9, "ERROR: creating file for write: %s", file);

	return fd;
}

int myRead(int fd, uint16_t* buf);
int myRead(int fd, uint16_t* buf) {
	int bytes = 0;
	if((bytes = read(fd, buf, 2)) != 2)
		err(4, "error: reading number to file");

	return bytes;
}

int myWrite(int fd, uint32_t* buf, int size);
int myWrite(int fd, uint32_t* buf, int size) {
	int bytes = 0;
	if((bytes = write(fd, buf, size)) != size)
		err(2, "error: writing number to file");

	return bytes;
}

int appendToFile(int fd, const char* arr);
int appendToFile(int fd, const char* arr) {
	int bytes = 0;
	int len = strlen(arr);
	if((bytes = write(fd, arr, len)) != len)
		err(3, "ERROR: writing string to file");

	return bytes;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 3 expected");

	char* f1 = argv[1];
	char* f2 = argv[2];

	uint32_t size = 0;
	int fd1 = openFile(f1, &size);
	int fd2 = createFile(f2);

	dup2(fd2, 1);
	setbuf(stdout, NULL);

	int bytes = 0;
	bytes = appendToFile(fd2, "#include <stdio.h>\n\0");
	bytes = appendToFile(fd2, "#include <stdint.h>\n\0");
	bytes = appendToFile(fd2, "\n\0");
	bytes = appendToFile(fd2, "const uint32_t arrN = \0");
	printf("%d;\n", size);
	bytes = appendToFile(fd2, "\n\0");
	bytes = appendToFile(fd2, "const uint16_t arr[arrN] = { \0");

	uint16_t buf;
	for(uint32_t i = 0; i < size - 1; ++i) {
		myRead(fd1, &buf);
		printf("%d, ", buf);
	}

	myRead(fd1, &buf);
	printf("%d };\n", buf);

	close(fd1);
	close(fd2);
	
	return 0;
}
