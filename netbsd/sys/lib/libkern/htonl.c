/*	$NetBSD: htonl.c,v 1.9 1998/03/27 01:30:01 cgd Exp $	*/

/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: htonl.c,v 1.9 1998/03/27 01:30:01 cgd Exp $");
#endif

#include <sys/types.h>

#undef htonl

in_addr_t 
htonl(x)
	in_addr_t x;
{
#if BYTE_ORDER == LITTLE_ENDIAN
	u_char *s = (u_char *)&x;
	return (in_addr_t)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
#else
	return x;
#endif
}
