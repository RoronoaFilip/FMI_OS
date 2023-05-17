#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>
int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: args count");

	pid_t pid = fork();
	if(pid == -1)
		errx(2, "ERROR: forking");

	if(pid == 0) {
		execlp(argv[1], argv[1], (char*)NULL);	
	} else {
		int status;
		wait(&status);
		if(!WIFEXITED(status))
			err(3, "ERROR: child did not terminate normally");

		if(write(1, argv[1], strlen(argv[1])))
			err(4, "ERROR: Writing to stdout");
	}


	return 0;
}
