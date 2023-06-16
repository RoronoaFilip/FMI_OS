#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count. 1 expected");

	int fd;
	if((fd = open("temp", O_RDONLY)) == -1)
		err(2, "ERROR: opening fifo for read");

	pid_t pid = fork();

	if(pid < 0)
		err(3, "ERROR: running fork()");

	if(pid == 0) {
		dup2(fd, 0);
		close(fd);

		if(execl(argv[1], argv[1], (char*)NULL) == -1)
			err(4, "ERROR: running execl for command: %s", argv[1]);
	}

	if(wait(NULL) == -1)
		err(6, "ERROR: waiting for child to finish");

	close(fd);
	if(unlink("temp") == -1)
		err(5, "ERROR: deleting file temp");
	return 0;
}
