/*	$NetBSD: ustarfs.c,v 1.8 1999/03/31 01:50:26 cgd Exp $	*/

/* [Notice revision 2.2]
 * Copyright (c) 1997, 1998 Avalon Computer Systems, Inc.
 * All rights reserved.
 *
 * Author: Ross Harvey
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright and
 *    author notice, this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Avalon Computer Systems, Inc. nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. This copyright will be assigned to The NetBSD Foundation on
 *    1/1/2000 unless these terms (including possibly the assignment
 *    date) are updated in writing by Avalon prior to the latest specified
 *    assignment date.
 *
 * THIS SOFTWARE IS PROVIDED BY AVALON COMPUTER SYSTEMS, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL AVALON OR THE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


/*
 ******************************* USTAR FS *******************************
 */

/*
 * Implement an ROFS with an 8K boot area followed by ustar-format data.
 * The point: minimal FS overhead, and it's easy (well, `possible') to
 * split files over multiple volumes.
 *
 * XXX - TODO LIST
 * --- - ---- ----
 * XXX - tag volume numbers and verify that the correct volume is
 *       inserted after volume swaps.
 *
 * XXX - stop hardwiring FS metadata for floppies...embed it in a file,
 * 	 file name, or something. (Remember __SYMDEF? :-)
 *
 * XXX Does not currently implement:
 * XXX
 * XXX LIBSA_NO_FS_CLOSE
 * XXX LIBSA_NO_FS_SEEK
 * XXX LIBSA_NO_FS_WRITE
 * XXX LIBSA_NO_FS_SYMLINK (does this even make sense?)
 * XXX LIBSA_FS_SINGLECOMPONENT
 */

#ifdef _STANDALONE
#include <lib/libkern/libkern.h>
#else
#include <string.h>
#endif
#include "stand.h"
#include "ustarfs.h"

#define	BBSIZE	8192
#define	USTAR_NAME_BLOCK 512

/*
 * Virtual offset: relative to start of ustar archive
 * Logical offset: volume-relative
 * Physical offset: the usual meaning
 */

/* virtual offset to volume number */
 
#define	vda2vn(_v,_volsize) ((_v) / (_volsize))	

/* conversions between the three different levels of disk addresses */

#define	vda2lda(_v,_volsize) ((_v) % (_volsize))
#define	lda2vda(_v,_volsize,_volnumber) ((_v) + (_volsize) * (_volnumber))

#define	lda2pda(_lda) ((_lda) + ustarfs_mode_offset)
#define	pda2lda(_pda) ((_pda) - ustarfs_mode_offset)
/*
 * Change this to off_t if you want to support big volumes. If we only use
 * ustarfs on floppies it can stay int for libsa code density.
 *
 * It needs to be signed.
 */
typedef	int ustoffs;

typedef struct ustar_struct {
	char	ust_name[100],
		ust_mode[8],
		ust_uid[8],
		ust_gid[8],
		ust_size[12],
		ust_misc[12 + 8 + 1 + 100],
		ust_magic[6];
	/* there is more, but we don't care */
} ustar_t;

/*
 * We buffer one even cylindar of data...it's actually only really one
 * cyl on a 1.44M floppy, but on other devices it's fast enough with any
 * kind of block buffering, so we optimize for the slowest device.
 */

typedef struct ust_active_struct {
	ustar_t	uas_active;
	char	uas_1cyl[18 * 2 * 512];
	ustoffs	uas_volsize;		/* XXX this is hardwired now */
	ustoffs	uas_windowbase;		/* relative to volume 0 */
	ustoffs	uas_filestart;		/* relative to volume 0 */
	ustoffs	uas_fseek;		/* relative to file */
	ustoffs	uas_filesize;		/* relative to volume 0 */
	int	uas_init_window;	/* data present in window */
	int	uas_init_fs;		/* ust FS actually found */
	int	uas_volzerosig;		/* ID volume 0 by signature */
	int	uas_offset;		/* amount of cylinder below lba 0 */
} ust_active_t;

