/*	$NetBSD: linux_ipc.c,v 1.19 1999/01/03 05:18:01 erh Exp $	*/

/*-
 * Copyright (c) 1995, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Frank van der Linden and Eric Haszlakiewicz.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "opt_sysv.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <sys/mount.h>
#include <sys/syscallargs.h>

#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_util.h>

#include <compat/linux/linux_syscallargs.h>
#include <compat/linux/linux_syscall.h>

#include <compat/linux/common/linux_ipc.h>
#include <compat/linux/common/linux_msg.h>
#include <compat/linux/common/linux_shm.h>
#include <compat/linux/common/linux_sem.h>
#include <compat/linux/common/linux_ipccall.h>

/*
 * Note: Not all linux architechtures have explicit versions
 *	of the SYSV* syscalls.  On the ones that don't
 *	we pretend that they are defined anyway.  *_args and
 *	prototypes are defined in individual headers;
 *	syscalls.master lists those syscalls as NOARGS.
 *
 *	The functions in multiarch are the ones that just need
 *	the arguments shuffled around and then use the
 *	normal NetBSD syscall.
 *
 * Function in multiarch:
 *	linux_sys_ipc		: linux_ipccall.c
 *	liunx_semop		: linux_ipccall.c
 *	linux_semget		: linux_ipccall.c
 *	linux_msgsnd		: linux_ipccall.c
 *	linux_msgrcv		: linux_ipccall.c
 *	linux_msgget		: linux_ipccall.c
 *	linux_shmdt		: linux_ipccall.c
 *	linux_shmget		: linux_ipccall.c
 */

#if defined (SYSVSEM) || defined(SYSVSHM) || defined(SYSVMSG)
/*
 * Convert between Linux and NetBSD ipc_perm structures. Only the
 * order of the fields is different.
 */
void
linux_to_bsd_ipc_perm(lpp, bpp)
	struct linux_ipc_perm *lpp;
	struct ipc_perm *bpp;
{

	bpp->key = lpp->l_key;
	bpp->uid = lpp->l_uid;
	bpp->gid = lpp->l_gid;
	bpp->cuid = lpp->l_cuid;
	bpp->cgid = lpp->l_cgid;
	bpp->mode = lpp->l_mode;
	bpp->seq = lpp->l_seq;
}

void
bsd_to_linux_ipc_perm(bpp, lpp)
	struct ipc_perm *bpp;
	struct linux_ipc_perm *lpp;
{

	lpp->l_key = bpp->key;
	lpp->l_uid = bpp->uid;
	lpp->l_gid = bpp->gid;
	lpp->l_cuid = bpp->cuid;
	lpp->l_cgid = bpp->cgid;
	lpp->l_mode = bpp->mode;
	lpp->l_seq = bpp->seq;
}
#endif

#ifdef SYSVSEM
/*
 * Semaphore operations. Most constants and structures are the same on
 * both systems. Only semctl() needs some extra work.
 */

/*
 * Convert between Linux and NetBSD semid_ds structures.
 */
void
bsd_to_linux_semid_ds(bs, ls)
	struct semid_ds *bs;
	struct linux_semid_ds *ls;
{

	bsd_to_linux_ipc_perm(&bs->sem_perm, &ls->l_sem_perm);
	ls->l_sem_otime = bs->sem_otime;
	ls->l_sem_ctime = bs->sem_ctime;
	ls->l_sem_nsems = bs->sem_nsems;
	ls->l_sem_base = bs->sem_base;
}

void
linux_to_bsd_semid_ds(ls, bs)
	struct linux_semid_ds *ls;
	struct semid_ds *bs;
{

	linux_to_bsd_ipc_perm(&ls->l_sem_perm, &bs->sem_perm);
	bs->sem_otime = ls->l_sem_otime;
	bs->sem_ctime = ls->l_sem_ctime;
	bs->sem_nsems = ls->l_sem_nsems;
	bs->sem_base = ls->l_sem_base;
}

