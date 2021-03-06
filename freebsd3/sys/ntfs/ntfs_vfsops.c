/*-
 * Copyright (c) 1998, 1999 Semen Ustimenko
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: ntfs_vfsops.c,v 1.2.2.1 1999/03/14 09:47:05 semenu Exp $
 */


#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/fcntl.h>
#include <sys/malloc.h>

#include <vm/vm.h>
#include <vm/vm_param.h>
#include <vm/vm_prot.h>
#include <vm/vm_page.h>
#include <vm/vm_object.h>
#include <vm/vm_extern.h>

#include <miscfs/specfs/specdev.h>

/*#define NTFS_DEBUG 1*/
#include <ntfs/ntfs.h>
#include <ntfs/ntfs_inode.h>
#include <ntfs/ntfs_subr.h>
#include <ntfs/ntfs_vfsops.h>
#include <ntfs/ntfs_ihash.h>
#include <ntfs/ntfs_extern.h>
#include <ntfs/ntfsmount.h>

#if __FreeBSD_version >= 300000
MALLOC_DEFINE(M_NTFSMNT, "NTFS mount", "NTFS mount structure");
MALLOC_DEFINE(M_NTFSNTNODE,"NTFS ntnode",  "NTFS ntnode information");
MALLOC_DEFINE(M_NTFSFNODE,"NTFS fnode",  "NTFS fnode information");
MALLOC_DEFINE(M_NTFSDIR,"NTFS dir",  "NTFS dir buffer");
#endif

static int	ntfs_mount __P((struct mount *, char *, caddr_t,
				struct nameidata *, struct proc *));
static int	ntfs_quotactl __P((struct mount *, int, uid_t, caddr_t,
				   struct proc *));
static int	ntfs_root __P((struct mount *, struct vnode **));
static int	ntfs_start __P((struct mount *, int, struct proc *));
static int	ntfs_statfs __P((struct mount *, struct statfs *,
				 struct proc *));
static int	ntfs_sync __P((struct mount *, int, struct ucred *,
			       struct proc *));
static int	ntfs_unmount __P((struct mount *, int, struct proc *));
static int	ntfs_vget __P((struct mount *mp, ino_t ino,
			       struct vnode **vpp));
static int	ntfs_mountfs __P((register struct vnode *, struct mount *, 
				  struct ntfs_args *, struct proc *));
static int	ntfs_vptofh __P((struct vnode *, struct fid *));

#if __FreeBSD_version >= 300000
static int	ntfs_init __P((struct vfsconf *));
static int	ntfs_fhtovp __P((struct mount *, struct fid *,
				 struct sockaddr *, struct vnode **,
				 int *, struct ucred **));
#else
static int	ntfs_init __P((void));
static int	ntfs_fhtovp __P((struct mount *, struct fid *,
				 struct mbuf *, struct vnode **,
				 int *, struct ucred **));
#endif

#if __FreeBSD_version >= 300000
static int
ntfs_init (
	struct vfsconf *vcp )
#else
static int
ntfs_init ()
#endif
{
	static first=1;

	if(!first) return (0);
	first = 1;

	printf("ntfs_init(): \n");

	ntfs_nthashinit();

	return 0;
}

static int
ntfs_mount ( 
	struct mount *mp,
	char *path,
	caddr_t data,
	struct nameidata *ndp,
	struct proc *p )
{
	u_int		size;
	int		err = 0;
	struct vnode	*devvp;
	struct ntfs_args args;

	/*
	 * Use NULL path to flag a root mount
	 */
	if( path == NULL) {
		/*
		 ***
		 * Mounting root file system
		 ***
		 */
	
		/* Get vnode for root device*/
		if( bdevvp( rootdev, &rootvp))
			panic("ffs_mountroot: can't setup bdevvp for root");

		/*
		 * FS specific handling
		 */
		mp->mnt_flag |= MNT_RDONLY;	/* XXX globally applicable?*/

		/*
		 * Attempt mount
		 */
		if( ( err = ntfs_mountfs(rootvp, mp, &args, p)) != 0) {
			/* fs specific cleanup (if any)*/
			goto error_1;
		}

		goto dostatfs;		/* success*/

	}

