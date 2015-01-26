#include <stdio.h>
#include "usb.h"
#include "mx.h"

int main(int argc, char **argv) {
	int err;

	(void) argc;
	(void) argv;


	initialize_usb();

	err = find_device();
	if (err < 0){
		finish_usb();
		return -1;
	}

	read_info();

	finish_usb();

	return 0;
}
