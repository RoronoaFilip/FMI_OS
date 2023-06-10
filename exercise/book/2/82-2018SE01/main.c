#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/wait.h>

void myPipe(int p[2], int error, const char* message);
void myPipe(int p[2], int error, const char* message) {
	if(pipe(p) == -1)
		err(error, message);
}

pid_t myFork(int error, const char* message);
pid_t myFork(int error, const char* message) {
	pid_t pid = fork();

	if(pid == -1)
		err(error, message);
	
	return pid;
}

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count. 1 folder expected");
	
	int p1[2];
	myPipe(p1, 2, "ERROR: running pipe on p1");

	pid_t pid = myFork(3, "ERROR: running fork() for p1"); 
	
	if(pid == 0) {
		close(p1[0]);
		
		dup2(p1[1], 1);
		close(p1[1]);

		if(execlp("find", "find", argv[1], "-type", "f", "-printf", "%T@ %p\n", (char*)NULL) == -1)
			err(4, "ERROR: running find command");
	}

	close(p1[1]);
	
	int p2[2];
	myPipe(p2, 5, "ERROR: running pipe() for p2");	

	pid = myFork(6, "ERROR: running fork() for p2");

	if(pid == 0) {
		close(p2[0]);
		
		dup2(p1[0], 0);
		close(p1[0]);

		dup2(p2[1], 1);
		close(p2[1]);

		if(execlp("sort", "sort", "-r", "-n", (char*)NULL)  == -1)
			err(7, "ERROR: running sort command");
	}

	close(p1[0]);
	close(p2[1]);

	int p3[2];
	myPipe(p3, 8, "ERROR: running pipe() on p3");	

	pid = myFork(9, "ERROR: running fork() for p3");

	if(pid == 0) {
		close(p3[0]);

		dup2(p2[0], 0);
		close(p2[0]);

		dup2(p3[1], 1);
		close(p3[1]);

		if(execlp("head", "head", "-n", "1", (char*)NULL) == -1)
			err(10, "ERROR: running head -n 1");
	}

	close(p2[0]);
	close(p3[1]);

	pid = myFork(11, "ERROR: running last fork()");

	if(pid == 0) {
		dup2(p3[0], 0);
		close(p3[0]);
		
		if(execlp("cut", "cut", "-d", " ", "-f", "2-", (char*)NULL) == -1)
			err(12, "ERROR: running cut command");
	}

	close(p3[0]);

	while(wait(NULL) > 0) {}

	return 0;
}
