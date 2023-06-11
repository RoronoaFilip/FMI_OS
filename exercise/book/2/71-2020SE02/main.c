#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>

int myWrite(int fd, const char* file, uint16_t pos);
int myWrite(int fd, const char* file, uint16_t pos) {
	int bytes = 0;
	if((bytes = write(fd, &pos, sizeof(pos))) == -1)
		err(4, "ERROR: writing pos: %d to file: %s", pos, file);

	return bytes;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 files expected");

	char* scl = argv[1];
	char* sdl = argv[2];

	int fd1, fd2;
	if((fd1 = open(scl, O_RDONLY)) == -1)
		err(2, "ERROR: opening file1 for read: %s", scl);

	if((fd2 = open(sdl, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
		err(3, "ERROR: creating or truncating file2 for write: %s", sdl);

	uint16_t i = 1;
	uint8_t buf;
	int bytes = 0;
	while((bytes = read(fd1, &buf, sizeof(buf))) > 0) {
		if(buf == 0x00) {
			//nothing
		} else if(buf == 0x01) {
			myWrite(fd2, sdl, i+1);
		} else if(buf == 0x10) {
			myWrite(fd2, sdl, i);
		} else if(buf == 0x11) {
			myWrite(fd2, sdl, i);
			myWrite(fd2, sdl, i+1);
		} else {
			errx(9, "ERROR: invalid byte on position: %d. Must be 00,01,10 or 11", i);
		}

		i += 2;
	}
	if(bytes < 0)
		err(6, "ERROR: reading from file1: %s", scl);

	close(fd1);
	close(fd2);
	return 0;
}
