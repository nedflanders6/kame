SRCDIR=	${.CURDIR}/../../kame/libipsec

LIB=	ipsec
CFLAGS+=-g -Wall -Werror
CPPFLAGS+=-DIPSEC_DEBUG -DIPSEC -DINET6 -I. -I${SRCDIR} -DYY_NO_UNPUT

.PATH:	${SRCDIR}
#${.CURDIR}/../../sys/netkey
#SRCS=	pfkey.c pfkey_dump.c
#SRCS+=	key_debug.c
SRCS+=	ipsec_strerror.c policy_parse.y policy_token.l
SRCS+=	ipsec_get_policylen.c ipsec_dump_policy.c
YPREFIX=__libyy
LPREFIX=__libyy
YHEADER=1

#MAN=	ipsec_set_policy.3 ipsec_strerror.3
#MLINKS+=ipsec_set_policy.3 ipsec_get_policylen.3 \
#	ipsec_set_policy.3 ipsec_dump_policy.3

MKPIC=no

libinstall::

.include <bsd.lib.mk>