	/*
	 ***
	 * Mounting non-root file system or updating a file system
	 ***
	 */

	/* copy in user arguments*/
	err = copyin(data, (caddr_t)&args, sizeof (struct ntfs_args));
	if (err)
		goto error_1;		/* can't get arguments*/

	/*
	 * If updating, check whether changing from read-only to
	 * read/write; if there is no device name, that's all we do.
	 */
	if (mp->mnt_flag & MNT_UPDATE) {
		printf("ntfs_mount(): MNT_UPDATE not supported\n");
		err = EINVAL;
		goto error_1;

#if 0
		ump = VFSTOUFS(mp);
		fs = ump->um_fs;
		err = 0;
		if (fs->fs_ronly == 0 && (mp->mnt_flag & MNT_RDONLY)) {
			flags = WRITECLOSE;
			if (mp->mnt_flag & MNT_FORCE)
				flags |= FORCECLOSE;
			if (vfs_busy(mp)) {
				err = EBUSY;
				goto error_1;
			}
			err = ffs_flushfiles(mp, flags, p);
			vfs_unbusy(mp);
		}
		if (!err && (mp->mnt_flag & MNT_RELOAD))
			err = ffs_reload(mp, ndp->ni_cnd.cn_cred, p);
		if (err) {
			goto error_1;
		}
		if (fs->fs_ronly && (mp->mnt_flag & MNT_WANTRDWR)) {
			if (!fs->fs_clean) {
				if (mp->mnt_flag & MNT_FORCE) {
					printf("WARNING: %s was not properly dismounted.\n",fs->fs_fsmnt);
				} else {
					printf("WARNING: R/W mount of %s denied. Filesystem is not clean - run fsck.\n",
					    fs->fs_fsmnt);
					err = EPERM;
					goto error_1;
				}
			}
			fs->fs_ronly = 0;
		}
		if (fs->fs_ronly == 0) {
			fs->fs_clean = 0;
			ffs_sbupdate(ump, MNT_WAIT);
		}
		/* if not updating name...*/
		if (args.fspec == 0) {
			/*
			 * Process export requests.  Jumping to "success"
			 * will return the vfs_export() error code.
			 */
			err = vfs_export(mp, &ump->um_export, &args.export);
			goto success;
		}
#endif
	}

	/*
	 * Not an update, or updating the name: look up the name
	 * and verify that it refers to a sensible block device.
	 */
	NDINIT(ndp, LOOKUP, FOLLOW, UIO_USERSPACE, args.fspec, p);
	err = namei(ndp);
	if (err) {
		/* can't get devvp!*/
		goto error_1;
	}

	devvp = ndp->ni_vp;

