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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "usb.h"
#include "mx.h"

#define PROFILE_UNSET -1
#define HELP(x) help(x,argv[0])

static void help(int status, char *program_name);
static int valid_hex(char *s);
static int is_on_off(char *s);

int main(int argc, char **argv) {
	int opt,
		profile=PROFILE_UNSET,
		n_addtl_cmds,
		verbose=0,
		err;
	char *command, *sec_cmd, *third_cmd;
	MXCommand action;
	const char *short_opt = "hp:vV";
	struct option long_opt[] = {
		{"help",	no_argument, NULL, 'h'},
		{"profile", required_argument, NULL, 'p'},
		{"verbose", no_argument, NULL, 'v'},
		{"version", no_argument, NULL, 'V'},
		{NULL,		0,			 NULL,	0}
	};

	/*
		Parse any options
	*/

	while ((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1){
		switch (opt) {
			case 'p':
				profile = atoi(optarg);
				if (profile < 1 || profile > 4){
					fprintf(stderr, "Profile %s out of range (1-4)\n", optarg);
					HELP(-2);
				}
				break;

			case 'v':
				verbose=1;
				break;

			case 'V':
				printf("mx2000drv v%s\n", VERSION);
				exit(0);
				break;


			case 'h':
				HELP(0);
				break;
			case -1: //no more args
			case 0:	//long options toggles
				break;
			case ':':
			case '?':
				HELP(-2);
				break;
			default:
				fprintf(stderr, "%s: invalid option -- %c\n", argv[0], opt);
				HELP(-2);
				break;
		}
	}

	/*
		Parse command and arguments
	*/

	if (argv[optind] == NULL){
		fprintf(stderr, "A command is required. None provided\n");
		HELP(-2);
	}
	command = argv[optind];
	n_addtl_cmds = argc - optind - 1;

	if (n_addtl_cmds <= 0 || argv[optind+1] == NULL) {
		sec_cmd = NULL;
	} else {
		sec_cmd = argv[optind+1];
	}
	if ( n_addtl_cmds <= 1 || argv[optind+2] == NULL) {
		third_cmd = NULL;
	} else {
		third_cmd = argv[optind+2];
	}

	action = NULL;

	if (strcmp(command,"profile") == 0) {
		if (n_addtl_cmds == 0){
			action = print_profile;
		} else if (n_addtl_cmds == 1){
			unsigned char p = (unsigned char) atoi(argv[optind+1]);
			if (p > 0 && p <= 4) {
				action = change_profile;
			} else {
				fprintf(stderr, "Error: Profile number must be 1-4\n");
				HELP(-2);
			}
		} else {
			fprintf(stderr, "invalid number of arguments for 'profile' command\n");
			HELP(-2);
		}

	} else if (strcmp(command,"button") == 0 ) {
		action = button;
		if (n_addtl_cmds < 1 || n_addtl_cmds > 2) {
			fprintf(stderr, "Invalid number of arguments\n");
			HELP(-2);
		}
		int button_num = atoi(sec_cmd);
		if (button_num < 1 || button_num > 8) {
			fprintf(stderr, "Error: Button number out of range (1-8).\n");
			HELP(-2);
		}
	} else if (strcmp(command,"color") == 0 ) {
		action = color;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for color command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (!valid_hex(sec_cmd)){
				fprintf(stderr, "Invalid color string. Must be 6-character hexadecimal (0-9,a-f) like 00FF00 for green\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"macro") == 0 ) {
		action = macro;
		if (n_addtl_cmds < 1 || n_addtl_cmds > 2) {
			fprintf(stderr, "Invalid number of arguments\n");
			HELP(-2);
		}
		int button_num = atoi(sec_cmd);
		if (button_num < 1 || button_num > 8) {
			fprintf(stderr, "Error: Button number out of range (1-8).\n");
			HELP(-2);
		}
	} else if (strcmp(command,"breathe") == 0 ) {
		action = breathe;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for breathe command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (!is_on_off(sec_cmd)) {
				fprintf(stderr, "Error. Valid arguments for breathe are 'on' or 'off'\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"cycle") == 0 ) {
		action = cycle;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for cycle command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (!is_on_off(sec_cmd)) {
				fprintf(stderr, "Error. Valid arguments for cycle are 'on' or 'off'\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"lit") == 0 ) {
		action = lit_time;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for lit command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (atoi(sec_cmd) > 15) {
				fprintf(stderr, "Error: Maximum time is 15\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"dark") == 0 ) {
		action = dark_time;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for dark command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (atoi(sec_cmd) > 15) {
				fprintf(stderr, "Error: Maximum time is 15\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"pulse") == 0 ) {
		action = pulse_time;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for pulse command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (atoi(sec_cmd) > 63) {
				fprintf(stderr, "Error: maximum pulse time is 63\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"standby") == 0 ) {
		action = standby_time;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for standby command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (atoi(sec_cmd) > 63) {
				fprintf(stderr, "Error: maximum standby time is 63\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"backlight") == 0 ) {
		action = backlight;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for backlight command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (!is_on_off(sec_cmd)) {
				fprintf(stderr, "Error. Valid arguments for backlight are 'on' or 'off'\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"sensitivity") == 0 ) {
		action = sensitivity;
		if (n_addtl_cmds == 0) {
			fprintf(stderr, "Invalid number of arguments. Sensitivity requires X or Y be specified\n");
			HELP(-2);
		}

		if (n_addtl_cmds > 0) {
			if (sec_cmd[1] != '\0') {
				fprintf(stderr, "Invalid argument: next arg must be X or Y\n");
				HELP(-2);
			}
			if (sec_cmd[0] != 'x' && sec_cmd[0] != 'X' &&
			    sec_cmd[0] != 'y' && sec_cmd[0] != 'Y') {
				fprintf(stderr, "Invalid argument: next arg must be X or Y\n");
				HELP(-2);
			}
		}

		if (n_addtl_cmds == 2) {
			int value = atoi(third_cmd);
			if (value < -5 || value > 5) {
				fprintf(stderr, "Error: value out of range. Sensitivity must be between -5 and 5\n");
				HELP(-2);
			}
		} else if (n_addtl_cmds > 2) {
			fprintf(stderr, "Invalid number of arguments (too many)!\n");
			HELP(-2);
		}
	} else if (strcmp(command,"accel") == 0 ) {
		action = accel;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for accel command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			int val = atoi(sec_cmd);
			if (val < 0 || val > 255) {
				fprintf(stderr, "Error, value out of range (0-255)\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"dpi") == 0 ) {
		action = dpi_value;
		if (n_addtl_cmds == 0) {
			fprintf(stderr, "Invalid number of arguments. 'dpi' requires preset number to be specified (1-4)\n");
			HELP(-2);
		}

		int preset = atoi(sec_cmd);
		if (preset < 1 || preset > 4) {
			fprintf(stderr, "Invalid argument: preset must be in range 1-4\n");
			HELP(-2);
		}

		if (n_addtl_cmds == 2) {
			int value = atoi(third_cmd);
			if (value < 100 || value > 5600) {
				fprintf(stderr, "Error: value out of range. dpi value must be between 100 and 5600\n");
				HELP(-2);
			}
			if (value % 100 != 0) {
				fprintf(stderr, "Error: value must be an even multiple of 100\n");
				HELP(-2);
			}
		} else if (n_addtl_cmds > 2) {
			fprintf(stderr, "Invalid number of arguments (too many)!\n");
			HELP(-2);
		}
	} else if (strcmp(command,"dpiset") == 0 ) {
		action = dpi_active;
		if (n_addtl_cmds > 1) {
			fprintf(stderr, "Invalid number of arguments for dpiset command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			int val = atoi(sec_cmd);
			if (val < 1 || val > 4) {
				fprintf(stderr, "Error, value out of range (1-4)\n");
				exit(-2);
			}
		}
	} else if (strcmp(command,"poll") == 0 ) {
		action = poll_rates;
		if (n_addtl_cmds > 1){
			fprintf(stderr, "Invalid number of arguments for poll command.\n");
			HELP(-2);
		} else if (n_addtl_cmds == 1) {
			if (strcmp(sec_cmd,"1000") != 0 &&
			    strcmp(sec_cmd,"500") != 0 &&
			    strcmp(sec_cmd,"250") != 0 &&
			    strcmp(sec_cmd,"125") != 0) {
				fprintf(stderr, "Invalid polling rate '%s'. Must be one of: 1000,500,250,125\n", sec_cmd);
				exit(-2);
			}
		}
	} else if (strcmp(command,"reset") == 0 ) {
		action = factory_reset;
	} else if (strcmp(command,"dump") == 0 ) {
		action = read_info;
	} else if (strcmp(command,"load") == 0 ) {
		action = load_info;
	} else {
		fprintf(stderr, "%s is not a valid command.\n", command);
		HELP(-2);
	}

	if (action == NULL){
		fprintf(stderr, "Command not found or not implemented\n");
		return -1;
	}

	/*
		Setup USB, get device, perform commands, and close up shop
	*/

	err = initialize_usb();
	if (err < 0){
		finish_usb();
		return err;
	}

	err = find_device();
	if (err < 0){
		finish_usb();
		fprintf(stderr, "Error: Mouse not found, is it plugged in?.\n");
		return err;
	}
	VB_PRINT("Connected to mouse\n");

	if (profile == PROFILE_UNSET) {
		profile = get_active_profile()+1;
		VB_PRINT("No profile provided, using active profile %d if needed\n", profile );
	}

	err = action(n_addtl_cmds, argv+(optind+1), profile-1, verbose);
	if (err < 0){
		finish_usb();
		return err;
	}

	finish_usb();
	return 0;
}

static void help(int status, char *pgname) {
	const char *usage = "Usage: %s [OPTIONS] COMMAND [ARGUMENTS]\n"
			 "\tOptions:\n"
			 "\t\t-h,--help\tPrint this usage info and exit\n"
			 "\t\t-p,--profile=PROFILE\tSelect a particular profile\n"
			 "\t\t-v,--verbose\tIncrease debug output\n"
			 "\t\t-V,--version\tPrint program version info and exit\n"
			 "\tCommands:\n"
			 "\t\treset\tResets mouse to factory settings (careful!)\n"
			 "\t\tdump FILE\tSave current mouse configuration to FILE\n"
			 "\t\tload FILE\tRestore mouse configuration to settings in FILE\n"
			 "See man page for additional commands and settings options\n";
	printf(usage, pgname);
	exit(status);
}

static int valid_hex(char *s) {
	if (strlen(s) == 6 && strspn(s,"0123456789abcdefABCDEF") == 6){
		return 1;
	} else {
		return 0;
	}
}

static int is_on_off(char *s) {
	int len = strlen(s);

	if (len < 2 || len > 3) {
		return 0;
	}

	if (s[0] != 'o' && s[0] != 'O') {
		return 0;
	}


	if (s[1] == 'n' || s[1]=='N') {
		if (len == 3) {
			return 0;
		}
	} else if (s[1] == 'f' || s[1]=='F') {
		if (len == 2 || (s[2] != 'f' && s[2] != 'F')) {
			return 0;
		}
	} else {
		//second letter incorrect
		return 0;
	}


	return 1;
}
