/* $NetBSD: wskbd.c,v 1.20 1999/03/27 11:22:23 drochner Exp $ */

/*
 * Copyright (c) 1996, 1997 Christopher G. Demetriou.  All rights reserved.
 *
 * Keysym translator:
 * Contributed to The NetBSD Foundation by Juergen Hannken-Illjes.
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
 *      This product includes software developed by Christopher G. Demetriou
 *	for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

static const char _copyright[] __attribute__ ((unused)) =
    "Copyright (c) 1996, 1997 Christopher G. Demetriou.  All rights reserved.";
static const char _rcsid[] __attribute__ ((unused)) =
    "$NetBSD: wskbd.c,v 1.20 1999/03/27 11:22:23 drochner Exp $";

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	@(#)kbd.c	8.2 (Berkeley) 10/30/93
 */

/*
 * Keyboard driver (/dev/wskbd*).  Translates incoming bytes to ASCII or
 * to `wscons_events' and passes them up to the appropriate reader.
 */

#include "opt_ddb.h"

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/syslog.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/tty.h>
#include <sys/signalvar.h>
#include <sys/errno.h>
#include <sys/fcntl.h>

#include <dev/wscons/wsconsio.h>
#include <dev/wscons/wskbdvar.h>
#include <dev/wscons/wsksymdef.h>
#include <dev/wscons/wsksymvar.h>
#include <dev/wscons/wseventvar.h>
#include <dev/wscons/wscons_callbacks.h>

#include "opt_wsdisplay_compat.h"

#include "wskbd.h"

struct wskbd_internal {
	const struct wskbd_mapdata *t_keymap;

	const struct wskbd_consops *t_consops;
	void	*t_consaccesscookie;

	int	t_modifiers;
	int	t_composelen;		/* remaining entries in t_composebuf */
	keysym_t t_composebuf[2];

	struct wskbd_softc *t_sc;	/* back pointer */
};

struct wskbd_softc {
	struct device	sc_dv;

	struct wskbd_internal *id;

	const struct wskbd_accessops *sc_accessops;
	void *sc_accesscookie;

	int	sc_ledstate;

	int	sc_ready;		/* accepting events */
	struct wseventvar sc_events;	/* event queue state */

	int	sc_isconsole;
	struct device	*sc_displaydv;

	struct wskbd_bell_data sc_bell_data;
	struct wskbd_keyrepeat_data sc_keyrepeat_data;

	int	sc_repeating;		/* we've called timeout() */
	keysym_t sc_repeatsym;		/* repeated symbol */

	int	sc_translating;		/* xlate to chars for emulation */

	int	sc_maplen;		/* number of entries in sc_map */
	struct wscons_keymap *sc_map;	/* current translation map */
	kbd_t sc_layout; /* current layout */
};

#define MOD_SHIFT_L		(1 << 0)
#define MOD_SHIFT_R		(1 << 1)
#define MOD_SHIFTLOCK		(1 << 2)
#define MOD_CAPSLOCK		(1 << 3)
#define MOD_CONTROL_L		(1 << 4)
#define MOD_CONTROL_R		(1 << 5)
#define MOD_META_L		(1 << 6)
#define MOD_META_R		(1 << 7)
#define MOD_MODESHIFT		(1 << 8)
#define MOD_NUMLOCK		(1 << 9)
#define MOD_COMPOSE		(1 << 10)
#define MOD_HOLDSCREEN		(1 << 11)
#define MOD_COMMAND		(1 << 12)
#define MOD_COMMAND1		(1 << 13)
#define MOD_COMMAND2		(1 << 14)

#define MOD_ANYSHIFT		(MOD_SHIFT_L | MOD_SHIFT_R | MOD_SHIFTLOCK)
#define MOD_ANYCONTROL		(MOD_CONTROL_L | MOD_CONTROL_R)
#define MOD_ANYMETA		(MOD_META_L | MOD_META_R)

#define MOD_ONESET(id, mask)	(((id)->t_modifiers & (mask)) != 0)
#define MOD_ALLSET(id, mask)	(((id)->t_modifiers & (mask)) == (mask))

