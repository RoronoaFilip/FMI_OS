#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

char ding[5] = "DING ";
char dong[5] = "DONG\n";

int myRead(int fd, uint8_t* buf, int size, int error, const char* message);
int myRead(int fd, uint8_t* buf, int size, int error, const char* message) {
	int bytes = 0;
	if((bytes = read(fd, buf, size)) != size)
		err(error, message);

	return bytes;
}

int myWrite(int fd, char* buf, int size, int error, const char* message);
int myWrite(int fd, char* buf, int size, int error, const char* message) {
	int bytes;
	if((write(fd, buf, size)) != size)
		err(error, message);

	return bytes;
}

pid_t subprocess(int r[2], int w[2]);
pid_t subprocess(int r[2], int w[2]) {
	if(pipe(r) == -1)
		err(3, "ERROR: running pipe for read");

	if(pipe(w) == -1)	
		err(3, "ERROR: running pipe for write");

	pid_t pid = fork();

	if(pid < 0)
		err(2, "ERROR: starting a subprocess");

	if(pid > 0)
		return pid;
	
	close(w[0]);
	close(r[1]);

	uint8_t buf;
	int bytes = 0;
	uint8_t size = 1;

	while(true) {
		bytes = myRead(r[0], &buf, 1, 4, "ERROR: reading signal from parent");

		if(buf == 0x00)
			break;
		
		bytes = myWrite(1, dong, 5, 6, "ERROR: writing dong in child");

		bytes = myWrite(w[1], (char*)&size, size, 7, "ERROR: sending signal to parent process in child");
	}
	if(bytes < 0)
		err(5, "ERROR: reading from pipe in child process");
	
	close(r[0]);
	close(w[1]);	
	exit(0);
	return pid;
}

int main(int argc, char** argv) {
	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");

	int N = atoi(argv[1]);
	int D = atoi(argv[2]);

	int r[2];
	int w[2];

	pid_t pid = subprocess(w, r);

	uint8_t buf = 0;
	int bytes = 0;
	uint8_t size = 1;
	for(int i = 0; i < N; ++i) {
		bytes = myWrite(1, ding, 5, 9, "ERROR: writing ding to stdout in parent");
		
		bytes = myWrite(w[1], (char*)&size, size, 10, "ERROR: sending singnal to child process from parent");
	
		bytes = myRead(r[0], &buf, size, 11, "ERROR: reading signal from child process in parent");

		sleep(D);
	}
	
	char end = 0x00;
	myWrite(w[1], &end, size, 12, "ERROR: sending final signal to child");

	if(waitpid(pid, NULL, 0) == -1)
		err(12, "ERROR: waiting for child to finish&& ");

	close(r[0]);
	close(w[1]);
	return 0;
}
