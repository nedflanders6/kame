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

/*
 * Copyright (c) 1982, 1986, 1988, 1990, 1993, 1995
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)tcp_subr.c	8.2 (Berkeley) 5/24/95
 *	$Id: tcp_subr.c,v 1.49.2.1 1999/02/04 06:40:28 msmith Exp $
 */

#include "opt_compat.h"
#include "opt_tcpdebug.h"
#include "opt_inet.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/domain.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/syslog.h>

#include <vm/vm_zone.h>

#include <net/route.h>
#include <net/if.h>

#define _IP_VHL
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/in_pcb.h>
#include <netinet6/in6_pcb.h>
#include <netinet/in_var.h>
#include <netinet/ip_var.h>
#include <netinet6/ip6_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/tcpip.h>
#ifdef TCPDEBUG
#include <netinet/tcp_debug.h>
#endif

#ifdef IPSEC
#include <netinet6/ipsec.h>
#endif /*IPSEC*/

int 	tcp_mssdflt = TCP_MSS;
SYSCTL_INT(_net_inet_tcp, TCPCTL_MSSDFLT, mssdflt,
	CTLFLAG_RW, &tcp_mssdflt , 0, "");

int 	tcp_v6mssdflt = TCP6_MSS;
SYSCTL_INT(_net_inet_tcp, TCPCTL_V6MSSDFLT, v6mssdflt,
	CTLFLAG_RW, &tcp_v6mssdflt , 0, "");

static int 	tcp_rttdflt = TCPTV_SRTTDFLT / PR_SLOWHZ;
SYSCTL_INT(_net_inet_tcp, TCPCTL_RTTDFLT, rttdflt,
	CTLFLAG_RW, &tcp_rttdflt , 0, "");

static int	tcp_do_rfc1323 = 1;
SYSCTL_INT(_net_inet_tcp, TCPCTL_DO_RFC1323, rfc1323,
	CTLFLAG_RW, &tcp_do_rfc1323 , 0, "");

static int	tcp_do_rfc1644 = 0;
SYSCTL_INT(_net_inet_tcp, TCPCTL_DO_RFC1644, rfc1644,
	CTLFLAG_RW, &tcp_do_rfc1644 , 0, "");

SYSCTL_INT(_net_inet_tcp, OID_AUTO, pcbcount, CTLFLAG_RD, &tcbinfo.ipi_count,
	   0, "Number of active PCBs");

static void	tcp_cleartaocache __P((void));
static void	tcp_notify __P((struct inpcb *, int));

/*
 * Target size of TCP PCB hash tables. Must be a power of two.
 *
 * Note that this can be overridden by the kernel environment
 * variable net.inet.tcp.tcbhashsize
 */
#ifndef TCBHASHSIZE
#define TCBHASHSIZE	512
#endif

/*
 * This is the actual shape of what we allocate using the zone
 * allocator.  Doing it this way allows us to protect both structures
 * using the same generation count, and also eliminates the overhead
 * of allocating tcpcbs separately.  By hiding the structure here,
 * we avoid changing most of the rest of the code (although it needs
 * to be changed, eventually, for greater efficiency).
 */
#define	ALIGNMENT	32
#define	ALIGNM1		(ALIGNMENT - 1)
struct	inp_tp {
	union {
		struct	inpcb inp;
		char	align[(sizeof(struct inpcb) + ALIGNM1) & ~ALIGNM1];
	} inp_tp_u;
	struct	tcpcb tcb;
};
#undef ALIGNMENT
#undef ALIGNM1

int	tcp_freeq __P((struct tcpcb *tp));

/*
 * Tcp initialization
 */
void
tcp_init()
{
	int hashsize;
	
	tcp_iss = random();	/* wrong, but better than a constant */
	tcp_ccgen = 1;
	tcp_cleartaocache();
	LIST_INIT(&tcb);
	tcbinfo.listhead = &tcb;
	if (!(getenv_int("net.inet.tcp.tcbhashsize", &hashsize)))
		hashsize = TCBHASHSIZE;
	if (!powerof2(hashsize)) {
		printf("WARNING: TCB hash size not a power of 2\n");
		hashsize = 512; /* safe default */
	}
	tcbinfo.hashbase = hashinit(hashsize, M_PCB, &tcbinfo.hashmask);
	tcbinfo.porthashbase = hashinit(hashsize, M_PCB,
					&tcbinfo.porthashmask);
	tcbinfo.ipi_zone = zinit("tcpcb", sizeof(struct inp_tp), maxsockets,
				 ZONE_INTERRUPT, 0);
#ifdef INET6
#define TCP_LGHDR (sizeof(struct tcpip6hdr))
#else /* INET6 */
#define TCP_LGHDR (sizeof(struct tcpiphdr))
#endif /* INET6 */
	if (max_protohdr < TCP_LGHDR)
		max_protohdr = TCP_LGHDR;
	if ((max_linkhdr + TCP_LGHDR) > MHLEN)
		panic("tcp_init");
#undef TCP_LGHDR
}