int	wskbd_match __P((struct device *, struct cfdata *, void *));
void	wskbd_attach __P((struct device *, struct device *, void *));
static inline void update_leds __P((struct wskbd_internal *));
static inline void update_modifier __P((struct wskbd_internal *, u_int, int, int));
static int internal_command __P((struct wskbd_softc *, u_int *, keysym_t));
static keysym_t wskbd_translate __P((struct wskbd_internal *, u_int, int));
static void wskbd_holdscreen __P((struct wskbd_softc *, int));


struct cfattach wskbd_ca = {
	sizeof (struct wskbd_softc), wskbd_match, wskbd_attach,
};

#if NWSKBD > 0
extern struct cfdriver wskbd_cd;
#endif

#ifndef WSKBD_DEFAULT_BELL_PITCH
#define	WSKBD_DEFAULT_BELL_PITCH	1500	/* 1500Hz */
#endif
#ifndef WSKBD_DEFAULT_BELL_PERIOD
#define	WSKBD_DEFAULT_BELL_PERIOD	100	/* 100ms */
#endif
#ifndef WSKBD_DEFAULT_BELL_VOLUME
#define	WSKBD_DEFAULT_BELL_VOLUME	50	/* 50% volume */
#endif

struct wskbd_bell_data wskbd_default_bell_data = {
	WSKBD_BELL_DOALL,
	WSKBD_DEFAULT_BELL_PITCH,
	WSKBD_DEFAULT_BELL_PERIOD,
	WSKBD_DEFAULT_BELL_VOLUME,
};

#ifndef WSKBD_DEFAULT_KEYREPEAT_DEL1
#define	WSKBD_DEFAULT_KEYREPEAT_DEL1	400	/* 400ms to start repeating */
#endif
#ifndef WSKBD_DEFAULT_KEYREPEAT_DELN
#define	WSKBD_DEFAULT_KEYREPEAT_DELN	100	/* 100ms to between repeats */
#endif

struct wskbd_keyrepeat_data wskbd_default_keyrepeat_data = {
	WSKBD_KEYREPEAT_DOALL,
	WSKBD_DEFAULT_KEYREPEAT_DEL1,
	WSKBD_DEFAULT_KEYREPEAT_DELN,
};

cdev_decl(wskbd);
static void wskbd_repeat __P((void *v));

static int wskbd_console_initted;
static struct wskbd_softc *wskbd_console_device;
static struct wskbd_internal wskbd_console_data;

/*
 * Print function (for parent devices).
 */
int
wskbddevprint(aux, pnp)
	void *aux;
	const char *pnp;
{
#if 0
	struct wskbddev_attach_args *ap = aux;
#endif

	if (pnp)
		printf("wskbd at %s", pnp);
#if 0
	printf(" console %d", ap->console);
#endif

	return (UNCONF);
}

int
wskbd_match(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct wskbddev_attach_args *ap = aux;

	if (match->wskbddevcf_console != WSKBDDEVCF_CONSOLE_UNK) {
		/*
		 * If console-ness of device specified, either match
		 * exactly (at high priority), or fail.
		 */
		if (match->wskbddevcf_console != 0 && ap->console != 0)
			return (10);
		else
			return (0);
	}

	/* If console-ness unspecified, it wins. */
	return (1);
}

void
wskbd_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)self;
	struct wskbddev_attach_args *ap = aux;

	if (ap->console)
		printf(": console keyboard");
	printf("\n");

	if (ap->console) {
		sc->id = &wskbd_console_data;
	} else {
		sc->id = malloc(sizeof(struct wskbd_internal),
				M_DEVBUF, M_WAITOK);
		sc->id->t_keymap = ap->keymap;
		sc->id->t_modifiers = 0;
	}

	sc->id->t_sc = sc;

	sc->sc_accessops = ap->accessops;
	sc->sc_accesscookie = ap->accesscookie;
	sc->sc_ready = 0;				/* sanity */
	sc->sc_repeating = 0;
	sc->sc_translating = 1;
	sc->sc_ledstate = -1; /* force update */

	if (wskbd_load_keymap(sc->id->t_keymap,
			      &sc->sc_map, &sc->sc_maplen) != 0)
		panic("cannot load keymap");

	sc->sc_layout = sc->id->t_keymap->layout;

	if (ap->console) {
		KASSERT(wskbd_console_initted); 
		KASSERT(wskbd_console_device == NULL);
		wskbd_console_device = sc;
	}
	sc->sc_isconsole = ap->console;
	sc->sc_displaydv = NULL;

	/* set default bell and key repeat data */
	sc->sc_bell_data = wskbd_default_bell_data;
	sc->sc_keyrepeat_data = wskbd_default_keyrepeat_data;
}

