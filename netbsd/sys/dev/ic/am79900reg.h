/*	$NetBSD: am79900reg.h,v 1.2 1998/08/15 10:18:13 mycroft Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
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

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell and Rick Macklem.
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
 *	@(#)if_lereg.h	8.1 (Berkeley) 6/10/93
 */

/*
 * Receive message descriptor
 */
struct lermd {
	u_int32_t rmd0;
	u_int32_t rmd1;
	u_int32_t rmd2;
	int32_t rmd3;
};

/*
 * Transmit message descriptor
 */
struct letmd {
	u_int32_t tmd0;
	u_int32_t tmd1;
	u_int32_t tmd2;
	int32_t tmd3;
};

/*
 * Initialization block
 */
struct leinit {
	u_int32_t init_mode;		/* +0x0000 */
	u_int32_t init_padr[2];		/* +0x0002 */
	u_int16_t init_ladrf[4];	/* +0x0008 */
	u_int32_t init_rdra;		/* +0x0010 */
	u_int32_t init_tdra;		/* +0x0014 */
	int32_t	  pad;		/* Pad to 16 shorts */
};

/* Receive message descriptor 1 (rmd1_bits) */ 
#define	LE_R1_OWN	(1<<31)		/* LANCE owns the packet */
#define	LE_R1_ERR	(1<<30)		/* error summary */
#define	LE_R1_FRAM	(1<<29)		/* framing error */
#define	LE_R1_OFLO	(1<<28)		/* overflow error */
#define	LE_R1_CRC	(1<<27)		/* CRC error */
#define	LE_R1_BUFF	(1<<26)		/* buffer error */
#define	LE_R1_STP	(1<<25)		/* start of packet */
#define	LE_R1_ENP	(1<<24)		/* end of packet */
#define	LE_R1_ONES	(0xf<<12)		/* end of packet */

#define	LE_R1_BITS \
    "\20\40OWN\37ERR\36FRAM\35OFLO\34CRC\33BUFF\32STP\31ENP"

/* Transmit message descriptor 1 (tmd1_bits) */ 
#define	LE_T1_OWN	(1<<31)		/* LANCE owns the packet */
#define	LE_T1_ERR	(1<<30)		/* error summary */
#define	LE_T1_MORE	(1<<28)		/* multiple collisions */
#define	LE_T1_ONE	(1<<27)		/* single collision */
#define	LE_T1_DEF	(1<<26)		/* defferred transmit */
#define	LE_T1_STP	(1<<25)		/* start of packet */
#define	LE_T1_ENP	(1<<24)		/* end of packet */
#define	LE_T1_ONES	(0xf<<12)		/* end of packet */

#define	LE_T1_BITS \
    "\20\40OWN\37ERR\36RES\35MORE\34ONE\33DEF\32STP\31ENP"

/* Transmit message descriptor 3 (tmd3) */ 
#define	LE_T2_BUFF	(1<<31)		/* buffer error */
#define	LE_T2_UFLO	(1<<30)		/* underflow error */
#define	LE_T2_LCOL	(1<<28)		/* late collision */
#define	LE_T2_LCAR	(1<<27)		/* loss of carrier */
#define	LE_T2_RTRY	(1<<26)		/* retry error */
#if 0
#define	LE_T3_TDR_MASK	0x03ff		/* time domain reflectometry counter */
#endif

#define	LE_T3_BITS \
    "\12\40BUFF\37UFLO\35LCOL\34LCAR\33RTRY"
