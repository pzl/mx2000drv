#include <stdio.h>
#include "usb.h"
#include "events.h"


int main(int argc, char **argv) {
	int err, profile;

	(void) argc;
	(void) argv;

	initialize_usb();
	err = find_device();

	if (err < 0){
		fprintf(stderr, "Error getting mouse (%d), aborting\n", err);
		finish_usb();
		return -1;
	}

	printf("mouse ready. Enter profile: \n");
	scanf("%d", &profile);

	err = change_profile(profile);
	if (err < 0){
		fprintf(stderr, "Error changing mouse profile (%d), aborting\n", err);
		finish_usb();
		return -1;
	}
	
	finish_usb();
	return 0;
}
