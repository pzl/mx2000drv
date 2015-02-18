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
	err = set_profile(profile_uc);

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


MXCOMMAND(backlight) {
	int err;
	unsigned char addr;
	unsigned char buf[MSG_LEN];

	addr = COLOR_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading backlight info\n");
		return -1;
	}

	if (argc == 0){
		if (buf[4] & BACKLIGHT_ENABLED_MSK) {
			printf("on\n");
		} else {
			printf("off\n");
		}
	} else {
		if (argv[0][1] == 'n' || argv[0][1] == 'N'){
			buf[4] |= BACKLIGHT_ENABLED_MSK; 
		} else {
			buf[4] &= ~(BACKLIGHT_ENABLED_MSK);
		}

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing backlight info\n");
			return -1;
		}		
	}

	return 0;
}

MXCOMMAND(cycle) {
	int err;
	unsigned char addr;
	unsigned char buf[MSG_LEN];

	addr = COLOR_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading cycle info\n");
		return -1;
	}

	if (argc == 0){
		if (buf[4] & CYCLE_ENABLED_MSK) {
			printf("on\n");
		} else {
			printf("off\n");
		}
	} else {
		if (argv[0][1] == 'n' || argv[0][1] == 'N'){
			buf[4] |= CYCLE_ENABLED_MSK; 
		} else {
			buf[4] &= ~(CYCLE_ENABLED_MSK);
		}

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing cycle info\n");
			return -1;
		}
	}

	return 0;
}

MXCOMMAND(color) {
	int err;
	unsigned char addr;
	unsigned char buf[MSG_LEN];

	addr = COLOR_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;


	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading color info\n");
		return -1;
	}

	if (argc == 0) {
		printf("%02hx%02hx%02hx\n", buf[5],buf[6],buf[7]);
		return 0;
	} else {
		char *end;
		unsigned long new_color = strtoul(argv[0],&end,16);

		buf[0] = buf[4]; //keep breathe, backlight, and cycle info
		buf[1] = ( (new_color & 0xff0000) >> 16);
		buf[2] = ( (new_color & 0x00ff00) >> 8);
		buf[3] = ( (new_color & 0x0000ff) );

		if (*end != '\0') {
			fprintf(stderr, "Error: failed to change color, input was not valid hex\n");
			return -1;
		}
		err = write_addr(GLOBAL_PROFILE,addr,buf);
		if (err < 0){
			fprintf(stderr, "Error writing color info\n");
			return -1;
		}
	}

	return 0;
}

MXCOMMAND(breathe) {
	int err;
	unsigned char addr;
	unsigned char buf[MSG_LEN];

	addr = COLOR_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading breathe info\n");
		return -1;
	}

	if (argc == 0) {
		if (buf[4] & BREATHE_ENABLED_MSK) {
			printf("on\n");
		} else {
			printf("off\n");
		}
	} else {
	
		if (argv[0][1] == 'n' || argv[0][1] == 'N'){
			buf[4] |= BREATHE_ENABLED_MSK; 
		} else {
			buf[4] &= ~(BREATHE_ENABLED_MSK);
		}

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing breathe info\n");
			return -1;
		}
	}

	return 0;
}

MXCOMMAND(lit_time) {
	int err;
	unsigned char addr, value;
	unsigned char buf[MSG_LEN];

	addr = COLOR_TIME_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading lit info\n");
		return -1;
	}

	if (argc == 0){
		value = (buf[5] & LIT_TIME_MSK) >> 4;
		printf("%d\n", value);
	} else {
		unsigned long val_ul;
		char *end;
		val_ul = strtoul(argv[0],&end,10);
		if (*end != '\0') {
			fprintf(stderr, "Error: failed to parse numeric input for lit time\n");
			return -1;
		}

		if (val_ul > 15) {
			fprintf(stderr, "Error: lit time out of range (0-15)\n");
			return -1;
		}
		value = (unsigned char) val_ul;

		buf[5] &= ~(LIT_TIME_MSK); /* clear higher 4 bits */
		buf[5] |= (value << 4);

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing lit info\n");
			return -1;
		}
	}

	return 0;
}

