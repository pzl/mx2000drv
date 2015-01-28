#include <stdio.h>
#include <stdlib.h> /* free */
#include <string.h> /* memcpy */
#include "usb.h"
#include "mx.h"


int get_active_profile(void) {
	/*
	int err;
	unsigned char *command[MSG_LEN] = {
		0xb3
	}

	err = send_command()
	*/
	return 0;
}

int read_info(int argc, char **argv) {
	FILE *fp;
	int err, i, j;
	unsigned char *buf,
				  *bufp,
				  response[MSG_LEN],
				  stg_read[MSG_LEN] = {
		0xb3, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	(void) argc;
	(void) argv;

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
	*bufp = 0;

	fp = fopen("mem.bin","wb");
	if (fp == NULL){
		fprintf(stderr, "Error opening file for writing\n");
		return -1;
	}

	fwrite(buf, sizeof(unsigned char), BUF_SIZE, fp);

	fclose(fp);

	free(buf);

	return err;

}

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
