#include <stdio.h>
#include <stdlib.h> /* free */
#include <string.h> /* memcpy */
#include "usb.h"
#include "mx.h"



MXCOMMAND(print_profile) {
	unsigned char profile;

	(void) argc;
	(void) argv;
	(void) target_profile;

	profile = get_active_profile();
	printf("%d\n", profile+1);
	return 0;
}

MXCOMMAND(change_profile) {
	long profile_l;
	int err;
	char *end;
	unsigned char profile_uc;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	(void) argc;
	(void) target_profile;
	
	profile_l = strtol(argv[0], &end, 10);
	if (*end != '\0') {
		fprintf(stderr, "Error: failed to change profile, input was not single int 1-4\n");
		return -1;
	}

	if (profile_l < 1 || profile_l > 4){
		fprintf(stderr, "Error: profile number must be between 1 and 4, inclusive\n");
		return -1;
	}

	/* range checks already done since we force 1-4 range above */
	profile_uc = (unsigned char) (profile_l - 1);
	command[3] = profile_uc;


	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error getting active mouse profile\n");
		return -1;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error getting active mouse profile\n");
		return -1;
	}

	if ( (response[4] >> 4) != profile_uc) {
		fprintf(stderr, "Error setting profile: mouse did not acknowledge change\n");
		return -1;
	}

	if (response[0] != 0xb3 ||
	    response[1] != 0x20 ||
	    response[2] != 0x00 ||
	    response[5] != 0x00 ||
	    response[6] != 0x0e ||
	    response[7] != 0x0f){
		fprintf(stderr, "Warn: mouse profile may have changed, but got unknown response\n");
	}


	return 0;
}

MXCOMMAND(read_info) {
	FILE *fp;
	int err, i;
	unsigned char *buf,
				  *bufp,
				  response[MSG_LEN],
				  stg_read[MSG_LEN] = {
		0xb3, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	(void) target_profile;


	if (argc == 0 || argv[0][0]=='-'){
		fp = stdout;
	} else {
		fp = fopen(argv[0],"wb");
		if (fp == NULL){
			fprintf(stderr, "Error opening file for writing\n");
			return -1;
		}
	}

	buf = malloc(sizeof(unsigned char)*BUF_SIZE);

	bufp = buf;

	/* read macro memory */
	for (i=0; i<NUM_PROFILES; i++){
		read_section(i,bufp);
		bufp += ADDR_DATA_LEN * (ADDR_STOP/ADDR_STEP + 1);
	}

	/* read settings memory */
	read_section(GLOBAL_PROFILE, bufp);
	bufp += ADDR_DATA_LEN * (ADDR_STOP/ADDR_STEP + 1);


	/* read DPI settings */
	err = send_command(stg_read);
	err = read_back(response);
	memcpy(bufp, response+3, 2);
	bufp += 2;


	/* read polling rates */
	err = get_poll_rates(response);
	memcpy(bufp, response, 2);
	bufp += 2;

	/* get active profile */
	*bufp = get_active_profile();

	fwrite(buf, sizeof(unsigned char), BUF_SIZE, fp);

	fclose(fp);	/* word of caution:
		we may have just closed stdout here, if no filename was given (or it was '-').
		So any printfs or fprintf(stdout) will be quite a problem. But since to reach
		this scenario, we are printing binary info to stdout, so it's likely being piped
		into a file or into something like xxd, we likely *don't* want to print anything
		else to stdout, since that would mess with the output. So closing is fine, but
		we need to make sure we strictly refrain from printing to stdout. Almost everything
		should go to stderr.
	*/

	free(buf);
	return err;

}


MXCOMMAND(print_poll) {
	int err, poll;
	unsigned char rates[2];

	(void) argc;
	(void) argv;

	err = get_poll_rates(rates);
	if (err < 0){
		return err;
	}


	if (target_profile == 0) {
		poll = (int) rates[0] >> 4;
	} else if (target_profile == 1) {
		poll = (int) rates[0] & 0x0F;
	} else if (target_profile == 2) {
		poll = (int) rates[1] >> 4;
	} else {
		poll = (int) rates[1] & 0x0F;
	}

	switch (poll) {
		case 0:
			poll = 125;
			break;
		case 1:
			poll = 250;
			break;
		case 2:
			poll = 500;
			break;
		case 3:
			poll = 1000;
			break;
		default:
			poll = 0;
			break;
	}

	printf("%dHz\n", poll);
	return 0;
}


MXCOMMAND(change_poll) {
	int err;
	long poll_l;
	char *end;
	unsigned char poll_uc;
	unsigned char rates[2],
				  response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x22, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	(void) argc;

	poll_l = strtol(argv[0], &end, 10);
	if (*end != '\0') {
		fprintf(stderr, "Error: failed to change poll rate, input was not valid number\n");
		return -1;
	}


	switch (poll_l) {
		case 1000:
			poll_uc = 3;
			break;
		case 500:
			poll_uc = 2;
			break;
		case 250:
			poll_uc = 1;
			break;
		case 125:
			poll_uc = 0;
			break;
		default:
			fprintf(stderr, "Error: invalid poll rate given. Must be one of: 1000,500,250,125\n");
			return -1;
	}

	err = get_poll_rates(rates);
	if (err < 0) {
		return err;
	}



	if (target_profile == 0) {
		rates[0] &= 0x0F; //clear old high bits
		rates[0] ^= (poll_uc << 4); //set high bits
	} else if (target_profile == 1) {
		rates[0] &= 0xF0; //clear low bits
		rates[0] ^= poll_uc; //set low bits
	} else if (target_profile == 2) {
		rates[1] &= 0x0F; //clear old high bits
		rates[1] ^= (poll_uc << 4); //set high bits
	} else {
		rates[1] &= 0xF0; //clear low bits
		rates[1] ^= poll_uc; //set low bits
	}


	command[3] = rates[0];
	command[4] = rates[1];


	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error setting poll rate (command)\n");
		return -1;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error setting poll rate (response)\n");
		return -1;
	}

	if (response[0] != 0xb3 ||
	    response[1] != 0x22 ||
	    response[2] != 0x00 ||
	    response[5] != 0x00 ||
	    response[6] != 0x00 ||
	    response[7] != 0x00){
		fprintf(stderr, "Warn: unknown extra data received when setting poll rate\n");
	}


	if (response[3] != rates[0] ||
	    response[4] != rates[1]) {
		fprintf(stderr, "Poll rates may not have been set properly\n");
	}

	return 0;
}