MXCOMMAND(dark_time) {
	int err;
	unsigned char addr, value;
	unsigned char buf[MSG_LEN];

	addr = COLOR_TIME_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading dark info\n");
		return -1;
	}

	if (argc == 0) {
		value = buf[5] & DARK_TIME_MSK;
		printf("%d\n", value);
	} else {
		unsigned long val_ul;
		char *end;
		val_ul = strtoul(argv[0],&end,10);
		if (*end != '\0') {
			fprintf(stderr, "Error: failed to parse numeric input for dark time\n");
			return -1;
		}

		if (val_ul > 15) {
			fprintf(stderr, "Error: dark time out of range (0-15)\n");
			return -1;
		}
		value = (unsigned char) val_ul;

		buf[5] &= ~(DARK_TIME_MSK); /* clear higher 4 bits */
		buf[5] |= value;

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing dark info\n");
			return -1;
		}	
	}
	return 0;
}


MXCOMMAND(pulse_time) {
	int err;
	unsigned char addr, value;
	unsigned char buf[MSG_LEN];

	addr = COLOR_TIME_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading pulse info\n");
		return -1;
	}

	if (argc == 0) {
		value = buf[4]/4;
		printf("%d\n", value);
	} else {
		unsigned long val_ul;
		char *end;
		val_ul = strtoul(argv[0],&end,10);
		if (*end != '\0') {
			fprintf(stderr, "Error: failed to parse numeric input for pulse time\n");
			return -1;
		}

		if (val_ul > 63) {
			fprintf(stderr, "Error: pulse time out of range (0-60)\n");
			return -1;
		}
		value = (unsigned char) val_ul;

		buf[4] = value*4;

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing pulse info\n");
			return -1;
		}	
	}

	return 0;
}
MXCOMMAND(standby_time) {
	int err;
	unsigned char addr, value;
	unsigned char buf[MSG_LEN];

	addr = COLOR_TIME_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading standby info\n");
		return -1;
	}

	if (argc == 0) {
		value = buf[6]/4;
		printf("%d\n", value);
	} else {
		unsigned long val_ul;
		char *end;
		val_ul = strtoul(argv[0],&end,10);
		if (*end != '\0') {
			fprintf(stderr, "Error: failed to parse numeric input for standby time\n");
			return -1;
		}

		if (val_ul > 63) {
			fprintf(stderr, "Error: standby time out of range (0-60)\n");
			return -1;
		}
		value = (unsigned char) val_ul;

		buf[6] = value*4;

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing standby info\n");
			return -1;
		}		
	}

	return 0;
}