void    
wskbd_cnattach(consops, conscookie, mapdata)
	const struct wskbd_consops *consops;
	void *conscookie;
	const struct wskbd_mapdata *mapdata;
{

	KASSERT(!wskbd_console_initted);

	wskbd_console_data.t_keymap = mapdata;

	wskbd_console_data.t_consops = consops;
	wskbd_console_data.t_consaccesscookie = conscookie;

	wsdisplay_set_cons_kbd(wskbd_cngetc, wskbd_cnpollc);

	wskbd_console_initted = 1;
}

static void
wskbd_repeat(v)
	void *v;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)v;
	int s = spltty();

	if (!sc->sc_repeating) {
		/*
		 * race condition: a "key up" event came in when wskbd_repeat()
		 * was already called but not yet spltty()'d
		 */
		splx(s);
		return;
	}
	if (sc->sc_displaydv != NULL)
		wsdisplay_kbdinput(sc->sc_displaydv, sc->sc_repeatsym);
	timeout(wskbd_repeat, sc,
		(hz * sc->sc_keyrepeat_data.delN) / 1000);
	splx(s);
}

void
wskbd_input(dev, type, value)
	struct device *dev;
	u_int type;
	int value;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dev; 
	struct wscons_event *ev;
	struct timeval xxxtime;
	keysym_t ks;
	int put;

	if (sc->sc_repeating) {
		sc->sc_repeating = 0;
		untimeout(wskbd_repeat, sc);
	}

	/*
	 * If /dev/kbd is not connected in event mode translate and
	 * send upstream.
	 */
	if (sc->sc_translating) {
		ks = wskbd_translate(sc->id, type, value);
		if (ks != KS_voidSymbol) {
			sc->sc_repeatsym = ks;
			if (sc->sc_displaydv != NULL)
				wsdisplay_kbdinput(sc->sc_displaydv,
						   sc->sc_repeatsym);

			sc->sc_repeating = 1;
			timeout(wskbd_repeat, sc,
				(hz * sc->sc_keyrepeat_data.del1) / 1000);
		}
		return;
	}

	/*
	 * Keyboard is generating events.  Turn this keystroke into an
	 * event and put it in the queue.  If the queue is full, the
	 * keystroke is lost (sorry!).
	 */

	/* no one to receive; punt!*/
	if (!sc->sc_ready)
		return;

	put = sc->sc_events.put;
	ev = &sc->sc_events.q[put];
	put = (put + 1) % WSEVENT_QSIZE;
	if (put == sc->sc_events.get) {
		log(LOG_WARNING, "%s: event queue overflow\n",
		    sc->sc_dv.dv_xname);
		return;
	}
	ev->type = type;
	ev->value = value;
	microtime(&xxxtime);
	TIMEVAL_TO_TIMESPEC(&xxxtime, &ev->time);
	sc->sc_events.put = put;
	WSEVENT_WAKEUP(&sc->sc_events);
}

#ifdef WSDISPLAY_COMPAT_RAWKBD
void
wskbd_rawinput(dev, buf, len)
	struct device *dev;
	char *buf;
	int len;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dev;
	int i;

	for (i = 0; i < len; i++)
		wsdisplay_kbdinput(sc->sc_displaydv, buf[i]);
	/* this is KS_GROUP_Ascii */
}
#endif

static void
wskbd_holdscreen(sc, hold)
	struct wskbd_softc *sc;
	int hold;
{
	int new_state;

	if (sc->sc_displaydv != NULL) {
		wsdisplay_kbdholdscreen(sc->sc_displaydv, hold);
		new_state = sc->sc_ledstate;
		if (hold)
			new_state |= WSKBD_LED_SCROLL;
		else
			new_state &= ~WSKBD_LED_SCROLL;
		if (new_state != sc->sc_ledstate) {
			(*sc->sc_accessops->set_leds)(sc->sc_accesscookie,
						      new_state);
			sc->sc_ledstate = new_state;
		}
	}
}

