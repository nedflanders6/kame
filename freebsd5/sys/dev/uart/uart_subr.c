/*
 * Copyright (c) 2004 Marcel Moolenaar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/sys/dev/uart/uart_subr.c,v 1.2 2004/03/20 08:38:33 marcel Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>

#include <machine/bus.h>
#include <machine/vmparam.h>

#include <dev/uart/uart.h>
#include <dev/uart/uart_cpu.h>

#define	UART_TAG_BR	0
#define	UART_TAG_CH	1
#define	UART_TAG_DB	2
#define	UART_TAG_DT	3
#define	UART_TAG_IO	4
#define	UART_TAG_MM	5
#define	UART_TAG_PA	6
#define	UART_TAG_RS	7
#define	UART_TAG_SB	8
#define	UART_TAG_XO	9

static bus_addr_t
uart_parse_addr(__const char **p)
{
	return (strtoul(*p, (char**)(uintptr_t)p, 0));
}

static long
uart_parse_long(__const char **p)
{
	return (strtol(*p, (char**)(uintptr_t)p, 0));
}

static int
uart_parse_parity(__const char **p)
{
	if (!strncmp(*p, "even", 4)) {
		*p += 4;
		return UART_PARITY_EVEN;
	}
	if (!strncmp(*p, "mark", 4)) {
		*p += 4;
		return UART_PARITY_MARK;
	}
	if (!strncmp(*p, "none", 4)) {
		*p += 4;
		return UART_PARITY_NONE;
	}
	if (!strncmp(*p, "odd", 3)) {
		*p += 3;
		return UART_PARITY_ODD;
	}
	if (!strncmp(*p, "space", 5)) {
		*p += 5;
		return UART_PARITY_SPACE;
	}
	return (-1);
}

static int
uart_parse_tag(__const char **p)
{
	int tag;

	if ((*p)[0] == 'b' && (*p)[1] == 'r') {
		tag = UART_TAG_BR;
		goto out;
	}
	if ((*p)[0] == 'c' && (*p)[1] == 'h') {
		tag = UART_TAG_CH;
		goto out;
	}
	if ((*p)[0] == 'd' && (*p)[1] == 'b') {
		tag = UART_TAG_DB;
		goto out;
	}
	if ((*p)[0] == 'd' && (*p)[1] == 't') {
		tag = UART_TAG_DT;
		goto out;
	}
	if ((*p)[0] == 'i' && (*p)[1] == 'o') {
		tag = UART_TAG_IO;
		goto out;
	}
	if ((*p)[0] == 'm' && (*p)[1] == 'm') {
		tag = UART_TAG_MM;
		goto out;
	}
	if ((*p)[0] == 'p' && (*p)[1] == 'a') {
		tag = UART_TAG_PA;
		goto out;
	}
	if ((*p)[0] == 'r' && (*p)[1] == 's') {
		tag = UART_TAG_RS;
		goto out;
	}
	if ((*p)[0] == 's' && (*p)[1] == 'b') {
		tag = UART_TAG_SB;
		goto out;
	}
	if ((*p)[0] == 'x' && (*p)[1] == 'o') {
		tag = UART_TAG_XO;
		goto out;
	}
	return (-1);

out:
	*p += 2;
	if ((*p)[0] != ':')
		return (-1);
	(*p)++;
	return (tag);
}

/*
 * Parse a device specification. The specification is a list of attributes
 * seperated by commas. Each attribute is a tag-value pair with the tag and
 * value seperated by a colon. Supported tags are:
 *
 *	br = Baudrate
 *	ch = Channel
 *	db = Data bits
 *	dt = Device type
 *	io = I/O port address
 *	mm = Memory mapped I/O address
 *	pa = Parity
 *	rs = Register shift
 *	sb = Stopbits
 *	xo = Device clock (xtal oscillator)
 *
 * The io and mm tags are mutually exclusive.
 */

int
uart_getenv(int devtype, struct uart_devinfo *di)
{
	__const char *spec;
	bus_addr_t addr = ~0U;

	/*
	 * Check the environment variables "hw.uart.console" and
	 * "hw.uart.dbgport". These variables, when present, specify
	 * which UART port is to be used as serial console or debug
	 * port (resp).
	 */
	if (devtype == UART_DEV_CONSOLE)
		spec = getenv("hw.uart.console");
	else if (devtype == UART_DEV_DBGPORT)
		spec = getenv("hw.uart.dbgport");
	else
		spec = NULL;
	if (spec == NULL)
		return (ENXIO);

	/* Set defaults. */
	di->ops = uart_ns8250_ops;
	di->bas.chan = 0;
	di->bas.regshft = 0;
	di->bas.rclk = 0;
	di->baudrate = 0;
	di->databits = 8;
	di->stopbits = 1;
	di->parity = UART_PARITY_NONE;

	/* Parse the attributes. */
	while (1) {
		switch (uart_parse_tag(&spec)) {
		case UART_TAG_BR:
			di->baudrate = uart_parse_long(&spec);
			break;
		case UART_TAG_CH:
			di->bas.chan = uart_parse_long(&spec);
			break;
		case UART_TAG_DB:
			di->databits = uart_parse_long(&spec);
			break;
		case UART_TAG_DT:
			return (EINVAL);	/* XXX not yet implemented. */
			break;
		case UART_TAG_IO:
			di->bas.bst = uart_bus_space_io;
			addr = uart_parse_addr(&spec);
			break;
		case UART_TAG_MM:
			di->bas.bst = uart_bus_space_mem;
			addr = uart_parse_addr(&spec);
			break;
		case UART_TAG_PA:
			di->parity = uart_parse_parity(&spec);
			break;
		case UART_TAG_RS:
			di->bas.regshft = uart_parse_long(&spec);
			break;
		case UART_TAG_SB:
			di->stopbits = uart_parse_long(&spec);
			break;
		case UART_TAG_XO:
			di->bas.rclk = uart_parse_long(&spec);
			break;
		default:
			return (EINVAL);
		}
		if (*spec == '\0')
			break;
		if (*spec != ',')
			return (EINVAL);
		spec++;
	}

	/*
	 * If we still have an invalid address, the specification must be
	 * missing an I/O port or memory address. We don't like that.
	 */
	if (addr == ~0U)
		return (EINVAL);
	/* XXX the size of the mapping depends on the UART class. */
	if (bus_space_map(di->bas.bst, addr, 8, 0, &di->bas.bsh) != 0)
		return (EINVAL);
	return (0);
}
