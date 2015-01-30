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
		Parse command and arguments
	*/
	if (argv[optind] == NULL){
		fprintf(stderr, "A command is required. None provided\n");
		help(-2,argv[0]);
	}
	command = argv[optind];

	action = NULL;

	if (strcmp(command,"profile") == 0) {
		if (optind == argc-1){
			action = print_profile;
		} else if (optind == argc-2){
			unsigned char p = (unsigned char) atoi(argv[optind+1]);
			if (p > 0 && p <= 4) {
				action = change_profile;
			} else {
				fprintf(stderr, "Error: Profile number must be 1-4\n");
				help(-2,argv[0]);
			}
		} else {
			fprintf(stderr, "invalid number of arguments for 'profile' command\n");
			help(-2,argv[0]);
		}

	} else if (strcmp(command,"button") == 0 ) {

	} else if (strcmp(command,"color") == 0 ) {

	} else if (strcmp(command,"macro") == 0 ) {

	} else if (strcmp(command,"breathe") == 0 ) {

	} else if (strcmp(command,"cycle") == 0 ) {

	} else if (strcmp(command,"lit") == 0 ) {

	} else if (strcmp(command,"dark") == 0 ) {

	} else if (strcmp(command,"pulse") == 0 ) {

	} else if (strcmp(command,"standby") == 0 ) {

	} else if (strcmp(command,"backlight") == 0 ) {

	} else if (strcmp(command,"sensitivity") == 0 ) {

	} else if (strcmp(command,"accel") == 0 ) {

	} else if (strcmp(command,"dpi") == 0 ) {

	} else if (strcmp(command,"poll") == 0 ) {
		if (optind == argc -1){
			action = print_poll;
		} else if (optind == argc-2) {
			if (strcmp(argv[optind+1],"1000") == 0 ||
			    strcmp(argv[optind+1],"500") == 0 ||
			    strcmp(argv[optind+1],"250") == 0 ||
			    strcmp(argv[optind+1],"125") == 0) {
				action = change_poll;
			} else {
				fprintf(stderr, "Invalid polling rate '%s'. Must be one of: 1000,500,250,125\n", argv[optind+1]);
				exit(-2);
			}
		} else {
			fprintf(stderr, "invalid number of arguments for 'poll' command\n");
			help(-2,argv[0]);
		}

	} else if (strcmp(command,"reset") == 0 ) {
		action = factory_reset;
	} else if (strcmp(command,"dump") == 0 ) {
		action = read_info;
	} else if (strcmp(command,"load") == 0 ) {
		action = load_info;
	} else {
		fprintf(stderr, "%s is not a valid command.\n", command);
		help(-2, argv[0]);
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
		return -1;
	}

	err = find_device();
	if (err < 0){
		finish_usb();
		return -1;
	}

	if (profile == PROFILE_UNSET) {
		profile = get_active_profile()+1;
		VB_PRINT("No profile provided, using active profile %d if needed\n", profile );
	}

	err = action(argc - (optind+1), argv+(optind+1), profile-1);
	if (err < 0){
		finish_usb();
		return -1;
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