/*
 * Create template to be used to send tcp packets on a connection.
 * Call after host entry created, allocates an mbuf and fills
 * in a skeletal tcp/ip header, minimizing the amount of work
 * necessary when the connection is used.
 */
struct tcptemp *
tcp_template(tp)
	struct tcpcb *tp;
{
	register struct inpcb *inp = tp->t_inpcb;
	register struct mbuf *m;
	register struct tcptemp *n;

	if ((n = tp->t_template) == 0) {
		m = m_get(M_DONTWAIT, MT_HEADER);
		if (m == NULL)
			return (0);
		m->m_len = sizeof (struct tcptemp);
		n = mtod(m, struct tcptemp *);
	}
	bzero(n->tt_x1, sizeof(n->tt_x1));
	n->tt_pr = IPPROTO_TCP;
	n->tt_len = htons(sizeof (struct tcpiphdr) - sizeof (struct ip));
	n->tt_src = inp->inp_laddr;
	n->tt_dst = inp->inp_faddr;
	n->tt_sport = inp->inp_lport;
	n->tt_dport = inp->inp_fport;
	n->tt_seq = 0;
	n->tt_ack = 0;
	n->tt_x2 = 0;
	n->tt_off = 5;
	n->tt_flags = 0;
	n->tt_win = 0;
	n->tt_sum = 0;
	n->tt_urp = 0;
#ifdef INET6
	n->tt_flow = inp->inp_flow & IPV6_FLOWINFO_MASK;
	if (ip6_auto_flowlabel) {
		n->tt_flow &= ~IPV6_FLOWLABEL_MASK;
		n->tt_flow |= (htonl(ip6_flow_seq++) & IPV6_FLOWLABEL_MASK);
	}
	n->tt_vfc = IPV6_VERSION;
	n->tt_pr6 = IPPROTO_TCP;
	n->tt_len6 = n->tt_len;
	n->tt_src6 = inp->in6p_laddr;
	n->tt_dst6 = inp->in6p_faddr;
#endif /* INET6 */
	return (n);
}

/*
 * Send a single message to the TCP at address specified by
 * the given TCP/IP header.  If m == 0, then we make a copy
 * of the tcpiphdr at ti and send directly to the addressed host.
 * This is used to force keep alive messages out using the TCP
 * template for a connection tp->t_template.  If flags are given
 * then we send a message back to the TCP which originated the
 * segment ti, and discard the mbuf containing it and any other
 * attached mbufs.
 *
 * In any case the ack and sequence number of the transmitted
 * segment are as specified by the parameters.
 *
 * NOTE: If m != NULL, then ti must point to *inside* the mbuf.
 */
void
tcp_respond(tp, iph, th, m, ack, seq, flags, isipv6)
	struct tcpcb *tp;
	void *iph;
	register struct tcphdr *th;
	register struct mbuf *m;
	tcp_seq ack, seq;
	int flags;
#ifdef INET6
	int isipv6;
