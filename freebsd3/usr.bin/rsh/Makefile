#	@(#)Makefile	8.1 (Berkeley) 7/19/93
# $FreeBSD: src/usr.bin/rsh/Makefile,v 1.9.2.1 1999/08/29 15:32:22 peter Exp $

PROG=	rsh
SRCS=	rsh.c
CFLAGS+=-Wall

.if exists(${DESTDIR}${LIBDIR}/libkrb.a) && defined(MAKE_KERBEROS4)
SRCS+=	krcmd.c kcmd.c rcmd_util.c encrypt.c
CFLAGS+=-DKERBEROS -DCRYPT -DHAVE_CONFIG_H \
	-I${.CURDIR}/../rlogin -I${.CURDIR} -I- \
	-I${.CURDIR}/../../kerberosIV/include \
	-I${.CURDIR}/../../crypto/kerberosIV/include \
	-I${.CURDIR}/../../crypto/kerberosIV/lib/roken \
	-I${.CURDIR}/../../crypto/kerberosIV/appl/bsd
DPADD=	${LIBUTIL} ${LIBKRB} ${LIBDES} ${LIBCRYPT}
LDADD=	-lutil -lkrb -ldes -lcrypt
DISTRIBUTION=	krb
.PATH:	${.CURDIR}/../../crypto/kerberosIV/appl/bsd
.endif

BINMODE=4555
INSTALLFLAGS=-fschg

LDADD+= -L${.OBJDIR}/../../lib/libinet6 \
        -L/usr/local/v6/lib -linet6
DPADD+= ${.OBJDIR}/../../lib/libinet6/libinet6.a \
        /usr/local/v6/lib/libinet6.a
CFLAGS+=-DINET6
#.PATH:	${.CURDIR}/../rlogin

.include <bsd.prog.mk>
