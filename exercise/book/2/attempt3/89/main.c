#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>

char L[3][4] = { "tic ", "tac ", "toe\n" };

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int nc = atoi(argv[1]);
	int wc = atoi(argv[2]);

	if(nc < 1 || nc > 7)
		errx(2, "ERROR: nc must be 1<nc<7");
	if(wc < 1 || wc > 10000000)
		errx(3, "ERROR wc must be 1<wc<35");

	int pds[8][2];
	for(int i = 0; i < nc + 1; ++i)
		if(pipe(pds[i]) == -1)
			err(4, "ERROR: running pipe for pipe num: %d", i);

	int id = nc;
	pid_t pid = 12345;
	for(int i = 0; i < nc; ++i) {
		pid = fork();

		if(pid < 0)
			err(5, "ERROR: running fork()");

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
	} else if (pid == 0) {
			for(int i = 0; i < nc + 1; ++i) {
				if(i == id) {
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
		if(write(1, L[count], 4) != 4)
		err(6, "ERROR: writing first word number: %d to stdou", count);

		++count;

		if(write(pds[0][1], &count, sizeof(count)) != sizeof(count))
			err(7, "ERROR: sending first signal to process id: %d count: %d", 0, count);
	}

	int bytes = 0;
	while((bytes = read(pds[id][0], &count, sizeof(count))) > 0) {
		if(count >= wc)
			break;

		if(write(1, L[count % 3], 4) != 4)
		err(6, "ERROR: writing word number: %d to stdou", count);

		++count;

		int index = id+1;
		if(id == nc)
			index = 0;

		if(write(pds[index][1], &count, sizeof(count)) != sizeof(count))
			err(7, "ERROR: sending signal to process id: %d count: %d", index, count);
	}
	if(bytes < 0)
		err(10, "ERROR: reading count from pipe in process with id: %d", id);


	if(id == nc) {
		close(pds[0][1]);
		close(pds[nc][0]);
	} else {
		close(pds[id][0]);
		close(pds[id+1][1]);
	}
	return 0;
}