#endif /* INET6 */
{
	register int tlen;
	int win = 0;
	struct route *ro = 0;
	struct route sro;
	struct ip *ip = iph;
	struct tcpiphdr *ti = iph;
	struct tcphdr *nth;
#ifdef INET6
	struct route_in6 *ro6 = 0;
	struct route_in6 sro6;
	struct ip6_hdr *ip6 = iph;
	struct tcpip6hdr *ti6 = iph;
#endif /* INET6 */

	if (tp) {
		if (!(flags & TH_RST))
			win = sbspace(&tp->t_inpcb->inp_socket->so_rcv);
#ifdef INET6
		if (isipv6)
			ro6 = &tp->t_inpcb->in6p_route;
		else
#endif /* INET6 */
		ro = &tp->t_inpcb->inp_route;
	} else {
#ifdef INET6
		if (isipv6) {
			ro6 = &sro6;
			bzero(ro6, sizeof *ro6);
		} else {
#endif /* INET6 */
		ro = &sro;
		bzero(ro, sizeof *ro);
#ifdef INET6
		}
#endif /* INET6 */
	}
	if (m == 0) {
		m = m_gethdr(M_DONTWAIT, MT_HEADER);
		if (m == NULL)
			return;
#ifdef TCP_COMPAT_42
		tlen = 1;
#else
		tlen = 0;
#endif
		m->m_data += max_linkhdr;
#ifdef INET6
		if (isipv6) {
			ti6 = mtod(m, struct tcpip6hdr *);
			bcopy((caddr_t)ip6, (caddr_t)&ti6->ti6_i,
			      sizeof(struct ip6_hdr));
			ip6 = &ti6->ti6_i;
			nth = &ti6->ti6_t;
		} else {
#endif /* INET6 */
		ti = mtod(m, struct tcpiphdr *);
		bcopy((caddr_t)ip, (caddr_t)&ti->ti_i, sizeof(struct ip));
		ip = (struct ip *)&ti->ti_i;
		nth = &ti->ti_t;
#ifdef INET6
		}
#endif /* INET6 */
		bcopy((caddr_t)th, (caddr_t)nth, sizeof(struct tcphdr));
		flags = TH_ACK;
	} else {
		m_freem(m->m_next);
		m->m_next = 0;
		m->m_data = (caddr_t)ti;
		/* m_len is set later */
		tlen = 0;
#define xchg(a,b,type) { type t; t=a; a=b; b=t; }
#ifdef INET6
		if (isipv6) {
			struct in6_addr t;

			t = ip6->ip6_dst;
			ip6->ip6_dst = ip6->ip6_src;
			ip6->ip6_src = t;
			nth = (struct tcphdr *)(ip6 + 1);
			if (th != nth) {
				/*
				 * this is the case if an extension header
				 * exists between the IPv6 header and the
				 * TCP header.
				 */
				nth->th_sport = th->th_sport;
				nth->th_dport = th->th_dport;
			}
		} else {
#endif /* INET6 */
			xchg(ti->ti_dst.s_addr, ti->ti_src.s_addr, n_long);
			nth = th;
#ifdef INET6
		}
#endif /* INET6 */
		xchg(nth->th_dport, nth->th_sport, n_short);
#undef xchg
	}
	nth->th_seq = htonl(seq);
	nth->th_ack = htonl(ack);
	nth->th_x2 = 0;
	nth->th_off = sizeof (struct tcphdr) >> 2;
	nth->th_flags = flags;
	if (tp)
		nth->th_win = htons((u_short) (win >> tp->rcv_scale));
	else
		nth->th_win = htons((u_short)win);
	nth->th_urp = 0;
	nth->th_sum = 0;
	tlen += sizeof (struct tcphdr);
#ifdef INET6
	if (isipv6) {
		m->m_len = tlen + sizeof(struct ip6_hdr);
		m->m_pkthdr.len = tlen + sizeof(struct ip6_hdr);
		m->m_pkthdr.rcvif = (struct ifnet *) 0;
 		ip6->ip6_plen = htons((u_short)tlen);
		ip6->ip6_nxt = IPPROTO_TCP;
		ip6->ip6_hlim = ip6_defhlim;
		nth->th_sum = in6_cksum(m, IPPROTO_TCP,
					 sizeof(struct ip6_hdr), tlen);
		ip6->ip6_flow &= ~IPV6_FLOWLABEL_MASK;
		if (ip6_auto_flowlabel) {
			ip6->ip6_flow |=
				(htonl(ip6_flow_seq++) & IPV6_FLOWLABEL_MASK);
		}
	} else {
#endif /* INET6 */
	ti->ti_len = htons((u_short)(tlen));
	m->m_len = tlen + sizeof(struct ip);
	m->m_pkthdr.len = tlen + sizeof(struct ip);
	m->m_pkthdr.rcvif = (struct ifnet *) 0;
	bzero(ti->ti_x1, sizeof(ti->ti_x1));
	nth->th_sum = in_cksum(m, tlen + sizeof(struct ip));
	ip->ip_len = tlen + sizeof (struct ip);
	ip->ip_ttl = ip_defttl;
#ifdef INET6
	}
#endif /* INET6 */
#ifdef TCPDEBUG
	if (tp == NULL || (tp->t_inpcb->inp_socket->so_options & SO_DEBUG))
		tcp_trace(TA_OUTPUT, 0, tp,
#ifdef INET6
			  isipv6 ? (void *)ip6 :
#endif /* INET6 */
			  ip,
			  nth, 0);
#endif
#ifdef IPSEC
	m->m_pkthdr.rcvif = tp ? (struct ifnet *)tp->t_inpcb->inp_socket
		: NULL;
#endif /*IPSEC*/
#ifdef INET6
	if (isipv6) {
		(void)ip6_output(m, NULL, ro6, 0, NULL);
		if (ro6 == &sro6 && ro6->ro_rt)
			RTFREE(ro6->ro_rt);
	} else {
#endif /* INET6 */
	(void)ip_output(m, NULL, ro, 0, NULL);
	if (ro == &sro && ro->ro_rt) {
		RTFREE(ro->ro_rt);
	}
#ifdef INET6
	}
#endif /* INET6 */
}

/*
 * Create a new TCP control block, making an
 * empty reassembly queue and hooking it to the argument
 * protocol control block.  The `inp' parameter must have
 * come from the zone allocator set up in tcp_init().
 */
struct tcpcb *
tcp_newtcpcb(inp)
	struct inpcb *inp;
{
	struct inp_tp *it;
	register struct tcpcb *tp;
#ifdef INET6
	int isipv6 = (inp->inp_vflag & INP_IPV4) == 0;
#endif /* INET6 */

	it = (struct inp_tp *)inp;
	tp = &it->tcb;
	bzero((char *) tp, sizeof(struct tcpcb));
	tp->segq.lh_first = NULL;
	tp->t_maxseg = tp->t_maxopd =
#ifdef INET6
		isipv6 ? tcp_v6mssdflt :
#endif /* INET6 */
		tcp_mssdflt;

