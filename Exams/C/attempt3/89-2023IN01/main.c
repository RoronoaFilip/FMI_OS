#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdlib.h>

const char L[3][4] = { "tic ", "tac ", "toe\n" };

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int nc = atoi(argv[1]);
	int wc = atoi(argv[2]);

	if(nc < 1 || nc > 100)
		errx(2, "ERROR: first param must be 1<nc<7");
	
	int pd[100][2];

	for(int i = 0; i < nc + 1; ++i)
		if(pipe(pd[i]) == -1)
			err(3, "ERROR: running pipe for index: %d", i);
	
	pid_t pid = 1234;
	int id = nc;
	for(int i = 0; i < nc; ++i) {
		pid = fork();

		if(pid < 0)
			err(4, "ERROR: running fork()");

		if(pid == 0) {
			id = i;
			break;
		} 
	}

	if(pid > 0) {
		for(int i = 1; i < nc; ++i) {
			close(pd[i][0]);
			close(pd[i][1]);
		}
		close(pd[id][1]);
		close(pd[0][0]);
	} else {
		for(int i = 0; i < nc + 1; ++i) {
			if(i == id) {
				close(pd[id][1]);
				close(pd[id + 1][0]);
				++i;
			} else {
				close(pd[i][0]);
				close(pd[i][1]);
			}
		}	
	}

	int index = 0;
	if(pid > 0) {
		if(write(1, L[index], 4) != 4)
			err(12, "ERROR: writing first word to stdout");

		++index;

		if(write(pd[0][1], &index, sizeof(index)) != sizeof(index))
			err(13, "ERROR: signaling first child to continue");
	}

	while(read(pd[id][0], &index, sizeof(index)) > 0) {
		if(index == wc)
			break;

		if(write(1, L[index % 3], 4) != 4)
			err(14, "ERROR: writing %d word to stdout", index);

		++index;

		int to = id+1;
		if(id == nc)
			to = 0;

		if(write(pd[to][1], &index, sizeof(index)) != sizeof(index))
			err(14, "ERROR: signaling next child to continue");
	}

	if(id == nc) {
		close(pd[nc][0]);
		close(pd[0][1]);
	} else {
		close(pd[id][0]);
		close(pd[id+1][1]);
	}

	return 0;
}
