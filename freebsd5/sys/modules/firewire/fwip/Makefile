# $FreeBSD: src/sys/modules/firewire/fwip/Makefile,v 1.1 2004/07/12 13:12:56 dfr Exp $

# Makefile for the fwip(4) module (IP over IEEE1394)

.PATH: ${.CURDIR}/../../../dev/firewire ${.CURDIR}/../../../net

KMOD    = if_fwip
SRCS    = bus_if.h device_if.h \
          opt_inet.h opt_inet6.h opt_mac.h \
          if_fwip.c if_fwipvar.h \
          firewire.h firewirereg.h \
          if_fwsubr.c

opt_inet.h:
	echo "#define INET      1" > ${.TARGET}

#CFLAGS += -DDEVICE_POLLING

.include <bsd.kmod.mk>
