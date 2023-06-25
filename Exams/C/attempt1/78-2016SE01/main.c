#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");

	int pf[2];
	if(pipe(pf) == -1)
		errx(2, "ERROR: using pipe()");

	pid_t pid = fork();

	if(pid == 0) {
		close(pf[0]);

		close(1);
		if(dup2(pf[1], 1) == -1)
			err(6, "ERROR: redirecting stdout in child");
		close(pf[1]);

		execlp("cat", "cat", argv[1], (char*)NULL);

		err(3, "ERROR: executing cat in child for file: %s", argv[1]);
		exit(1);
	}

	close(pf[1]);

	close(0);
	if(dup2(pf[0], 0) == -1)
		err(7, "ERROR: redirecting stdin in parent");
	close(pf[0]);
	
	execlp("sort", "sort", (char*)NULL);

	err(4, "ERROR: executing sort in parent");

	return 1;
}
