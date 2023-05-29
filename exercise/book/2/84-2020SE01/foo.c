#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");

	char* fileName = argv[1];
	
	if(mkfifo("temp", S_IRUSR | S_IWUSR) == -1)	
		err(4, "ERROR: Running mkfifo");

	int fd;
	if((fd = open("temp", O_WRONLY)) == -1)
		err(5, "ERROR: opening named pipe for write");

	dup2(fd, 1);

	if(execlp("cat", "cat", fileName, (char*)NULL) == -1)
		err(2, "ERROR: running execlp");

	return 0;
}
