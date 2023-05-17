#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");

	pid_t pid = fork();
	if(pid == -1)
		err(2, "ERROR: forking");

	if(pid == 0) {
		int fd;
		if((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR + S_IWUSR)) == -1)
			err(3, "ERROR: opening file for read in chiild: %s", argv[1]);

		char* str = "fooba";
		if(write(fd, str, strlen(str)) == -1)
			err(4, "ERROR: writing to file in child: %s", argv[1]);

		close(fd);
	} else {
		int status;
		wait(&status);
		
		if(!WIFEXITED(status))
			err(5, "ERROR: child process did not exit normally");

		int fd;
		if((fd = open(argv[1], O_RDONLY)) == -1)
			err(6, "ERROR: opening file for read in parent: %s", argv[1]);

		char buf[2];
		int bytes;
		while((bytes = read(fd, buf, 2)) > 0) {
			if(write(1, buf, bytes) == -1)
				err(7, "ERROR: writing to stdout in parent");
			if(write(1, " ", 1) == -1)
				err(7, "ERROR: writing to stdout in parent");
		}
		if(bytes != 0)
			err(8, "ERROR: reading from file int parent: %s", argv[1]);

		if(write(1, "\n", 1) == -1)
			err(7, "ERROR: writing to stdout in parent");
		
		close(fd);
	}

	return 0;
}
