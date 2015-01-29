#ifndef MX_MX_H
#define MX_MX_H


#define MSG_LEN 8
#define ADDR_DATA_LEN 4
#define BUF_SIZE 1285
/* 64 addresses * 5 memory areas * 4 bytes + (1)active profile + (2)DPI + (2)polling rates */

#define NUM_PROFILES 4
#define GLOBAL_PROFILE 5

#define ADDR_START 0x00
#define ADDR_STOP 0xFC
#define ADDR_STEP 0x04


typedef int (*MXCommand)(int, char **, int);
#define MXCOMMAND(func_name) int func_name(int argc, char **argv, int target_profile)

/*
	User-facing commands
*/
MXCOMMAND(read_info);
MXCOMMAND(print_profile);
MXCOMMAND(change_profile);
MXCOMMAND(print_poll);
MXCOMMAND(change_poll);

/*
	Helpers
*/
int read_addr(int profile, unsigned char addr, unsigned char *response);
int write_addr(unsigned char profile, unsigned char addr, unsigned char *buf);
unsigned char get_active_profile(void);
int get_poll_rates(unsigned char *);
int mouse_sleep(void);
int mouse_wake(void);
int read_section(unsigned char section_num, unsigned char *buf);
int erase_section(unsigned char section_num);


#endif
