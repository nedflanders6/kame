/*	$KAME: mip6config.h,v 1.6 2001/03/29 03:28:33 itojun Exp $	*/

/*
 * Copyright (c) 1999 Ericsson Radio Systems AB
 * All rights reserved.
 *
 * Author:  Magnus Braathen <magnus.braathen@era.ericsson.se>
 */

#define PROGNAME "mip6config" /* name of the program */

struct config_tmpl {
	char *comstring;
	int (*parse) __P((char *, u_long));
#if defined(__FreeBSD__) && __FreeBSD__ < 3
	int command;
#else
	u_long command;
#endif
};