/*
 * Most of this can be handled by directly passing the arguments on,
 * but IPC_* require a lot of copy{in,out} because of the extra indirection
 * (we need to pass a pointer to a union cointaining a pointer to a semid_ds
 * structure.  Linux actually handles this better than we do.)
 */
int
linux_sys_semctl(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct linux_sys_semctl_args /* {
		syscallarg(int) semid;
		syscallarg(int) semnum;
		syscallarg(int) cmd;
		syscallarg(union linux_semun) arg;
	} */ *uap = v;
	caddr_t sg;
	struct sys___semctl_args nua;
	struct semid_ds *bmp, bm;
	struct linux_semid_ds lm;
	union semun *bup;
	int error;

	SCARG(&nua, semid) = SCARG(uap, semid);
	SCARG(&nua, semnum) = SCARG(uap, semnum);
	switch (SCARG(uap, cmd)) {
	case LINUX_IPC_STAT:
		sg = stackgap_init(p->p_emul);
		bup = stackgap_alloc(&sg, sizeof (union semun));
		bmp = stackgap_alloc(&sg, sizeof (struct semid_ds));
		if ((error = copyout(&bmp, bup, sizeof bmp)))
			return error;
		SCARG(&nua, cmd) = IPC_STAT;
		SCARG(&nua, arg) = bup;
		if ((error = sys___semctl(p, &nua, retval)))
			return error;
		if ((error = copyin(bmp, &bm, sizeof bm)))
			return error;
		bsd_to_linux_semid_ds(&bm, &lm);
		return copyout(&lm, SCARG(uap, arg).l_buf, sizeof lm);
	case LINUX_IPC_SET:
		if ((error = copyin(SCARG(uap, arg).l_buf, &lm, sizeof lm)))
			return error;
		linux_to_bsd_semid_ds(&lm, &bm);
		sg = stackgap_init(p->p_emul);
		bup = stackgap_alloc(&sg, sizeof (union semun));
		bmp = stackgap_alloc(&sg, sizeof (struct semid_ds));
		if ((error = copyout(&bm, bmp, sizeof bm)))
			return error;
		if ((error = copyout(&bmp, bup, sizeof bmp)))
			return error;
		SCARG(&nua, cmd) = IPC_SET;
		SCARG(&nua, arg) = bup;
		break;
	case LINUX_IPC_RMID:
		SCARG(&nua, cmd) = IPC_RMID;
		break;
	case LINUX_GETVAL:
		SCARG(&nua, cmd) = GETVAL;
		break;
	case LINUX_GETPID:
		SCARG(&nua, cmd) = GETPID;
		break;
	case LINUX_GETNCNT:
		SCARG(&nua, cmd) = GETNCNT;
		break;
	case LINUX_GETZCNT:
		SCARG(&nua, cmd) = GETZCNT;
		break;
	case LINUX_SETVAL:
		SCARG(&nua, cmd) = SETVAL;
		break;
	default:
		return EINVAL;
	}
	return sys___semctl(p, &nua, retval);
}
#endif /* SYSVSEM */

#ifdef SYSVMSG

void
linux_to_bsd_msqid_ds(lmp, bmp)
	struct linux_msqid_ds *lmp;
	struct msqid_ds *bmp;
{

	linux_to_bsd_ipc_perm(&lmp->l_msg_perm, &bmp->msg_perm);
	bmp->msg_first = lmp->l_msg_first;
	bmp->msg_last = lmp->l_msg_last;
	bmp->msg_cbytes = lmp->l_msg_cbytes;
	bmp->msg_qnum = lmp->l_msg_qnum;
	bmp->msg_qbytes = lmp->l_msg_qbytes;
	bmp->msg_lspid = lmp->l_msg_lspid;
	bmp->msg_lrpid = lmp->l_msg_lrpid;
	bmp->msg_stime = lmp->l_msg_stime;
	bmp->msg_rtime = lmp->l_msg_rtime;
	bmp->msg_ctime = lmp->l_msg_ctime;
}

