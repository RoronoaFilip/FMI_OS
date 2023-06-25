#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <sys/wait.h>

char ding[5] = "DING ";
char dong[5] = "DONG\n";
int size = 5;

pid_t startChild(int* rd, int* wr);
pid_t startChild(int* rd, int* wr) {
	int r[2];
	int w[2];

	if(pipe(r) == -1)
		err(2, "ERROR: running pipe for 1st pipe");
	if(pipe(w) == -1)
		err(3, "ERROR: running pipe for 2nd pipe");

	pid_t pid = fork();

	if(pid < 0)
		err(4, "ERROR: starting a child process");

	if(pid == 0) {
		close(r[1]);
		close(w[0]);

		int R = r[0];
		int W = w[1];

		uint8_t buf = 0;
		int bytes = 0;
		while((bytes = read(R, &buf, sizeof(buf))) > 0) {
			if(buf == 0x00)
				break;

			if(write(1, dong, size) != size)
				err(6, "ERROR: writing dong to stdout");

			uint8_t a = 0x01;
			if(write(W, &a, sizeof(a)) != sizeof(a))
				err(7, "ERROR: signaling parent to continue");
		}
		if(bytes < 0)
			err(5, "ERROR: reading from pipe");

		close(R);
		close(W);
		exit(0);
	}
	
	close(w[1]);
	close(r[0]);
	*rd = w[0];
	*wr = r[1];

	return pid;
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count, 2 expected");
	
	int n = atoi(argv[1]);
	int d = atoi(argv[2]);

	int r;
	int w;
	
	pid_t pid = startChild(&r, &w);

	uint8_t a = 0x01;
	uint8_t buf;
	for(int i = 0; i < n; ++i) {
		if(write(1, ding, size) != size)
			err(8, "ERROR: writing ding to stdout");

		
		if(write(w, &a, sizeof(a)) != sizeof(a))
			err(9, "ERROR: signaling child to write dong");

		if(read(r, &buf, sizeof(buf)) != sizeof(buf))
			err(10, "ERROR: accepting signal from child");

		sleep(d);
	}

	a = 0x00;
	if(write(w, &a, sizeof(a)) != sizeof(a))
		err(11, "ERROR: singaling child to stop");
	
	if(wait(NULL) == -1)
		err(12, "ERROR: waiting for child to finish");

	close(r);
	close(w);
	return 0;
}