MXCOMMAND(sensitivity) {
	int err, axis;
	long val_l;
	unsigned char addr, value;
	unsigned char buf[MSG_LEN];

	addr = SENSITIVITY_ADDR;
	addr += SETTING_ADDR_PROFILE_STEP * target_profile;

	err = read_addr(GLOBAL_PROFILE,addr,buf);
	if (err < 0){
		fprintf(stderr, "Error reading standby info\n");
		return -1;
	}

	if (argv[0][0] == 'x' || argv[0][0] == 'X') {
		axis = 4;
	} else {
		axis = 5;
	}
		


	if (argc == 1) {
		value = buf[axis];

		if ( (value & 0x0F) == 5) {
			//somewhat normal convention
			val_l = (value >> 4) - 5;


			/* weird logic. not sure why these values are what they are */
		} else if (value == 0x11) {
			val_l = 0;
		} else if (value == 0x1a) {
			val_l = -5;
		} else if (value == 0x21) {
			val_l = 5;
		} else {
			printf("Error: unknown sensitivity response: 0x%02hx\n", buf[axis]);
			val_l = 0;
		}

		printf("%ld\n", val_l);
	} else {
		char *end;
		val_l = strtol(argv[1],&end,10);
		if (*end != '\0') {
			fprintf(stderr, "Error: failed to parse numeric input for sensitivity\n");
			return -1;
		}

		if (val_l < -5 || val_l > 5) {
			fprintf(stderr, "Error: sensitivity out of range (-5)-5\n");
			return -1;
		}

		if (val_l > -5 || val_l < 5) {
			value = (unsigned char) ( (5+val_l) << 4) | 5;
		}
		if (val_l == 0) {
			value = 0x11;
		} else if (val_l == -5) {
			value = 0x1a;
		} else if (val_l == 5) {
			value = 0x21;
		}

		buf[axis] = value;

		err = write_addr(GLOBAL_PROFILE,addr,buf+4);
		if (err < 0){
			fprintf(stderr, "Error writing sensitivity info\n");
			return -1;
		}		
	}

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


MXCOMMAND(load_info) {
	FILE *fp;
	int err;
	size_t len;
	unsigned char *buf;

	(void) target_profile;

	if (argc == 0 || argv[0][0] == '-') {
		freopen(NULL, "rb", stdin);
		fp = stdin;
	} else {
		fp = fopen(argv[0],"rb");
		if (fp == NULL) {
			fprintf(stderr, "Error opening file for reading\n");
			return -1;
		}
	}

	buf = malloc(sizeof(unsigned char)*BUF_SIZE);

	len = fread(buf, sizeof(unsigned char), BUF_SIZE, fp);
	if (len != BUF_SIZE) {
		fprintf(stderr, "Warn: possible data underflow when loading file\n");
	}

	err = write_buf(buf);
	if (err < 0){
		fprintf(stderr, "Error writing file to memory\n");
	}

	if (fp != stdin){
		fclose(fp);
	}
	free(buf);
	return err;
}


MXCOMMAND(factory_reset) {
	int err;
	unsigned char *buf, *bufp;

	(void)argc;
	(void)argv;
	(void)target_profile;

	buf = malloc(sizeof(unsigned char)*BUF_SIZE);
	bufp = buf;

	/* all macro memory is blank */
	memset(bufp,0xFF, (ADDR_STOP/ADDR_STEP+1)*ADDR_DATA_LEN*NUM_PROFILES);
	bufp += (ADDR_STOP/ADDR_STEP+1)*ADDR_DATA_LEN*NUM_PROFILES;

	memcpy(bufp, factory_settings, (ADDR_STOP/ADDR_STEP+1)*ADDR_DATA_LEN);
	bufp += (ADDR_STOP/ADDR_STEP+1)*ADDR_DATA_LEN;

	*bufp++ = 0x00; /* DPI */
	*bufp++ = 0x00;
	*bufp++ = 0x33; /* poll rate */
	*bufp++ = 0x33;
	*bufp = 0x00;   /* active profile */

	err = write_buf(buf);

	free(buf);
	return err;
}


/* ---------------------------------------------------
		Helper functions,
		not necessarily outside commands
------------------------------------------------------ */


/*
	Reads data from section 'profile' and address 'addr' into the
	already-allocated response buffer which MUST be MSG_LEN(8) size
*/
int read_addr(int profile, unsigned char addr, unsigned char *response){
	int err;
	unsigned char command[MSG_LEN] = {
		0xb3, 0x02, addr, 0x04, 0x00, 0x00, 0x00, 0x00
	};
	command[4] = (unsigned char) profile;

	err = send_command(command);
	err = read_back(response);


	return err;
}

/*
	changes data at section 'profile' and address 'addr' to be 'buf'.
	Includes full-section operations. Operation occurs immediately

	profile: section number, must be 0-3 or 5
	addr: in the range 0x00-0xFC inclusive, in increments of 4
	buf: array of ADDR_DATA_LEN(4) uchars of data to write
*/
int write_addr(unsigned char profile, unsigned char addr, unsigned char *buf) {
	unsigned char cur_prof;
	unsigned char sec[SEC_SIZE];
	unsigned char *secp;
	int err;

	cur_prof = get_active_profile();


	/* get all existing data we'll be writing back */
	err = read_section(profile,sec);
	if (err < 0) {
		fprintf(stderr, "Error: couldn't write data. Section read failed\n");
		return -1;
	}

	printf("read: data at addr 0x%02hx: %02hx%02hx%02hx%02hx\n",
	       addr,
	       sec[addr], sec[addr+1],sec[addr+2],sec[addr+3]);


	/* change data */
	secp = sec + addr; /* point to start of data to change */
	memcpy(secp,buf,ADDR_DATA_LEN);

	printf("write: data at addr 0x%02hx: %02hx%02hx%02hx%02hx\n",
	       addr,
	       sec[addr], sec[addr+1],sec[addr+2],sec[addr+3]);

	/* write back entire section to mouse */
	err = mouse_sleep();
	if (err < 0){
		fprintf(stderr, "Error: couldn't write data. Failed trying to put mouse to sleep\n");
		mouse_wake();
		return -1;
	}

	err = write_section(profile,sec);
	if (err < 0) {
		fprintf(stderr, "Error: problem writing data\n");
		mouse_wake();
		return -1;
	}

	err = mouse_wake();
	if (err < 0) {
		fprintf(stderr, "Error: failed waking mouse back up\n");
		return -1;
	}

	set_profile(cur_prof);

	return 0;
}

/*
	performs action of writing the ADDR_DATA_LEN(4) 'buf' to 'addr' in section 
	number 'profile'. Writes immediately without any reads, erases, or  section-
	level operations. Should not be performed in isolation, but only through
	section-level commands. after proper sleep, read, and erase.
*/
int set_addr(unsigned char profile, unsigned char addr, unsigned char *buf) {
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x01, addr, 0x04, 0x00, 0x00, 0x00, 0x00
	};

	if (profile == NUM_PROFILES || profile > GLOBAL_PROFILE) {
		fprintf(stderr, "Error: invalid section number to write to\n");
		return -1;
	}

	command[3] |= profile << 4;
	command[4] = buf[0];
	command[5] = buf[1];
	command[6] = buf[2];
	command[7] = buf[3];

	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error writing to addr (command)\n");
		return -1;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error writing to addr (response)\n");
		return -1;
	}


	if (response[0] != 0xb3 ||
	    response[1] != 0x01 ||
	    response[2] != 0x01){
		fprintf(stderr, "Warn: unknown extra data received when writing addr\n");
	}

	if (response[3] != 0x38 + profile ||
	    response[4] != buf[0] ||
	    response[5] != buf[1] ||
	    response[6] != buf[2] ||
	    response[7] != buf[3]){
		fprintf(stderr, "Error: mouse did not properly write data to addr\n");
		return -1;
	}

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