	if (tcp_do_rfc1323)
		tp->t_flags = (TF_REQ_SCALE|TF_REQ_TSTMP);
	if (tcp_do_rfc1644)
		tp->t_flags |= TF_REQ_CC;
	tp->t_inpcb = inp;	/* XXX */
	/*
	 * Init srtt to TCPTV_SRTTBASE (0), so we can tell that we have no
	 * rtt estimate.  Set rttvar so that srtt + 4 * rttvar gives
	 * reasonable initial retransmit time.
	 */
	tp->t_srtt = TCPTV_SRTTBASE;
	tp->t_rttvar = ((TCPTV_RTOBASE - TCPTV_SRTTBASE) << TCP_RTTVAR_SHIFT) / 4;
	tp->t_rttmin = TCPTV_MIN;
	tp->t_rxtcur = TCPTV_RTOBASE;
	tp->snd_cwnd = TCP_MAXWIN << TCP_MAX_WINSHIFT;
	tp->snd_ssthresh = TCP_MAXWIN << TCP_MAX_WINSHIFT;
	inp->inp_ip_ttl = ip_defttl;
	inp->inp_ppcb = (caddr_t)tp;
	return (tp);		/* XXX */
}

/*
 * Drop a TCP connection, reporting
 * the specified error.  If connection is synchronized,
 * then send a RST to peer.
 */
struct tcpcb *
tcp_drop(tp, errno)
	register struct tcpcb *tp;
	int errno;
{
	struct socket *so = tp->t_inpcb->inp_socket;

	if (TCPS_HAVERCVDSYN(tp->t_state)) {
		tp->t_state = TCPS_CLOSED;
		(void)tcp_output(tp);
		tcpstat.tcps_drops++;
	} else
		tcpstat.tcps_conndrops++;
	if (errno == ETIMEDOUT && tp->t_softerror)
		errno = tp->t_softerror;
	so->so_error = errno;
	return (tcp_close(tp));
}

/*
 * Close a TCP control block:
 *	discard all space held by the tcp
 *	discard internet protocol block
 *	wake up any sleepers
 */
struct tcpcb *
tcp_close(tp)
	register struct tcpcb *tp;
{
	struct inpcb *inp = tp->t_inpcb;
	struct socket *so = inp->inp_socket;
#ifdef INET6
	int isipv6 = INP_CHECK_SOCKAF(so, AF_INET6);
#endif /* INET6 */
	register struct rtentry *rt;
	int dosavessthresh;

	/*
	 * If we got enough samples through the srtt filter,
	 * save the rtt and rttvar in the routing entry.
	 * 'Enough' is arbitrarily defined as the 16 samples.
	 * 16 samples is enough for the srtt filter to converge
	 * to within 5% of the correct value; fewer samples and
	 * we could save a very bogus rtt.
	 *
	 * Don't update the default route's characteristics and don't
	 * update anything that the user "locked".
	 */
	if (tp->t_rttupdated >= 16) {
		register u_long i = 0;
#ifdef INET6
		if (isipv6) {
			struct sockaddr_in6 *sin6;

			if ((rt = inp->in6p_route.ro_rt) == NULL)
				goto no_valid_rt;
			sin6 = (struct sockaddr_in6 *)rt_key(rt);
			if (IN6_IS_ADDR_UNSPECIFIED(&sin6->sin6_addr))
				goto no_valid_rt;
		}
		else
#endif /* INET6 */		
		if ((rt = inp->inp_route.ro_rt) == NULL ||
		    ((struct sockaddr_in *)rt_key(rt))->sin_addr.s_addr
		    == INADDR_ANY)
			goto no_valid_rt;

		if ((rt->rt_rmx.rmx_locks & RTV_RTT) == 0) {
			i = tp->t_srtt *
			    (RTM_RTTUNIT / (PR_SLOWHZ * TCP_RTT_SCALE));
			if (rt->rt_rmx.rmx_rtt && i)
				/*
				 * filter this update to half the old & half
				 * the new values, converting scale.
				 * See route.h and tcp_var.h for a
				 * description of the scaling constants.
				 */
				rt->rt_rmx.rmx_rtt =
				    (rt->rt_rmx.rmx_rtt + i) / 2;
			else
				rt->rt_rmx.rmx_rtt = i;
			tcpstat.tcps_cachedrtt++;
		}
		if ((rt->rt_rmx.rmx_locks & RTV_RTTVAR) == 0) {
			i = tp->t_rttvar *
			    (RTM_RTTUNIT / (PR_SLOWHZ * TCP_RTTVAR_SCALE));
			if (rt->rt_rmx.rmx_rttvar && i)
				rt->rt_rmx.rmx_rttvar =
				    (rt->rt_rmx.rmx_rttvar + i) / 2;
			else
				rt->rt_rmx.rmx_rttvar = i;
			tcpstat.tcps_cachedrttvar++;
		}
		/*
		 * The old comment here said:
		 * update the pipelimit (ssthresh) if it has been updated
		 * already or if a pipesize was specified & the threshhold
		 * got below half the pipesize.  I.e., wait for bad news
		 * before we start updating, then update on both good
		 * and bad news.
		 *
		 * But we want to save the ssthresh even if no pipesize is
		 * specified explicitly in the route, because such
		 * connections still have an implicit pipesize specified
		 * by the global tcp_sendspace.  In the absence of a reliable
		 * way to calculate the pipesize, it will have to do.
		 */
		i = tp->snd_ssthresh;
		if (rt->rt_rmx.rmx_sendpipe != 0)
			dosavessthresh = (i < rt->rt_rmx.rmx_sendpipe / 2);
		else
			dosavessthresh = (i < so->so_snd.sb_hiwat / 2);
		if (((rt->rt_rmx.rmx_locks & RTV_SSTHRESH) == 0 &&
		     i != 0 && rt->rt_rmx.rmx_ssthresh != 0)
		    || dosavessthresh) {
			/*
			 * convert the limit from user data bytes to
			 * packets then to packet data bytes.
			 */
			i = (i + tp->t_maxseg / 2) / tp->t_maxseg;
			if (i < 2)
				i = 2;
			i *= (u_long)(tp->t_maxseg +
#ifdef INET6
				      isipv6 ? sizeof (struct tcpip6hdr) :
#endif /* INET6 */
				      sizeof (struct tcpiphdr));
			if (rt->rt_rmx.rmx_ssthresh)
				rt->rt_rmx.rmx_ssthresh =
				    (rt->rt_rmx.rmx_ssthresh + i) / 2;
			else
				rt->rt_rmx.rmx_ssthresh = i;
			tcpstat.tcps_cachedssthresh++;
		}
	}
    no_valid_rt:
	/* free the reassembly queue, if any */
	(void) tcp_freeq(tp);

	if (tp->t_template)
		(void) m_free(dtom(tp->t_template));
	inp->inp_ppcb = NULL;
	soisdisconnected(so);
#ifdef INET6
	if (isipv6)
		in6_pcbdetach(inp);
	else
#endif /* INET6 */
	in_pcbdetach(inp);
	tcpstat.tcps_closed++;
	return ((struct tcpcb *)0);
}

