#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int myRead(char param[5]);
int myRead(char param[5]) {
	int bytes = 0;
	int totalBytes = 0;
	char buf;
	int index = 0;
	for(int i = 0; i < 4; ++i) {
		if((bytes = read(0, &buf, sizeof(buf))) == -1)
			err(3, "ERROR: reading from stdint");

		if(bytes == 0 || buf == '\n' || buf == '\0' || buf == ' ') {
			param[i] = '\0';
			return bytes;
		}

		totalBytes += bytes;

		param[index++] = buf;
	}

	if((bytes = read(0, &buf, sizeof(buf))) == -1)
		err(4, "ERROR: reading from stdint");

	
	if(bytes != 0 && buf != '\n' && buf != '\0' && buf != ' ')
		errx(5, "ERROR: string with len > 4 detected");

	totalBytes += bytes;
	param[index++] = '\0';

	return totalBytes;
}

int getParams(char params[2][5]);
int getParams(char params[2][5]) {
	int bytes = myRead(params[0]);

	if(bytes == 0)
		return 0;

	bytes = myRead(params[1]);

	return bytes;
}

void execute(char* cmd, char params[2][5]);
void execute(char* cmd, char params[2][5]) {
	pid_t pid = fork();

	if(pid < 0)
		err(6, "ERROR: starting a child process");

	if(pid == 0) {
		if(params[1][0] != '\0') {
			if(execlp(cmd, cmd, params[0], params[1], (char*)NULL) == -1)
					err(8, "ERROR: running exec");
		} else {
			if(execlp(cmd, cmd, params[0], (char*)NULL) == -1)
				err(8, "ERROR: running exec");
		}

	}

	int status;
	if(wait(&status) == -1)
		err(7, "ERROR: waiting for child to finish");

	if(!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		errx(8, "ERROR: child did not finish normally");
	return;
}

int main(int argc, char** argv) {
	char command[5] = "echo\0";

	char* cmd = command;

	if(argc > 2)
		errx(1, "ERROR: params count. 0 or 1 expected");

	if(argc == 2) {
		if(strlen(argv[1]) > 4)
			errx(2, "ERROR: the command passed i must be of length <= 4");
		else 
			cmd = argv[1];
	}

	char params[2][5];

	while(1) {
		int bytes = getParams(params);

		if(params[0][0] == '\0')
			break;

		execute(cmd, params);

		if(bytes == 0)
			break;
	}

	return 0;
}