	if (devvp->v_type != VBLK) {
		err = ENOTBLK;
		goto error_2;
	}
	if (major(devvp->v_rdev) >= nblkdev) {
		err = ENXIO;
		goto error_2;
	}
	if (mp->mnt_flag & MNT_UPDATE) {
#if 0
		/*
		 ********************
		 * UPDATE
		 ********************
		 */

		if (devvp != ntmp->um_devvp)
			err = EINVAL;	/* needs translation */
		else
			vrele(devvp);
		/*
		 * Update device name only on success
		 */
		if( !err) {
			/* Save "mounted from" info for mount point (NULL pad)*/
			copyinstr(	args.fspec,
					mp->mnt_stat.f_mntfromname,
					MNAMELEN - 1,
					&size);
			bzero( mp->mnt_stat.f_mntfromname + size, MNAMELEN - size);
		}
#endif
	} else {
		/*
		 ********************
		 * NEW MOUNT
		 ********************
		 */

		/*
		 * Since this is a new mount, we want the names for
		 * the device and the mount point copied in.  If an
		 * error occurs,  the mountpoint is discarded by the
		 * upper level code.
		 */
		/* Save "last mounted on" info for mount point (NULL pad)*/
		copyinstr(	path,				/* mount point*/
				mp->mnt_stat.f_mntonname,	/* save area*/
				MNAMELEN - 1,			/* max size*/
				&size);				/* real size*/
		bzero( mp->mnt_stat.f_mntonname + size, MNAMELEN - size);

		/* Save "mounted from" info for mount point (NULL pad)*/
		copyinstr(	args.fspec,			/* device name*/
				mp->mnt_stat.f_mntfromname,	/* save area*/
				MNAMELEN - 1,			/* max size*/
				&size);				/* real size*/
		bzero( mp->mnt_stat.f_mntfromname + size, MNAMELEN - size);

		err = ntfs_mountfs(devvp, mp, &args, p);
	}
	if (err) {
		goto error_2;
	}

dostatfs:
	/*
	 * Initialize FS stat information in mount struct; uses both
	 * mp->mnt_stat.f_mntonname and mp->mnt_stat.f_mntfromname
	 *
	 * This code is common to root and non-root mounts
	 */
	(void)VFS_STATFS(mp, &mp->mnt_stat, p);

	goto success;


error_2:	/* error with devvp held*/

	/* release devvp before failing*/
	vrele(devvp);

error_1:	/* no state to back out*/

success:
	return( err);
}

/*
 * Common code for mount and mountroot
 */
int
ntfs_mountfs(devvp, mp, argsp, p)
	register struct vnode *devvp;
	struct mount *mp;
	struct ntfs_args *argsp;
	struct proc *p;
{
	struct buf *bp;
	struct ntfsmount *ntmp;
	dev_t dev = devvp->v_rdev;
	int error, ronly, ncount, i;
	struct vnode *vp;

	/*
	 * Disallow multiple mounts of the same device.
	 * Disallow mounting of a device that is currently in use
	 * (except for root, which might share swap device for miniroot).
	 * Flush out any old buffers remaining from a previous use.
	 */
	error = vfs_mountedon(devvp);
	if (error)
		return (error);
	ncount = vcount(devvp);
	if (devvp->v_object)
		ncount -= 1;
	if (ncount > 1 && devvp != rootvp)
		return (EBUSY);
#if __FreeBSD_version >= 300000
	vn_lock(devvp, LK_EXCLUSIVE | LK_RETRY, p);
	error = vinvalbuf(devvp, V_SAVE, p->p_ucred, p, 0, 0);
	VOP_UNLOCK(devvp, 0, p);
#else
	error = vinvalbuf(devvp, V_SAVE, p->p_ucred, p, 0, 0);
#endif
	if (error)
		return (error);

	ronly = (mp->mnt_flag & MNT_RDONLY) != 0;
	error = VOP_OPEN(devvp, ronly ? FREAD : FREAD|FWRITE, FSCRED, p);
	if (error)
		return (error);

	bp = NULL;

	error = bread(devvp, BBLOCK, BBSIZE, NOCRED, &bp);
	if (error)
		goto out;
	ntmp = malloc( sizeof *ntmp, M_NTFSMNT, M_WAITOK );
	bzero( ntmp, sizeof *ntmp );
	bcopy( bp->b_data, &ntmp->ntm_bootfile, sizeof(struct bootfile) );
	brelse( bp );
	bp = NULL;

	{
		int8_t cpr = ntmp->ntm_mftrecsz;
		if( cpr > 0 )
			ntmp->ntm_bpmftrec = ntmp->ntm_spc * cpr;
		else
			ntmp->ntm_bpmftrec = (1 << (-cpr)) / ntmp->ntm_bps;
	}
	printf("ntfs_mountfs(): bps: %d, spc: %d, media: %x, mftrecsz: %d (%d sects)\n",
		ntmp->ntm_bps,ntmp->ntm_spc,ntmp->ntm_bootfile.bf_media,
		ntmp->ntm_mftrecsz,ntmp->ntm_bpmftrec);
	printf("ntfs_mountfs(): mftcn: 0x%x|0x%x\n",
		(u_int32_t)ntmp->ntm_mftcn,(u_int32_t)ntmp->ntm_mftmirrcn);