/*
		Helper functions,
		not necessarily outside commands
*/

int read_addr(int profile, unsigned char addr, unsigned char *response){
	int err;
	unsigned char command[MSG_LEN] = {
		0xb3, 0x02, 0x00, ADDR_READ, 0x00, 0x00, 0x00, 0x00
	};

	command[2] = addr;
	command[4] = (unsigned char) profile;

	err = send_command(command);
	err = read_back(response);


	return err;

}

unsigned char get_active_profile(void) {
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error getting active mouse profile\n");
		return 0;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error getting active mouse profile\n");
		return 0;
	}

	return (response[4] >> 4) ;

}

/*
	rates should already have allocated
	enough space for TWO (2) uchars
*/
int get_poll_rates(unsigned char *rates){
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error getting poll rate (command)\n");
		return -1;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error getting poll rate (response)\n");
		return -1;
	}

	if (response[0] != 0xb3 ||
	    response[1] != 0x22 ||
	    response[2] != 0x00 ||
	    response[5] != 0x00 ||
	    response[6] != 0x00 ||
	    response[7] != 0x00){
		fprintf(stderr, "Warn: unknown extra data received when getting poll rate\n");
	}

	rates[0] = response[3];
	rates[1] = response[4];

	return 0;
}

int mouse_sleep(void) {
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x20, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error putting mouse to sleep\n");
		return -1;
	}
	err = read_back(response);


	return err;
}

int mouse_wake(void) {
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error waking mouse back up\n");
		return -1;
	}
	err = read_back(response);


	return err;
}

int read_section(unsigned char section_num, unsigned char *buf) {
	int err, i;
	unsigned char response[MSG_LEN];

	if (section_num == NUM_PROFILES || section_num > GLOBAL_PROFILE) {
		fprintf(stderr, "Error: section number for reading is out of range: %02hx\n", section_num);
		return -1;
	}

	/* read settings memory */
	for (i=ADDR_START; i<=ADDR_STOP; i+=ADDR_STEP){
		err = read_addr(section_num,(unsigned char)i,response);
		memcpy(buf, response+4, ADDR_DATA_LEN);
		buf += ADDR_DATA_LEN;
	}

	return err;
}

int erase_section(unsigned char section_num) {
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x03, 0x00, 0xff, 0x55, 0xaa, 0x55, 0x00
	};

	if (section_num == NUM_PROFILES || section_num > GLOBAL_PROFILE){
		fprintf(stderr, "Error: section number for erasing is out of range: %02hx\n", section_num);
		return -1;
	}

	command[2] = section_num;
	command[3] = 0xff - section_num;


	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error erasing mouse section (command)\n");
		return -1;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error erasing mouse section (response)\n");
		return -1;
	}


	if (response[0] != 0xb3 ||
	    response[1] != 0x03 ||
	    response[2] != 0x01 ||
	    response[4] != 0x00 ||
	    response[5] != 0x00 ||
	    response[6] != 0x00 ||
	    response[7] != 0x00){
		fprintf(stderr, "Warn: unknown extra data received when erasing section\n");
	}

	if (response[3] != 0x38 + section_num){
		fprintf(stderr, "Error: mouse did not properly acknowledge correct section was erased\n");
		return -1;
	}

	return 0;
}