int
wskbd_enable(dev, on)
	struct device *dev;
	int on;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dev;
	int res;

	/* XXX reference count? */
	if (!on && (!sc->sc_translating || sc->sc_displaydv))
		return (EBUSY);

	res = (*sc->sc_accessops->enable)(sc->sc_accesscookie, on);
	return (res);
}

int
wskbdopen(dev, flags, mode, p)
	dev_t dev;
	int flags, mode;
	struct proc *p;
{
#if NWSKBD > 0
	struct wskbd_softc *sc;
	int unit;

	unit = minor(dev);
	if (unit >= wskbd_cd.cd_ndevs ||	/* make sure it was attached */
	    (sc = wskbd_cd.cd_devs[unit]) == NULL)
		return (ENXIO);

	if (sc->sc_events.io)			/* and that it's not in use */
		return (EBUSY);

	sc->sc_events.io = p;
	wsevent_init(&sc->sc_events);		/* may cause sleep */

	sc->sc_translating = 0;
	sc->sc_ready = 1;			/* start accepting events */

	wskbd_enable((struct device *)sc, 1);

	return (0);
#else
	return (ENXIO);
#endif /* NWSKBD > 0 */
}

int
wskbdclose(dev, flags, mode, p)
	dev_t dev;
	int flags, mode;
	struct proc *p;
{
#if NWSKBD > 0
	struct wskbd_softc *sc = wskbd_cd.cd_devs[minor(dev)];

	sc->sc_ready = 0;			/* stop accepting events */
	sc->sc_translating = 1;

	wsevent_fini(&sc->sc_events);
	sc->sc_events.io = NULL;

	wskbd_enable((struct device *)sc, 0);
#endif /* NWSKBD > 0 */
	return (0);
}