void
bsd_to_linux_msqid_ds(bmp, lmp)
	struct msqid_ds *bmp;
	struct linux_msqid_ds *lmp;
{

	bsd_to_linux_ipc_perm(&bmp->msg_perm, &lmp->l_msg_perm);
	lmp->l_msg_first = bmp->msg_first;
	lmp->l_msg_last = bmp->msg_last;
	lmp->l_msg_cbytes = bmp->msg_cbytes;
	lmp->l_msg_qnum = bmp->msg_qnum;
	lmp->l_msg_qbytes = bmp->msg_qbytes;
	lmp->l_msg_lspid = bmp->msg_lspid;
	lmp->l_msg_lrpid = bmp->msg_lrpid;
	lmp->l_msg_stime = bmp->msg_stime;
	lmp->l_msg_rtime = bmp->msg_rtime;
	lmp->l_msg_ctime = bmp->msg_ctime;
}

int
linux_sys_msgctl(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct linux_sys_msgctl_args /* {
		syscallarg(int) msqid;
		syscallarg(int) cmd;
		syscallarg(struct linux_msqid_ds *) buf;
	} */ *uap = v;
	caddr_t sg;
	struct sys_msgctl_args nua;
	struct msqid_ds *bmp, bm;
	struct linux_msqid_ds lm;
	int error;

	SCARG(&nua, msqid) = SCARG(uap, msqid);
	switch (SCARG(uap, cmd)) {
	case LINUX_IPC_STAT:
		sg = stackgap_init(p->p_emul);
		bmp = stackgap_alloc(&sg, sizeof (struct msqid_ds));
		SCARG(&nua, cmd) = IPC_STAT;
		SCARG(&nua, buf) = bmp;
		if ((error = sys_msgctl(p, &nua, retval)))
			return error;
		if ((error = copyin(bmp, &bm, sizeof bm)))
			return error;
		bsd_to_linux_msqid_ds(&bm, &lm);
		return copyout(&lm, SCARG(uap, buf), sizeof lm);
	case LINUX_IPC_SET:
		if ((error = copyin(SCARG(uap, buf), &lm, sizeof lm)))
			return error;
		linux_to_bsd_msqid_ds(&lm, &bm);
		sg = stackgap_init(p->p_emul);
		bmp = stackgap_alloc(&sg, sizeof bm);
		if ((error = copyout(&bm, bmp, sizeof bm)))
			return error;
		SCARG(&nua, cmd) = IPC_SET;
		SCARG(&nua, buf) = bmp;
		break;
	case LINUX_IPC_RMID:
		SCARG(&nua, cmd) = IPC_RMID;
		SCARG(&nua, buf) = NULL;
		break;
	default:
		return EINVAL;
	}
	return sys_msgctl(p, &nua, retval);
}
#endif /* SYSVMSG */

#ifdef SYSVSHM
/*
 * shmat(2). Very straightforward, except that Linux passes a pointer
 * in which the return value is to be passed. This is subsequently
 * handled by libc, apparently.
 */
int
linux_sys_shmat(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct linux_sys_shmat_args /* {
		syscallarg(int) shmid;
		syscallarg(void *) shmaddr;
		syscallarg(int) shmflg;
		syscallarg(u_long *) raddr;
	} */ *uap = v;
	int error;

	if ((error = sys_shmat(p, uap, retval)))
		return error;

	if ((error = copyout(&retval[0], (caddr_t) SCARG(uap, raddr),
	     sizeof retval[0])))
		return error;
	
	retval[0] = 0;
	return 0;
}

/*
 * Convert between Linux and NetBSD shmid_ds structures.
 * The order of the fields is once again the difference, and
 * we also need a place to store the internal data pointer
 * in, which is unfortunately stored in this structure.
 *
 * We abuse a Linux internal field for that.
 */
void
linux_to_bsd_shmid_ds(lsp, bsp)
	struct linux_shmid_ds *lsp;
	struct shmid_ds *bsp;
{

