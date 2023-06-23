#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>

int openFile(char* file);
int openFile(char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

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
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(5, "ERROR: creating file for write: %s", file);

	return fd;
}

bool getBit(uint16_t num, int index);
bool getBit(uint16_t num, int index) {
	return ((1 << index) & num) != 0;
}

uint8_t setBit(uint8_t byte, int index, bool bit);
uint8_t setBit(uint8_t byte, int index, bool bit) {
	if(bit)
		return ((1 << index) | byte);
	else
		return (~(1 << index) & byte);
}

uint8_t decodeByte(uint16_t num);
uint8_t decodeByte(uint16_t num) {
	uint8_t result = 0x00;

	for(int i = 15; i >= 0; i-=2) {
		bool first = getBit(num, i);
		bool second = getBit(num, i-1);

		if(first && !second)
			result = setBit(result, i/2, 1);
		else if(!first && second)
			result = setBit(result, i/2, 0);
	}

	return result;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int fd1 = openFile(argv[1]);
	int fd2 = createFile(argv[2]);

	uint16_t num;
	int bytes = 0;
	while((bytes = read(fd1, &num, sizeof(num))) > 0) {
		uint8_t byte = decodeByte(num);

		if(write(fd2, &byte, sizeof(byte)) != sizeof(byte))
			err(7, "ERROR: writing to file");
	}
	if(bytes < 0)
		err(6, "ERROR: reading num from file");

	close(fd1);
	close(fd2);
	return 0;
}
