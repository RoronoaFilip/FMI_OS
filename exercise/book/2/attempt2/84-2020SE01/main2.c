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

	unlink("temp");

	dup2(fd, 0);
	close(fd);

	if(execl(argv[1], argv[1], (char*)NULL) == -1)
		err(4, "ERROR: running execl for command: %s", argv[1]);
	return 0;
}
