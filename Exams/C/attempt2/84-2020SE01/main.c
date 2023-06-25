#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count. 1 expected");

	if(mkfifo("temp", 0644) == -1)
		err(2, "ERROR: running mkfifo ");

	int fd;
	if((fd = open("temp", O_WRONLY)) == -1)
		err(3, "ERROR: opening fifo for write");

	dup2(fd, 1);
	close(fd);

	if(execlp("cat", "cat", argv[1], (char*)NULL) == -1)
		err(4, "ERROR: running exec for cat command");
	return 0;
}
