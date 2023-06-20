#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");

	char* command = argv[1];

	pid_t pid = fork();
	if(pid == -1)
		err(4, "ERROR: forking");
	if(pid == 0) {
		int fd;
		if((fd = open("temp", O_RDONLY)) == -1)
			err(2, "ERROR: opening pipe for read");
		
		dup2(fd, 0);
		close(fd);
	
		if(execl(command, command, (char*)NULL) == -1)
			err(3, "ERROR: running exec");
	}

	wait(NULL);
	unlink("temp");

	return 0;
}