	ntmp->ntm_mountp = mp;
	ntmp->ntm_dev = dev;
	ntmp->ntm_devvp = devvp;
	ntmp->ntm_uid = argsp->uid;
	ntmp->ntm_gid = argsp->gid;
	ntmp->ntm_mode = argsp->mode;
	ntmp->ntm_flag = argsp->flag;
	mp->mnt_data = (qaddr_t)ntmp;

	printf("ntfs_mountfs(): case-%s,%s uid: %d, gid: %d, mode: %o\n",
		(ntmp->ntm_flag & NTFS_MFLAG_CASEINS)?"insens.":"sens.",
		(ntmp->ntm_flag & NTFS_MFLAG_ALLNAMES)?" allnames,":"",
		ntmp->ntm_uid, ntmp->ntm_gid, ntmp->ntm_mode);

	printf("ntfs_mountfs(): reading system nodes...\n");
	{
		i = NTFS_MFTINO;
		error = VFS_VGET(mp, i, &(ntmp->ntm_sysvn[i]));
		if(error)
			goto out1;
		VREF(ntmp->ntm_sysvn[i]);
		vput(ntmp->ntm_sysvn[i]);
		i = NTFS_ROOTINO;
		error = VFS_VGET(mp, i, &(ntmp->ntm_sysvn[i]));
		if(error)
			goto out1;
		VREF(ntmp->ntm_sysvn[i]);
		vput(ntmp->ntm_sysvn[i]);
	}

	MALLOC( ntmp->ntm_upcase, wchar *, 65536 * sizeof(wchar),
		M_NTFSMNT, M_WAITOK);

	printf("ntfs_mountfs(): opening $UpCase\n");
	error = VFS_VGET(mp, NTFS_UPCASEINO, &vp );
	if(error) 
		goto out1;
	printf("ntfs_mountfs(): reading $UpCase\n");
	error = ntfs_readattr( ntmp, VTONT(vp), NTFS_A_DATA, NULL,
			0, 65536*sizeof(wchar), ntmp->ntm_upcase);
	printf("ntfs_mountfs(): closing $UpCase\n");
	vput(vp);
	if(error) 
		goto out1;

	{
		int num,j;
		struct attrdef ad;

		printf("ntfs_mountfs(): opening $AttrDef\n");
		error = VFS_VGET(mp, NTFS_ATTRDEFINO, &vp );
		if(error) 
			goto out1;

		for(num=0;;num++) {
			error = ntfs_readattr(ntmp, VTONT(vp),
					NTFS_A_DATA, NULL,
					num * sizeof(ad), sizeof(ad),
					&ad);
			if (error)
				goto out1;
			if (ad.ad_name[0] == 0)
				break;
		}
		printf("ntfs_mountfs(): reading %d attrdefs\n",num);

		MALLOC(ntmp->ntm_ad, struct ntvattrdef *,
			num * sizeof(struct ntvattrdef),
			M_NTFSMNT, M_WAITOK);

		ntmp->ntm_adnum = num;

		for(i=0;i<num;i++){
			error = ntfs_readattr(ntmp, VTONT(vp),
					NTFS_A_DATA, NULL,
					i * sizeof(ad), sizeof(ad),
					&ad);
			if (error)
				goto out1;
			j = 0;
			do {
				ntmp->ntm_ad[i].ad_name[j] = ad.ad_name[j];
			} while(ad.ad_name[j++]);
			ntmp->ntm_ad[i].ad_namelen = j - 1;
			ntmp->ntm_ad[i].ad_type = ad.ad_type;
			printf("ntfs_mountfs(): attribute: %s, type: 0x%x\n",
				ntmp->ntm_ad[i].ad_name,
				ntmp->ntm_ad[i].ad_type);
		}
		printf("ntfs_mountfs(): closing $AttrDef\n");
		vput(vp);
	}

