#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <sys/stat.h>

struct Header {
	uint16_t magic;
	uint16_t type;
	uint32_t count;
};

struct Header h;

struct Pair {
	uint16_t pos, val;
};

struct Pair p;

void setSeek(int fd, const char* file, int pos);
void setSeek(int fd, const char* file, int pos) {
	if(lseek(fd, 8 + pos, SEEK_SET) == -1)
		err(2, "ERROR: running lseek for file: %s on pos: %d", file, pos);
}

int openFileForRead(const char* file, int elSize, int type);
int openFileForRead(const char* file, int elSize, int type) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(4, "ERROR: opening file for read: %s", file);

	struct stat st;

	if(stat(file, &st) == -1)
		err(5, "ERROR: running stat on file: %s", file);

	if(read(fd, &h, sizeof(h)) == -1)
		err(6, "ERROR: reading header from file: %s", file);

	if(h.type != type)
		errx(7, "ERROR: invalid file type for: %s", file);

	if(h.magic != 0x5A4D)
		errx(10, "ERROR: invalid file magic number: %s", file);

	if((st.st_size - 8) % elSize != 0)
		errx(8, "ERROR: invalid elements size in file: %s", file);

	if((st.st_size - 8) / elSize != h.count)
		errx(9, "ERROR: invalid elements count in file: %s", file);

	return fd;
}

int createFileForWrite(const char* file, uint16_t size, int elSize);
int createFileForWrite(const char* file, uint16_t size, int elSize) {
	h.count = size;
	h.type = 3;
	h.magic = 0x5A4D;

	int fd;
	if((fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
		err(11, "ERROR: creating file for write: %s", file);

	if(write(fd, &h, sizeof(h)) != sizeof(h))
		err(12, "ERROR: writing header to file: %s", file);

	uint64_t buf = 0x0000000000000000;
	for(uint16_t i = 0; i < size; ++i)
		if(write(fd, &buf, elSize) != elSize)
			err(13, "ERROR: writing 0x00 bytes to file: %s", file);

	setSeek(fd, file, 0);

	return fd;
}

int myWrite(int fd, const char* file, uint32_t num, int size);
int myWrite(int fd, const char* file, uint32_t num, int size) {
	int bytes = 0;
	if((bytes = write(fd, &num, size)) != size)
		err(3, "ERROR: writing %d to file: %s", num, file);

	return bytes;
}

int myRead(int fd, const char* file, uint32_t* el, int size);
int myRead(int fd, const char* file,  uint32_t* el, int size) {
	int bytes = 0;
	if((bytes = read(fd, el, size)) != size)
		err(14, "ERROR: reading from file: %s", file);

	return bytes;
}

uint16_t findMaxElement(int fd, const char* file);
uint16_t findMaxElement(int fd, const char* file) {
	setSeek(fd, file, 0);

	int bytes = 0;
	uint16_t max = 0;
	while((bytes = read(fd, &p, sizeof(p))) > 0) {
		if(p.val > max)
			max = p.val;
	}
	if(bytes < 0)
		err(15, "ERROR: finding max element int file1: %s", file);

	setSeek(fd, file, 0);

	return max;
}

int main(int argc, char** argv) {

	if(argc != 4) 
		errx(1, "ERROR: params count. 3 expected");

	char* file1 = argv[1];
	char* file2 = argv[2];
	char* file3 = argv[3];

	int fd1 = openFileForRead(file1, 2, 1);
	int fd2 = openFileForRead(file2, 4, 2);
	
	uint16_t maxVal = findMaxElement(fd1, file1);
	int fd3 = createFileForWrite(file3, maxVal, 8);

	int bytes = 0;
	while((bytes = read(fd1, &p, sizeof(p))) > 0) {
		setSeek(fd2, file2, p.pos * 4);
		setSeek(fd3, file3, p.val * 8);

		uint32_t element = 0;
		myRead(fd2, file2, &element, 4);

		myWrite(fd3, file3, element, 4);
	}
	if(bytes < 0)
		err(17, "ERROR: reading pairs from file1: %s", file1);

	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