int
tcp_freeq(tp)
	struct tcpcb *tp;
{
	register struct ipqent *qe;
	int rv = 0;

	while ((qe = tp->segq.lh_first) != NULL) {
		LIST_REMOVE(qe, ipqe_q);
		m_freem(qe->ipqe_m);
		FREE(qe, M_IPQ);
		rv = 1;
	}
	return (rv);
}

void
tcp_drain()
{

}

/*
 * Notify a tcp user of an asynchronous error;
 * store error as soft error, but wake up user
 * (for now, won't do anything until can select for soft error).
 */
static void
tcp_notify(inp, error)
	struct inpcb *inp;
	int error;
{
	register struct tcpcb *tp = (struct tcpcb *)inp->inp_ppcb;
	register struct socket *so = inp->inp_socket;

	/*
	 * Ignore some errors if we are hooked up.
	 * If connection hasn't completed, has retransmitted several times,
	 * and receives a second error, give up now.  This is better
	 * than waiting a long time to establish a connection that
	 * can never complete.
	 */
	if (tp->t_state == TCPS_ESTABLISHED &&
	     (error == EHOSTUNREACH || error == ENETUNREACH ||
	      error == EHOSTDOWN)) {
		return;
	} else if (tp->t_state < TCPS_ESTABLISHED && tp->t_rxtshift > 3 &&
	    tp->t_softerror)
		so->so_error = error;
	else
		tp->t_softerror = error;
	wakeup((caddr_t) &so->so_timeo);
	sorwakeup(so);
	sowwakeup(so);
}

