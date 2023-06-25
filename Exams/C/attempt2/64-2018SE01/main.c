#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdbool.h>
#include <string.h>

int getIndex(const char* arr, char ch);
int getIndex(const char* arr, char ch) {
	for(int i = 0; arr[i] != '\0'; ++i)
		if(arr[i] == ch)
			return i;

	return -1;
}

int main(int argc, char** argv) {

	bool D = false;
	bool S = false;

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expeected");

	char* param1 = argv[1];
	char* param2 = argv[2];
	if(strcmp("-d", param1) == 0) {
		D = true;
	} else if(strcmp("-s", param1) == 0) {
		S = true;
	} else {
		if(strlen(param1) != strlen(param2))
			errx(2, "ERROR: params for translating are different lengths");
	}

	int bytes = 0;
	char buf = '\0';
	char prev = '\0';
	while((bytes = read(0, &buf, sizeof(buf))) > 0) {
		if(D) {
			int index = getIndex(param2, buf);
			if(index == -1)
				if(write(1, &buf, sizeof(buf)) != sizeof(buf))
					err(3, "ERROR: writing to stdout in delete option");
		} else if (S) {
			int index = getIndex(param2, buf);
			if(index == -1 || (index != -1 && buf != prev))
				if(write(1, &buf, sizeof(buf)) != sizeof(buf))
					err(3, "ERROR: writing to stdout in S option");

			prev = buf;
		} else {
			int index = getIndex(param1, buf);
			if(index != -1)
				buf = param2[index];

			if(write(1, &buf, sizeof(buf)) != sizeof(buf))
				err(3, "ERROR: writing to stdout with no options");
		}
	}


	return 0;
}
