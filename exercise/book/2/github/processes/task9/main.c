#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {

	if(argc != 1)
		errx(1, "ERROR: params count");

	int fd;
	if((fd =open("file", O_CREAT | O_RDWR | O_TRUNC, S_IWUSR + S_IRUSR)) == -1)
		err(2, "ERROR: opening file: file");

	pid_t pid = fork();

	if(pid == -1)
		err(1, "ERROR: starting child process");

	if(pid == 0) {
		if(write(fd, "foo", 3) == -1)
			err(3, "ERROR: writing to file in child 1");
		exit(0);
	}

	int pid2 = fork();
	if(pid2 == -1)
		err(1, "ERROR: starting child process");

	if(pid2 == 0) {
		int status;
		waitpid(pid, &status, 0);

		if(!WIFEXITED(status))
			err(5, "ERROR: child 1 did not finish normally");

		if(write(fd, "bar", 3) == -1)
			err(3, "ERROR: writing to file in child 2");
		exit(0);
	}

	dprintf(1, "Program executed corectly\n");

	return 0;
}
