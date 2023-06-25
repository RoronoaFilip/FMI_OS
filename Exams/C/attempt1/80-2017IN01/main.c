#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	int pf[2];
	if(pipe(pf) == -1)
		errx(1, "ERROR: calling pipe() for pf");

	pid_t pid;
	if((pid = fork()) == 0) {
		close(pf[0]);
		
		dup2(pf[1], 1);
		close(pf[1]);

		execlp("cat", "cat", "/etc/passwd", (char*)NULL);

		err(2, "ERROR: running cat");
		return 1;
	}

	close(pf[1]);

	int pf2[2];
	if(pipe(pf2) == -1)
		errx(1, "ERROR: calling pipe() for pf2");

	if((pid = fork()) == 0) {
		close(pf2[0]);
		
		dup2(pf[0], 0);
		close(pf[0]);
		
		dup2(pf2[1], 1);
		close(pf2[1]);

		execlp("cut", "cut", "-d", ":", "-f", "7", (char*)NULL);

		err(3, "ERROR: running  cut -d : -f 7");
		return 3;
	}

	close(pf[0]);
	close(pf2[1]);

	int pf3[2];
	if(pipe(pf3) == -1)
		errx(1, "ERROR: calling pipe() for pf3");

	if((pid = fork()) == 0) {
		close(pf3[0]);

		dup2(pf2[0], 0);
		close(pf2[0]);

		dup2(pf3[1], 1);
		close(pf3[1]);

		execlp("sort", "sort", (char*)NULL);

		err(4, "ERROR: running sort1");
		exit(4);
	}

	close(pf2[0]);
	close(pf3[1]);

	int pf4[2];
	if(pipe(pf4) == -1)
		errx(1, "ERROR: calling pipe() for pf4");

	if((pid = fork()) == 0) {
		close(pf4[0]);

		dup2(pf3[0], 0);
		close(pf3[0]);

		dup2(pf4[1], 1);
		close(pf4[1]);

		execlp("uniq", "uniq", "-c", (char*)NULL);

		err(5, "ERROR: running uniq -c");
		exit(5);
	}

	close(pf3[0]);
	close(pf4[1]);

	if((pid = fork()) == 0) {
		dup2(pf4[0], 0);
		close(pf4[0]);

		execlp("sort", "sort", "-n", (char*)NULL);

		err(6, "ERROR: running sort2");
		return 6;
	}

	close(pf4[0]);

	int status;
	while(wait(&status) > 0) {}

	return 0;
}
