/* $NetBSD: waveform.h,v 1.2 1996/03/18 20:50:06 mark Exp $ */

/*
 * Copyright (c) 1994,1995 Mark Brinicombe.
 * Copyright (c) 1994 Brini.
 * All rights reserved.
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
 *	This product includes software developed by Brini.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRINI ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BRINI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

static unsigned char beep_waveform[] = {
    0x00, 0x06, 0x18, 0x2a, 0x3e, 0x4a, 0x54, 0x60,
    0x64, 0x68, 0x6a, 0x6a, 0x66, 0x62, 0x54, 0x3e,
    0x00, 0x45, 0x61, 0x6f, 0x7f, 0x87, 0x8d, 0x91,
    0x93, 0x95, 0x93, 0x91, 0x8b, 0x85, 0x73, 0x59,
    0x00, 0x5c, 0x78, 0x8a, 0x96, 0xa0, 0xa6, 0xa8,
    0xaa, 0xaa, 0xa8, 0xa6, 0xa2, 0x94, 0x86, 0x6a,
    0x00, 0x6b, 0x8b, 0x9d, 0xa7, 0xaf, 0xb5, 0xb9,
    0xbb, 0xbb, 0xb9, 0xb3, 0xad, 0xa5, 0x93, 0x77,
    0x00, 0x78, 0x96, 0xa8, 0xb2, 0xbc, 0xc2, 0xc4,
    0xc6, 0xc6, 0xc4, 0xc0, 0xb8, 0xac, 0x9e, 0x82,
    0x00, 0x83, 0xa3, 0xb1, 0xbf, 0xc5, 0xcb, 0xcd,
    0xcf, 0xcf, 0xcd, 0xc9, 0xc3, 0xb7, 0xa7, 0x89,
    0x00, 0x88, 0xa8, 0xb8, 0xc4, 0xcc, 0xd0, 0xd4,
    0xd6, 0xd4, 0xd2, 0xce, 0xc8, 0xbe, 0xac, 0x8e,
    0x00, 0x8f, 0xaf, 0xc3, 0xcb, 0xd3, 0xd9, 0xdd,
    0xdf, 0xdd, 0xdb, 0xd5, 0xcf, 0xc5, 0xb3, 0x95,
    0x00, 0x96, 0xb4, 0xc6, 0xd0, 0xd8, 0xe0, 0xe2,
    0xe4, 0xe2, 0xe2, 0xdc, 0xd2, 0xc8, 0xb8, 0x9a,
    0x00, 0x9d, 0xbb, 0xcb, 0xd7, 0xe1, 0xe5, 0xe7,
    0xe9, 0xe7, 0xe5, 0xe3, 0xd9, 0xcf, 0xbf, 0xa1,
    0x00, 0xa2, 0xc0, 0xce, 0xdc, 0xe4, 0xe8, 0xea,
    0xec, 0xea, 0xe8, 0xe4, 0xde, 0xd2, 0xc2, 0xa4,
    0x00, 0xa5, 0xc5, 0xd5, 0xe3, 0xe7, 0xed, 0xef,
    0xf1, 0xef, 0xed, 0xe9, 0xe3, 0xd7, 0xc7, 0xa9,
    0x00, 0xa8, 0xc6, 0xd8, 0xe4, 0xea, 0xee, 0xf2,
    0xf4, 0xf2, 0xf0, 0xec, 0xe6, 0xda, 0xc8, 0xaa,
    0x00, 0xab, 0xcb, 0xdd, 0xe7, 0xef, 0xf3, 0xf7,
    0xf9, 0xf7, 0xf5, 0xef, 0xe9, 0xdf, 0xcd, 0xaf,
    0x00, 0xae, 0xcc, 0xe0, 0xea, 0xf0, 0xf6, 0xfa,
    0xfc, 0xfa, 0xf8, 0xf2, 0xea, 0xe2, 0xd0, 0xb0,
    0x00, 0xb1, 0xd1, 0xe3, 0xed, 0xf5, 0xfb, 0xff,
    0xff, 0xff, 0xfb, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb4,
    0x00, 0xb5, 0xd3, 0xe5, 0xef, 0xf7, 0xfd, 0xff,
    0xff, 0xff, 0xfd, 0xf7, 0xef, 0xe5, 0xd3, 0xb5,
    0x00, 0xb4, 0xd2, 0xe4, 0xee, 0xf6, 0xfc, 0xfe,
    0xfe, 0xfe, 0xfc, 0xf6, 0xee, 0xe4, 0xd2, 0xb2,
    0x00, 0xb3, 0xd3, 0xe5, 0xef, 0xf7, 0xfb, 0xff,
    0xff, 0xff, 0xfb, 0xf5, 0xef, 0xe5, 0xd1, 0xb3,
    0x00, 0xb2, 0xd0, 0xe4, 0xec, 0xf4, 0xfa, 0xfe,
    0xfe, 0xfe, 0xfa, 0xf4, 0xec, 0xe4, 0xd0, 0xb2,
    0x00, 0xb3, 0xd1, 0xe5, 0xed, 0xf5, 0xfb, 0xff,
    0xff, 0xff, 0xfb, 0xf5, 0xed, 0xe5, 0xd1, 0xb3,
    0x00, 0xb2, 0xd0, 0xe4, 0xec, 0xf4, 0xfa, 0xfc,
    0xfe, 0xfc, 0xfa, 0xf4, 0xec, 0xe2, 0xd0, 0xb2,
    0x00, 0xb3, 0xd1, 0xe3, 0xed, 0xf5, 0xf9, 0xfd,
    0xff, 0xfd, 0xf9, 0xf5, 0xed, 0xe3, 0xd1, 0xb1,
    0x00, 0xb0, 0xd0, 0xe2, 0xec, 0xf2, 0xf8, 0xfc,
    0xfc, 0xfc, 0xf8, 0xf2, 0xec, 0xe2, 0xd0, 0xb0,
    0x00, 0xb1, 0xcf, 0xe3, 0xed, 0xf3, 0xf9, 0xfd,
    0xfd, 0xfd, 0xf9, 0xf3, 0xed, 0xe3, 0xcf, 0xb1,
    0x00, 0xb0, 0xce, 0xe2, 0xea, 0xf2, 0xf8, 0xfa,
    0xfc, 0xfa, 0xf8, 0xf2, 0xea, 0xe2, 0xce, 0xb0,
    0x00, 0xb1, 0xcf, 0xe3, 0xeb, 0xf3, 0xf9, 0xfb,
    0xfd, 0xfb, 0xf7, 0xf3, 0xeb, 0xe3, 0xcf, 0xb1,
    0x00, 0xb0, 0xce, 0xe2, 0xea, 0xf2, 0xf6, 0xfa,
    0xfa, 0xfa, 0xf6, 0xf2, 0xea, 0xe2, 0xce, 0xae,
    0x00, 0xaf, 0xcf, 0xe3, 0xeb, 0xf1, 0xf7, 0xfb,
    0xfb, 0xfb, 0xf7, 0xf1, 0xeb, 0xe1, 0xcf, 0xaf,
    0x00, 0xae, 0xce, 0xe0, 0xea, 0xf0, 0xf6, 0xf8,
    0xfa, 0xf8, 0xf6, 0xf0, 0xea, 0xe0, 0xcc, 0xae,
    0x00, 0xaf, 0xcd, 0xe1, 0xeb, 0xf1, 0xf7, 0xf9,
    0xfb, 0xf9, 0xf7, 0xf1, 0xe9, 0xe1, 0xcd, 0xaf,
    0x00, 0xae, 0xcc, 0xe0, 0xe8, 0xf0, 0xf4, 0xf8,
    0xf8, 0xf8, 0xf4, 0xf0, 0xe8, 0xe0, 0xcc, 0xae,
    0x00, 0xaf, 0xcd, 0xe1, 0xe9, 0xf1, 0xf5, 0xf9,
    0xf9, 0xf9, 0xf5, 0xf1, 0xe9, 0xdf, 0xcd, 0xad,
    0x00, 0xac, 0xcc, 0xde, 0xe8, 0xee, 0xf4, 0xf6,
    0xf8, 0xf6, 0xf4, 0xee, 0xe8, 0xde, 0xcc, 0xac,
    0x00, 0xad, 0xcd, 0xdf, 0xe9, 0xef, 0xf5, 0xf7,
    0xf9, 0xf7, 0xf5, 0xef, 0xe9, 0xdf, 0xcb, 0xad,
    0x00, 0xac, 0xca, 0xde, 0xe8, 0xee, 0xf2, 0xf6,
    0xf6, 0xf6, 0xf2, 0xee, 0xe8, 0xde, 0xca, 0xac,
    0x00, 0xad, 0xcb, 0xdd, 0xe7, 0xef, 0xf3, 0xf7,
    0xf7, 0xf7, 0xf3, 0xef, 0xe7, 0xdd, 0xcb, 0xad,
    0x00, 0xac, 0xca, 0xdc, 0xe6, 0xee, 0xf2, 0xf4,
    0xf6, 0xf4, 0xf2, 0xee, 0xe6, 0xdc, 0xca, 0xaa,
    0x00, 0xab, 0xcb, 0xdd, 0xe7, 0xed, 0xf3, 0xf5,
    0xf7, 0xf5, 0xf3, 0xed, 0xe7, 0xdd, 0xcb, 0xab,
    0x00, 0xaa, 0xca, 0xdc, 0xe6, 0xec, 0xf2, 0xf4,
    0xf4, 0xf4, 0xf0, 0xec, 0xe6, 0xda, 0xc8, 0xaa,
    0x00, 0xab, 0xc9, 0xdb, 0xe7, 0xed, 0xf1, 0xf5,
    0xf5, 0xf5, 0xf1, 0xed, 0xe7, 0xdb, 0xc9, 0xab,
    0x00, 0xaa, 0xc8, 0xda, 0xe6, 0xec, 0xf0, 0xf4,
    0xf4, 0xf2, 0xf0, 0xec, 0xe6, 0xda, 0xc8, 0xaa,
    0x00, 0xab, 0xc9, 0xdb, 0xe5, 0xed, 0xf1, 0xf3,
    0xf5, 0xf3, 0xf1, 0xeb, 0xe5, 0xdb, 0xc9, 0xa9,
    0x00, 0xa8, 0xc8, 0xd8, 0xe4, 0xea, 0xf0, 0xf2,
    0xf2, 0xf2, 0xf0, 0xea, 0xe4, 0xd8, 0xc8, 0xa8,
    0x00, 0xa9, 0xc9, 0xd9, 0xe5, 0xeb, 0xef, 0xf3,
    0xf3, 0xf3, 0xef, 0xeb, 0xe5, 0xd9, 0xc9, 0xa9,
    0x00, 0xa8, 0xc6, 0xd8, 0xe4, 0xea, 0xee, 0xf2,
    0xf2, 0xf2, 0xee, 0xea, 0xe4, 0xd8, 0xc6, 0xa8,
    0x00, 0xa9, 0xc7, 0xd9, 0xe5, 0xeb, 0xef, 0xf1,
    0xf3, 0xf1, 0xef, 0xeb, 0xe5, 0xd7, 0xc7, 0xa9,
    0x00, 0xa8, 0xc6, 0xd6, 0xe4, 0xea, 0xee, 0xf0,
    0xf0, 0xf0, 0xee, 0xe8, 0xe2, 0xd6, 0xc6, 0xa8,
    0x00, 0xa7, 0xc7, 0xd7, 0xe3, 0xe9, 0xed, 0xf1,
    0xf1, 0xf1, 0xed, 0xe9, 0xe3, 0xd7, 0xc7, 0xa7,
    0x00, 0xa6, 0xc6, 0xd6, 0xe2, 0xe8, 0xec, 0xf0,
    0xf0, 0xf0, 0xec, 0xe8, 0xe2, 0xd6, 0xc6, 0xa6,
    0x00, 0xa7, 0xc5, 0xd7, 0xe3, 0xe9, 0xed, 0xef,
    0xf1, 0xef, 0xed, 0xe9, 0xe3, 0xd5, 0xc5, 0xa7,
    0x00, 0xa6, 0xc4, 0xd4, 0xe2, 0xe8, 0xec, 0xee,
    0xee, 0xee, 0xec, 0xe8, 0xe2, 0xd4, 0xc4, 0xa6,
    0x00, 0xa7, 0xc5, 0xd5, 0xe3, 0xe7, 0xed, 0xef,
    0xef, 0xef, 0xeb, 0xe7, 0xe3, 0xd5, 0xc5, 0xa7,
    0x00, 0xa6, 0xc4, 0xd4, 0xe0, 0xe6, 0xea, 0xee,
    0xee, 0xee, 0xea, 0xe6, 0xe0, 0xd4, 0xc4, 0xa4,
    0x00, 0xa5, 0xc5, 0xd3, 0xe1, 0xe7, 0xeb, 0xed,
    0xef, 0xed, 0xeb, 0xe7, 0xe1, 0xd3, 0xc5, 0xa5,
    0x00, 0xa4, 0xc4, 0xd2, 0xe0, 0xe6, 0xea, 0xec,
    0xec, 0xec, 0xea, 0xe6, 0xe0, 0xd2, 0xc2, 0xa4,
    0x00, 0xa5, 0xc3, 0xd3, 0xdf, 0xe7, 0xeb, 0xed,
    0xed, 0xed, 0xeb, 0xe7, 0xdf, 0xd3, 0xc3, 0xa5,
    0x00, 0xa4, 0xc2, 0xd2, 0xde, 0xe4, 0xe8, 0xec,
    0xec, 0xec, 0xe8, 0xe4, 0xde, 0xd0, 0xc2, 0xa4,
    0x00, 0xa5, 0xc3, 0xd1, 0xdf, 0xe5, 0xe9, 0xeb,
    0xed, 0xeb, 0xe9, 0xe5, 0xdf, 0xd1, 0xc3, 0xa3,
    0x00, 0xa2, 0xc2, 0xd0, 0xdc, 0xe4, 0xe8, 0xea,
    0xea, 0xea, 0xe8, 0xe4, 0xdc, 0xd0, 0xc2, 0xa2,
    0x00, 0xa3, 0xc3, 0xd1, 0xdd, 0xe5, 0xe9, 0xeb,
    0xeb, 0xeb, 0xe9, 0xe5, 0xdd, 0xd1, 0xc1, 0xa3,
    0x00, 0xa2, 0xc0, 0xce, 0xdc, 0xe4, 0xe6, 0xea,
    0xea, 0xea, 0xe6, 0xe4, 0xda, 0xce, 0xc0, 0xa2,
    0x00, 0xa3, 0xc1, 0xcf, 0xdb, 0xe3, 0xe7, 0xe9,
    0xeb, 0xe9, 0xe7, 0xe3, 0xdb, 0xcf, 0xc1, 0xa3,
    0x00, 0xa2, 0xc0, 0xce, 0xda, 0xe2, 0xe6, 0xe8,
    0xe8, 0xe8, 0xe6, 0xe2, 0xda, 0xce, 0xbe, 0xa0,
    0x00, 0xa1, 0xbf, 0xcf, 0xdb, 0xe3, 0xe7, 0xe9,
    0xe9, 0xe9, 0xe7, 0xe3, 0xd9, 0xcd, 0xbf, 0xa1,
    0x00, 0xa0, 0xbe, 0xcc, 0xd8, 0xe2, 0xe6, 0xe8,
    0xe8, 0xe8, 0xe4, 0xe2, 0xd8, 0xcc, 0xbc, 0x9e,
    0x00, 0x9f, 0xbd, 0xcd, 0xd9, 0xe3, 0xe5, 0xe7,
    0xe9, 0xe7, 0xe5, 0xe1, 0xd9, 0xcd, 0xbd, 0x9f,
    0x00, 0x9e, 0xbc, 0xcc, 0xd8, 0xe0, 0xe4, 0xe6,
    0xe6, 0xe6, 0xe4, 0xe0, 0xd6, 0xcc, 0xbc, 0x9e,
    0x00, 0x9f, 0xbd, 0xcd, 0xd7, 0xe1, 0xe5, 0xe7,
    0xe7, 0xe7, 0xe5, 0xe1, 0xd7, 0xcb, 0xbb, 0x9d,
    0x00, 0x9c, 0xba, 0xca, 0xd6, 0xde, 0xe4, 0xe6,
    0xe6, 0xe6, 0xe4, 0xde, 0xd6, 0xca, 0xba, 0x9c,
    0x00, 0x9d, 0xbb, 0xcb, 0xd5, 0xdf, 0xe3, 0xe5,
    0xe7, 0xe5, 0xe3, 0xdf, 0xd5, 0xcb, 0xbb, 0x9d,
    0x00, 0x9a, 0xb8, 0xca, 0xd4, 0xde, 0xe2, 0xe4,
    0xe4, 0xe4, 0xe2, 0xdc, 0xd4, 0xca, 0xb8, 0x9a,
    0x00, 0x9b, 0xb9, 0xc9, 0xd5, 0xdd, 0xe3, 0xe5,
    0xe5, 0xe5, 0xe3, 0xdd, 0xd5, 0xc9, 0xb9, 0x9b,
    0x00, 0x9a, 0xb8, 0xc8, 0xd2, 0xdc, 0xe2, 0xe4,
    0xe4, 0xe4, 0xe2, 0xdc, 0xd2, 0xc8, 0xb6, 0x98,
    0x00, 0x99, 0xb7, 0xc9, 0xd3, 0xdb, 0xe1, 0xe3,
    0xe5, 0xe3, 0xe1, 0xdb, 0xd3, 0xc9, 0xb7, 0x99,
    0x00, 0x98, 0xb6, 0xc8, 0xd2, 0xda, 0xe0, 0xe2,
    0xe2, 0xe2, 0xe0, 0xda, 0xd2, 0xc6, 0xb6, 0x98,
    0x00, 0x99, 0xb7, 0xc7, 0xd1, 0xd9, 0xdf, 0xe3,
    0xe3, 0xe3, 0xdf, 0xd9, 0xd1, 0xc7, 0xb5, 0x97,
    0x00, 0x96, 0xb4, 0xc6, 0xd0, 0xd8, 0xde, 0xe2,
    0xe2, 0xe2, 0xde, 0xd8, 0xd0, 0xc6, 0xb4, 0x96,
    0x00, 0x97, 0xb5, 0xc7, 0xd1, 0xd9, 0xdf, 0xe1,
    0xe3, 0xe1, 0xdd, 0xd7, 0xcf, 0xc7, 0xb3, 0x95,
    0x00, 0x94, 0xb2, 0xc4, 0xce, 0xd6, 0xdc, 0xe0,
    0xe0, 0xe0, 0xdc, 0xd6, 0xce, 0xc4, 0xb2, 0x94,
    0x00, 0x95, 0xb3, 0xc5, 0xcf, 0xd7, 0xdd, 0xdf,
    0xe1, 0xdf, 0xdd, 0xd7, 0xcf, 0xc5, 0xb3, 0x95,
    0x00, 0x94, 0xb2, 0xc4, 0xce, 0xd4, 0xda, 0xde,
    0xde, 0xde, 0xda, 0xd4, 0xcc, 0xc4, 0xb0, 0x92,
    0x00, 0x93, 0xb1, 0xc5, 0xcd, 0xd5, 0xdb, 0xdd,
    0xdf, 0xdd, 0xdb, 0xd5, 0xcd, 0xc3, 0xb1, 0x93,
    0x00, 0x92, 0xb0, 0xc2, 0xcc, 0xd4, 0xd8, 0xdc,
    0xdc, 0xdc, 0xd8, 0xd2, 0xcc, 0xc2, 0xb0, 0x90,
    0x00, 0x91, 0xaf, 0xc3, 0xcb, 0xd3, 0xd9, 0xdb,
    0xdd, 0xdb, 0xd9, 0xd3, 0xcb, 0xc3, 0xaf, 0x91,
    0x00, 0x90, 0xae, 0xc2, 0xca, 0xd2, 0xd6, 0xda,
    0xda, 0xda, 0xd6, 0xd2, 0xca, 0xc2, 0xae, 0x90,
    0x00, 0x91, 0xaf, 0xc3, 0xcb, 0xd1, 0xd7, 0xd9,
    0xdb, 0xd9, 0xd7, 0xd1, 0xcb, 0xc1, 0xad, 0x8f,
    0x00, 0x8e, 0xac, 0xc0, 0xc8, 0xd0, 0xd4, 0xd8,
    0xd8, 0xd8, 0xd4, 0xd0, 0xc8, 0xc0, 0xac, 0x8e,
    0x00, 0x8f, 0xad, 0xbf, 0xc9, 0xcf, 0xd5, 0xd7,
    0xd9, 0xd7, 0xd5, 0xcf, 0xc9, 0xbf, 0xad, 0x8f,
    0x00, 0x8c, 0xac, 0xbe, 0xc8, 0xce, 0xd2, 0xd6,
    0xd6, 0xd6, 0xd2, 0xce, 0xc8, 0xbe, 0xaa, 0x8c,
    0x00, 0x8d, 0xab, 0xbd, 0xc7, 0xcf, 0xd3, 0xd5,
    0xd7, 0xd5, 0xd3, 0xcd, 0xc7, 0xbd, 0xab, 0x8d,
    0x00, 0x8c, 0xaa, 0xbc, 0xc6, 0xcc, 0xd2, 0xd4,
    0xd4, 0xd4, 0xd0, 0xcc, 0xc6, 0xba, 0xaa, 0x8a,
    0x00, 0x8b, 0xa9, 0xbb, 0xc7, 0xcd, 0xd1, 0xd5,
    0xd5, 0xd3, 0xd1, 0xcd, 0xc5, 0xbb, 0xa9, 0x8b,
    0x00, 0x8a, 0xa8, 0xba, 0xc4, 0xca, 0xd0, 0xd2,
    0xd2, 0xd2, 0xd0, 0xca, 0xc4, 0xb8, 0xa8, 0x8a,
    0x00, 0x8b, 0xa9, 0xb9, 0xc5, 0xcb, 0xcf, 0xd3,
    0xd3, 0xd1, 0xcf, 0xcb, 0xc5, 0xb9, 0xa7, 0x89,
    0x00, 0x88, 0xa6, 0xb8, 0xc4, 0xca, 0xce, 0xd0,
    0xd0, 0xd0, 0xce, 0xc8, 0xc2, 0xb6, 0xa6, 0x88,
    0x00, 0x89, 0xa7, 0xb7, 0xc3, 0xc9, 0xcd, 0xd1,
    0xd1, 0xcf, 0xcd, 0xc9, 0xc3, 0xb7, 0xa7, 0x87,
    0x00, 0x86, 0xa6, 0xb4, 0xc2, 0xc8, 0xcc, 0xce,
    0xce, 0xce, 0xcc, 0xc8, 0xc2, 0xb4, 0xa4, 0x86,
    0x00, 0x87, 0xa5, 0xb5, 0xc3, 0xc7, 0xcb, 0xcf,
    0xcf, 0xcf, 0xcb, 0xc7, 0xc1, 0xb5, 0xa5, 0x87,
    0x00, 0x86, 0xa4, 0xb2, 0xc0, 0xc6, 0xca, 0xcc,
    0xcc, 0xcc, 0xca, 0xc6, 0xc0, 0xb2, 0xa2, 0x84,
    0x00, 0x85, 0xa3, 0xb3, 0xbf, 0xc5, 0xc9, 0xcd,
    0xcd, 0xcd, 0xc9, 0xc5, 0xbf, 0xb1, 0xa3, 0x85,
    0x00, 0x84, 0xa2, 0xb0, 0xbc, 0xc4, 0xc8, 0xca,
    0xca, 0xca, 0xc8, 0xc4, 0xbc, 0xb0, 0xa2, 0x82,
    0x00, 0x83, 0xa3, 0xb1, 0xbd, 0xc5, 0xc7, 0xcb,
    0xcb, 0xcb, 0xc7, 0xc3, 0xbb, 0xaf, 0xa1, 0x83,
    0x00, 0x82, 0xa0, 0xae, 0xba, 0xc2, 0xc6, 0xc8,
    0xc8, 0xc8, 0xc6, 0xc2, 0xba, 0xae, 0x9e, 0x82,
    0x00, 0x83, 0x9f, 0xaf, 0xb9, 0xc3, 0xc7, 0xc9,
    0xc9, 0xc9, 0xc5, 0xc3, 0xb9, 0xad, 0x9f, 0x81,
    0x00, 0x80, 0x9c, 0xac, 0xb8, 0xc0, 0xc4, 0xc6,
    0xc6, 0xc6, 0xc4, 0xc0, 0xb6, 0xac, 0x9c, 0x7e,
    0x00, 0x7f, 0x9d, 0xab, 0xb7, 0xbf, 0xc5, 0xc7,
    0xc7, 0xc7, 0xc5, 0xbf, 0xb7, 0xab, 0x9b, 0x7d,
    0x00, 0x7c, 0x9a, 0xaa, 0xb4, 0xbe, 0xc2, 0xc4,
    0xc4, 0xc4, 0xc2, 0xbc, 0xb4, 0xaa, 0x98, 0x7c,
    0x00, 0x7d, 0x99, 0xa9, 0xb5, 0xbd, 0xc3, 0xc5,
    0xc5, 0xc5, 0xc3, 0xbb, 0xb3, 0xa9, 0x99, 0x7b,
    0x00, 0x7a, 0x96, 0xa8, 0xb2, 0xba, 0xc0, 0xc2,
    0xc2, 0xc2, 0xc0, 0xba, 0xb0, 0xa6, 0x96, 0x78,
    0x00, 0x79, 0x97, 0xa7, 0xb1, 0xb9, 0xbf, 0xc3,
    0xc3, 0xc3, 0xbf, 0xb9, 0xb1, 0xa7, 0x95, 0x77,
    0x00, 0x76, 0x94, 0xa6, 0xae, 0xb6, 0xbc, 0xc0,
    0xc0, 0xc0, 0xbc, 0xb6, 0xae, 0xa4, 0x92, 0x76,
    0x00, 0x75, 0x93, 0xa5, 0xaf, 0xb5, 0xbb, 0xbf,
    0xbf, 0xbf, 0xbb, 0xb5, 0xad, 0xa5, 0x91, 0x75,
    0x00, 0x74, 0x90, 0xa4, 0xac, 0xb4, 0xb8, 0xbc,
    0xbc, 0xbc, 0xb8, 0xb2, 0xac, 0xa2, 0x90, 0x72,
    0x00, 0x73, 0x91, 0xa3, 0xab, 0xb3, 0xb7, 0xbb,
    0xbb, 0xbb, 0xb7, 0xb1, 0xab, 0xa3, 0x8f, 0x71,
    0x00, 0x70, 0x8e, 0xa0, 0xaa, 0xb0, 0xb4, 0xb8,
    0xb8, 0xb8, 0xb4, 0xb0, 0xa8, 0xa0, 0x8c, 0x6e,
    0x00, 0x6f, 0x8d, 0x9f, 0xa9, 0xaf, 0xb5, 0xb7,
    0xb7, 0xb7, 0xb3, 0xaf, 0xa9, 0x9f, 0x8b, 0x6f,
    0x00, 0x6e, 0x8a, 0x9c, 0xa6, 0xac, 0xb2, 0xb4,
    0xb4, 0xb4, 0xb0, 0xac, 0xa6, 0x9a, 0x8a, 0x6c,
    0x00, 0x6d, 0x89, 0x9b, 0xa5, 0xab, 0xb1, 0xb3,
    0xb3, 0xb3, 0xaf, 0xab, 0xa5, 0x99, 0x89, 0x6b,
    0x00, 0x6a, 0x88, 0x98, 0xa4, 0xaa, 0xae, 0xb0,
    0xb0, 0xb0, 0xae, 0xa8, 0xa2, 0x96, 0x86, 0x68,
    0x00, 0x69, 0x87, 0x97, 0xa3, 0xa9, 0xad, 0xaf,
    0xaf, 0xaf, 0xad, 0xa7, 0xa3, 0x95, 0x85, 0x69,
    0x00, 0x66, 0x84, 0x94, 0xa0, 0xa6, 0xaa, 0xac,
    0xac, 0xac, 0xaa, 0xa6, 0x9e, 0x92, 0x84, 0x66,
    0x00, 0x67, 0x83, 0x93, 0x9f, 0xa5, 0xa9, 0xab,
    0xab, 0xab, 0xa9, 0xa5, 0x9d, 0x91, 0x83, 0x65,
    0x00, 0x64, 0x82, 0x8e, 0x9a, 0xa2, 0xa6, 0xa8,
    0xa8, 0xa8, 0xa6, 0xa2, 0x9a, 0x8e, 0x80, 0x62,
    0x00, 0x63, 0x7f, 0x8d, 0x99, 0xa1, 0xa5, 0xa7,
    0xa7, 0xa7, 0xa5, 0xa1, 0x97, 0x8d, 0x7d, 0x61,
    0x00, 0x60, 0x7c, 0x8a, 0x94, 0x9e, 0xa2, 0xa4,
    0xa4, 0xa4, 0xa2, 0x9c, 0x94, 0x8a, 0x7a, 0x5e,
    0x00, 0x5f, 0x79, 0x89, 0x93, 0x9b, 0xa1, 0xa3,
    0xa3, 0xa3, 0xa1, 0x9b, 0x91, 0x87, 0x77, 0x5b,
    0x00, 0x5a, 0x76, 0x86, 0x90, 0x96, 0x9c, 0xa0,
    0xa0, 0xa0, 0x9c, 0x96, 0x8e, 0x84, 0x72, 0x58,
    0x00, 0x59, 0x73, 0x85, 0x8d, 0x95, 0x99, 0x9d,
    0x9d, 0x9d, 0x99, 0x93, 0x8d, 0x83, 0x71, 0x55,
    0x00, 0x54, 0x70, 0x82, 0x8a, 0x90, 0x96, 0x98,
    0x98, 0x98, 0x94, 0x90, 0x88, 0x80, 0x6c, 0x52,
    0x00, 0x51, 0x6d, 0x7f, 0x87, 0x8d, 0x93, 0x95,
    0x95, 0x95, 0x91, 0x8d, 0x87, 0x7b, 0x6b, 0x4f,
    0x00, 0x4e, 0x6a, 0x7a, 0x84, 0x8a, 0x8e, 0x90,
    0x90, 0x90, 0x8c, 0x88, 0x82, 0x76, 0x66, 0x4a,
    0x00, 0x4b, 0x67, 0x75, 0x83, 0x87, 0x8b, 0x8d,
    0x8d, 0x8d, 0x8b, 0x87, 0x7f, 0x73, 0x65, 0x49,
    0x00, 0x48, 0x62, 0x70, 0x7c, 0x82, 0x86, 0x88,
    0x88, 0x88, 0x86, 0x82, 0x78, 0x6e, 0x60, 0x44,
    0x00, 0x45, 0x5f, 0x6d, 0x77, 0x7f, 0x83, 0x85,
    0x85, 0x85, 0x83, 0x7d, 0x75, 0x6b, 0x5b, 0x43,
    0x00, 0x42, 0x58, 0x68, 0x70, 0x78, 0x7c, 0x80,
    0x80, 0x7e, 0x7c, 0x76, 0x6e, 0x64, 0x54, 0x3c,
    0x00, 0x3d, 0x53, 0x63, 0x6b, 0x73, 0x77, 0x79,
    0x79, 0x79, 0x75, 0x6f, 0x69, 0x61, 0x4f, 0x37,
    0x00, 0x34, 0x4c, 0x5c, 0x66, 0x6a, 0x6e, 0x70,
    0x70, 0x70, 0x6c, 0x68, 0x62, 0x56, 0x48, 0x30,
    0x00, 0x2f, 0x47, 0x55, 0x5f, 0x65, 0x67, 0x69,
    0x69, 0x69, 0x65, 0x63, 0x59, 0x4f, 0x43, 0x2b,
    0x00, 0x28, 0x40, 0x4a, 0x52, 0x5a, 0x5e, 0x60,
    0x60, 0x5e, 0x5a, 0x54, 0x4c, 0x44, 0x36, 0x22,
    0x00, 0x23, 0x33, 0x43, 0x49, 0x4d, 0x4f, 0x51,
    0x51, 0x4f, 0x4d, 0x47, 0x43, 0x37, 0x2b, 0x19,
    0x00, 0x16, 0x26, 0x30, 0x36, 0x3c, 0x40, 0x40,
    0x40, 0x3c, 0x38, 0x32, 0x2c, 0x24, 0x1a, 0x0c,
    0x00, 0x0b, 0x15, 0x1b, 0x21, 0x23, 0x23, 0x21,
    0x1f, 0x19, 0x15, 0x0f, 0x09, 0x05, 0x03, 0x00,
};

/* End of waveform.h */
