#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count");

	pid_t pid1, pid2, firstPid;
	
	pid1 = fork();
	if(pid1 == 0) {
		execlp(argv[1], argv[1], (char*)NULL);
	}

	pid2 = fork();
	if(pid2 == 0) {
		execlp(argv[2], argv[2], (char*)NULL);
	}

	int status;
	firstPid = wait(&status);

	setbuf(stdout, NULL);
	printf("First finished pid: %d\n", firstPid);
	return 0;
}