int
wskbdread(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{
#if NWSKBD > 0
	struct wskbd_softc *sc = wskbd_cd.cd_devs[minor(dev)];

	return (wsevent_read(&sc->sc_events, uio, flags));
#else
	return (ENXIO);
#endif /* NWSKBD > 0 */
}

int
wskbdioctl(dev, cmd, data, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
#if NWSKBD > 0
	struct wskbd_softc *sc = wskbd_cd.cd_devs[minor(dev)];
	int error;

	/*      
	 * Try the generic ioctls that the wskbd interface supports.
	 */
	switch (cmd) {
	case FIONBIO:		/* we will remove this someday (soon???) */
		return (0);

	case FIOASYNC:
		sc->sc_events.async = *(int *)data != 0;
		return (0);

	case TIOCSPGRP:
		if (*(int *)data != sc->sc_events.io->p_pgid)
			return (EPERM);
		return (0);
	}

	/*
	 * Try the keyboard driver for WSKBDIO ioctls.  It returns -1
	 * if it didn't recognize the request.
	 */
	error = wskbd_displayioctl((struct device *)sc, cmd, data, flag, p);
	return (error != -1 ? error : ENOTTY);
#else
	return (ENXIO);
#endif /* NWSKBD > 0 */
}

/*
 * WSKBDIO ioctls, handled in both emulation mode and in ``raw'' mode.
 * Some of these have no real effect in raw mode, however.
 */
int
wskbd_displayioctl(dev, cmd, data, flag, p)
	struct device *dev;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dev;
	struct wskbd_bell_data *ubdp, *kbdp;
	struct wskbd_keyrepeat_data *ukdp, *kkdp;
	struct wskbd_map_data *umdp;
	struct wskbd_mapdata md;
	void *buf;
	int len, error;

	switch (cmd) {
#define	SETBELL(dstp, srcp, dfltp)					\
    do {								\
	(dstp)->pitch = ((srcp)->which & WSKBD_BELL_DOPITCH) ?		\
	    (srcp)->pitch : (dfltp)->pitch;				\
	(dstp)->period = ((srcp)->which & WSKBD_BELL_DOPERIOD) ?	\
	    (srcp)->period : (dfltp)->period;				\
	(dstp)->volume = ((srcp)->which & WSKBD_BELL_DOVOLUME) ?	\
	    (srcp)->volume : (dfltp)->volume;				\
	(dstp)->which = WSKBD_BELL_DOALL;				\
    } while (0)

	case WSKBDIO_BELL:
		if ((flag & FWRITE) == 0)
			return (EACCES);
		return ((*sc->sc_accessops->ioctl)(sc->sc_accesscookie,
		    WSKBDIO_COMPLEXBELL, (caddr_t)&sc->sc_bell_data, flag, p));

	case WSKBDIO_COMPLEXBELL:
		if ((flag & FWRITE) == 0)
			return (EACCES);
		ubdp = (struct wskbd_bell_data *)data;
		SETBELL(ubdp, ubdp, &sc->sc_bell_data);
		return ((*sc->sc_accessops->ioctl)(sc->sc_accesscookie,
		    WSKBDIO_COMPLEXBELL, (caddr_t)ubdp, flag, p));

	case WSKBDIO_SETBELL:
		if ((flag & FWRITE) == 0)
			return (EACCES);
		kbdp = &sc->sc_bell_data;
setbell:
		ubdp = (struct wskbd_bell_data *)data;
		SETBELL(kbdp, ubdp, kbdp);
		return (0);

	case WSKBDIO_GETBELL:
		kbdp = &sc->sc_bell_data;
getbell:
		ubdp = (struct wskbd_bell_data *)data;
		SETBELL(ubdp, kbdp, kbdp);
		return (0);

	case WSKBDIO_SETDEFAULTBELL:
		if ((error = suser(p->p_ucred, &p->p_acflag)) != 0)
			return (error);
		kbdp = &wskbd_default_bell_data;
		goto setbell;


	case WSKBDIO_GETDEFAULTBELL:
		kbdp = &wskbd_default_bell_data;
		goto getbell;

#undef SETBELL

#define	SETKEYREPEAT(dstp, srcp, dfltp)					\
    do {								\
	(dstp)->del1 = ((srcp)->which & WSKBD_KEYREPEAT_DODEL1) ?	\
	    (srcp)->del1 : (dfltp)->del1;				\
	(dstp)->delN = ((srcp)->which & WSKBD_KEYREPEAT_DODELN) ?	\
	    (srcp)->delN : (dfltp)->delN;				\
	(dstp)->which = WSKBD_KEYREPEAT_DOALL;				\
    } while (0)

	case WSKBDIO_SETKEYREPEAT:
		if ((flag & FWRITE) == 0)
			return (EACCES);
		kkdp = &sc->sc_keyrepeat_data;
setkeyrepeat:
		ukdp = (struct wskbd_keyrepeat_data *)data;
		SETKEYREPEAT(kkdp, ukdp, kkdp);
		return (0);

	case WSKBDIO_GETKEYREPEAT:
		kkdp = &sc->sc_keyrepeat_data;
getkeyrepeat:
		ukdp = (struct wskbd_keyrepeat_data *)data;
		SETKEYREPEAT(ukdp, kkdp, kkdp);
		return (0);

	case WSKBDIO_SETDEFAULTKEYREPEAT:
		if ((error = suser(p->p_ucred, &p->p_acflag)) != 0)
			return (error);
		kkdp = &wskbd_default_keyrepeat_data;
		goto setkeyrepeat;


	case WSKBDIO_GETDEFAULTKEYREPEAT:
		kkdp = &wskbd_default_keyrepeat_data;
		goto getkeyrepeat;

#undef SETKEYREPEAT

	case WSKBDIO_SETMAP:
		if ((flag & FWRITE) == 0)
			return (EACCES);
		umdp = (struct wskbd_map_data *)data;
		len = umdp->maplen*sizeof(struct wscons_keymap);
		buf = malloc(len, M_TEMP, M_WAITOK);
		error = copyin(umdp->map, buf, len);
		if (error == 0) {
			wskbd_init_keymap(umdp->maplen,
					  &sc->sc_map, &sc->sc_maplen);
			memcpy(sc->sc_map, buf, len);
			sc->sc_layout = KB_USER;
		}
		free(buf, M_TEMP);
		return(error);

	case WSKBDIO_GETMAP:
		umdp = (struct wskbd_map_data *)data;
		if (umdp->maplen > sc->sc_maplen)
			umdp->maplen = sc->sc_maplen;
		error = copyout(sc->sc_map, umdp->map,
				umdp->maplen*sizeof(struct wscons_keymap));
		return(error);

	case WSKBDIO_GETENCODING:
		*((kbd_t *) data) = sc->sc_layout;
		return(0);

	case WSKBDIO_SETENCODING:
		if ((flag & FWRITE) == 0)
			return (EACCES);
		md = *(sc->id->t_keymap); /* structure assignment */
		md.layout = *((kbd_t *)data);
		error = wskbd_load_keymap(&md, &sc->sc_map, &sc->sc_maplen);
		if (error == 0)
			sc->sc_layout = *((kbd_t *)data);
		return(error);
	}

	/*
	 * Try the keyboard driver for WSKBDIO ioctls.  It returns -1
	 * if it didn't recognize the request, and in turn we return
	 * -1 if we didn't recognize the request.
	 */
/* printf("kbdaccess\n"); */
	error = (*sc->sc_accessops->ioctl)(sc->sc_accesscookie, cmd, data,
					   flag, p);
#ifdef WSDISPLAY_COMPAT_RAWKBD
	if (!error && cmd == WSKBDIO_SETMODE && *(int *)data == WSKBD_RAW) {
		int s = spltty();
		sc->id->t_modifiers &= ~(MOD_SHIFT_L | MOD_SHIFT_R
					 | MOD_CONTROL_L | MOD_CONTROL_R
					 | MOD_META_L | MOD_META_R
					 | MOD_COMMAND
					 | MOD_COMMAND1 | MOD_COMMAND2);
		if (sc->sc_repeating) {
			sc->sc_repeating = 0;
			untimeout(wskbd_repeat, sc);
		}
		splx(s);
	}
#endif
	return (error);
}

