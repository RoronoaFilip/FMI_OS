#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

struct pair {
	uint16_t offset;
	uint8_t len;
	uint8_t res;
};

int main(int argc, char** argv) {
	if (argc != 5)
		errx(2, "ERROR: params count");

	int fd1;
	if((fd1 = open()))

	exit(0);
}
