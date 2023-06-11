#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>

uint16_t setBit(uint16_t x, int index, bool bit);
bool getBit(uint8_t byte, int index);
uint16_t encodeByte(uint8_t byte);
void manchesterEncodeBytes(uint8_t* buf, uint16_t* BUF, int size);

uint16_t setBit(uint16_t x, int index, bool bit) {
	if(bit){ 
		return (1 << index) | x;
	} else {
		return (~(1 << index)) & x;
	}
		
}
bool getBit(uint8_t byte, int index) {
	return (((1 << index) & byte) != 0);
}
uint16_t encodeByte(uint8_t byte) {
	uint16_t result = 0;
	for(int i = 0; i < 8; ++i) {
		bool bit = getBit(byte, i);
		if(bit) {
			result = setBit(result, i*2, 0);
			result = setBit(result, i*2 + 1, 1);
		} else {
			result = setBit(result, i*2, 1);
			result = setBit(result, i*2 + 1, 0);
		}
	}

	return result;
}
void manchesterEncodeBytes(uint8_t* buf, uint16_t* BUF, int size) {
	for(int i = 0; i < size; ++i) {
		BUF[i] = encodeByte(buf[i]);
	}
}

int main(int argc, char** argv) {

	if(argc != 3) 
		errx(1, "ERROR: params count");
	
	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);
	
	int fd2;
	if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
		err(3, "ERROR: opening file for write: %s", argv[2]);

	uint8_t buf[4096];
	uint16_t BUF[4096];

	int bytes;
	while((bytes = read(fd1, buf, sizeof(buf))) > 0) {
		 manchesterEncodeBytes(buf, BUF, bytes);
		
		int bytesWritten = write(fd2, BUF, bytes * 2);

		if(bytesWritten < 0)
			err(7, "ERROR: writing to file: %s", argv[2]);
		if(bytes * 2 != bytesWritten)
			err(8, "ERROR: could not write all bytes");
	}
	if(bytes != 0) {
		err(6, "ERROR: reading from file: %s", argv[1]);
	}


	close(fd1);
	close(fd2);
	return 0;
}