int
wskbdpoll(dev, events, p)
	dev_t dev;
	int events;
	struct proc *p;
{
#if NWSKBD > 0
	struct wskbd_softc *sc = wskbd_cd.cd_devs[minor(dev)];

	return (wsevent_poll(&sc->sc_events, events, p));
#else
	return (0);
#endif /* NWSKBD > 0 */
}

int
wskbd_is_console(dv)
	struct device *dv;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dv;

	KASSERT(sc != NULL);
	return (sc->sc_isconsole);
}

struct device *
wskbd_display(dv)
	struct device *dv;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dv;

	KASSERT(sc != NULL);
	return (sc->sc_displaydv);
}

void
wskbd_set_display(dv, displaydv)
	struct device *dv, *displaydv;
{
	struct wskbd_softc *sc = (struct wskbd_softc *)dv;

	KASSERT(sc != NULL);
	sc->sc_displaydv = displaydv;
}

/*
 * Console interface.
 */
int
wskbd_cngetc(dev)
	dev_t dev;
{
	u_int type;
	int data;
	keysym_t ks;

	if (!wskbd_console_initted)
		return 0;

	if (wskbd_console_device != NULL &&
	    !wskbd_console_device->sc_translating)
		return 0;

	for(;;) {
		(*wskbd_console_data.t_consops->getc)
		    (wskbd_console_data.t_consaccesscookie, &type, &data);
		ks = wskbd_translate(&wskbd_console_data, type, data);
		
		if (KS_GROUP(ks) == KS_GROUP_Ascii)
			return (KS_VALUE(ks));	
	}
}

void
wskbd_cnpollc(dev, poll)
	dev_t dev;
	int poll;
{

	if (!wskbd_console_initted)
		return;

	if (wskbd_console_device != NULL &&
	    !wskbd_console_device->sc_translating)
		return;

	(*wskbd_console_data.t_consops->pollc)
	    (wskbd_console_data.t_consaccesscookie, poll);
}

static inline void
update_leds(id)
	struct wskbd_internal *id;
{
	int new_state;

	new_state = 0;
	if (id->t_modifiers & (MOD_SHIFTLOCK | MOD_CAPSLOCK))
		new_state |= WSKBD_LED_CAPS;
	if (id->t_modifiers & MOD_NUMLOCK)
		new_state |= WSKBD_LED_NUM;
	if (id->t_modifiers & MOD_COMPOSE)
		new_state |= WSKBD_LED_COMPOSE;
	if (id->t_modifiers & MOD_HOLDSCREEN)
		new_state |= WSKBD_LED_SCROLL;

	if (id->t_sc && new_state != id->t_sc->sc_ledstate) {
		(*id->t_sc->sc_accessops->set_leds)
		    (id->t_sc->sc_accesscookie, new_state);
		id->t_sc->sc_ledstate = new_state;
	}
}

