#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdint.h>

uint8_t ignore = 0x55;
uint8_t escape = 0x7D;
uint8_t xor = 0x20;

int main(int argc, char** argv) {
	
	if(argc != 3)
		errx(1, "ERROR: params count");

	int pd[2];
	if(pipe(pd) == -1)
		errx(2, "ERROR: running pipe()");

	pid_t pid = fork();
	if(pid == -1)
		err(3, "ERROR: running fork()");
	else if(pid == 0){
		close(pd[0]);

		close(1);
		dup2(pd[1], 1);
		close(pd[1]);

		if(execlp("cat", "cat",  argv[1], (char*)NULL) == -1)
			err(4, "ERROR: running cat in child");
	}

	close(pd[1]);

	int fd2;
	if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(6, "ERROR: opening file 2 for write: %s", argv[2]);

	uint8_t buf;
	int bytes = 0;
	while((bytes = read(pd[0], &buf, sizeof(buf))) > 0) {
		if(buf == ignore)
			continue;

		if(buf == escape) {
			bytes = read(pd[0], &buf, sizeof(buf));

			if(bytes < 0)
				err(7, "ERROR: reading from file 1; %s", argv[1]);

			buf = buf ^ xor;
		}

		int writtenBytes = 0;
		if((writtenBytes = write(fd2, &buf, sizeof(buf))) < 0)
			err(8, "ERROR: writing to file 2: %s", argv[2]);

		if(writtenBytes != bytes)
			errx(9, "ERROR: not all read bytes were written");
	}
	if(bytes < 0)
		err(7, "ERROR: reading from file 1; %s", argv[1]);
	
	close(pd[0]);
	return 0;
}
