/*	$KAME: mip6.h,v 1.60 2003/08/14 06:33:07 t-momose Exp $	*/

/*
 * Copyright (C) 2001 WIDE Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _MIP6_H_
#define _MIP6_H_

/* protocol constants. */
#define MIP6_HA_DEFAULT_LIFETIME   1800
#define MIP6_MAX_UPDATE_RATE       5
#define MIP6_MAX_PFX_ADV_DELAY     1000
#define MIP6_DHAAD_INITIAL_TIMEOUT 2
#define MIP6_DHAAD_RETRIES         3
#define MIP6_BA_INITIAL_TIMEOUT    1
#define MIP6_BA_MAX_TIMEOUT        256
#define MIP6_BU_MAX_BACKOFF        7
#define MIP6_MAX_MOB_PFX_ADV_INTERVAL	86400
#define MIP6_MIN_MOB_PFX_ADV_INTERVAL	  600

#endif /* !_MIP6_H_ */
