#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>

int openFile(char* file);
int openFile(char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(3, "ERROR: creatin filr for write: %s", file);

	return fd;
}

bool getBit(uint8_t byte, int index);
bool getBit(uint8_t byte, int index) {
	return ((1<<index) & byte) != 0;
}

uint16_t setBit(uint16_t byte, int index, bool bit);
uint16_t setBit(uint16_t byte, int index, bool bit) {
	if(bit)
		return ((1 << index) | byte);
	else
		return (~(1<<index) & byte);
}

uint16_t encodeByte(uint8_t byte);
uint16_t encodeByte(uint8_t byte) {
	uint16_t result = 0x0000;

	for(int i = 7; i >= 0; --i) {
		bool bit = getBit(byte, i);

		if(bit) {
			result = setBit(result, i*2+1, 1);
			result = setBit(result, i*2, 0);
		} else {
			result = setBit(result, i*2+1, 0);
			result = setBit(result, i*2, 1);
		}
	}

	return result;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int fd1 = openFile(argv[1]);
	int fd2 = createFile(argv[2]);

	uint8_t byte = 0;
	int bytes = 0;
	while((bytes = read(fd1, &byte, sizeof(byte))) > 0) {
		uint16_t encoded = encodeByte(byte);

		if(write(fd2, &encoded, sizeof(encoded)) != sizeof(encoded))
			err(4, "ERROR: writing encoded byte to file 2");
	}
	if(bytes == -1)
		err(3, "ERROR: reading from file1");

	close(fd1);
	close(fd2);
	return 0;
}