static inline void
update_modifier(id, type, toggle, mask)
	struct wskbd_internal *id;
	u_int type;
	int toggle;
	int mask;
{
	if (toggle) {
		if (type == WSCONS_EVENT_KEY_DOWN)
			id->t_modifiers ^= mask;
	} else {
		if (type == WSCONS_EVENT_KEY_DOWN)
			id->t_modifiers |= mask;
		else
			id->t_modifiers &= ~mask;
	}
}

static int
internal_command(sc, type, ksym)
	struct wskbd_softc *sc;
	u_int *type;
	keysym_t ksym;
{
	switch (ksym) {
	case KS_Cmd:
		update_modifier(sc->id, *type, 0, MOD_COMMAND);
		break;

	case KS_Cmd1:
		update_modifier(sc->id, *type, 0, MOD_COMMAND1);
		break;

	case KS_Cmd2:
		update_modifier(sc->id, *type, 0, MOD_COMMAND2);
		break;
	}

	if (*type != WSCONS_EVENT_KEY_DOWN ||
	    (! MOD_ONESET(sc->id, MOD_COMMAND) &&
	     ! MOD_ALLSET(sc->id, MOD_COMMAND1 | MOD_COMMAND2)))
		return (0);

	switch (ksym) {
#ifdef DDB
	case KS_Cmd_Debugger:
		if (sc->sc_isconsole)
			console_debugger();
		/* discard this key (ddb discarded command modifiers) */
		*type = WSCONS_EVENT_KEY_UP;
		return (1);
#endif

	case KS_Cmd_Screen0:
	case KS_Cmd_Screen1:
	case KS_Cmd_Screen2:
	case KS_Cmd_Screen3:
	case KS_Cmd_Screen4:
	case KS_Cmd_Screen5:
	case KS_Cmd_Screen6:
	case KS_Cmd_Screen7:
	case KS_Cmd_Screen8:
	case KS_Cmd_Screen9:
		wsdisplay_switch(sc->sc_displaydv, ksym - KS_Cmd_Screen0, 0);
		return (1);
	case KS_Cmd_ResetEmul:
		wsdisplay_reset(sc->sc_displaydv, WSDISPLAY_RESETEMUL);
		return (1);
	case KS_Cmd_ResetClose:
		wsdisplay_reset(sc->sc_displaydv, WSDISPLAY_RESETCLOSE);
		return (1);
	}
	return (0);
}

