#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char** argv) {

	int pf[2];

	if(pipe(pf) == -1)
		errx(1, "ERROR: using pipe()");

	pid_t pid = fork();

	if(pid == 0) {
		printf("CAT");
		close(pf[0]);
		
		close(1);
		if(dup2(pf[1], 1) == -1)
			errx(2, "Redirecting stdout in child");
		close(pf[1]);

		execlp("cat", "cat", "/etc/passwd", (char*)NULL);

		errx(3, "ERROR: calling cat in child");
		return 0;
	}
	waitpid(pid, NULL, 0);

	pid = fork();
	if(pid == 0) {
		close(0); close(1);
		if(dup2(pf[0], 0) == -1 || dup2(pf[1], 1) == -1)
			errx(4, "ERROR: redirecting stdout or stdin in parent");

		close(pf[0]); close(pf[1]);

		execlp("cut", "cut", "-d", ":", "-f", "7", (char*)NULL);

		errx(5, "ERROR: calling cut in child");
	}
	
	waitpid(pid, NULL, 0);	

	pid = fork();
	if(pid == 0) {
		close(0); close(1);
		if(dup2(pf[0], 0) == -1 || dup2(pf[1], 1) == -1)
			errx(4, "ERROR: redirecting stdout or stdin in child");
		close(pf[0]); close(pf[1]);

		execlp("sort", "sort", (char*)NULL);

		errx(5, "ERROR: calling sort in child");
	}
	
	waitpid(pid, NULL, 0);

	pid = fork();
	if(pid == 0) {
		close(0); close(1);
		if(dup2(pf[0], 0) == -1 || dup2(pf[1], 1) == -1)
			errx(6, "ERROR: redirecting stdout or stdin in child");
		close(pf[0]); close(pf[1]);

		execlp("uniq", "uniq", "-c", (char*)NULL);

		errx(5, "ERROR: calling uniq -c in child");
		return 0;
	}

	waitpid(pid, NULL, 0);

	pid = fork();
	if(pid == 0) {
		close(0);
		if(dup2(pf[0], 0) == -1)
			errx(7, "ERROR: redirecting stdout or stdin in child");
		close(pf[0]); close(pf[1]);

		execlp("sort", "sort", (char*)NULL);

		errx(5, "ERROR: calling sort in child 2nd time");
	}

	return 0;
}
