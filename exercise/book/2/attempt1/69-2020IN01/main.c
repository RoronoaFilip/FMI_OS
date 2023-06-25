#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>

int f1Size = 0;

struct Header {
	uint32_t magic;
	uint8_t version;
	uint8_t dataVersion;
	uint16_t count;
	uint32_t reserved1, reserved2;
};

struct Header h;

struct Data1 {
	uint16_t offset;
	uint8_t old, new;
};

struct Data1 d1;

struct Data2 {
	uint32_t offset;
	uint16_t old, new;
};

struct Data2 d2;

int openFile1(char* patch);
int openFile1(char* patch) {
	int fd;
	if((fd = open(patch, O_RDONLY)) == -1)
		err(2, "ERROR: opening file1 for read: %s", patch);
	
	if(read(fd, &h, sizeof(h)) == -1)
		err(3, "ERROR: reading header from file1: %s", patch);
	
	if(h.magic != 0xEFBEADDE)
		errx(4, "ERROR: invalid magic value in file1 header");
	
	if(h.version != 0x01)
		errx(5, "ERROR: invalid header version in file1 header");

	uint16_t dataSize = ((h.dataVersion) ? sizeof(d2) : sizeof(d1));

	struct stat st;
	if(stat(patch, &st) == -1)
		err(6, "ERROR: running stat on file1: %s", patch);
	
	if((st.st_size - sizeof(h)) % dataSize != 0)
		errx(7, "ERROR: invalid structure for file1: %s", patch);
	
	return fd;
}

int openF1(char* f1);
int openF1(char* f1) {
	int fd1;
	if((fd1 = open(f1, O_RDONLY)) == -1)
		err(8, "ERROR: opening file2 for read: %s", f1);

	struct stat st;
	if((stat(f1, &st)) == -1)
		err(9, "ERROR: running stat on file2: %s", f1);
	
	f1Size = st.st_size;

	if(h.dataVersion == 0x00)
		return fd1;

	if(st.st_size % 2 != 0)
		errx(10, "ERROR: invalid structure in file2: %s", f1);

	return fd1;
}

int openF2(char* f2);
int openF2(char* f2) {
	int fd2;
	if((fd2 = open(f2, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
		err(11, "ERROR: creating file3 for write: %s", f2);

	return fd2;
}


uint8_t* readVersion00(int fd, int fd1);
uint8_t* readVersion00(int fd, int fd1) {
	uint8_t* arr = (uint8_t*) malloc(f1Size);

	int bytes = 0;
	if((bytes = read(fd1, arr, f1Size)) != f1Size)
		err(14, "ERROR: reading file2");

	while((bytes = read(fd, &d1, sizeof(d1))) > 0) {
		if(arr[d1.offset] != d1.old)
			errx(15, "ERROR: in file2 on position %d: Original:%d != Actual: %d", d1.offset, d1.old, arr[d1.offset]);
		else
			arr[d1.offset] = d1.new;
	}
	if(bytes < 0)
		err(13, "ERROR: reading from file1 for version 00");

	return arr;
}

uint16_t* readVersion01(int fd, int fd1);
uint16_t* readVersion01(int fd, int fd1) {
	uint16_t* arr = (uint16_t*) malloc(f1Size);

	int bytes = 0;
	if((bytes = read(fd1, arr, f1Size)) != f1Size)
		err(16, "ERROR: reading file2");

	while((bytes = read(fd, &d2, sizeof(d2))) > 0) {
		if(arr[d2.offset] != d2.old)
			errx(17, "ERROR: in file2 on position %d: Original:%d != Actual: %d", d2.offset, d2.old, arr[d2.offset]);
		else
			arr[d2.offset] = d2.new;
	}
	if(bytes < 0)
		err(18, "ERROR: reading from file1 for version 00");

	return arr;
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count");
	
	char* patch = argv[1];
	char* f1 = argv[2];
	char* f2 = argv[3];

	int fd = openFile1(patch);
	int fd1 = openF1(f1);

	void* arr;
	if(h.dataVersion == 0x00)
		arr = readVersion00(fd, fd1);
	else
		arr = readVersion01(fd, fd1);

	int fd2 = openF2(f2);

	if(write(fd2, arr, f1Size) != f1Size)
		err(19, "ERROR: writing to file2 in version 0x00");

	close(fd);
	close(fd1);
	close(fd2);
	free(arr);
	return 0;
}
