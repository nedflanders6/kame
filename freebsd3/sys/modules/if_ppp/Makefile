# $FreeBSD: src/sys/modules/if_ppp/Makefile,v 1.16.2.1 1999/08/29 16:27:16 peter Exp $

.PATH:  ${.CURDIR}/../../net
KMOD=   if_ppp
SRCS=	if_ppp.c ppp_tty.c slcompress.c \
	bpfilter.h ppp.h opt_inet.h opt_inet6.h opt_ipx.h opt_ppp.h vnode_if.h
NOMAN=
CLEANFILES+=	vnode_if.c vnode_if.h

NPPP?=		2
PPP_BSDCOMP?=	1	# 0/1
PPP_DEFLATE?=	1	# 0/1
PPP_FILTER?=	0	# 0/1 - requires bpf to be configured in kernel
PPP_INET?=	1	# 0/1 - requires INET to be configured in kernel
PPP_INET6?=	1	# 0/1 - requires INET to be configured in kernel
PPP_IPX?=	0	# 0/1 - requires IPX to be configured in kernel

CFLAGS+= ${PROTOS}

.if ${PPP_BSDCOMP} > 0
SRCS+=	bsd_comp.c
.endif
.if ${PPP_DEFLATE} > 0
SRCS+=	ppp_deflate.c zlib.c
.endif

CLEANFILES+=	bpfilter.h opt_inet.h opt_ipx.h opt_ppp.h ppp.h

bpfilter.h:
	echo "#define NBPFILTER ${PPP_FILTER}" > bpfilter.h

ppp.h:
	echo "#define NPPP ${NPPP}" > ppp.h

opt_inet.h:
	touch opt_inet.h
.if ${PPP_INET} > 0
	echo "#define INET 1" > opt_inet.h
.endif

opt_inet6.h:
	touch opt_inet6.h
.if ${PPP_INET6} > 0
	echo "#define INET6 1" > opt_inet6.h
.endif

opt_ipx.h:
	touch opt_ipx.h
.if ${PPP_IPX} > 0
	echo "#define IPX ${PPP_IPX}" > opt_ipx.h
.endif

opt_ppp.h:
	touch opt_ppp.h
.if ${PPP_BSDCOMP} > 0
	echo "#define PPP_BSDCOMP ${PPP_BSDCOMP}" > opt_ppp.h
.endif
.if ${PPP_DEFLATE} > 0
	echo "#define PPP_DEFLATE ${PPP_DEFLATE}" >> opt_ppp.h
.endif
.if ${PPP_FILTER} > 0
	echo "#define PPP_FILTER ${PPP_FILTER}" >> opt_ppp.h
.endif

.include <bsd.kmod.mk>
