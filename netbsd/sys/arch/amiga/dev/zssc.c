/*	$NetBSD: zssc.c,v 1.28 1999/01/10 13:24:11 tron Exp $	*/

/*
 * Copyright (c) 1994 Michael L. Hitch
 * Copyright (c) 1982, 1990 The Regents of the University of California.
 * All rights reserved.
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
 *	@(#)dma.c
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <dev/scsipi/scsi_all.h>
#include <dev/scsipi/scsipi_all.h>
#include <dev/scsipi/scsiconf.h>
#include <amiga/amiga/custom.h>
#include <amiga/amiga/cc.h>
#include <amiga/amiga/device.h>
#include <amiga/amiga/isr.h>
#include <amiga/dev/siopreg.h>
#include <amiga/dev/siopvar.h>
#include <amiga/dev/zbusvar.h>

void zsscattach __P((struct device *, struct device *, void *));
int  zsscmatch __P((struct device *, struct cfdata *, void *));
int  zssc_dmaintr __P((void *));
#ifdef DEBUG
void zssc_dump __P((void));
#endif

struct scsipi_device zssc_scsidev = {
	NULL,		/* use default error handler */
	NULL,		/* do not have a start functio */
	NULL,		/* have no async handler */
	NULL,		/* Use default done routine */
};


#ifdef DEBUG
#endif

struct cfattach zssc_ca = {
	sizeof(struct siop_softc), zsscmatch, zsscattach
};

/*
 * if we are an PPI Zeus
 */
int
zsscmatch(pdp, cfp, auxp)
	struct device *pdp;
	struct cfdata *cfp;
	void *auxp;
{
	struct zbus_args *zap;

	zap = auxp;
	if (zap->manid == 2026 && zap->prodid == 150)
		return(1);
	return(0);
}

void
zsscattach(pdp, dp, auxp)
	struct device *pdp, *dp;
	void *auxp;
{
	struct siop_softc *sc;
	struct zbus_args *zap;
	siop_regmap_p rp;

	printf("\n");

	zap = auxp;

	sc = (struct siop_softc *)dp;
	sc->sc_siopp = rp = (siop_regmap_p)((caddr_t)zap->va + 0x4000);

	/*
	 * CTEST7 = 00
	 */
	sc->sc_clock_freq = 66;		/* Clock = 66Mhz */
	sc->sc_ctest7 = 0x00;
	sc->sc_dcntl = 0x00;

	alloc_sicallback();

	sc->sc_adapter.scsipi_cmd = siop_scsicmd;
	sc->sc_adapter.scsipi_minphys = siop_minphys;

	sc->sc_link.scsipi_scsi.channel = SCSI_CHANNEL_ONLY_ONE;
	sc->sc_link.adapter_softc = sc;
	sc->sc_link.scsipi_scsi.adapter_target = 7;
	sc->sc_link.adapter = &sc->sc_adapter;
	sc->sc_link.device = &zssc_scsidev;
	sc->sc_link.openings = 2;
	sc->sc_link.scsipi_scsi.max_target = 7;
	sc->sc_link.scsipi_scsi.max_lun = 7;
	sc->sc_link.type = BUS_SCSI;

	siopinitialize(sc);

	sc->sc_isr.isr_intr = zssc_dmaintr;
	sc->sc_isr.isr_arg = sc;
	sc->sc_isr.isr_ipl = 6;
	add_isr(&sc->sc_isr);

	/*
	 * attach all scsi units on us
	 */
	config_found(dp, &sc->sc_link, scsiprint);
}

/*
 * Level 6 interrupt processing for the Progressive Peripherals Inc
 * Zeus SCSI.  Because the level 6 interrupt is above splbio, the
 * interrupt status is saved and an sicallback to the level 2 interrupt
 * handler scheduled.  This way, the actual processing of the interrupt
 * can be deferred until splbio is unblocked.
 */

int
zssc_dmaintr(arg)
	void *arg;
{
	struct siop_softc *sc = arg;
	siop_regmap_p rp;
	int istat;

	if (sc->sc_flags & SIOP_INTSOFF)
		return (0);	/* interrupts are not active */
	rp = sc->sc_siopp;
	istat = rp->siop_istat;
	if ((istat & (SIOP_ISTAT_SIP | SIOP_ISTAT_DIP)) == 0)
		return(0);
	/*
	 * save interrupt status, DMA status, and SCSI status 0
	 * (may need to deal with stacked interrupts?)
	 */
	sc->sc_sstat0 = rp->siop_sstat0;
	sc->sc_istat = istat;
	sc->sc_dstat = rp->siop_dstat;
	/*
	 * disable interrupts until the callback can process this
	 * interrupt.
	 */
	rp->siop_sien = 0;
	rp->siop_dien = 0;
	sc->sc_flags |= SIOP_INTDEFER | SIOP_INTSOFF;
	add_sicallback((sifunc_t)siopintr, sc, NULL);
	return(1);
}

#ifdef DEBUG
void
zssc_dump()
{
	extern struct cfdriver zssc_cd;
	int i;

	for (i = 0; i < zssc_cd.cd_ndevs; ++i)
		if (zssc_cd.cd_devs[i])
			siop_dump(zssc_cd.cd_devs[i]);
}
#endif
