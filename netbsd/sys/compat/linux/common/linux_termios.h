/*	$NetBSD: linux_termios.h,v 1.5 1998/12/15 19:31:40 itohy Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Eric Haszlakiewicz.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_TERMIOS_H
#define _LINUX_TERMIOS_H

struct linux_winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#define LINUX_NCC 8
struct linux_termio {
	unsigned short c_iflag;
	unsigned short c_oflag;
	unsigned short c_cflag;
	unsigned short c_lflag;
	unsigned char c_line;
	unsigned char c_cc[LINUX_NCC];
};

#define LINUX_NCCS 19
struct linux_termios {
	linux_tcflag_t	c_iflag;
	linux_tcflag_t	c_oflag;
	linux_tcflag_t	c_cflag;
	linux_tcflag_t	c_lflag;
	linux_cc_t	c_line;
	linux_cc_t	c_cc[LINUX_NCCS];
#if 0
	/* Present on some linux ports but unused: */
	linux_speed_t	c_ispeed;
	linux_speed_t	c_ospeed;
#endif
};

/* Linux modem line defines.. not sure if they'll be used */
#define LINUX_TIOCM_LE		0x0001
#define LINUX_TIOCM_DTR		0x0002
#define LINUX_TIOCM_RTS		0x0004
#define LINUX_TIOCM_ST		0x0008
#define LINUX_TIOCM_SR		0x0010
#define LINUX_TIOCM_CTS		0x0020
#define LINUX_TIOCM_CAR		0x0040
#define LINUX_TIOCM_RNG		0x0080
#define LINUX_TIOCM_DSR		0x0100
#define LINUX_TIOCM_CD		LINUX_TIOCM_CAR
#define LINUX_TIOCM_RI 		LINUX_TIOCM_RNG

#define	LINUX_TCIFLUSH		0
#define	LINUX_TCOFLUSH		1
#define	LINUX_TCIOFLUSH		2

#define	LINUX_TCOOFF		0
#define	LINUX_TCOON		1
#define	LINUX_TCIOFF		2
#define	LINUX_TCION		3

#define	LINUX_TCSANOW		0
#define	LINUX_TCSADRAIN		1
#define	LINUX_TCSAFLUSH		2

/* Linux line disciplines */
#define LINUX_N_TTY		0
#define LINUX_N_SLIP		1
#define LINUX_N_MOUSE		2
#define LINUX_N_PPP		3
#define	LINUX_N_STRIP		4

/* currently unused: */
#define	LINUX_N_AX25		5
#define	LINUX_N_X25		6
#define	LINUX_N_6PACK		7

#if defined(__i386__)
#include <compat/linux/arch/i386/linux_termios.h>
#elif defined(__m68k__)
#include <compat/linux/arch/m68k/linux_termios.h>
#elif defined(__alpha__)
#include <compat/linux/arch/alpha/linux_termios.h>
#else
#error Undefined linux_termios.h machine type.
#endif
#endif /* !_LINUX_TERMIOS_H */
