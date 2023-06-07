#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const int SIZE = 3;
const char L[3][4] = { "tic ", "tac ", "toe\n" };
const int LEN = 4;

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count");
	
	int NC = atoi(argv[1]);
	int WC = atoi(argv[2]);

	if(NC < 1 || NC > 7)
		errx(2, "ERROR: NC must be 1<NC<7");
	if(WC < 1 || WC > 1000)
		errx(2, "ERROR: WC must be 1<WC<35");

	int pds[8][2];
	for(int i = 0; i < NC + 1; ++i)
		if(pipe(pds[i]) == -1)
			err(3, "ERROR: running pipe()");

	int id = NC;
	pid_t pid = 1234;
	for(int i = 0; i < NC; ++i) {
		pid = fork();
		
		if(pid == -1)
			err(4, "ERROR: running fork() on iteration: %d", i);
	
		if(pid == 0) {
			id = i;
			break;
		}
	}

	if(pid > 0) {
		for(int i = 1; i < NC; ++i) {
			close(pds[i][0]);
			close(pds[i][1]);
		}
		close(pds[NC][1]);
		close(pds[0][0]);
	} else {
		for(int i = 0; i < NC + 1; ++i) {
			if(id == i) {
				close(pds[i][1]);
				close(pds[i+1][0]);
				++i;
			} else {
				close(pds[i][0]);
				close(pds[i][1]);
			}
		}
	}

	int count = 0;
	if(pid > 0) {
		if(write(1, L[count], LEN) == -1)
			err(4, "ERROR: writing first word");

		++count;
		if(write(pds[0][1], &count, sizeof(count)) == -1)
			err(5, "ERROR: writing initial number");
	}

	int bytes = 0;
	while((bytes = read(pds[id][0], &count, sizeof(count))) > 0) {
		if(count == WC)
			break;

		if(write(1, L[count % SIZE], LEN) == -1)
			err(4, "ERROR: writing word number: %d in child: %d", count, id);

		++count;
		int index = 0;
		if(pid == 0)
			index = id+1;

		if(write(pds[index][1], &count, sizeof(count)) == -1)
			err(5, "ERROR: writing number: %d int child: %d", count, id);
	}
	if(bytes < 0)
		err(6, "ERROR: reading from pipes when count is %d", count);

	if(pid > 0) {
		close(pds[NC][0]);
		close(pds[0][1]);
	} else {
		close(pds[id][0]);
		close(pds[id+1][1]);
	}

	return 0;
}
