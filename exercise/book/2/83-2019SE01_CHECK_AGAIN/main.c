#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

bool condition(int seconds, time_t start, time_t end);

bool condition(int seconds, time_t start, time_t end) {
	return (end-start) < seconds;
}

int main(int argc, char** argv) {

	if(argc < 3)		
		errx(1, "ERROR: params count");

	int seconds = atoi(argv[1]);
	char* Q = argv[2];
	char** args = argv + 2;

	int fd;
	if((fd = open("run.log", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) == -1)
		err(2, "ERROR: opening run.log for write");

	int counter = 0;
	while(true) {
		pid_t pid = fork();
		if(pid == -1)
			err(3, "ERROR: running fork()");

		if(pid == 0) {
			if(execvp(Q, args) == -1)
				err(4, "ERROR: running execvp in child");
		} else {
			time_t timeStart = time(NULL);
			

			int status;
			if(wait(&status) == -1)
				err(5, "ERROR: running wait in parent");

			time_t timeEnd = time(NULL);

			int exitStatus;
			if(WIFEXITED(status)) {
				exitStatus = WEXITSTATUS(status);
			} else {
				exitStatus = 129;

			}

			dprintf(fd, "%ld %ld %d\n", timeStart, timeEnd, exitStatus);

			if(exitStatus != 0 && condition(seconds, timeStart, timeEnd)) {
				if(counter == 1) {
					break;
				} else {
					counter = 1;
				}
			} else {
				counter = 0;
			}

		}
	}

	return 0;
}