static const char formatid[] = "USTARFS",
		  metaname[] = "USTAR.volsize.";

static int ustarfs_mode_offset = BBSIZE;

static int checksig __P((ust_active_t *));
static int convert __P((const char *, int, int));
static int get_volume __P((struct open_file *, int));
static void setwindow(ust_active_t *, ustoffs, ustoffs);
static int ustarfs_cylinder_read __P((struct open_file *, ustoffs, int));
static void ustarfs_sscanf __P((const char *, const char *, int *));
static int read512block __P((struct open_file *, ustoffs, char block[512]));

static int
convert(f, base, fw)
	const char *f;
	int base, fw;
{
	int	i, c, result = 0;

	while(fw > 0 && *f == ' ') {
		--fw;
		++f;
	}
	for(i = 0; i < fw; ++i) {
		c = f[i];
		if ('0' <= c && c < '0' + base) {
			c -= '0';
			result = result * base + c;
		} else	break;
	}
	return result;
}

static void
ustarfs_sscanf(s,f,xi)
	const char *s,*f;
	int *xi;
{
	*xi = convert(s, 8, convert(f + 1, 10, 99));
}

static int
ustarfs_cylinder_read(f, seek2, forcelabel)
	struct open_file *f;
	ustoffs seek2;
{
	int e = 0;	/* XXX work around gcc warning */
	ustoffs	lda;
	char *xferbase;
	ust_active_t *ustf;
	size_t	xferrqst, xfercount;

	ustf = f->f_fsdata;
	xferrqst = sizeof ustf->uas_1cyl;
	xferbase = ustf->uas_1cyl;
	lda = pda2lda(seek2);
	if (lda < 0) {
		lda = -lda;
		ustf->uas_offset = lda;
		/*
		 * don't read the label unless we have to. (Preserve
		 * sequential block access so tape boot works.)
		 */
		if (!forcelabel) {
			memset(xferbase, 0, lda);
			xferrqst -= lda;
			xferbase += lda;
			seek2    += lda;
		}
	} else
		ustf->uas_offset = 0;
	while(xferrqst > 0) {
		e = DEV_STRATEGY(f->f_dev)(f->f_devdata, F_READ, seek2 / 512,
			xferrqst, xferbase, &xfercount);
		if (e)
			break;
		if (xfercount != xferrqst)
			printf("Warning, unexpected short transfer %d/%d\n",
				(int)xfercount, (int)xferrqst);
		xferrqst -= xfercount;
		xferbase += xfercount;
	}
	return e;
}

static int
checksig(ustf)
	ust_active_t *ustf;
{
	int	i, rcs;

	for(i = rcs = 0; i < sizeof ustf->uas_1cyl; ++i)
		rcs += ustf->uas_1cyl[i];
	return rcs;
}

static int
get_volume(f, vn)
	struct open_file *f;
	int vn;
{
	int	e, needvolume, havevolume;
	ust_active_t *ustf;

	ustf = f->f_fsdata;
	havevolume = vda2vn(ustf->uas_windowbase, ustf->uas_volsize);
	needvolume = vn;
	while(havevolume != needvolume) {
		printf("\nPlease ");
		if (havevolume >= 0)
			printf("remove disk %d, ", havevolume + 1);
		printf("insert disk %d, and type return...",
			needvolume + 1);
		getchar();
		printf("\n");
		e = ustarfs_cylinder_read(f, 0, 1);
		if (e)		/* Try again on error, needed on i386 */
			e = ustarfs_cylinder_read(f, 0, 1);
		if (e)
			return e;
		if(strncmp(formatid, ustf->uas_1cyl, strlen(formatid))) {
			/* no magic, might be OK if we want volume 0 */
			if (ustf->uas_volzerosig == checksig(ustf)) {
				havevolume = 0;
				continue;
			}
			printf("Disk is not from the volume set?!\n");
			havevolume = -2;
			continue;
		}
		ustarfs_sscanf(ustf->uas_1cyl + strlen(formatid), "%9o",
			&havevolume);
		--havevolume;
	}
	return 0;
}

