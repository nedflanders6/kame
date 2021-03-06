/*	$NetBSD: gayle.c,v 1.1 2000/01/23 20:53:18 aymeric Exp $	*/

/*
 * Gayle management routines
 *
 *   Any module that uses gayle should call gayle_init() before using anything
 *   related to gayle. gayle_init() can be called multiple times.
 */

#include <amiga/amiga/gayle.h>
#include <amiga/dev/zbusvar.h>

struct gayle_struct *gayle_base_virtual_address;

#define GAYLE_PHYS_ADDRESS      0xda8000

void
gayle_init(void) {
	static int did_init = 0;

	if (did_init)
		return;
	did_init = 1;

	gayle_base_virtual_address =
		(struct gayle_struct *) ztwomap(GAYLE_PHYS_ADDRESS);
}