	mp->mnt_stat.f_fsid.val[0] = (long)dev;
#if __FreeBSD_version >= 300000
	mp->mnt_stat.f_fsid.val[1] = mp->mnt_vfc->vfc_typenum;
#else
	mp->mnt_stat.f_fsid.val[1] = MOUNT_NTFS;
#endif
	mp->mnt_maxsymlinklen = 0;
	mp->mnt_flag |= MNT_LOCAL;
#if __FreeBSD_version >= 300000
	devvp->v_specmountpoint = mp;
#else
	devvp->v_specflags |= SI_MOUNTEDON;
#endif
	return (0);
out1:
	for(i=0;i<NTFS_SYSNODESNUM;i++)
		if(ntmp->ntm_sysvn[i]) vrele(ntmp->ntm_sysvn[i]);
out:
#if __FreeBSD_version >= 300000
	devvp->v_specmountpoint = NULL;
#else
	devvp->v_specflags |= SI_MOUNTEDON;
#endif
	if (bp)
		brelse(bp);
	(void)VOP_CLOSE(devvp, ronly ? FREAD : FREAD|FWRITE, NOCRED, p);
	return (error);
}

static int
ntfs_start (
	struct mount *mp,
	int flags,
	struct proc *p )
{
	printf("\nntfs_start():\n");
	return (0);
}

static int
ntfs_unmount( 
	struct mount *mp,
	int mntflags,
	struct proc *p)
{
	register struct ntfsmount *ntmp;
	int error, ronly = 0, flags, i;

	printf("ntfs_unmount: unmounting...\n");
	ntmp = VFSTONTFS(mp);

	flags = 0;
	if(mntflags & MNT_FORCE)
		flags |= FORCECLOSE;

	printf("ntfs_unmount: vflushing...\n");
	error = vflush(mp,NULLVP,flags | SKIPSYSTEM);
	if (error) {
		printf("ntfs_unmount: vflush failed: %d\n",error);
		return (error);
	}
	for(i=0;i<NTFS_SYSNODESNUM;i++)
		 if(ntmp->ntm_sysvn[i]) vrele(ntmp->ntm_sysvn[i]);
	error = vflush(mp,NULLVP,flags);
	if (error)
		printf("ntfs_unmount: vflush failed: %d\n",error);

#if __FreeBSD_version >= 300000
	ntmp->ntm_devvp->v_specmountpoint = NULL;
#else
	ntmp->ntm_devvp->v_specflags &= ~SI_MOUNTEDON;

	VOP_LOCK(ntmp->ntm_devvp);
	vnode_pager_uncache(ntmp->ntm_devvp);
	VOP_UNLOCK(ntmp->ntm_devvp);
#endif

	vinvalbuf(ntmp->ntm_devvp, V_SAVE, NOCRED, p, 0, 0);
	error = VOP_CLOSE(ntmp->ntm_devvp, ronly ? FREAD : FREAD|FWRITE,
		NOCRED, p);

	vrele(ntmp->ntm_devvp);

	printf("ntfs_umount: freeing memory...\n");
	mp->mnt_data = (qaddr_t)0;
	mp->mnt_flag &= ~MNT_LOCAL;
	FREE(ntmp->ntm_ad, M_NTFSMNT);
	FREE(ntmp->ntm_upcase, M_NTFSMNT);
	FREE(ntmp, M_NTFSMNT);
	return (error);
}

static int
ntfs_root(
	struct mount *mp,
	struct vnode **vpp )
{
	struct vnode *nvp;
	int error = 0;

	dprintf(("ntfs_root(): sysvn: %p\n",
		VFSTONTFS(mp)->ntm_sysvn[NTFS_ROOTINO]));
	error = VFS_VGET(mp, (ino_t)NTFS_ROOTINO, &nvp);
	if(error) {
		printf("ntfs_root: VFS_VGET failed: %d\n",error);
		return (error);
	}

