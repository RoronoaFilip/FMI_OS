#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>

struct Interval {
	uint16_t offset, len;
};

struct Complect {
	struct Interval post, pre, in, suf;
};

struct Complect c;

struct Header {
	uint32_t begin;
	uint16_t count;
	uint16_t fill;
	uint64_t unused;
};

int openForRead(const char* name, int elSize, struct Header* h);
int createForWrite(const char* name);

int openForRead(const char* name, int elSize, struct Header* h) {
	int fd;
	if((fd = open(name,  O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", name);

	if(read(fd, h, sizeof(*h)) == -1)
		err(4, "ERROR: raeding header from file: %s", name);

	struct stat st;
	if(stat(name, &st) == -1)
		err(5, "ERROR: running stat on file: %s", name);

	if((st.st_size - sizeof(*h)) % elSize != 0)
		errx(6, "ERROR: file structure is invalid: %s", name);

	return fd;
}

int createForWrite(const char* name) {
	int fd;
	if((fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
		err(3, "ERROR: creating file for write: %s", name);

	return fd;
}

void setSeek(int fd, int pos, const char* file);
void setSeek(int fd, int pos, const char* file) {
	if(lseek(fd, pos, SEEK_SET) == -1)
		err(8, "ERROR: running lseek on file: %s for position %d", file, pos);
}

char* myRead(int fd, const char* file, int elSize, struct Interval* interval, int* readBytes);
char* myRead(int fd, const char* file, int elSize, struct Interval* interval, int* readBytes) {
	int blockSize = interval->len * elSize;

	char* arr = (char*) malloc(blockSize);
	setSeek(fd, interval->offset * elSize, file);

	if((*readBytes = read(fd, arr, blockSize)) < 0)
		err(7, "ERROR: reading block from file: %s", file);
	
	return arr;
}

int myWrite(int fd, const char* arr, int blockSize, const char* file);
int myWrite(int fd, const char* arr, int blockSize, const char* file) {
	int writtenBytes = 0;
	if((writtenBytes = write(fd, arr, blockSize)) != blockSize)
		err(9, "ERROR: writing block to file: %s", file);

	return writtenBytes;
}

int main(int argc, char** argv) {

	if(argc != 7)
		errx(1, "ERROR: params count");

	char* af = argv[1]; struct Header afh;
	char* post = argv[2]; struct Header posth;
	char* pre = argv[3]; struct Header preh;
	char* in = argv[4]; struct Header inh;
	char* suf = argv[5]; struct Header sufh;
	char* cruci = argv[6];

	int afd = openForRead(af, 2, &afh);
	int postd = openForRead(post, 4, &posth);
	int pred = openForRead(pre, 1, &preh);
	int ind = openForRead(in, 2, &inh);
	int sufd = openForRead(suf, 8, &sufh);
	int crucid = createForWrite(cruci);

	int bytes = 0;
	while((bytes = read(afd, &c, sizeof(c))) > 0) {
		int readBytes = 0;
		
		char* arr = myRead(postd, post, 4, &c.post, &readBytes);
		int writtenBytes = myWrite(crucid, arr, readBytes, cruci);
		free(arr);

		arr = myRead(pred, pre, 1, &c.pre, &readBytes);
		writtenBytes = myWrite(crucid, arr, readBytes, cruci);
		free(arr);

		arr = myRead(ind, in, 2, &c.in, &readBytes);
		writtenBytes = myWrite(crucid, arr, readBytes, cruci);
		free(arr);

		arr = myRead(sufd, suf, 8, &c.suf, &readBytes);
		writtenBytes = myWrite(crucid, arr, readBytes, cruci);
		free(arr);
	}	
	if(bytes < 0)
		err(6, "ERROR: reading intervals from file: %s", af);

	close(afd);
	close(postd);
	close(pred);
	close(ind);
	close(sufd);
	close(crucid);
	return 0;
}
