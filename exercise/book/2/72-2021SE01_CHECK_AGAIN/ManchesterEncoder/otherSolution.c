#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>

bool get_bit(uint8_t byte, int idx) {
	return ((byte & (1 << idx)) != 0);
}

uint16_t set_bit(uint16_t x, int idx, bool bit) {
	if (bit) {
		return (1 << idx) | x;
	} else {
		return (~(1 << idx)) & x;
	}
}

uint16_t manchester_encode_byte(uint8_t byte) {
	uint16_t result = 0;
	for (int i = 0; i < 8; i++) {
		bool bit = get_bit(byte, i);
		if (bit) {
			result = set_bit(result, i * 2, 1);
			result = set_bit(result, i * 2 + 1, 0);
		} else {
			result = set_bit(result, i * 2, 0);
			result = set_bit(result, i * 2 + 1, 1);
		}
	}

	return result;
}

void manchester_encode_bytes(uint8_t* in_buf, uint16_t* out_buf, int n) {
	for (int i = 0; i < n; i++) {
		out_buf[i] = manchester_encode_byte(in_buf[i]);
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		warnx("%s encodes a file to manchester encoding", argv[0]);
		errx(1, "usage: %s <input file> <output file>", argv[0]);
	}

	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) {
		err(1, "could not open %s", argv[1]);
	}

	int fd_out = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (fd_out < 0) {
		err(1, "could not open %s", argv[1]);
	}

	uint8_t in_buf[4096];
	uint16_t out_buf[4096];

	int bytes_read;
	while ((bytes_read = read(fd_in, in_buf, sizeof(in_buf))) > 0) {
		manchester_encode_bytes(in_buf, out_buf, bytes_read);
		int bytes_written = write(fd_out, out_buf, bytes_read * 2);
		if (bytes_written < 0) {
			err(1, "could not write data to %s", argv[2]);
		}
		if (bytes_read * 2 != bytes_written) {
			errx(1, "could not write all data at once");
		}
	}
	if (bytes_read < 0) {
		err(1, "could not read data from %s", argv[1]);
	}

	close(fd_in);
	close(fd_out);
	return 42;
}
