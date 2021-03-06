/* $NetBSD: kgdb_machdep.c,v 1.1 2001/05/14 18:23:02 drochner Exp $ */

/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratories.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)kgdb_stub.c	8.4 (Berkeley) 1/12/94
 */

/*
 * Machine-dependent part of the KGDB remote "stub"
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/kgdb.h>
#include <vm/vm.h>

void Debugger __P((void));

/*
 * XXX do kernacc call if safe, otherwise attempt
 * to simulate by simple bounds-checking.
 */
int kgdb_acc(va, ulen)
	vm_offset_t va;
	size_t ulen;
{
	extern void *proc0paddr;	/* XXX */
	extern void *kernel_map;		/* XXX! */

	if (kernel_map != NULL)
		return (kernacc((caddr_t)va, ulen, B_READ));
	if (va < (vm_offset_t)proc0paddr + USPACE)
	    return (1);
	return (0);
}

/*
 * Trap into kgdb to wait for debugger to connect,
 * noting on the console why nothing else is going on.
 */
void
kgdb_connect(verbose)
	int verbose;
{

	if (kgdb_dev < 0)
		return;

	if (verbose)
		printf("kgdb waiting...");

	Debugger();

	if (verbose)
		printf("connected.\n");

	kgdb_debug_panic = 1;
}

/*
 * Decide what to do on panic.
 *
 * The sun3 implementation of Debugger() arranges to call
 * either kgdb_trap() or kdb_trap() as appropriate, so
 * we can just call Debugger() here.
 */
void
kgdb_panic()
{
	if (kgdb_dev >= 0 && kgdb_debug_panic) {
		printf("entering kgdb\n");
		Debugger();
	}
}

void
Debugger()
{
	asm ("trap #15");
}