	*vpp = nvp;
	return (0);
}

static int
ntfs_quotactl ( 
	struct mount *mp,
	int cmds,
	uid_t uid,
	caddr_t arg,
	struct proc *p)
{
	printf("\nntfs_quotactl():\n");
	return EOPNOTSUPP;
}

static int
ntfs_statfs(
	struct mount *mp,
	struct statfs *sbp,
	struct proc *p)
{
	struct ntfsmount *ntmp = VFSTONTFS(mp);
	u_int64_t mftsize,mftallocated,bmsize,bmallocated;
	struct vnode *vp;
	int error,j,i;
	u_int8_t *tmp;

	dprintf(("ntfs_statfs():"));

	ntfs_filesize(ntmp, VTOF(ntmp->ntm_sysvn[NTFS_MFTINO]),
		      &mftsize, &mftallocated);

	error = VFS_VGET(mp, NTFS_BITMAPINO, &vp);
	if(error)
		return (error);

	ntfs_filesize(ntmp, VTOF(vp), &bmsize, &bmallocated);

	MALLOC(tmp, u_int8_t *, bmsize,M_TEMP, M_WAITOK);

	error = ntfs_readattr(ntmp, VTONT(vp), NTFS_A_DATA, NULL,
			       0, bmsize, tmp);
	if(error) {
		FREE(tmp, M_TEMP);
		vput(vp);
		return (error);
	}
	vput(vp);

	sbp->f_bfree = 0;
	for(i=0;i<bmsize;i++)
		for(j=0;j<8;j++)
			if(~tmp[i] & (1 << j)) sbp->f_bfree++;

	FREE(tmp, M_TEMP);

#if __FreeBSD_version >= 300000
	sbp->f_type = mp->mnt_vfc->vfc_typenum;
#else
	sbp->f_type = MOUNT_NTFS;
#endif
	sbp->f_bsize = ntmp->ntm_bps;
	sbp->f_iosize = ntmp->ntm_bps * ntmp->ntm_spc;
	sbp->f_blocks = ntmp->ntm_bootfile.bf_spv;
	sbp->f_bfree = sbp->f_bavail = ntfs_cntobn(sbp->f_bfree);
	sbp->f_ffree = sbp->f_bfree / ntmp->ntm_bpmftrec;
	sbp->f_files = mftallocated / ntfs_bntob(ntmp->ntm_bpmftrec) +
		       sbp->f_ffree;
	if (sbp != &mp->mnt_stat) {
		bcopy((caddr_t)mp->mnt_stat.f_mntonname,
			(caddr_t)&sbp->f_mntonname[0], MNAMELEN);
		bcopy((caddr_t)mp->mnt_stat.f_mntfromname,
			(caddr_t)&sbp->f_mntfromname[0], MNAMELEN);
	}
	sbp->f_flags = mp->mnt_flag;
	
	return (0);
}

static int
ntfs_sync (
	struct mount *mp,
	int waitfor,
	struct ucred *cred,
	struct proc *p)
{
	/*dprintf(("ntfs_sync():\n"));*/
	return (0);
}

#if __FreeBSD_version >= 300000
static int
ntfs_fhtovp(
	struct mount *mp,
	struct fid *fhp,
	struct sockaddr *nam,
	struct vnode **vpp,
	int *exflagsp,
	struct ucred **credanonp)
#else
static int
ntfs_fhtovp(
	struct mount *mp,
	struct fid *fhp,
	struct mbuf *nam,
	struct vnode **vpp,
	int *exflagsp,
	struct ucred **credanonp)
#endif
{
	printf("\ntfs_fhtovp():\n");
	return 0;
}

static int
ntfs_vptofh(
	struct vnode *vp,
	struct fid *fhp)
{
	printf("ntfs_vptofh():\n");
	return EOPNOTSUPP;
}

