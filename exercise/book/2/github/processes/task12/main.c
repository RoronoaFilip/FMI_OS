#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	setbuf(stdin, NULL);

	if(argc != 4) 
		errx(1, "ERROR: params count");

	int fd;
	if((fd = open(argv[3], O_CREAT | O_WRONLY | O_APPEND, S_IRUSR + S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write: %s", argv[3]);


	pid_t pid = fork();
	if(pid == 0) {
		execlp(argv[1], argv[1], (char*)NULL);
	}

	int status;
	if(waitpid(pid, &status, 0) == -1)
		err(3, "ERROR: waiting for child process with pid: %d executing command: %s", pid, argv[1]);
	if(!WEXITSTATUS(status)) {
		if(write(fd, argv[1], strlen(argv[1])) < 0)
			err(4, "ERROR: writing to file: %s", argv[3]);
	} else
		dprintf(1, "command %s did not execute properly\n", argv[1]);

	pid = fork();
	if(pid == 0) {
		execlp(argv[2], argv[2], (char*)NULL);
	}

	if(waitpid(pid, &status, 0) == -1)
		err(3, "ERROR: waiting for child process with pid: %d executing command: %s", pid, argv[2]);
	if(!WEXITSTATUS(status)) {
		if(write(fd, argv[2], strlen(argv[2])) < 0)
			err(4, "ERROR: writing to file: %s", argv[3]);
	} else 
		dprintf(1, "command %s did not execute properly\n", argv[2]);


	return 0;
}
