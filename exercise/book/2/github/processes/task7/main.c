#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {

	if(argc == 1)
		errx(1, "ERROR: arg count");

	for(int i = 1; i < argc; ++i) {
		pid_t pid = fork();

		if(pid == -1)
			err(2, "ERROR: forking");

		int status;
		if(pid == 0) {
			execlp(argv[i], argv[i], (char*)NULL);	
		} else {
			wait(&status);

			if(!WIFEXITED(status))
				err(3, "ERROR: child process did not finish normally");
			
			dprintf(1, "PID: %d\n", pid);
			dprintf(1, "EXITSTATUS: %d\n",  WEXITSTATUS(status));
		}
	}

	return 0;
}