int set_profile(unsigned char profile) {
	int err;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x20, 0x08, profile, 0x00, 0x00, 0x00, 0x00
	};


	if (profile > 3){
		fprintf(stderr, "Error: profile out of bounds\n");
		return -1;
	}

	err = send_command(command);
	if (err < 0){
		fprintf(stderr, "Error setting mouse profile\n");
		return -1;
	}
	err = read_back(response);
	if (err < 0){
		fprintf(stderr, "Error setting mouse profile\n");
		return -1;
	}

	if ( (response[4] >> 4) != profile) {
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

int write_section(unsigned char section_num, unsigned char *buf){
	int err, i;

	err = erase_section(section_num);
	if (err < 0){
		fprintf(stderr, "Error: problem erasing section before writing\n");
	}
	for (i=0; i<=ADDR_STOP; i+=ADDR_STEP) {
		err = set_addr(section_num, (unsigned char) i, buf);
		if (err < 0){
			fprintf(stderr, "Error writing %d-0x%02hx\n", section_num,i);
		}
		buf += ADDR_DATA_LEN;
	}

	return 0;

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

int write_buf(unsigned char *buf) {
	int err, i;
	unsigned char response[MSG_LEN],
				  command[MSG_LEN] = {
		0xb3, 0x21, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	err = mouse_sleep();
	if (err < 0){
		fprintf(stderr, "Error trying to put mouse to sleep\n");
		mouse_wake();
	}

	/* write macro sections */
	for (i=0; i<NUM_PROFILES; i++){
		write_section(i, buf);
		buf += (ADDR_STOP/ADDR_STEP+1)*ADDR_DATA_LEN;
	}
	/* write settings section */
	write_section(GLOBAL_PROFILE, buf);
	buf += (ADDR_STOP/ADDR_STEP+1)*ADDR_DATA_LEN;


	err = mouse_wake();
	if (err < 0){
		fprintf(stderr, "Error trying to wake mouse up\n");
	}

	/* DPI */
	command[3] = *buf++;
	command[4] = *buf++;
	err = send_command(command);
	err = read_back(response);

	/* poll rate */
	command[1] = 0x22;
	command[3] = *buf++;
	command[4] = *buf++;
	/* @todo: write poll rate */

	/* current profile */
	/* @todo: change profile */


	return err;
}
