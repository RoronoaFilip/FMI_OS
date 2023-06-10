#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>

struct Spaces {
	uint8_t s1,s2,s3;
};

struct Spaces s;

int openLog(void);
int openLog(void) {
	int fd;
	if((fd = open("foo.log", O_CREAT | O_APPEND | O_WRONLY, 0644)) == -1)
		err(1, "ERROR: opening foo.log for write with append");

	return fd;
}

void appendToFile(int fd, const char* str, int error, const char* message);
void appendToFile(int fd, const char* str, int error, const char* message) {
	int len = strlen(str);
	if(write(fd, str, len) != len)
		err(error, message);

}

int ps(const char* username);
int ps(const char* username) {
	int p[2];
	if(pipe(p) == -1)
		err(9, "ERROR: running pipe() on p");
	
	pid_t pid = fork();
	if(pid < 0)
		err(10, "ERROR: running fork()");
	
	if(pid == 0) {
		close(p[0]);
		dup2(p[1], 1);
		close(p[1]);
	
		if(execlp("ps", "ps", "-u", username, "-o", "pid=", (char*)NULL) == -1)
			err(11, "ERROR: running ps -u %s -o pid=", username);
	}

	close(p[1]);
	return p[0];
}

int readNextPid(int fd, char pid[30]);
int readNextPid(int fd, char pid[30]) {
	int readBytes = 0;
	int bytes = 0;

	if((bytes = read(fd, &s, sizeof(s))) == -1)
		err(12, "ERROR: reading spaces from pipe");
	
	if(bytes == 0)
		return bytes;

	readBytes += bytes;

	char buf;
	int index = 0;
	while((bytes = read(fd, &buf, sizeof(buf))) > 0) {
		if(buf == '\n')
			break;

		pid[index++] = buf;
	}

	pid[index++] = '\0';

	if(bytes == 0)
		return bytes;
	
	readBytes += index;

	return readBytes;
}

void myKill(char pid[30]);
void myKill(char pid[30]) {
	pid_t PID = fork();
	if(PID < 0)
		err(17, "ERROR: running fork for killing a process");
	
	if(PID == 0) {
		if(execlp("kill", "kill", "-KILL", pid, (char*)NULL) == -1)
			err(18, "ERROR: killing process with pid: %s", pid);
	}
}

void lockUser(const char* username);
void lockUser(const char* username) {
	pid_t pid = fork();

	if(pid < 0)
		err(20, "ERROR: running fork for locking user");

	if(pid == 0) {
		if(execlp("passwd", "passwd", "-l", username, (char*)NULL) == -1)
			err(21, "ERROR: running passwd -l %s", username);
	}
}

int main(int argc, char** argv) {
	time_t rawTime;
	rawTime = time(&rawTime);

	struct tm* tm = localtime(&rawTime);
	
	char buf[100];
	strftime(buf, 100, "%F %T ", tm);

	int fd = openLog();
	appendToFile(fd, buf, 2, "ERROR: writing time to log file");	
	
	uid_t id = getuid();
	struct passwd* pw = getpwuid(id);	

	appendToFile(fd, pw->pw_name, 3, "ERROR: writing username to logFile");
	appendToFile(fd, " ", 4, "ERROR: writing space to log file");

	for(int i = 0; i < argc; ++i) {
		appendToFile(fd, argv[i], 5, "ERROR: appending arguments to log file");
		appendToFile(fd, " ", 6, "ERROR: appending a space to log file");
	}
	appendToFile(fd, "\n", 7, "ERROR: appending a new line to log file");

	int p = ps(pw->pw_name);

	char pid[30];
	int bytes = 0;
	while((bytes = readNextPid(p, pid)) > 0) {
		if(bytes != 8)
			err(16, "ERROR: invalid number of read bytes from file: must be 8 was %d", bytes);
		printf("%s\n", pid);	
		//myKill(pid);
	}

	//lockUser(pw->pw_name);

	close(fd);
	close(p);
	return 0;
}