static void
setwindow(ust_active_t *ustf, ustoffs pda, ustoffs vda)
{
	ustf->uas_windowbase = lda2vda(pda2lda(pda), ustf->uas_volsize,
					vda2vn(vda, ustf->uas_volsize))
			     + ustf->uas_offset;
	ustf->uas_init_window = 1;
}

static int
read512block(f, vda, block)
	struct open_file *f;
	ustoffs vda;
	char block[512];
{
	ustoffs pda;
	ssize_t	e;
	int	dienow;
	ust_active_t *ustf;

	dienow = 0;
	ustf = f->f_fsdata;

	if (!ustf->uas_init_window
	&&   ustf->uas_windowbase == 0) {
		/*
		 * The algorithm doesn't require this, but without it we would
		 * need some trick to get the cylinder zero signature computed.
		 * That signature is used to identify volume zero, which we
		 * don't give a USTARFS label to. (It's platform-dependent.)
		 */
		e = ustarfs_cylinder_read(f, 0, 0);
		if (e)
			return e;
		ustf->uas_volzerosig = checksig(ustf);
		setwindow(ustf, 0, 0);
	}
	/*
	 * if (vda in window)
	 * 	copy out and return data
	 * if (vda is on some other disk)
	 * 	do disk swap
	 * get physical disk address
	 * round down to cylinder boundary
	 * read cylindar
	 * set window (in vda space) and try again
	 * [ there is an implicit assumption that windowbase always identifies
	 *    the current volume, even if initwindow == 0. This way, a
	 *    windowbase of 0 causes the initial volume to be disk 0 ]
	 */
tryagain:
	if(ustf->uas_init_window
	&& ustf->uas_windowbase <= vda && vda <
	   ustf->uas_windowbase + sizeof ustf->uas_1cyl - ustf->uas_offset) {
		memcpy(block, ustf->uas_1cyl
				+ (vda - ustf->uas_windowbase)
				+ ustf->uas_offset, 512);
		return 0;
	}
	if (dienow++)
		panic("ustarfs read512block");
	ustf->uas_init_window = 0;
	e = get_volume(f, vda2vn(vda, ustf->uas_volsize));
	if (e)
		return e;
	pda = lda2pda(vda2lda(vda, ustf->uas_volsize));
	pda-= pda % sizeof ustf->uas_1cyl;
	e = ustarfs_cylinder_read(f, pda, 0);
	if (e)
		return e;
	setwindow(ustf, pda, vda);
	goto tryagain;
}

int
ustarfs_open(path, f)
	char *path;
	struct open_file *f;

{
	ust_active_t *ustf;
	ustoffs offset;
	char	block[512];
	int	filesize;
	int	e, e2;
	int	newvolblocks;

	if (*path == '/')
		++path;
	e = EINVAL;
	f->f_fsdata = ustf = alloc(sizeof *ustf);
	memset(ustf, 0, sizeof *ustf);
	offset = 0;
	/* default to 2880 sector floppy */
	ustf->uas_volsize = 80 * 2 * 18 * 512 - lda2pda(0);
	ustf->uas_fseek = 0;
	for(;;) {
		ustf->uas_filestart = offset;
		e2 = read512block(f, offset, block);
		if (e2) {
			e = e2;
			break;
		}
		memcpy(&ustf->uas_active, block, sizeof ustf->uas_active);
		if(strncmp(ustf->uas_active.ust_magic, "ustar", 5))
			break;
		e = ENOENT;	/* it must be an actual ustarfs */
		ustf->uas_init_fs = 1;
		/* if volume metadata is found, use it */
		if(strncmp(ustf->uas_active.ust_name, metaname,
		    strlen(metaname)) == 0) {
			ustarfs_sscanf(ustf->uas_active.ust_name
				+ strlen(metaname), "%99o", &newvolblocks);
			ustf->uas_volsize = newvolblocks * 512
					  - lda2pda(0);
		}
		ustarfs_sscanf(ustf->uas_active.ust_size,"%12o",&filesize);
		if(strncmp(ustf->uas_active.ust_name, path,
		    sizeof ustf->uas_active.ust_name) == 0) {
			ustf->uas_filesize = filesize;
			e = 0;
			break;
		}
		offset += USTAR_NAME_BLOCK + filesize;
		filesize %= 512;
		if (filesize)
			offset += 512 - filesize;
	}
	if (e) {
		free(ustf, sizeof *ustf);
		f->f_fsdata = 0;
	}
	return e;
}

