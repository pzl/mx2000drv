#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "usb.h"
#include "mx.h"

#define PROFILE_UNSET -1
#define VB_PRINT(...) do { if (verbose){ fprintf(stderr, __VA_ARGS__); } } while (0)

static void help(int status, char *program_name);

int main(int argc, char **argv) {
	int opt,
		profile=PROFILE_UNSET,
		verbose=0,
		err;
	char *command;
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
				if (profile < 0 || profile > 4){
					fprintf(stderr, "Profile %s out of range (0-4)\n", optarg);
					help(-2,argv[0]);
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
				help(0,argv[0]);
				break;
			case -1: //no more args
			case 0:	//long options toggles
				break;
			case ':':
			case '?':
				help(-2,argv[0]);
				break;
			default:
				fprintf(stderr, "%s: invalid option -- %c\n", argv[0], opt);
				help(-2,argv[0]);
				break;
		}
	}

	/*
		Find command
	*/
	if (argc < 2){
		help(-2,argv[0]);
	}




	if (profile == PROFILE_UNSET) {
		profile = get_active_profile();
		VB_PRINT("No profile provided, using active profile %d if needed\n", profile);
	}


	initialize_usb();

	err = find_device();
	if (err < 0){
		finish_usb();
		return -1;
	}

	read_info();

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
