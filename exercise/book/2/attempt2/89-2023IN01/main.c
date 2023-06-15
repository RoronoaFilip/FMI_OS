#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>

const char L[3][4] = {"tic ", "tac ", "toe\n"};
int size = 4;

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 3 expected");

	int nc = atoi(argv[1]);
	int wc = atoi(argv[2]);

	if(nc < 1 || nc > 7)
		errx(2, "ERROR: invalid first param. 1 < NC < 7");
	else if(wc < 1 || wc > 20000)
		errx(3, "ERROR: invalid second param. 1 < WC < 35");

	int pds[8][2];
	
	for(int i = 0; i < nc + 1; ++i)
		if(pipe(pds[i]) == -1)
			err(4, "ERROR: running pipe() for index: %d", i);

	int id = nc;
	pid_t pid = 1234;
	for(int i = 0; i < nc; ++i) {
		pid = fork();

		if(pid < 0)
			err(5, "ERROR: starting process with index: %d", i);

		if(pid == 0) {
			id = i;
			break;
		}
	}

	if(pid > 0) {
		for(int i = 1; i < nc; ++i) {
			close(pds[i][0]);
			close(pds[i][1]);
		}
		close(pds[nc][1]);
		close(pds[0][0]);
	} else {
		for(int i = 0; i < nc + 1; ++i) {
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
	if(id == nc) {
		if(write(1, L[count], size) < 0)
			err(6, "ERROR: writing first word to stdout");

		++count;
		if(write(pds[0][1], &count, sizeof(count)) < 0)
			err(7, "ERROR: writing first number to pipe 1");
	}

	int bytes = 0;
	while((bytes = read(pds[id][0], &count, sizeof(count))) > 0) {
		if(count >= wc)
			break;

		if(write(1, L[count % 3], size) < 0)
			err(9, "ERROR: writing word number: %d to stdout", count);
		
		++count;

		int index = id + 1;
		if(id == nc)
			index = 0;

		if(write(pds[index][1], &count, sizeof(count)) < 0)
			err(7, "ERROR: writing number: %d to pipe %d", count, index);
	}
	if(bytes < 0)
		err(8, "ERROR: reading number from pipe: %d", id);

	if(id == nc) {
		close(pds[id][0]);
		close(pds[0][1]);
	} else {
		close(pds[id][0]);
		close(pds[id+1][1]);
	}

	return 0;
}
