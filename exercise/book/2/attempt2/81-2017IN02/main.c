#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>

int isSeparator(char buf);
int isSeparator(char buf) {
	return buf == ' ' || buf == '\n' || buf == '\0';
}

int readNextInput(char arr[5]);
int readNextInput(char arr[5]) {
	char buf;
	int bytes = 0;
	int index = 0;
	for(int i = 0; i < 4; ++i) {
		if((bytes = read(0, &buf, sizeof(buf))) < 0)
			err(2, "ERROR: reading from stdin");
		
		if(bytes == 0 || isSeparator(buf)) {
			arr[index++] = '\0';
			return bytes;
		}

		arr[index++] = buf;
	}

	if((bytes = read(0, &buf, sizeof(buf))) == -1)
		err(3, "ERROR: reading 4th character from stdin");

	if(!isSeparator(buf))
		errx(4, "ERROR: word with length more then 4 detected");

	arr[index++] = '\0';
	return bytes;
}

int createInput(char arr[2][5]);
int createInput(char arr[2][5]) {

	int val = readNextInput(arr[0]);

	if (val == 0)
		return 0;

	val = readNextInput(arr[1]);

	return val;
}

void execute(const char* cmd, char arr[2][5]);
void execute(const char* cmd, char arr[2][5]) {
	pid_t pid = fork();

	if(pid < 0)
		err(7, "ERROR: running fork ");


	if(pid == 0 && arr[1][0] != '\0')
		if(execlp(cmd, cmd, arr[0], arr[1], (char*)NULL) == -1)
			err(8, "ERROR: running exec in child process");
	
	if(pid == 0)
		if(execlp(cmd, cmd, arr[0], (char*)NULL) == -1)
			err(9, "ERROR: running exec in child process");

	int status;
	wait(&status);

	if(!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		err(10, "ERROR: child process did not exit normally");
}

int main(int argc, char** argv) {
	char arr[5] = "echo";

	char* command = arr;
	if(argc == 2)
		command = argv[1];
	else if(argc > 2)
		errx(1, "ERROR: params count. 0 or 1 expected");

	char args[2][5];
	while(createInput(args)) {
		execute(command, args);
	}

	if(args[0][0] != '\0')
		execute(command, args);

	return 0;
}
