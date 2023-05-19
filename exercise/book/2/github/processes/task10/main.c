#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
int main(int argc, char** argv) {

	if(argc != 3) {
		errx(1, "ERROR: args count");
	}

	pid_t pid = fork();
	if(pid == -1)
		errx(1, "ERROR: starting child process");

	if(pid == 0) {
		execlp(argv[1], argv[1], (char*)NULL);
	}

	int status;
	if(waitpid(pid, &status, 0) == -1)
		err(42, "ERROR: waiting for child process with pid: %d", pid);
	if(WEXITSTATUS(status))
		err(42, "ERROR: child process with pid : %d did not exit normally", pid);

	pid = fork();

	if(pid == 0) {
		execlp(argv[2], argv[2], (char*)NULL);
	}

	if(waitpid(pid, &status, 0) == -1)
		err(42, "ERROR: waiting for child process with pid: %d", pid);
	if(WEXITSTATUS(status))
		err(42, "ERROR: child process with pid: %d did not exit normally", pid);

	return 0;
}