int
ntfs_vgetex(
	struct mount *mp,
	ino_t ino,
	u_int32_t attrtype,
	char *attrname,
	u_long lkflags,
	u_long flags,
	struct proc *p,
	struct vnode **vpp) 
{
	int error;
	register struct ntfsmount *ntmp;
	struct ntnode *ip;
	struct fnode *fp;
	struct vnode *vp;

	dprintf(("ntfs_vgetex: ino: %d, attr: 0x%x:%s, lkf: 0x%x, f: 0x%x\n",
		ino, attrtype, attrname?attrname:"", lkflags, flags ));

	ntmp = VFSTONTFS(mp);
	*vpp = NULL;

	/* Get ntnode */
	error = ntfs_ntget(ntmp, ino, &ip);
	if (error) {
		printf("ntfs_vget: ntfs_ntget failed\n");
		return (error);
	}

	error = ntfs_fget(ntmp, ip, attrtype, attrname, &fp);
	if (error) {
		printf("ntfs_vget: ntfs_fget failed\n");
		ntfs_ntrele(ip);
		return (error);
	}

	if (FTOV(fp)) {
		vget(FTOV(fp), lkflags, p);
		*vpp = FTOV(fp);
		ntfs_ntrele(ip);
		return (0);
	}

	/* It may be not initialized fully, so force load it */
	if (!(flags & VG_DONTLOAD) && !(ip->i_flag & IN_LOADED)) {
		error = ntfs_loadntnode(ntmp, ip);
		if(error) {
			printf("ntfs_vget: CAN'T LOAD ATTRIBUTES FOR INO: %d\n",
			       ip->i_number);
			ntfs_ntrele(ip);
			return (error);
		}
	}

	error = getnewvnode(VT_NTFS, ntmp->ntm_mountp, ntfs_vnodeop_p, &vp);
	if(error) {
		ntfs_frele(fp);
		ntfs_ntrele(ip);
		return (error);
	}
	dprintf(("ntfs_vget: vnode: %p for ntnode: %d\n", vp,ino));

	lockinit(&fp->f_lock, PINOD, "fnode", 0, 0);
	fp->f_vp = vp;
	vp->v_data = fp;

	if (ip->i_frflag & NTFS_FRFLAG_DIR)
		vp->v_type = fp->f_type = VDIR;
	else
		vp->v_type = fp->f_type = VREG;	

	if (ino == NTFS_ROOTINO)
		vp->v_flag |= VROOT;
	if (ino < NTFS_SYSNODESNUM)
		vp->v_flag |= VSYSTEM;

	ntfs_ntrele(ip);

	if (lkflags & LK_TYPE_MASK) {
		error = vn_lock(vp, lkflags, p);
		if (error) {
			vput(vp);
			return (error);
		}
	}

	VREF(fp->f_devvp);
	*vpp = vp;
	return (0);
	
}

static int
ntfs_vget(
	struct mount *mp,
	ino_t ino,
	struct vnode **vpp) 
{
	return ntfs_vgetex(mp, ino, NTFS_A_DATA, NULL,
			   LK_EXCLUSIVE, 0, curproc, vpp);
}

#if __FreeBSD_version >= 300000
static struct vfsops ntfs_vfsops = {
	ntfs_mount,
	ntfs_start,
	ntfs_unmount,
	ntfs_root,
	ntfs_quotactl,
	ntfs_statfs,
	ntfs_sync,
	ntfs_vget,
	ntfs_fhtovp,
	ntfs_vptofh,
	ntfs_init,
	NULL,
	NULL
};
VFS_SET(ntfs_vfsops, ntfs, 0);
#else
static struct vfsops ntfs_vfsops = {
	ntfs_mount,
	ntfs_start,
	ntfs_unmount,
	ntfs_root,
	ntfs_quotactl,
	ntfs_statfs,
	ntfs_sync,
	ntfs_vget,
	ntfs_fhtovp,
	ntfs_vptofh,
	ntfs_init,
};

VFS_SET(ntfs_vfsops, ntfs, MOUNT_NTFS, 0);
#endif


