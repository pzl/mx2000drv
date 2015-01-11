#include <stdio.h>
#include "usb.h"
#include "events.h"


int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	initialize_usb();
	find_device();
	finish_usb();

	return 0;
}