static int
tcp_pcblist SYSCTL_HANDLER_ARGS
{
	int error, i, n, s;
	struct inpcb *inp, **inp_list;
	inp_gen_t gencnt;
	struct xinpgen xig;

	/*
	 * The process of preparing the TCB list is too time-consuming and
	 * resource-intensive to repeat twice on every request.
	 */
	if (req->oldptr == 0) {
		n = tcbinfo.ipi_count;
		req->oldidx = 2 * (sizeof xig)
			+ (n + n/8) * sizeof(struct xtcpcb);
		return 0;
	}

	if (req->newptr != 0)
		return EPERM;

	/*
	 * OK, now we're committed to doing something.
	 */
	s = splnet();
	gencnt = tcbinfo.ipi_gencnt;
	n = tcbinfo.ipi_count;
	splx(s);

	xig.xig_len = sizeof xig;
	xig.xig_count = n;
	xig.xig_gen = gencnt;
	xig.xig_sogen = so_gencnt;
	error = SYSCTL_OUT(req, &xig, sizeof xig);
	if (error)
		return error;

	inp_list = malloc(n * sizeof *inp_list, M_TEMP, M_WAITOK);
	if (inp_list == 0)
		return ENOMEM;
	
	s = splnet();
	for (inp = tcbinfo.listhead->lh_first, i = 0; inp && i < n;
	     inp = inp->inp_list.le_next) {
		if (inp->inp_gencnt <= gencnt)
			inp_list[i++] = inp;
	}
	splx(s);
	n = i;

	error = 0;
	for (i = 0; i < n; i++) {
		inp = inp_list[i];
		if (inp->inp_gencnt <= gencnt) {
			struct xtcpcb xt;
			xt.xt_len = sizeof xt;
			/* XXX should avoid extra copy */
			bcopy(inp, &xt.xt_inp, sizeof *inp);
			bcopy(inp->inp_ppcb, &xt.xt_tp, sizeof xt.xt_tp);
			if (inp->inp_socket)
				sotoxsocket(inp->inp_socket, &xt.xt_socket);
			error = SYSCTL_OUT(req, &xt, sizeof xt);
		}
	}
	if (!error) {
		/*
		 * Give the user an updated idea of our state.
		 * If the generation differs from what we told
		 * her before, she knows that something happened
		 * while we were processing this request, and it
		 * might be necessary to retry.
		 */
		s = splnet();
		xig.xig_gen = tcbinfo.ipi_gencnt;
		xig.xig_sogen = so_gencnt;
		xig.xig_count = tcbinfo.ipi_count;
		splx(s);
		error = SYSCTL_OUT(req, &xig, sizeof xig);
	}
	free(inp_list, M_TEMP);
	return error;
}

SYSCTL_PROC(_net_inet_tcp, TCPCTL_PCBLIST, pcblist, CTLFLAG_RD, 0, 0,
	    tcp_pcblist, "S,xtcpcb", "List of active TCP connections");

void
tcp_ctlinput(cmd, sa, vip)
	int cmd;
	struct sockaddr *sa;
	void *vip;
{
	register struct ip *ip = vip;
	register struct tcphdr *th;
	void (*notify) __P((struct inpcb *, int)) = tcp_notify;

	if (cmd == PRC_QUENCH)
		notify = tcp_quench;
	else if (cmd == PRC_MSGSIZE)
		notify = tcp_mtudisc;
	else if (!PRC_IS_REDIRECT(cmd) &&
		 ((unsigned)cmd > PRC_NCMDS || inetctlerrmap[cmd] == 0))
		return;
	if (ip) {
		th = (struct tcphdr *)((caddr_t)ip 
				       + (IP_VHL_HL(ip->ip_vhl) << 2));
		in_pcbnotify(&tcb, sa, th->th_dport, ip->ip_src, th->th_sport,
			cmd, notify);
	} else
		in_pcbnotify(&tcb, sa, 0, zeroin_addr, 0, cmd, notify);
}

#ifdef INET6
void
tcp6_ctlinput(cmd, sa, ip6, m ,off)
	int cmd;
	struct sockaddr *sa;
	register struct ip6_hdr *ip6;
	struct mbuf *m;
	int off;
{
	register struct tcphdr *thp;
	struct tcphdr th;
	void (*notify) __P((struct inpcb *, int)) = tcp_notify;
	struct sockaddr_in6 sa6;

	if (cmd == PRC_QUENCH)
		notify = tcp_quench;
	else if (cmd == PRC_MSGSIZE)
		notify = tcp_mtudisc;
	else if (!PRC_IS_REDIRECT(cmd) &&
		 ((unsigned)cmd > PRC_NCMDS || inet6ctlerrmap[cmd] == 0))
		return;

	/* translate addresses into internal form */
	if (sa->sa_family != AF_INET6 ||
	    sa->sa_len != sizeof(struct sockaddr_in6))
		return;
	sa6 = *(struct sockaddr_in6 *)sa;
	if (IN6_IS_ADDR_LINKLOCAL(&sa6.sin6_addr))
		sa6.sin6_addr.s6_addr16[1] = htons(m->m_pkthdr.rcvif->if_index);

	if (ip6) {
		/*
		 * XXX: We assume that when IPV6 is non NULL,
		 * M and OFF are valid.
		 */
		struct in6_addr s;

		/* translate addresses into internal form */
		memcpy(&s, &ip6->ip6_dst, sizeof(s));
		if (IN6_IS_ADDR_LINKLOCAL(&s))
			s.s6_addr16[1] = htons(m->m_pkthdr.rcvif->if_index);

		if (m->m_len < off + sizeof(*thp)) {
			/*
			 * this should be rare case,
			 * so we compromise on this copy...
			 */
			m_copydata(m, off, sizeof(th), (caddr_t)&th);
			thp = &th;
		} else
			thp = (struct tcphdr *)(mtod(m, caddr_t) + off);
		in6_pcbnotify(&tcb, (struct sockaddr *)&sa6, thp->th_dport,
			      &s, thp->th_sport, cmd, notify);
	} else
		in6_pcbnotify(&tcb, (struct sockaddr *)&sa6, 0, &zeroin6_addr,
			      0, cmd, notify);
}
#endif /* INET6 */

