#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdbool.h>
#include <stdint.h>

bool getBit(uint16_t byte, int index);
bool getBit(uint16_t byte, int index) {
	return ((1 << index) & byte) != 0;
}

uint8_t setBit(uint8_t byte, int index, bool bit);
uint8_t setBit(uint8_t byte, int index, bool bit) {
	if(bit) {
		return ((1 << index) | byte);
	} else {
		return (~(1 << index) & byte);
	}
}

uint8_t encodeByte(uint16_t byte);
uint8_t encodeByte(uint16_t byte) {
	uint8_t num = 0x00;

	for(int i = 0; i < 16; i += 2) {
		bool first = getBit(byte, i + 1);
		bool second = getBit(byte, i);

		if(!first && second) {
			num = setBit(num, i/2, 0);
		} else if (first && !second) {
			num = setBit(num, i/2, 1);
		} else {
			errx(6, "ERROR: invalid sequence");
		}
	}

	return num;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	char* file1 = argv[1];
	char* file2 = argv[2];

	int fd1; 
	if((fd1 = open(file1, O_RDONLY)) == -1)
		err(2, "ERROR: opening file1 for read");

	int fd2;
	if((fd2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(3, "ERROR: creating file2 for write");

	int bytes = 0;
	uint16_t buf;
	while((bytes = read(fd1, &buf, sizeof(buf))) > 0) {
		uint8_t byte = encodeByte(buf);

		if(write(fd2, &byte, sizeof(byte)) != bytes/2)
			err(5, "ERROR: writing encoded byte to file2");
	}
	if(bytes < 0)
		err(4, "ERROR: reading from file1");

	close(fd1);
	close(fd2);
	return 0;
}
