# Copyright (c) 1996 WIDE Project. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modifications, are permitted provided that the above copyright notice
# and this paragraph are duplicated in all such forms and that any
# documentation, advertising materials, and other materials related to
# such distribution and use acknowledge that the software was developed
# by the WIDE Project, Japan. The name of the Project may not be used to
# endorse or promote products derived from this software without
# specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS IS''
# AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
# LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE.

.if exists(${.CURDIR}/../Makefile.opsys)
.include "${.CURDIR}/../Makefile.opsys"
.endif

BINDIR=	$(PREFIX)/sbin
PROG=	inet6d

LDADD+=	-lutil
.if (${OPSYS} != NetBSD)
CFLAGS+= -Wall
.endif
.if (${OPSYS} != NetBSD)
CFLAGS+= -DIPSEC -DINET6
.else
CPPFLAGS+= -DIPSEC -DINET6
.endif
LDADD+=	-L${.OBJDIR}/../libipsec -L${.OBJDIR}/../libipsec/obj \
	-L${.OBJDIR}/../libinet6 -L${.OBJDIR}/../libinet6/obj \
	-L/usr/local/v6/lib -lipsec -linet6
DPADD+=	${.OBJDIR}/../libipsec/libipsec.a \
	${.OBJDIR}/../libipsec/obj/libipsec.a \
	/usr/local/v6/lib/libipsec.a
DPADD+=	${.OBJDIR}/../libinet6/libinet6.a \
	${.OBJDIR}/../libinet6/obj/libinet6.a \
	/usr/local/v6/lib/libinet6.a

.if (${OPSYS} != "NetBSD")
MAN8=	inet6d.8
.else
MAN=	inet6d.8
.endif

.include <bsd.prog.mk>
