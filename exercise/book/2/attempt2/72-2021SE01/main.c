#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>

bool getBit(uint8_t byte, int index);
bool getBit(uint8_t byte, int index) {
	return ((1 << index) & byte) != 0;
}

uint16_t setBit(uint16_t byte, int index, bool bit);
uint16_t setBit(uint16_t byte, int index, bool bit) {
	if(bit) {
		return ((1 << index)) | byte;
	} else {
		return (~(1 << index)) & byte;
	}
}

uint16_t encodeByte(uint8_t byte);
uint16_t encodeByte(uint8_t byte) {
	uint16_t num = 0x0000;

	for(int i = 0; i < 8; ++i) {
		bool bit = getBit(byte, i);

		if(bit) {
			num = setBit(num, i*2 + 1, 1);
			num = setBit(num, i*2, 0);
		} else {
			num = setBit(num, i*2 + 1, 0);
			num = setBit(num, i*2, 1);
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
	uint8_t buf;
	while((bytes = read(fd1, &buf, sizeof(buf))) > 0) {
		uint16_t num = encodeByte(buf);

		if(write(fd2, &num, sizeof(num)) != bytes*2)
			err(6, "ERROR: writing encoded byte to file2");
	}
	if(bytes < 0)
		err(4, "ERROR: reading bytes from file 1");

	close(fd1);
	close(fd2);
	return 0;
}
