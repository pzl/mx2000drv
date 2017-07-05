/*
MX2000DRV - portable driver for the MX-2000 II gaming mouse
Copyright (C) 2015  Dan Panzarella

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MX_MX_H
#define MX_MX_H


#define VB_PRINT(...) do { if (verbose){ fprintf(stderr, __VA_ARGS__); } } while (0)

#define MSG_LEN 8
#define ADDR_DATA_LEN 4
#define SEC_SIZE 256
/* 64 addresses (00-FC/step) * 4 bytes */
#define BUF_SIZE SEC_SIZE*5+5
/* 5 memory areas *  (1)active profile + (2)DPI + (2)polling rates */

#define NUM_PROFILES 4
#define GLOBAL_PROFILE 5

#define GLOBAL_PREFIX 0xB3

#define ADDR_START 0x00
#define ADDR_STOP 0xFC
#define ADDR_STEP 0x04

#define ADDR_WRITE 0x01
#define ADDR_READ 0x02
#define SEC_ERASE 0x03

/* gap between repeated settings for next profile */
#define SETTING_ADDR_PROFILE_STEP 0x08
#define BUTTON_ADDR_PROFILE_STEP 0x20

#define BUTTON_ADDR_START 0x00
#define MACRO_ADDR_START 0x18
#define MACRO_ADDR_NEXT_STEP 0x04
#define BUTTON_ADDR_STEP 0x04

/* First profile addr of each setting */
#define COLOR_ADDR 0x80
#define COLOR_TIME_ADDR 0x84
#define SENSITIVITY_ADDR 0xC4
#define DPI_VAL_ADDR 0xE0


/* command types, second byte after b3 */
#define ADMIN_POLL_RATE 0x22
#define ADMIN_DPI_PRE 0x21
#define ADMIN_GENERAL 0x20
#define ADMIN_PROFILE 0x08

/* third byte, command actions */
#define ADMIN_WRITE 0x0F
#define ADMIN_READ 0x00
#define ADMIN_SLEEP 0x84
#define ADMIN_WAKE 0x80

#define BACKLIGHT_ENABLED_MSK (1 << 7)
#define BREATHE_ENABLED_MSK 0x70
#define CYCLE_ENABLED_MSK 0x08
#define LIT_TIME_MSK 0xF0
#define DARK_TIME_MSK 0x0F


typedef int (*MXCommand)(int, char **, int, int);
#define MXCOMMAND(func_name) int func_name(int argc, char **argv, int target_profile, int verbose)


static const unsigned char factory_settings[] = {
	0x81, 0x42, 0x82, 0x42, 0x84, 0x42, 0x90, 0x42,
	0x88, 0x42, 0x23, 0x22, 0x8a, 0x21, 0x07, 0x4a,
	0x01, 0x43, 0xff, 0x43, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x81, 0x42, 0x82, 0x42, 0x84, 0x42, 0x90, 0x42,
	0x88, 0x42, 0x23, 0x22, 0x8a, 0x21, 0x07, 0x4a,
	0x01, 0x43, 0xff, 0x43, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x81, 0x42, 0x82, 0x42, 0x84, 0x42, 0x90, 0x42,
	0x88, 0x42, 0x23, 0x22, 0x8a, 0x21, 0x07, 0x4a,
	0x01, 0x43, 0xff, 0x43, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x81, 0x42, 0x82, 0x42, 0x84, 0x42, 0x90, 0x42,
	0x88, 0x42, 0x23, 0x22, 0x8a, 0x21, 0x07, 0x4a,
	0x01, 0x43, 0xff, 0x43, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0xff, 0x00, 0x00, 0x14, 0x11, 0x04, 0x00,
	0xf0, 0xff, 0xff, 0x00, 0x14, 0x11, 0x04, 0x00,
	0xf0, 0x00, 0xff, 0x00, 0x14, 0x11, 0x04, 0x00,
	0xf0, 0x00, 0x00, 0xff, 0x14, 0x11, 0x04, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x0f, 0x05, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x41, 
	0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x41,
	0x0f, 0x1e, 0x2d, 0x38, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x1e, 0x2d, 0x38, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x1e, 0x2d, 0x38, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x1e, 0x2d, 0x38, 0xff, 0xff, 0xff, 0xff
};



/*
	User-facing commands
*/
MXCOMMAND(read_info);
MXCOMMAND(load_info);
MXCOMMAND(factory_reset);
MXCOMMAND(print_profile);
MXCOMMAND(change_profile);
MXCOMMAND(poll_rates);
MXCOMMAND(backlight);
MXCOMMAND(cycle);
MXCOMMAND(color);
MXCOMMAND(breathe);
MXCOMMAND(lit_time);
MXCOMMAND(dark_time);
MXCOMMAND(pulse_time);
MXCOMMAND(standby_time);
MXCOMMAND(sensitivity);
MXCOMMAND(accel);
MXCOMMAND(dpi_value);
MXCOMMAND(dpi_active);
MXCOMMAND(button);
MXCOMMAND(macro);

/*
	Helpers
*/

int read_addr(int profile, unsigned char addr, unsigned char *response); /* get info at address, safe */
int write_addr(unsigned char profile, unsigned char addr, unsigned char *buf, int verbose); /* change info at address to new data. Performs sleep, section erasing and rewriting, and awakens. Can be called as long as device is open, but performs action immediately. Multiple changes to data should perform tasks on section-level basis, not this */
int set_addr(unsigned char profile, unsigned char addr, unsigned char *buf); /* writes directly to addr. Does not check if mouse is sleeping, or erase has been performed. Should not be used outside of write_section */

unsigned char get_active_profile(void);
int set_profile(unsigned char);
int change_poll_rates(unsigned char rw, unsigned char *);
int dpi_presets(unsigned char rw, unsigned char *);

int button_map(unsigned char rw, unsigned char profile, unsigned char button_num, unsigned char *keys, int verbose);

int mouse_sleep(void);
int mouse_wake(void);

int read_section(unsigned char section_num, unsigned char *buf);
int write_section(unsigned char section_num, unsigned char *buf);
int erase_section(unsigned char section_num);

int write_buf(unsigned char *buf);

/*
	kairos' macro functions
*/

int macro_map(char* macro_keys);
int findlength(char* str);
int isUpper(char key);
unsigned long convert_key(char key);

#endif