/*
 * When a source quench is received, close congestion window
 * to one segment.  We will gradually open it again as we proceed.
 */
void
tcp_quench(inp, errno)
	struct inpcb *inp;
	int errno;
{
	struct tcpcb *tp = intotcpcb(inp);

	if (tp)
		tp->snd_cwnd = tp->t_maxseg;
}

/*
 * When `need fragmentation' ICMP is received, update our idea of the MSS
 * based on the new value in the route.  Also nudge TCP to send something,
 * since we know the packet we just sent was dropped.
 * This duplicates some code in the tcp_mss() function in tcp_input.c.
 */
void
tcp_mtudisc(inp, errno)
	struct inpcb *inp;
	int errno;
{
	struct tcpcb *tp = intotcpcb(inp);
	struct rtentry *rt;
	struct rmxp_tao *taop;
	struct socket *so = inp->inp_socket;
	int offered;
	int mss;
#ifdef INET6
	int isipv6 = (tp->t_inpcb->inp_vflag & INP_IPV4) == 0;
#endif /* INET6 */

	if (tp) {
#ifdef INET6
		if (isipv6)
			rt = tcp_rtlookup6(inp);
		else
#endif /* INET6 */
		rt = tcp_rtlookup(inp);
		if (!rt || !rt->rt_rmx.rmx_mtu) {
			tp->t_maxopd = tp->t_maxseg =
#ifdef INET6
				isipv6 ? tcp_v6mssdflt :
#endif /* INET6 */
				tcp_mssdflt;
			return;
		}
		taop = rmx_taop(rt->rt_rmx);
		offered = taop->tao_mssopt;
		mss = rt->rt_rmx.rmx_mtu -
#ifdef INET6
			(isipv6 ?
			 sizeof(struct tcpip6hdr) :
#endif /* INET6 */
			 sizeof(struct tcpiphdr)
#ifdef INET6
			 )
#endif /* INET6 */
			;

	    {
		int tmp;
		struct ipoption *p;

		/* why we don't have this here?  could anyone comment? */
#ifdef INET6
		if (isipv6) {
			if (inp->in6p_outputopts) {
				tmp = ip6_optlen(inp);
				if (mss > tmp)
					mss -= tmp;
				else {
					printf("tcp_mtudisc: "
					       "invalid mss(IP6 option)\n");
				}
			}
		} else
#endif /* INET6 */
		if (inp->inp_options) {
			tmp = inp->inp_options->m_len - sizeof(p->ipopt_dst);
			if (mss > tmp)
				mss -= tmp;
			else {
				printf("tcp_mtudisc: "
					"invalid mss(IP4 option)\n");
			}
		}
#ifdef IPSEC
		/* plug for AH/ESP. */
		tmp = ipsec_hdrsiz_tcp(tp,
#ifdef INET6
				       isipv6
#else
				       0
#endif
				       );
		if (mss > tmp)
			mss -= tmp;
		else
			printf("tcp_mtudisc: invalid mss(IPsec)\n");
#endif /*IPSEC*/
	    }
		if (offered)
			mss = min(mss, offered);
		/*
		 * XXX - The above conditional probably violates the TCP
		 * spec.  The problem is that, since we don't know the
		 * other end's MSS, we are supposed to use a conservative
		 * default.  But, if we do that, then MTU discovery will
		 * never actually take place, because the conservative
		 * default is much less than the MTUs typically seen
		 * on the Internet today.  For the moment, we'll sweep
		 * this under the carpet.
		 *
		 * The conservative default might not actually be a problem
		 * if the only case this occurs is when sending an initial
		 * SYN with options and data to a host we've never talked
		 * to before.  Then, they will reply with an MSS value which
		 * will get recorded and the new parameters should get
		 * recomputed.  For Further Study.
		 */
		if (tp->t_maxopd <= mss)
			return;
		tp->t_maxopd = mss;

		if ((tp->t_flags & (TF_REQ_TSTMP|TF_NOOPT)) == TF_REQ_TSTMP &&
		    (tp->t_flags & TF_RCVD_TSTMP) == TF_RCVD_TSTMP)
			mss -= TCPOLEN_TSTAMP_APPA;
		if ((tp->t_flags & (TF_REQ_CC|TF_NOOPT)) == TF_REQ_CC &&
		    (tp->t_flags & TF_RCVD_CC) == TF_RCVD_CC)
			mss -= TCPOLEN_CC_APPA;
#if	(MCLBYTES & (MCLBYTES - 1)) == 0
		if (mss > MCLBYTES)
			mss &= ~(MCLBYTES-1);
#else
		if (mss > MCLBYTES)
			mss = mss / MCLBYTES * MCLBYTES;
#endif
		if (so->so_snd.sb_hiwat < mss)
			mss = so->so_snd.sb_hiwat;

		tp->t_maxseg = mss;

		tcpstat.tcps_mturesent++;
		tp->t_rtt = 0;
		tp->snd_nxt = tp->snd_una;
		tcp_output(tp);
	}
}