#ifndef LIBSA_NO_FS_WRITE
int
ustarfs_write(f, start, size, resid)
	struct open_file *f;
	void *start;
	size_t size;
	size_t *resid;
{
	return (EROFS);
}
#endif /* !LIBSA_NO_FS_WRITE */

#ifndef LIBSA_NO_FS_SEEK
off_t
ustarfs_seek(f, offs, whence)
	struct open_file *f;
	off_t offs;
	int whence;
{
	ust_active_t *ustf;

	ustf = f->f_fsdata;
	switch (whence) {
	    case SEEK_SET:
		ustf->uas_fseek = offs;
		break;
	    case SEEK_CUR:
		ustf->uas_fseek += offs;
		break;
	    case SEEK_END:
		ustf->uas_fseek = ustf->uas_filesize - offs;
		break;
	    default:
		return -1;
	}
	return ustf->uas_fseek;
}
#endif /* !LIBSA_NO_FS_CLOSE */

int
ustarfs_read(f, start, size, resid)
	struct open_file *f;
	void *start;
	size_t size;
	size_t *resid;
{
	ust_active_t *ustf;
	int	e;
	char	*space512;
	int	blkoffs,
		readoffs,
		bufferoffset;
	size_t	seg;
	int	infile,
		inbuffer;

	e = 0;
	space512 = alloc(512);
	ustf = f->f_fsdata;
	while(size != 0) {
		if (ustf->uas_fseek >= ustf->uas_filesize)
			break;
		bufferoffset = ustf->uas_fseek % 512;
		blkoffs  = ustf->uas_fseek - bufferoffset;
		readoffs = ustf->uas_filestart + 512 + blkoffs;
		e = read512block(f, readoffs, space512);
		if (e)
			break;
		seg = size;
		inbuffer = 512 - bufferoffset;
		if (inbuffer < seg)
			seg = inbuffer;
		infile = ustf->uas_filesize - ustf->uas_fseek;
		if (infile < seg)
			seg = infile;
		memcpy(start, space512 + bufferoffset, seg);
		ustf->uas_fseek += seg;
		start = (caddr_t)start + seg;
		size  -= seg;
	}
	if (resid)
		*resid = size;
	free(space512, 512);
	return e;
}

int
ustarfs_stat(f, sb)
	struct open_file *f;
	struct stat *sb;
{
	int	mode, uid, gid;
	ust_active_t *ustf;

	if (f == NULL)
		return EINVAL;
	ustf = f->f_fsdata;
	memset(sb, 0, sizeof *sb);
	ustarfs_sscanf(ustf->uas_active.ust_mode, "%8o", &mode);
	ustarfs_sscanf(ustf->uas_active.ust_uid, "%8o", &uid);
	ustarfs_sscanf(ustf->uas_active.ust_gid, "%8o", &gid);
	sb->st_mode = mode;
	sb->st_uid  = uid;
	sb->st_gid  = gid;
	sb->st_size = ustf->uas_filesize;
	return 0;
}

#ifndef LIBSA_NO_FS_CLOSE
int
ustarfs_close(f)
	struct open_file *f;
{
	if (f == NULL || f->f_fsdata == NULL)
		return EINVAL;
	free(f->f_fsdata, sizeof(ust_active_t));
	f->f_fsdata = 0;
	return 0;
}
#endif /* !LIBSA_NO_FS_CLOSE */
