#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>

void execute(const char* command);
void execute(const char* command) {
	pid_t pid = fork();

	if(pid < 0)
		err(2, "ERROR: starting a child process");

	if(pid == 0)
		if(execlp(command, command, (char*)NULL) == -1)
			err(3, "ERROR: runnign command: %s", command);

	if(wait(NULL) == -1)
		err(4, "ERROR: waiting for child to finish");
}

int main(int argc, char** argv) {

	if(argc != 1)
		errx(1, "ERROR: params count. None expected");

	char buf[100];
	char ch;
	int bytes = 0;
	while(1) {
		if(write(1, "Enter Command:\n", 15) != 15)
			err(5, "ERROR: writing prompt to stdout");
		
		int index = 0;
		while((bytes = read(0, &ch, sizeof(ch))) > 0) {
			if(ch == '\0' || ch == '\n') {
				buf[index++] = '\0';
				break;
			}
			
			buf[index++] = ch;
		}
		if(bytes < 0)
			err(6, "ERROR: reading from stdin");

		if(strcmp(buf, "exit") == 0)
			break;

		execute(buf);
	}

	return 0;
}
