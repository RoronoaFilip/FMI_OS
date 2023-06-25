#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {

	char arr[20];
	while(true) {
		char c;
		int bytes;
		int i = 0;
		while((bytes = read(0, &c, sizeof(c))) > 0 && c != '\n') {
			arr[i++] = c;
		}
		arr[i] = '\0';

		if(bytes == -1)
			err(1, "ERROR: reading from stdin");

		if(strcmp("exit", arr) == 0)
			break;

		pid_t pid = fork();
		if(pid == 0) {
			execlp(arr, arr, (char*)NULL);
		}

		int status;
		waitpid(pid, &status, 0);

		if(WEXITSTATUS(status) != 0)
			dprintf(2, "ERROR: command %s did not execute correctly", arr);

		printf("\nnext\n\n");
	}

	return 0;
}
