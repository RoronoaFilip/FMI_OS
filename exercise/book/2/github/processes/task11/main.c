#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdio.h>

int main(int argc, char** argv) {

	int success = 0;
	int fail = 0;
	for(int i = 1; i < argc; ++i) {
		pid_t pid = fork();
		if(pid == -1)
			errx(1, "ERROR: starting a child process");

		if(pid == 0) {
			if(execlp(argv[i], argv[i], (char*)NULL) == -1)
				return 1;
		}

		int status;
		if(waitpid(pid, &status, 0) == -1)
			err(2, "ERROR: waiting for child process with pid: %d", pid);
		if(!WEXITSTATUS(status))
			++success;
		else
			++fail;
	}	

	dprintf(1, "Successful: %d\nFailed:%d\n", success, fail);

	return 0;
}
