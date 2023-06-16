#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#define int32 uint32_t
#define int16 uint16_t
#define int8 uint8_t

int elSize;

struct Header {
	int32 magic;
	int8 headerVersion;
	int8 dataVersion;
	int16 count;
	int32 r1, r2;
};

struct Header h;

void myLseek(int fd, int pos);
void myLseek(int fd, int pos) {
	if(lseek(fd, pos*elSize, SEEK_SET) == -1)
		err(11, "ERROR: running lseek");
}

int openPatch(const char* file);
int openPatch(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening patch file for read: %s", file);

	if(read(fd, &h, sizeof(h)) != sizeof(h))
		err(3, "ERROR: reading header from patch file: %s", file);

	if(h.magic != 0xEFBEADDE)
		errx(4, "ERROR: invalid magic value find in patch header: %s", file);
	
	if(h.headerVersion != 0x01)
		errx(5, "ERROR: invalid header version in patch header: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(5, "ERROR: running stat on patch file: %s", file);

	int size = ((h.dataVersion == 0x00) ? 4 : 8);

	if(h.dataVersion == 0x00)
		elSize = 1;
	else if(h.dataVersion == 0x01)
		elSize = 2;
	else
		errx(6, "ERROR: invalid data version found in patch header: %s", file);

	if((st.st_size - sizeof(h)) / size != h.count)
		errx(7, "ERROR: invalid data count found in patch header: %s", file);

	return fd;
}

int openF1(const char* file);
int openF1(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(8, "ERROR: opening f1 for read");

	if(h.dataVersion == 0x00)
		return fd;
	
	struct stat st;
	if(stat(file, &st) == -1)
		err(9, "ERROR: running stat on f1");
	
	if(st.st_size % 2 != 0)
		errx(10, "ERROR: data in f1 not devisible by 2");

	return fd;
}

int openF2(const char* file);
int openF2(const char* file) {
	int fd;
	if((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		err(12, "ERROR: creating f2 for write: %s", file);

	return fd;
}

int readData(int fd, int32* offset, int16* old, int16* new);
int readData(int fd, int32* offset, int16* old, int16* new) {
	int totalBytes = 0;
	int bytes = 0;
	if((bytes = read(fd, offset, 2*elSize)) < 0)
		err(15, "ERROR: reading offset from patch file");
	
	if(bytes == 0)
		return 0;

	totalBytes += bytes;

	if((bytes = read(fd, old, 1*elSize)) < 0)
		err(16, "ERROR: reading old byte from patch file");

	if(bytes == 0)
		return 0;

	totalBytes += bytes;

	if((bytes = read(fd, new, 1*elSize)) < 0)
		err(17, "ERROR: reading new byte from patch file");

	if(bytes == 0)
		return 0;

	totalBytes += bytes;

	if(totalBytes != 4 * elSize)
		errx(18, "ERROR: data size in patch header is invalid");

	return totalBytes;
}

int compareAndWrite(int fd1, int fd2, int32* offset, int16* old, int16* new, int16* actual);
int compareAndWrite(int fd1, int fd2, int32* offset, int16* old, int16* new, int16* actual) {
	myLseek(fd1, (int)*offset);
	myLseek(fd2, (int)*offset);

	if(read(fd1, actual, 1*elSize) != 1*elSize)
		err(20, "ERROR: reading old byte from f1");
	
	if(*actual != *old)
		return -1;

	if(write(fd2, new, 1*elSize) != 1*elSize)
		err(21, "ERROR: writing new byte to f2");

	return 1;
}

void myUnlink(const char* file, int error, const char* message, int n1, int n2, int n3);
void myUnlink(const char* file, int error, const char* message, int n1, int n2, int n3) {
	if(unlink(file) == -1)
		err(error, "ERROR: deleting file: %s", file);

	err(error, message, n1, n2, n3);
}

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count. 3 expected");

	char* patch = argv[1];
	char* f1 = argv[2];
	char* f2 = argv[3];

	int patchFd = openPatch(patch);
	int fd1 = openF1(f1);
	int fd2 = openF2(f2);

	int bytes = 0;
	int8 buf = 0;
	while((bytes = read(fd1, &buf, sizeof(buf))) > 0) {
		if(write(fd2, &buf, sizeof(buf)) != bytes)
			err(13, "ERROR: writing bytes while copying f1 into f2");
	}
	if(bytes < 0)
		err(13, "ERROR: copying f1 into f2");

	int32 offset;
	int16 old;
	int16 new;
	int16 actual;
	while(readData(patchFd, &offset, &old, &new) > 0) {
		if(compareAndWrite(fd1, fd2, &offset, &old, &new, &actual) == -1)
			myUnlink(f2, 25, "ERROR: invalid byte found in f1 on pos: %d. Must be %d was: %d", offset, old, actual);
	}

	close(patchFd);
	close(fd1);
	close(fd2);
	return 0;
}
