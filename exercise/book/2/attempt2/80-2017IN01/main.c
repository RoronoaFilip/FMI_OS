#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	
	if(argc != 1)
		errx(1, "ERROR: params count. 0 expected");

	int p[2];
	if(pipe(p) == -1)
		err(2, "ERROR: running pipe for p1");
	
	pid_t pid = fork();
	if(pid < 0)
		err(3, "ERROR: running fork for cat command");

	if(pid == 0) {
		close(p[0]);

		dup2(p[1], 1);
		close(p[1]);

		execlp("cat", "cat", "/etc/passwd", (char*)NULL);

		err(4, "ERROR: running cat command");
		exit(4);
	}

	close(p[1]);

	int p2[2];
	if(pipe(p2) == -1)
		err(2, "ERROR: running pipe for p2");
	
	pid = fork();
	if(pid < 0)
		err(3, "ERROR: running fork for cut command");

	if(pid == 0) {
		close(p2[0]);

		dup2(p[0], 0);
		close(p[0]);

		dup2(p2[1], 1);
		close(p2[1]);

		execlp("cut", "cut", "-d", ":", "-f", "7", (char*)NULL);	
		
		err(5, "ERROR: running cut -d: -f 7");
	}

	close(p[0]);
	close(p2[1]);
	
	int p3[2];
	if(pipe(p3) == -1)
		err(2, "ERROR: running pipe for p3");

	pid = fork();
	if(pid < 0)
		err(3, "ERROR: running fork for sort command");

	if(pid == 0) {
		close(p3[0]);

		dup2(p2[0], 0);
		close(p2[0]);

		dup2(p3[1], 1);
		close(p3[1]);

		execlp("sort", "sort", (char*)NULL);	
		
		err(5, "ERROR: running sort");
	}
	
	close(p2[0]);
	close(p3[1]);

	int p4[2];
	if(pipe(p4) == -1)
		err(2, "ERROR: running pipe for p4");


	pid = fork();
	if(pid < 0)
		err(3, "ERROR: running fork for uniq command");

	if(pid == 0) {
		close(p4[0]);

		dup2(p3[0], 0);
		close(p3[0]);

		dup2(p4[1], 1);
		close(p4[1]);

		execlp("uniq", "uniq", "-c", (char*)NULL);	
		
		err(5, "ERROR: running uniq -c");
		exit(5);
	}
	
	close(p3[0]);
	close(p4[1]);

	pid = fork();
	if(pid < 0)
		err(3, "ERROR: running fork for sort -n command");

	if(pid == 0) {
		dup2(p4[0], 0);
		close(p4[0]);

		execlp("sort", "sort", "-n", (char*)NULL);	
		
		err(5, "ERROR: running sort -n");
		exit (5);
	}

	while(wait(NULL) > 0) {}

	close(p4[0]);
	return 0;
}
