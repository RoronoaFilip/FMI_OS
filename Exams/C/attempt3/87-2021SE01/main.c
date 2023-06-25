#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

int openLog(void);
int openLog(void) {
	int fd;
	if((fd = open("file.log", O_CREAT | O_APPEND | O_WRONLY, 0644)) == -1)
		err(2, "ERROR: opening log file");

	return fd;
}

int appendToFile(int fd, const char* arr);
int appendToFile(int fd, const char* arr) {
	int bytes = 0;
	int len = strlen(arr);
	if((bytes = write(fd, arr, len)) != len)
		err(3, "ERROR: writing to file");

	return bytes;
}

int ps(const char* username);
int ps(const char* username) {
	int p[2];
	if(pipe(p) == -1)
		err(6, "ERROR: running pipe");

	pid_t pid = fork();

	if(pid < 0)
		err(7, "ERROR: running fork");

	if(pid == 0) {
		close(p[0]);
		dup2(p[1], 1);
		close(p[1]);

		if(execlp("ps", "ps", "-u", username, "-o", "pid=", (char*)NULL) == -1)
			err(8, "ERROR: running ps command");
	}

	close(p[1]);

	return p[0];
}

int readNextPid(int fd, char buf[30]);
int readNextPid(int fd, char buf[30]) {
	int bytes = 0;
	int totalBytes = 0;
	char ch;
	int index = 0;
	while((bytes = read(fd, &ch, sizeof(ch))) > 0) {
		if(ch == ' ')
			continue;
		if(ch == '\n' || ch == '\0')
			break;

		totalBytes += bytes;
		buf[index++] = ch;
	}
	if(bytes == -1)
		err(9, "ERROR: reading next pid");
	
	buf[index++] = '\0';

	if(bytes == 0)
		return 0;

	return totalBytes + 1;
}

void killProcess(char buf[30]);
void killProcess(char buf[30]) {	
	pid_t pid = fork();

	if(pid < 0)
		err(7, "ERROR: running fork to kill process with pid: %s", buf);

	if(pid == 0)
		 if(execlp("kill", "kill", "-KILL", buf, (char*)NULL) == -1)
			 err(10, "ERROR: killing process with pid: %s", buf);
}

void lockUser(const char* username);
void lockUser(const char* username) {
	pid_t pid = fork();

	if(pid < 0)
		err(7, "ERROR: running fork to lock user");

	if(pid == 0)
		 if(execlp("passwd", "passwd", "-l", username, (char*)NULL) == -1)
			 err(11, "ERROR: locking user with username: %s", username);
}

int main(int argc, char** argv) {
	
	int logFd = openLog();

	struct timeval tv;
	if(gettimeofday(&tv, NULL) == -1)
		err(4, "ERROR: running gettimeofday");
	
	struct tm* tm = localtime(&tv.tv_sec);
	if(tm == NULL)
		err(5, "ERROR: running localtime");
	
	char buf[30];
	strftime(buf, 30, "%F %T.", tm);

	uid_t id = getuid();
	struct passwd* pw = getpwuid(id);

	printf("UID: %d\nUSERNAME: %s\n", id, pw->pw_name);

	appendToFile(logFd, buf);
	dprintf(logFd, "%ld ", tv.tv_usec);
	appendToFile(logFd, pw->pw_name);
	appendToFile(logFd, " \0");

	for(int i = 0; i < argc; ++i) {
		appendToFile(logFd, argv[i]);
		appendToFile(logFd, " \0");
	}

	appendToFile(logFd, "\n\0");

	//lockUser(pw->pw.name);

	int p = ps(pw->pw_name);	

	while(readNextPid(p, buf) > 0) {
		printf("PID: %s\n", buf);
		//killProcess(buf);
	}

	close(logFd);
	close(p);
	return 0;
}
