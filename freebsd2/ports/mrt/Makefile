# New ports collection makefile for:	mrt
# Version required:	1.5.2a
# Date created:		Sat Jun  6 00:09:52 JST 1998
# Whom:			Jun-ichiro itojun Itoh <itojun@itojun.org>
#
# $Id: Makefile,v 1.3 1999/11/01 12:39:07 itojun Exp $
#

DISTNAME=	mrt-${VERBASE}${VERADD}-src
PKGNAME=	mrt-${VERBASE}
CATEGORIES=	net
MASTER_SITES=	ftp://ftp.merit.edu/net-research/mrt/

MAINTAINER=	itojun@itojun.org

WRKSRC=		${WRKDIR}/mrt-${VERBASE}/src.kame
GNU_CONFIGURE=	yes

VERBASE=	2.0.1a
VERADD=

MAN8=	mrtd.8
EXAMPLES=	programs/bgpsim/bgpsim.conf programs/bgpsim/bgpsim6.conf \
		programs/mrtd/mrtd.conf programs/mrtd/mrtd.ripng.conf \
		programs/mrtd/solaris.ripng.conf

post-extract:
	(cd ${WRKDIR}/mrt-${VERBASE}; ./make-sym-links kame)

post-install:
	${MKDIR} ${PREFIX}/share/examples/mrt
	for i in ${EXAMPLES}; do \
		${INSTALL_DATA} ${WRKSRC}/$$i ${PREFIX}/share/examples/mrt; \
	done

.include <bsd.port.mk>
