#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

char command[5] = "echo"; 

void execute(char params[2][5]);
void execute(char params[2][5]) {
	pid_t pid = fork();

	if(pid == 0 && params[1][0] != '\0') {
		if(execlp(command, command, params[0], params[1], (char*)NULL) == -1)
			err(4, "ERROR: running execlp with command %s with params: %s, %s", command, params[0], params[1]);
	} else if (pid == 0) {
		if(execlp(command, command, params[0], (char*)NULL) == -1)
			err(4, "ERROR: running execlp with command %s with param: %s", command, params[0]);
	}

	int status;
	wait(&status);

	if(!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		err(3, "ERROR: child process did not exit normaly");
}

int readFromStdin(char params[2][5], int index);
int readFromStdin(char params[2][5], int index) {
	char buf;
	int bytes = 0;
	for(int i = 0; i < 4; ++i) {
		if((bytes = read(0, &buf, sizeof(buf))) < 0)
			err(5, "ERROR: reading from stdint");

		if(bytes == 0 || buf == ' ' || buf == '\n') {
			params[index][i] = '\0';
			return bytes;
		}

		params[index][i] = buf;
	}

	if(read(0, &buf, sizeof(buf)) < 0)
		err(6, "ERROR: reading from stdint");

	if(buf != ' ' && buf != '\n')
		errx(10, "ERROR: strings lengths must be <= 4");

	params[index][4] = '\0';

	return bytes;
}

int main(int argc, char** argv) {
	
	if(argc > 2)
		errx(1, "ERROR: params count");
	if(argc == 2) {
		if(strlen(argv[1]) > 4)
			errx(2, "ERROR: command length must be <= 4");
		strcpy(command, argv[1]);
	}

	char params[2][5];
	params[0][0] = '\0';
	params[0][1] = '\0';

	int bytes = 0;
	int read = 1;
	while(read) {
		read = readFromStdin(params, 0);
		if(read != 0)
			read = readFromStdin(params, 1);
		else
			break;

		execute(params);
	}

	if(bytes < 0)
		err(7, "ERROR: reading from stdin");

	return 0;
}
