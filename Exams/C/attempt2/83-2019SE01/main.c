#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>

bool condition(time_t* start, time_t* end, int* seconds);
bool condition(time_t* start, time_t* end, int* seconds) {
	return (*end - *start) < *seconds;
}

int execute(char* args[], int fd, int* counter, int* seconds);
int execute(char* args[], int fd, int* counter, int* seconds) {
	pid_t pid = fork();

	if(pid < 0)
		err(2, "ERROR: starting a child process");

	if(pid == 0)
		if(execvp(args[0], args) == -1)
			err(3, "ERROR: executing command: %s", args[0]);
	
	time_t start = time(NULL);

	int status;
	wait(&status);

	time_t end = time(NULL);

	int exitStatus;
	if(WIFEXITED(status))
		exitStatus = WEXITSTATUS(status);
	else 
		exitStatus = 129;

	dprintf(fd, "Start: %ld, End: %ld, Status: %d\n", start, end, exitStatus);

	if(exitStatus != 0 && condition(&start, &end, seconds)) {
		if(*counter == 0) {
			*counter = 1;
		} else {
			return -1;
		}
	} else {
		*counter = 0;
	}	

	return 0;
}

int main(int argc, char** argv) {
	
	if(argc < 3)
		err(1, "ERROR: params count. Atleast 2 expected");

	int seconds = atoi(argv[1]);

	if(seconds < 1 || seconds > 9)
		errx(1, "ERROR: first param must be 1-9");
	
	char** args = argv + 2;

	int fd;
	if((fd = open("file.log", O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1)
		err(2, "ERROR: creating file.log for write and append");

	int counter = 0;
	while(execute(args, fd, &counter, &seconds) != -1) {}

	close(fd);
	return 0;
}
