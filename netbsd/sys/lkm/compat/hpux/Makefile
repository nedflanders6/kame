#	$NetBSD: Makefile,v 1.5 2001/12/12 11:44:45 lukem Exp $

.include "../Makefile.inc"

.PATH:	$S/compat/hpux $S/arch/${MACHINE}/${MACHINE}

CPPFLAGS+=	-DSYSVSEM -DSYSVMSG -DSYSVSHM -DCOMPAT_HPUX
CPPFLAGS+=	-nostdinc
AFLAGS=		-x assembler-with-cpp -traditional-cpp -D_LOCORE -D_KERNEL \
		-I. -I${S}

KMOD=	compat_hpux

# common source files
SRCS=	lkminit_emul.c
SRCS+=	hpux_compat.c hpux_errno.c hpux_exec.c hpux_file.c hpux_ipc.c \
	hpux_net.c hpux_sig.c hpux_syscalls.c hpux_sysent.c hpux_tty.c
SRCS+=	hpux_machdep.c

# MD source files
.if ${MACHINE} == "hp300"
.endif

.include <bsd.kmod.mk>
