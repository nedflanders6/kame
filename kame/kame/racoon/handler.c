/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
/* YIPS @(#)$Id: handler.c,v 1.12 2000/01/12 04:08:13 itojun Exp $ */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "var.h"
#include "misc.h"
#include "vmbuf.h"
#include "plog.h"
#include "sockmisc.h"
#include "debug.h"

#include "schedule.h"
#include "policy.h"
#include "isakmp_var.h"
#include "isakmp.h"
#include "oakley.h"
#include "handler.h"

static LIST_HEAD(_ph1tree_, ph1handle) ph1tree;
static LIST_HEAD(_ph2tree_, ph2handle) ph2tree;

/*
 * functions about management of the isakmp status table
 */
/* %%% management phase 1 handler */
/*
 * search for isakmpsa handler with isakmp index.
 */
struct ph1handle *
getph1byindex(index)
	isakmp_index *index;
{
	struct ph1handle *p;

	LIST_FOREACH(p, &ph1tree, chain) {
		if (memcmp(&p->index, index, sizeof(*index)) == 0)
			return p;
	}

	return NULL;
}

/*
 * search for isakmp handler by i_ck in index.
 */
struct ph1handle *
getph1byindex0(index)
	isakmp_index *index;
{
	struct ph1handle *p;

	LIST_FOREACH(p, &ph1tree, chain) {
		if (memcmp(&p->index, index, sizeof(cookie_t)) == 0)
			return p;
	}

	return NULL;
}

/*
 * search for isakmpsa handler by remote address.
 * don't use port number to search because this function search
 * with phase 2's destinaion.
 */
struct ph1handle *
getph1byaddr(remote)
	struct sockaddr *remote;
{
	struct ph1handle *p;

	LIST_FOREACH(p, &ph1tree, chain) {
		if (cmpsaddrwop(remote, p->remote) == 0)
			return p;
	}

	return NULL;
}

/*
 * dump isakmp-sa
 */
vchar_t *
dumpph1(proto)
	u_int proto;
{
#if 0
	struct ph1handle *iph1;
	struct ph2handle *iph2;
	int cnt = 0;
	vchar_t *buf;
	caddr_t bufp;

	/* get length of buffer */
	LIST_FOREACH(p, &ph1tree, chain)
		cnt++;

	tlen = (sizeof(struct ph1handle) * ph1tab.len);
	for (iph1 = ph1tab.head;
	     iph1 != NULL;
	     iph1 = iph1->next) {

		tlen += iph1->remote->sa_len;
		tlen += iph1->local->sa_len;

		tlen += (sizeof(struct ph2handle) * iph1->ph2tab.len);
		for (iph2 = iph1->ph2tab.head;
		     iph2 != NULL;
		     iph2 = iph2->next) {

			if (iph2->pst == NULL)
				continue;

			tlen += iph2->pst->src->sa_len;
			tlen += iph2->pst->dst->sa_len;
		}
	}

	buf = vmalloc(tlen);
	if (buf == NULL) {
		plog(logp, LOCATION, NULL,
			"vmalloc(%s)\n", strerror(errno));
		return NULL;
	}
	bufp = buf->v;

	for (iph1 = ph1tab.head;
	     iph1 != NULL;
	     iph1 = iph1->next) {

		/* copy ph1 entry */
		memcpy(bufp, iph1, sizeof(*iph1));
		bufp += sizeof(*iph1);
		memcpy(bufp, iph1->local, iph1->local->sa_len);
		bufp += iph1->local->sa_len;
		memcpy(bufp, iph1->remote, iph1->remote->sa_len);
		bufp += iph1->remote->sa_len;

		/* copy ph2 entries */
		for (iph2 = iph1->ph2tab.head;
		     iph2 != NULL;
		     iph2 = iph2->next) {

			/* copy ph2 entry */
			memcpy(bufp, iph2, sizeof(*iph2));
			bufp += sizeof(*iph2);

			if (iph2->pst == NULL)
				continue;

			memcpy(bufp, iph2->pst->src, iph2->pst->src->sa_len);
			bufp += iph2->pst->src->sa_len;
			memcpy(bufp, iph2->pst->dst, iph2->pst->dst->sa_len);
			bufp += iph2->pst->dst->sa_len;
		}
	}

	return buf;
#endif
	return NULL;
}

/*
 * create new isakmp Phase 1 status record to handle isakmp in Phase1
 */
struct ph1handle *
newph1()
{
	struct ph1handle *iph1;

	/* create new iph1 */
	iph1 = CALLOC(sizeof(*iph1), struct ph1handle *);
	if (iph1 == NULL)
		return NULL;

	iph1->status = PHASE1ST_SPAWN;

	return iph1;
}

/*
 * delete new isakmp Phase 1 status record to handle isakmp in Phase1
 */
void
delph1(iph1)
	struct ph1handle *iph1;
{
	if (iph1->ivm) {
		oakley_delivm(iph1->ivm);
		iph1->ivm = NULL;
	}
	if (iph1->sce)
		SCHED_KILL(iph1->sce);
	if (iph1->scr)
		SCHED_KILL(iph1->scr);
	/* XXX do more free parameters */
	free(iph1);
}

/*
 * create new isakmp Phase 1 status record to handle isakmp in Phase1
 */