static keysym_t
wskbd_translate(id, type, value)
	struct wskbd_internal *id;
	u_int type;
	int value;
{
	struct wskbd_softc *sc = id->t_sc;
	keysym_t ksym, res, *group;
	struct wscons_keymap kpbuf, *kp;
	int iscommand = 0;

	if (type == WSCONS_EVENT_ALL_KEYS_UP) {
		id->t_modifiers &= ~(MOD_SHIFT_L | MOD_SHIFT_R
				| MOD_CONTROL_L | MOD_CONTROL_R
				| MOD_META_L | MOD_META_R
				| MOD_MODESHIFT
				| MOD_COMMAND | MOD_COMMAND1 | MOD_COMMAND2);
		update_leds(id);
		return (KS_voidSymbol);
	}

	if (sc != NULL) {
		if (value < 0 || value >= sc->sc_maplen) {
#ifdef DEBUG
			printf("wskbd_translate: keycode %d out of range\n",
			       value);
#endif
			return (KS_voidSymbol);
		}
		kp = sc->sc_map + value;
	} else {
		kp = &kpbuf;
		wskbd_get_mapentry(id->t_keymap, value, kp);
	}

	/* if this key has a command, process it first */
	if (sc != NULL && kp->command != KS_voidSymbol)
		iscommand = internal_command(sc, &type, kp->command);

	/* Now update modifiers */
	switch (kp->group1[0]) {
	case KS_Shift_L:
		update_modifier(id, type, 0, MOD_SHIFT_L);
		break;

	case KS_Shift_R:
		update_modifier(id, type, 0, MOD_SHIFT_R);
		break;

	case KS_Shift_Lock:
		update_modifier(id, type, 1, MOD_SHIFTLOCK);
		break;

	case KS_Caps_Lock:
		update_modifier(id, type, 1, MOD_CAPSLOCK);
		break;

	case KS_Control_L:
		update_modifier(id, type, 0, MOD_CONTROL_L);
		break;

	case KS_Control_R:
		update_modifier(id, type, 0, MOD_CONTROL_R);
		break;

	case KS_Alt_L:
		update_modifier(id, type, 0, MOD_META_L);
		break;

	case KS_Alt_R:
		update_modifier(id, type, 0, MOD_META_R);
		break;

	case KS_Mode_switch:
		update_modifier(id, type, 0, MOD_MODESHIFT);
		break;

	case KS_Num_Lock:
		update_modifier(id, type, 1, MOD_NUMLOCK);
		break;

	case KS_Hold_Screen:
		if (sc != NULL) {
			update_modifier(id, type, 1, MOD_HOLDSCREEN);
			wskbd_holdscreen(sc, id->t_modifiers & MOD_HOLDSCREEN);
		}
		break;
	}

	/* If this is a key release or we are in command mode, we are done */
	if (type != WSCONS_EVENT_KEY_DOWN || iscommand) {
		update_leds(id);
		return (KS_voidSymbol);
	}

	/* Get the keysym */
	if (id->t_modifiers & MOD_MODESHIFT)
		group = & kp->group2[0];
	else
		group = & kp->group1[0];

	if ((id->t_modifiers & MOD_NUMLOCK) != 0 &&
	    KS_GROUP(group[1]) == KS_GROUP_Keypad) {
		if (MOD_ONESET(id, MOD_ANYSHIFT))
			ksym = group[0];
		else
			ksym = group[1];
	} else if (! MOD_ONESET(id, MOD_ANYSHIFT | MOD_CAPSLOCK)) {
		ksym = group[0];
	} else if (MOD_ONESET(id, MOD_CAPSLOCK)) {
		if (! MOD_ONESET(id, MOD_SHIFT_L | MOD_SHIFT_R))
			ksym = group[0];
		else
			ksym = group[1];
		if (ksym >= KS_a && ksym <= KS_z)
			ksym += KS_A - KS_a;
		else if (ksym >= KS_agrave && ksym <= KS_thorn &&
			 ksym != KS_division)
			ksym += KS_Agrave - KS_agrave;
	} else if (MOD_ONESET(id, MOD_ANYSHIFT)) {
		ksym = group[1];
	} else {
		ksym = group[0];
	}

	/* Process compose sequence and dead accents */
	res = KS_voidSymbol;

	switch (KS_GROUP(ksym)) {
	case KS_GROUP_Ascii:
	case KS_GROUP_Keypad:
	case KS_GROUP_Function:
		res = ksym;
		break;

	case KS_GROUP_Mod:
		if (ksym == KS_Multi_key) {
			update_modifier(id, 1, 0, MOD_COMPOSE);
			id->t_composelen = 2;
		}
		break;

	case KS_GROUP_Dead:
		if (id->t_composelen == 0) {
			update_modifier(id, 1, 0, MOD_COMPOSE);
			id->t_composelen = 1;
			id->t_composebuf[0] = ksym;
		} else
			res = ksym;
		break;
	}

	if (res == KS_voidSymbol) {
		update_leds(id);
		return (res);
	}

	if (id->t_composelen > 0) {
		id->t_composebuf[2 - id->t_composelen] = res;
		if (--id->t_composelen == 0) {
			res = wskbd_compose_value(id->t_composebuf);
			update_modifier(id, 0, 0, MOD_COMPOSE);
		} else {
			return (KS_voidSymbol);
		}
	}

	update_leds(id);

	/* We are done, return the symbol */
	if (KS_GROUP(res) == KS_GROUP_Ascii) {
		if (MOD_ONESET(id, MOD_ANYCONTROL)) {
			if ((res >= KS_at && res <= KS_z) || res == KS_space)
				res = res & 0x1f;
			else if (res == KS_2)
				res = 0x00;
			else if (res >= KS_3 && res <= KS_7)
				res = KS_Escape + (res - KS_3);
			else if (res == KS_8)
				res = KS_Delete;
		}
		if (MOD_ONESET(id, MOD_ANYMETA))
			res |= 0x80;
	}

	return (res);
}
