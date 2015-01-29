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
	int err, i, j;
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
		for (j=ADDR_START; j<=ADDR_STOP; j+=ADDR_STEP){
			err = read_addr(i,(unsigned char)j,response);
			memcpy(bufp, response+4, ADDR_DATA_LEN);
			bufp += ADDR_DATA_LEN;
		}
	}
	/* read settings memory */
	for (j=ADDR_START; j<=ADDR_STOP; j+=ADDR_STEP){
		err = read_addr(5,(unsigned char)j,response);
		memcpy(bufp, response+4, ADDR_DATA_LEN);
		bufp += ADDR_DATA_LEN;
	}

	/* read DPI settings */
	err = send_command(stg_read);
	err = read_back(response);
	memcpy(bufp, response+3, 2);
	bufp += 2;


	/* read polling rates */
	stg_read[1] = 0x22;
	err = send_command(stg_read);
	err = read_back(response);
	memcpy(bufp, response+3, 2);
	bufp +=2;

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