int
insph1(iph1)
	struct ph1handle *iph1;
{
	/* validity check */
	if (iph1->remote == NULL) {
		plog(logp, LOCATION, NULL,
			"invalid isakmp SA handler. no remote address.\n");
		return -1;
	}
	LIST_INSERT_HEAD(&ph1tree, iph1, chain);

	return 0;
}

void
remph1(iph1)
	struct ph1handle *iph1;
{
	/* XXX unbindph2() or remph2() should be called */

	LIST_REMOVE(iph1, chain);
}

/*
 * flush isakmp-sa
 */
void
flushph1(proto)
	u_int proto;
{
	struct ph1handle *p, *next;

	/* get length of buffer */
	for (p = LIST_FIRST(&ph1tree); p; p = next) {
		next = LIST_NEXT(p, chain);
		delph1(p);
	}

	return;
}

void
initph1tree()
{
	LIST_INIT(&ph1tree);
}

/* %%% management phase 2 handler */
/*
 * search ph2handle with policyindex.
 */
struct ph2handle *
getph2byspidx(spidx)
	struct policyindex *spidx;
{
	struct ph2handle *p;

	LIST_FOREACH(p, &ph2tree, chain) {
		/*
		 * there are ph2handle independent on policy
		 * such like informational exchange.
		 */
		if (p->spidx == NULL)
			continue;
		if (cmpspidx(p->spidx, spidx) == 0)
			return p;
	}

	return NULL;
}

/*
 * search ph2handle with sequence number.
 */
struct ph2handle *
getph2byseq(seq)
	u_int32_t seq;
{
	struct ph2handle *p;

	LIST_FOREACH(p, &ph2tree, chain) {
		if (p->seq == seq)
			return p;
	}

	return NULL;
}

/*
 * search ph2handle with message id.
 */
struct ph2handle *
getph2bymsgid(iph1, msgid)
	struct ph1handle *iph1;
	u_int32_t msgid;
{
	struct ph2handle *p;

	LIST_FOREACH(p, &ph2tree, chain) {
		if (p->msgid == msgid)
			return p;
	}

	return NULL;
}

struct ph2handle *
getph2bysaidx(src, dst, proto_id, spi)
	struct sockaddr *src, *dst;
	u_int proto_id;
	u_int32_t spi;
{
	struct ph2handle *p;
	struct ipsecsakeys *k;

	LIST_FOREACH(p, &ph2tree, chain) {
		if (p->keys == NULL)
			continue;
		for (k = p->keys; k != NULL; k = k->next) {
			if (proto_id == k->proto_id
			 && (spi == k->spi || spi == k->spi_p)
			 && cmpsaddrwop(k->dst, dst) == 0)
				return p;
		}
	}

	return NULL;
}

/*
 * create new isakmp Phase 2 status record to handle isakmp in Phase2
 */
struct ph2handle *
newph2()
{
	struct ph2handle *iph2 = NULL;

	/* create new iph2 */
	iph2 = CALLOC(sizeof(*iph2), struct ph2handle *);
	if (iph2 == NULL)
		return NULL;

	iph2->status = PHASE1ST_SPAWN;

	return iph2;
}

/* initialize ph2handle */
void
initph2(iph2)
	struct ph2handle *iph2;
{
	if (iph2->id) {
		vfree(iph2->id);
		iph2->id = NULL;
	}
	if (iph2->id_p) {
		vfree(iph2->id_p);
		iph2->id_p = NULL;
	}
	if (iph2->nonce_p) {
		vfree(iph2->nonce_p);
		iph2->nonce_p = NULL;
	}
	if (iph2->dhpub_p) {
		vfree(iph2->dhpub_p);
		iph2->dhpub_p = NULL;
	}
	if (iph2->dhgxy) {
		vfree(iph2->dhgxy);
		iph2->dhgxy = NULL;
	}
	if (iph2->ivm) {
		oakley_delivm(iph2->ivm);
		iph2->ivm = NULL;
	}
	if (iph2->sce)
		SCHED_KILL(iph2->sce);
	if (iph2->scr)
		SCHED_KILL(iph2->scr);
	/* XXX more do it !!! */
}

/*
 * delete new isakmp Phase 2 status record to handle isakmp in Phase2
 */
void
delph2(iph2)
	struct ph2handle *iph2;
{
	initph2(iph2);
	/* XXX do more free parameters */

	free(iph2);
}

/*
 * create new isakmp Phase 2 status record to handle isakmp in Phase2
 */
int
insph2(iph2)
	struct ph2handle *iph2;
{
	LIST_INSERT_HEAD(&ph2tree, iph2, chain);

	return 0;
}

void
remph2(iph2)
	struct ph2handle *iph2;
{
	LIST_REMOVE(iph2, chain);
}

void
initph2tree()
{
	LIST_INIT(&ph2tree);
}

/* %%% */
void
bindph12(iph1, iph2)
	struct ph1handle *iph1;
	struct ph2handle *iph2;
{
	iph2->ph1 = iph1;
	LIST_INSERT_HEAD(&iph1->ph2tree, iph2, ph1bind);
}

void
unbindph12(iph2)
	struct ph2handle *iph2;
{
	if (iph2->ph1 != NULL) {
		iph2->ph1 = NULL;
		LIST_REMOVE(iph2, ph1bind);
	}
}
