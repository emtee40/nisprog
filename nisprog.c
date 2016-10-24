/*
 *	nisprog - Nissan ECU communications utility
 *
 * Copyright (c) 2014-2016 fenugrec
 *
 * Licensed under GPLv3
 *
 * This standalone program compiles against components of freediag, and provides
 * Nissan-specific features aimed at dumping + reflashing ROMs.

 * This is experimental, and only tested on a handful of ECUs. So far it hasn't caused any permanent damage.

 * As-is, the code is of hack quality (low) and "trespasses" levels to go faster, skips some
 * checks, and is generally not robust. But it does work on a few different hardware setups and ECUs.
 */


//#include <stdlib.h>
#include <stdio.h>

#include "diag.h"
#include "diag_os.h"
#include "scantool_cli.h"

#include "nisprog.h"

FILE *dbg_stream=NULL;	//for nislib
enum npstate_t npstate;


#define NP_PROGNAME "nisprog"
const struct cmd_tbl_entry np_cmdtable[];

/*
 * Explain command line usage
 */
static void do_usage (void) {
	fprintf( stderr,	"nisprog utility for Nissan ECUs\n\n"
						"  Usage -\n"
						"	nisprog [-h] [-f <file]\n\n"
						"  Where:\n"
						"\t-h   -- Display this help message\n"
						"\t-f <file> Runs the commands from <file> at startup\n"
						"\n" ) ;
}


int np_init(void) {
	npstate = NP_DISC;
	return 0;
}

int main(int argc, char **argv) {
		int i ;
		const char *startfile=NULL;	/* optional commands to run at startup */

		dbg_stream = tmpfile();
		if (!dbg_stream) {
				printf("can't create temp file !\n");
				goto badexit;
		}

		for ( i = 1 ; i < argc ; i++ ) {
				if ( argv[i][0] == '-' || argv[i][0] == '+' ) {
						switch ( argv[i][1] ) {
								case 'f' :
										i++;
										if (i < argc) {
												startfile = argv[i];
										} else {
												do_usage();
												goto badexit;
										}
										break;
								case 'h' : do_usage() ; goto goodexit;
								default : do_usage() ; goto badexit;
						}
				} else {
						do_usage() ;
						goto badexit;
				}
		}

		np_init();

		enter_cli(NP_PROGNAME, startfile, np_cmdtable);

		/* Done */
goodexit:
		if (dbg_stream) fclose(dbg_stream);
		return 0;

badexit:
		if (dbg_stream) fclose(dbg_stream);
		return 1;
}

const struct cmd_tbl_entry np_cmdtable[]=
{
		{ "dumpmem", "dumpmem <file> <start> <#_of_bytes>", "(shorthand: \"dm\") dump memory from ROM/RAM address space\n"
								"\tExample: \"dm asdf.bin 0x1000 16\" will dump 0x10 bytes of ROM (0x1000-0x100F) to asdf.bin",
				cmd_dumpmem, 0, NULL},
		{ "dm", "dm <file> <start> <#_of_bytes>", "dump memory from ROM/RAM address space\n"
								"\tExample: \"dm asdf.bin 0x1000 16\" will dump 0x10 bytes of ROM (0x1000-0x100F) to asdf.bin",
				cmd_dumpmem, FLAG_HIDDEN, NULL},
		{ "npt", "npt [testnum]", "temporary / testing commands. Refer to source code",
				cmd_npt, 0, NULL},
		{ NULL, NULL, NULL, NULL, 0, NULL}
};

