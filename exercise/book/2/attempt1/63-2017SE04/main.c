#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>

int main(int argc, char** argv) {
	bool isStdin = false;

	if(argc == 1)
		isStdin = true;

	char buf;
	int bytes;

	if(isStdin) {
		while((bytes = read(0, &buf, sizeof(buf))) > 0) {
			if(write(1, &buf, sizeof(buf)) == -1)
				err(2, "ERROR: writing to stdout");
		}	
		if(bytes != 0)
			err(1, "ERROR: reading from stdin");
	} else {
		for(int i = 1; i < argc; ++i) {
			int fd;
			if(strcmp("-", argv[i]) == 0)
				fd = 0;
			else
				if((fd = open(argv[i], O_RDONLY)) == -1)
					err(3, "ERROR: opening file for read: %s", argv[i]);

			while((bytes = read(fd, &buf, sizeof(buf))) > 0) {
				if(write(1, &buf, sizeof(buf)) == -1) {
					if(fd == 0)
						err(1, "ERROR: reading from stdin");
					else
						err(1, "ERROR: reading from file: %s", argv[i]);
				} 
			}
			if(bytes != 0)
				err(4, "ERROR: reading from file: %s", argv[i]);

			if(fd != 0)
				close(fd);
		}
	}

	return 0;
}