/*
 * Look-up the routing entry to the peer of this inpcb.  If no route
 * is found and it cannot be allocated the return NULL.  This routine
 * is called by TCP routines that access the rmx structure and by tcp_mss
 * to get the interface MTU.
 */
struct rtentry *
tcp_rtlookup(inp)
	struct inpcb *inp;
{
	struct route *ro;
	struct rtentry *rt;

	ro = &inp->inp_route;
	rt = ro->ro_rt;
	if (rt == NULL || !(rt->rt_flags & RTF_UP)) {
		/* No route yet, so try to acquire one */
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			ro->ro_dst.sa_family = AF_INET;
			ro->ro_dst.sa_len = sizeof(ro->ro_dst);
			((struct sockaddr_in *) &ro->ro_dst)->sin_addr =
				inp->inp_faddr;
			rtalloc(ro);
			rt = ro->ro_rt;
		}
	}
	return rt;
}

#ifdef INET6
struct rtentry *
tcp_rtlookup6(inp)
	struct inpcb *inp;
{
	struct route_in6 *ro6;
	struct rtentry *rt;

	ro6 = &inp->in6p_route;
	rt = ro6->ro_rt;
	if (rt == NULL || !(rt->rt_flags & RTF_UP)) {
		/* No route yet, so try to acquire one */
		if (!IN6_IS_ADDR_UNSPECIFIED(&inp->in6p_faddr)) {
			ro6->ro_dst.sin6_family = AF_INET6;
			ro6->ro_dst.sin6_len = sizeof(ro6->ro_dst);
			ro6->ro_dst.sin6_addr = inp->in6p_faddr;
			rtalloc((struct route *)ro6);
			rt = ro6->ro_rt;
		}
	}
	return rt;
}
#endif /* INET6 */

#ifdef IPSEC
/* compute ESP/AH header size for TCP, including outer IP header. */
size_t
ipsec_hdrsiz_tcp(tp, isipv6)
	struct tcpcb *tp;
#ifdef INET6
	int isipv6;
#endif /* INET6 */
{
	struct inpcb *inp;
	struct mbuf *m;
	size_t hdrsiz;
	struct ip *ip;
#ifdef INET6
	struct ip6_hdr *ip6 = NULL;
#endif /* INET6 */
	struct tcphdr *th;

	if (!tp || !tp->t_template || !(inp = tp->t_inpcb))
		return 0;
	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if (!m)
		return 0;
	
#ifdef INET6
	if (isipv6) {
		ip6 = mtod(m, struct ip6_hdr *);
		th = (struct tcphdr *)(ip6 + 1);
		m->m_pkthdr.len = m->m_len = sizeof(struct tcpip6hdr);
		bcopy((caddr_t)&tp->t_template->tt_i6, (caddr_t)ip6,
		      sizeof(struct ip6_hdr));
		bcopy((caddr_t)&tp->t_template->tt_t, (caddr_t)th,
		      sizeof(struct tcphdr));
	} else {
#endif /* INET6 */
	ip = mtod(m, struct ip *);
	th = (struct tcphdr *)(ip + 1);
	m->m_pkthdr.len = m->m_len = sizeof(struct tcpiphdr);
	bcopy((caddr_t)&tp->t_template->tt_i, (caddr_t)ip, sizeof(struct ip));
	bcopy((caddr_t)&tp->t_template->tt_t, (caddr_t)th,
	      sizeof(struct tcphdr));
#ifdef INET6
	}
#endif /* INET6 */

#ifdef INET6
	if (isipv6)
		hdrsiz = ipsec6_hdrsiz(m, inp);
	else
#endif /* INET6 */
	hdrsiz = ipsec4_hdrsiz(m, inp);

	m_free(m);
	return hdrsiz;
}
#endif /*IPSEC*/

/*
 * Return a pointer to the cached information about the remote host.
 * The cached information is stored in the protocol specific part of
 * the route metrics.
 */
struct rmxp_tao *
tcp_gettaocache(inp)
	struct inpcb *inp;
{
#ifdef INET6
	int isipv6 = (inp->inp_vflag & INP_IPV4) == 0;
#endif /* INET6 */
	struct rtentry *rt;

#ifdef INET6
	if (isipv6)
		rt = tcp_rtlookup6(inp);
	else
#endif /* INET6 */
	rt = tcp_rtlookup(inp);

	/* Make sure this is a host route and is up. */
	if (rt == NULL ||
	    (rt->rt_flags & (RTF_UP|RTF_HOST)) != (RTF_UP|RTF_HOST))
		return NULL;

	return rmx_taop(rt->rt_rmx);
}

/*
 * Clear all the TAO cache entries, called from tcp_init.
 *
 * XXX
 * This routine is just an empty one, because we assume that the routing
 * routing tables are initialized at the same time when TCP, so there is
 * nothing in the cache left over.
 */
static void
tcp_cleartaocache()
{
}
