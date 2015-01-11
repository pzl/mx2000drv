#include <stdio.h>
#include <string.h> /* memset */
#include <fcntl.h> /* open */
#include <unistd.h> /* close */
#include <linux/input.h>
#include <linux/uinput.h>
#include "events.h"


/*
	initializes uinput.

	returns -1 on any error
	returns file descriptor on success
*/
int register_device(void){
	int fd,
		i;
	struct uinput_user_dev uidev;

	/* @todo support /dev/input/uinput */
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "Error opening /dev/uinput\n");
		return -1;
	}

	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0){
		fprintf(stderr, "Error setting device characteristics\n");
		return -1;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_REL) < 0){
		fprintf(stderr, "Error setting device characteristics\n");
		return -1;
	}
	
	/* register all the keys!! */
	for (i=0; i<255; i++){
		if (ioctl(fd, UI_SET_KEYBIT, i) < 0){
			fprintf(stderr, "Error allowing char %d\n", i);
		}
	}
	

	memset(&uidev, 0, sizeof(struct uinput_user_dev));
	sprintf(uidev.name, "mx2000iib");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	if (write(fd, &uidev, sizeof(struct uinput_user_dev)) < 0){
		fprintf(stderr, "Error creating character device\n");
		return -1;
	}
	if (ioctl(fd, UI_DEV_CREATE) < 0) {
		fprintf(stderr, "Error creating character device\n");
		return -1;
	}

	/* give X time to discover keyboard */
	sleep(1);
	return fd;
}

void destroy_device(int fd) {
	if (ioctl(fd, UI_DEV_DESTROY) < 0){
		fprintf(stderr, "Error destroying character device\n");
	}
	close(fd);
}

int write_character(int fd, int c) {
	int err=0;
	struct input_event ev;

	(void) c;

	memset(&ev, 0, sizeof(struct input_event));
	ev.type=EV_KEY;
	ev.code=c;
	ev.value=1;

	if (write(fd, &ev, sizeof(struct input_event)) < 0) {
		/*fprintf(stderr, "Error writing character\n");*/
		perror("Error writing character");
		err=1;
	}

	ev.type=EV_SYN;
	ev.code=SYN_REPORT;
	ev.value=0;
	if (write(fd, &ev, sizeof(struct input_event)) < 0) {
		fprintf(stderr, "Error writing sync\n");
		err=1;
	}

	memset(&ev, 0, sizeof(struct input_event));
	ev.type=EV_KEY;
	ev.code=c;
	ev.value=0;

	if (write(fd, &ev, sizeof(struct input_event)) < 0) {
		fprintf(stderr, "Error writing character\n");
		err=1;
	}

	ev.type=EV_SYN;
	ev.code=SYN_REPORT;
	ev.value=0;
	if (write(fd, &ev, sizeof(struct input_event)) < 0) {
		fprintf(stderr, "Error writing sync\n");
		err=1;
	}
	if (err){
		return -1;
	}
	return 0;

}
