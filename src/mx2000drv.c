#include <stdio.h>
#include "usb.h"
#include "events.h"


int main(int argc, char **argv) {
	int fd;

	(void) argc;
	(void) argv;


	fd = register_device();
	write_character(fd, 32);
	destroy_device(fd);

	return 0;
}