	linux_to_bsd_ipc_perm(&lsp->l_shm_perm, &bsp->shm_perm);
	bsp->shm_segsz = lsp->l_shm_segsz;
	bsp->shm_lpid = lsp->l_shm_lpid;
	bsp->shm_cpid = lsp->l_shm_cpid;
	bsp->shm_nattch = lsp->l_shm_nattch;
	bsp->shm_atime = lsp->l_shm_atime;
	bsp->shm_dtime = lsp->l_shm_dtime;
	bsp->shm_ctime = lsp->l_shm_ctime;
	bsp->shm_internal = lsp->l_private2;	/* XXX Oh well. */
}

void
bsd_to_linux_shmid_ds(bsp, lsp)
	struct shmid_ds *bsp;
	struct linux_shmid_ds *lsp;
{

	bsd_to_linux_ipc_perm(&bsp->shm_perm, &lsp->l_shm_perm);
	lsp->l_shm_segsz = bsp->shm_segsz;
	lsp->l_shm_lpid = bsp->shm_lpid;
	lsp->l_shm_cpid = bsp->shm_cpid;
	lsp->l_shm_nattch = bsp->shm_nattch;
	lsp->l_shm_atime = bsp->shm_atime;
	lsp->l_shm_dtime = bsp->shm_dtime;
	lsp->l_shm_ctime = bsp->shm_ctime;
	lsp->l_private2 = bsp->shm_internal;	/* XXX */
}

/*
 * shmctl. Not implemented (for now): IPC_INFO, SHM_INFO, SHM_STAT
 * SHM_LOCK and SHM_UNLOCK are passed on, but currently not implemented
 * by NetBSD itself.
 *
 * The usual structure conversion and massaging is done.
 */
int
linux_sys_shmctl(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct linux_sys_shmctl_args /* {
		syscallarg(int) shmid;
		syscallarg(int) cmd;
		syscallarg(struct linux_shmid_ds *) buf;
	} */ *uap = v;
	caddr_t sg;
	struct sys_shmctl_args nua;
	struct shmid_ds *bsp, bs;
	struct linux_shmid_ds ls;
	int error;

	SCARG(&nua, shmid) = SCARG(uap, shmid);
	switch (SCARG(uap, cmd)) {
	case LINUX_IPC_STAT:
		sg = stackgap_init(p->p_emul);
		bsp = stackgap_alloc(&sg, sizeof(struct shmid_ds));
		SCARG(&nua, cmd) = IPC_STAT;
		SCARG(&nua, buf) = bsp;
		if ((error = sys_shmctl(p, &nua, retval)))
			return error;
		if ((error = copyin(SCARG(&nua, buf), &bs, sizeof bs)))
			return error;
		bsd_to_linux_shmid_ds(&bs, &ls);
		return copyout(&ls, SCARG(uap, buf), sizeof ls);
	case LINUX_IPC_SET:
		if ((error = copyin(SCARG(uap, buf), &ls, sizeof ls)))
			return error;
		linux_to_bsd_shmid_ds(&ls, &bs);
		sg = stackgap_init(p->p_emul);
		bsp = stackgap_alloc(&sg, sizeof bs);
		if ((error = copyout(&bs, bsp, sizeof bs)))
			return error;
		SCARG(&nua, cmd) = IPC_SET;
		SCARG(&nua, buf) = bsp;
		break;
	case LINUX_IPC_RMID:
		SCARG(&nua, cmd) = IPC_RMID;
		SCARG(&nua, buf) = NULL;
		break;
	case LINUX_SHM_LOCK:
		SCARG(&nua, cmd) = SHM_LOCK;
		SCARG(&nua, buf) = NULL;
		break;
	case LINUX_SHM_UNLOCK:
		SCARG(&nua, cmd) = SHM_UNLOCK;
		SCARG(&nua, buf) = NULL;
		break;
	case LINUX_IPC_INFO:
	case LINUX_SHM_STAT:
	case LINUX_SHM_INFO:
	default:
		return EINVAL;
	}
	return sys_shmctl(p, &nua, retval);
}
#endif /* SYSVSHM */
