#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>

struct Data {
	uint32_t uid;
	uint16_t u1, u2;
	uint32_t t1, t2;
};

struct Data d;
int size;

struct Pair {
	int index;
	uint32_t uid, time;
};

int openFile(const char* file);
int openFile(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat on file: %s", file);

	if(st.st_size % sizeof(d) != 0)
		err(4, "ERROR: invalid data structure in file: %s", file);

	size = st.st_size / sizeof(d);

	return fd;
}

double getMean(struct Pair arr[size]);
double getMean(struct Pair arr[size]) {
	double mean = 0;
	for(int i = 0; i < size; ++i)
		mean += (double)arr[i].time;

	return mean / size;
}

double getD(struct Pair arr[size]);
double getD(struct Pair arr[size]) {
	double mean = getMean(arr);

	double D = 0;
	for(int i = 0; i < size; ++i) {
		double x = arr[i].time - mean;

		D += (x*x);
	}

	return D / size;
}

uint32_t getMaxByUid(struct Pair arr[size], uint32_t uid);
uint32_t getMaxByUid(struct Pair arr[size], uint32_t uid) {
	uint32_t max = 0;
	for(int i = 0; i < size; ++i) {
		if(arr[i].index == -1)
			continue;

		if(arr[i].uid == uid) {
			arr[i].index = -1;
			if(arr[i].time > max) {
				max = arr[i].time;
			}
		}
	}

	return max;
}

int main(int argc, char** argv) {

	if(argc != 2)
		err(1, "ERROR: params count. 1 expected");

	int fd = openFile(argv[1]);

	struct Pair arr[size];
	for(int i = 0; i < size; ++i)
		arr[i].index = -1;

	int bytes = 0;
	int index = 0;
	while((bytes = read(fd, &d, sizeof(d))) > 0) {
		struct Pair p;
		p.index = index;
		p.uid = d.uid;
		p.time = d.t2 - d.t1;

		arr[index++] = p;
	}
	if(bytes < 0)
		err(5, "ERROR: reading data from file: %s", argv[1]);

	double D = getD(arr);	

	for(int i = 0; i < size; ++i) {
		if(arr[i].index == -1)
			continue;

		uint32_t max = getMaxByUid(arr, arr[i].uid);

		if((max*max) > D)
			if(printf("UID: %d, TIME: %d", arr[i].uid, arr[i].time) == -1)
				err(8, "ERROR: printing formated string to stdout");
	}

	close(fd);
	return 0;
}
