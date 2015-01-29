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



typedef enum {
	ADDR_READ = 0x04,
	ADDR_WRITE = 0x54
} AddrAction;

typedef int (*MXCommand)(int, char **);


/*
	User-facing commands
*/
int read_info(int, char **);
int print_profile(int, char **);
int change_profile(int, char **);

/*
	Helpers
*/
int read_addr(int profile, unsigned char addr, unsigned char *response);
unsigned char get_active_profile(void);


#endif
