#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdint.h>

uint8_t ignore = 0x55;
uint8_t escape = 0x7D;

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

		if(execlp("xxd", "xxd", "-p", argv[1], (char*)NULL))
	}

	return 0;
}
