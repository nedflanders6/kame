/*	$NetBSD: linux_sysent.c,v 1.8 1999/03/30 02:25:32 abs Exp $	*/

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.32 1999/03/30 02:22:49 abs Exp 
 */

#include "opt_compat_43.h"
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent linux_sysent[] = {
	{ 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args),
	    sys_exit },				/* 1 = exit */
	{ 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct sys_read_args),
	    sys_read },				/* 3 = read */
	{ 3, s(struct sys_write_args),
	    sys_write },			/* 4 = write */
	{ 3, s(struct linux_sys_open_args),
	    linux_sys_open },			/* 5 = open */
	{ 1, s(struct sys_close_args),
	    sys_close },			/* 6 = close */
	{ 3, s(struct linux_sys_waitpid_args),
	    linux_sys_waitpid },		/* 7 = waitpid */
	{ 2, s(struct linux_sys_creat_args),
	    linux_sys_creat },			/* 8 = creat */
	{ 2, s(struct sys_link_args),
	    sys_link },				/* 9 = link */
	{ 1, s(struct linux_sys_unlink_args),
	    linux_sys_unlink },			/* 10 = unlink */
	{ 3, s(struct linux_sys_execve_args),
	    linux_sys_execve },			/* 11 = execve */
	{ 1, s(struct linux_sys_chdir_args),
	    linux_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct linux_sys_time_args),
	    linux_sys_time },			/* 13 = time */
	{ 3, s(struct linux_sys_mknod_args),
	    linux_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct linux_sys_chmod_args),
	    linux_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct linux_sys_lchown_args),
	    linux_sys_lchown },			/* 16 = lchown */
	{ 1, s(struct linux_sys_break_args),
	    linux_sys_break },			/* 17 = break */
	{ 0, 0,
	    sys_nosys },			/* 18 = obsolete ostat */
	{ 3, s(struct compat_43_sys_lseek_args),
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0,
	    sys_getpid },			/* 20 = getpid */
	{ 0, 0,
	    sys_nosys },			/* 21 = unimplemented mount */
	{ 0, 0,
	    sys_nosys },			/* 22 = unimplemented umount */
	{ 1, s(struct sys_setuid_args),
	    sys_setuid },			/* 23 = setuid */
	{ 0, 0,
	    sys_getuid },			/* 24 = getuid */
	{ 0, 0,
	    sys_nosys },			/* 25 = unimplemented stime */
	{ 0, 0,
	    sys_nosys },			/* 26 = unimplemented ptrace */
	{ 1, s(struct linux_sys_alarm_args),
	    linux_sys_alarm },			/* 27 = alarm */
	{ 0, 0,
	    sys_nosys },			/* 28 = obsolete ofstat */
	{ 0, 0,
	    linux_sys_pause },			/* 29 = pause */
	{ 2, s(struct linux_sys_utime_args),
	    linux_sys_utime },			/* 30 = utime */
	{ 0, 0,
	    sys_nosys },			/* 31 = obsolete stty */
	{ 0, 0,
	    sys_nosys },			/* 32 = obsolete gtty */
	{ 2, s(struct linux_sys_access_args),
	    linux_sys_access },			/* 33 = access */
	{ 1, s(struct linux_sys_nice_args),
	    linux_sys_nice },			/* 34 = nice */
	{ 0, 0,
	    sys_nosys },			/* 35 = obsolete ftime */
	{ 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct linux_sys_kill_args),
	    linux_sys_kill },			/* 37 = kill */
	{ 2, s(struct linux_sys_rename_args),
	    linux_sys_rename },			/* 38 = rename */
	{ 2, s(struct linux_sys_mkdir_args),
	    linux_sys_mkdir },			/* 39 = mkdir */
	{ 1, s(struct linux_sys_rmdir_args),
	    linux_sys_rmdir },			/* 40 = rmdir */
	{ 1, s(struct sys_dup_args),
	    sys_dup },				/* 41 = dup */
	{ 1, s(struct linux_sys_pipe_args),
	    linux_sys_pipe },			/* 42 = pipe */
	{ 1, s(struct linux_sys_times_args),
	    linux_sys_times },			/* 43 = times */
	{ 0, 0,
	    sys_nosys },			/* 44 = obsolete prof */
	{ 1, s(struct linux_sys_brk_args),
	    linux_sys_brk },			/* 45 = brk */
	{ 1, s(struct sys_setgid_args),
	    sys_setgid },			/* 46 = setgid */
	{ 0, 0,
	    sys_getgid },			/* 47 = getgid */
	{ 2, s(struct linux_sys_signal_args),
	    linux_sys_signal },			/* 48 = signal */
	{ 0, 0,
	    sys_geteuid },			/* 49 = geteuid */
	{ 0, 0,
	    sys_getegid },			/* 50 = getegid */
	{ 1, s(struct sys_acct_args),
	    sys_acct },				/* 51 = acct */
	{ 0, 0,
	    sys_nosys },			/* 52 = obsolete phys */
	{ 0, 0,
	    sys_nosys },			/* 53 = obsolete lock */
	{ 3, s(struct linux_sys_ioctl_args),
	    linux_sys_ioctl },			/* 54 = ioctl */
	{ 3, s(struct linux_sys_fcntl_args),
	    linux_sys_fcntl },			/* 55 = fcntl */
	{ 0, 0,
	    sys_nosys },			/* 56 = obsolete mpx */
	{ 2, s(struct sys_setpgid_args),
	    sys_setpgid },			/* 57 = setpgid */
	{ 0, 0,
	    sys_nosys },			/* 58 = obsolete ulimit */
	{ 1, s(struct linux_sys_oldolduname_args),
	    linux_sys_oldolduname },		/* 59 = oldolduname */
	{ 1, s(struct sys_umask_args),
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args),
	    sys_chroot },			/* 61 = chroot */
	{ 0, 0,
	    sys_nosys },			/* 62 = unimplemented ustat */
	{ 2, s(struct sys_dup2_args),
	    sys_dup2 },				/* 63 = dup2 */
	{ 0, 0,
	    sys_getppid },			/* 64 = getppid */
	{ 0, 0,
	    sys_getpgrp },			/* 65 = getpgrp */
	{ 0, 0,
	    sys_setsid },			/* 66 = setsid */
	{ 3, s(struct linux_sys_sigaction_args),
	    linux_sys_sigaction },		/* 67 = sigaction */
	{ 0, 0,
	    linux_sys_siggetmask },		/* 68 = siggetmask */
	{ 1, s(struct linux_sys_sigsetmask_args),
	    linux_sys_sigsetmask },		/* 69 = sigsetmask */
	{ 2, s(struct linux_sys_setreuid_args),
	    linux_sys_setreuid },		/* 70 = setreuid */
	{ 2, s(struct linux_sys_setregid_args),
	    linux_sys_setregid },		/* 71 = setregid */
	{ 3, s(struct linux_sys_sigsuspend_args),
	    linux_sys_sigsuspend },		/* 72 = sigsuspend */
	{ 1, s(struct linux_sys_sigpending_args),
	    linux_sys_sigpending },		/* 73 = sigpending */
	{ 2, s(struct compat_43_sys_sethostname_args),
	    compat_43_sys_sethostname },	/* 74 = sethostname */
	{ 2, s(struct compat_43_sys_setrlimit_args),
	    compat_43_sys_setrlimit },		/* 75 = setrlimit */
	{ 2, s(struct compat_43_sys_getrlimit_args),
	    compat_43_sys_getrlimit },		/* 76 = getrlimit */
	{ 2, s(struct sys_getrusage_args),
	    sys_getrusage },			/* 77 = getrusage */
	{ 2, s(struct sys_gettimeofday_args),
	    sys_gettimeofday },			/* 78 = gettimeofday */
	{ 2, s(struct sys_settimeofday_args),
	    sys_settimeofday },			/* 79 = settimeofday */
	{ 2, s(struct sys_getgroups_args),
	    sys_getgroups },			/* 80 = getgroups */
	{ 2, s(struct sys_setgroups_args),
	    sys_setgroups },			/* 81 = setgroups */
	{ 1, s(struct linux_sys_oldselect_args),
	    linux_sys_oldselect },		/* 82 = oldselect */
	{ 2, s(struct linux_sys_symlink_args),
	    linux_sys_symlink },		/* 83 = symlink */
	{ 2, s(struct compat_43_sys_lstat_args),
	    compat_43_sys_lstat },		/* 84 = oolstat */
	{ 3, s(struct linux_sys_readlink_args),
	    linux_sys_readlink },		/* 85 = readlink */
	{ 1, s(struct linux_sys_uselib_args),
	    linux_sys_uselib },			/* 86 = uselib */
	{ 1, s(struct compat_12_sys_swapon_args),
	    compat_12_sys_swapon },		/* 87 = swapon */
	{ 1, s(struct sys_reboot_args),
	    sys_reboot },			/* 88 = reboot */
	{ 3, s(struct linux_sys_readdir_args),
	    linux_sys_readdir },		/* 89 = readdir */
	{ 1, s(struct linux_sys_old_mmap_args),
	    linux_sys_old_mmap },		/* 90 = old_mmap */
	{ 2, s(struct sys_munmap_args),
	    sys_munmap },			/* 91 = munmap */
	{ 2, s(struct linux_sys_truncate_args),
	    linux_sys_truncate },		/* 92 = truncate */
	{ 2, s(struct compat_43_sys_ftruncate_args),
	    compat_43_sys_ftruncate },		/* 93 = ftruncate */
	{ 2, s(struct sys_fchmod_args),
	    sys_fchmod },			/* 94 = fchmod */
	{ 3, s(struct linux_sys_fchown_args),
	    linux_sys_fchown },			/* 95 = fchown */
	{ 2, s(struct sys_getpriority_args),
	    sys_getpriority },			/* 96 = getpriority */
	{ 3, s(struct sys_setpriority_args),
	    sys_setpriority },			/* 97 = setpriority */
	{ 4, s(struct sys_profil_args),
	    sys_profil },			/* 98 = profil */
	{ 2, s(struct linux_sys_statfs_args),
	    linux_sys_statfs },			/* 99 = statfs */
	{ 2, s(struct linux_sys_fstatfs_args),
	    linux_sys_fstatfs },		/* 100 = fstatfs */
	{ 3, s(struct linux_sys_ioperm_args),
	    linux_sys_ioperm },			/* 101 = ioperm */
	{ 2, s(struct linux_sys_socketcall_args),
	    linux_sys_socketcall },		/* 102 = socketcall */
	{ 0, 0,
	    sys_nosys },			/* 103 = unimplemented syslog */
	{ 3, s(struct sys_setitimer_args),
	    sys_setitimer },			/* 104 = setitimer */
	{ 2, s(struct sys_getitimer_args),
	    sys_getitimer },			/* 105 = getitimer */
	{ 2, s(struct linux_sys_stat_args),
	    linux_sys_stat },			/* 106 = stat */
	{ 2, s(struct linux_sys_lstat_args),
	    linux_sys_lstat },			/* 107 = lstat */
	{ 2, s(struct linux_sys_fstat_args),
	    linux_sys_fstat },			/* 108 = fstat */
	{ 1, s(struct linux_sys_olduname_args),
	    linux_sys_olduname },		/* 109 = olduname */
	{ 1, s(struct linux_sys_iopl_args),
	    linux_sys_iopl },			/* 110 = iopl */
	{ 0, 0,
	    sys_nosys },			/* 111 = unimplemented vhangup */
	{ 0, 0,
	    sys_nosys },			/* 112 = unimplemented idle */
	{ 0, 0,
	    sys_nosys },			/* 113 = unimplemented vm86old */
	{ 4, s(struct linux_sys_wait4_args),
	    linux_sys_wait4 },			/* 114 = wait4 */
	{ 0, 0,
	    sys_nosys },			/* 115 = unimplemented swapoff */
	{ 0, 0,
	    sys_nosys },			/* 116 = unimplemented sysinfo */
	{ 5, s(struct linux_sys_ipc_args),
	    linux_sys_ipc },			/* 117 = ipc */
	{ 1, s(struct sys_fsync_args),
	    sys_fsync },			/* 118 = fsync */
	{ 1, s(struct linux_sys_sigreturn_args),
	    linux_sys_sigreturn },		/* 119 = sigreturn */
	{ 0, 0,
	    sys_nosys },			/* 120 = unimplemented clone */
	{ 2, s(struct compat_09_sys_setdomainname_args),
	    compat_09_sys_setdomainname },	/* 121 = setdomainname */
	{ 1, s(struct linux_sys_uname_args),
	    linux_sys_uname },			/* 122 = uname */
	{ 3, s(struct linux_sys_modify_ldt_args),
	    linux_sys_modify_ldt },		/* 123 = modify_ldt */
	{ 0, 0,
	    sys_nosys },			/* 124 = unimplemented adjtimex */
	{ 3, s(struct sys_mprotect_args),
	    sys_mprotect },			/* 125 = mprotect */
	{ 3, s(struct linux_sys_sigprocmask_args),
	    linux_sys_sigprocmask },		/* 126 = sigprocmask */
	{ 0, 0,
	    sys_nosys },			/* 127 = unimplemented create_module */
	{ 0, 0,
	    sys_nosys },			/* 128 = unimplemented init_module */
	{ 0, 0,
	    sys_nosys },			/* 129 = unimplemented delete_module */
	{ 0, 0,
	    sys_nosys },			/* 130 = unimplemented get_kernel_syms */
	{ 0, 0,
	    sys_nosys },			/* 131 = unimplemented quotactl */
	{ 1, s(struct linux_sys_getpgid_args),
	    linux_sys_getpgid },		/* 132 = getpgid */
	{ 1, s(struct sys_fchdir_args),
	    sys_fchdir },			/* 133 = fchdir */
	{ 0, 0,
	    sys_nosys },			/* 134 = unimplemented bdflush */
	{ 0, 0,
	    sys_nosys },			/* 135 = unimplemented sysfs */
	{ 1, s(struct linux_sys_personality_args),
	    linux_sys_personality },		/* 136 = personality */
	{ 0, 0,
	    sys_nosys },			/* 137 = unimplemented afs_syscall */
	{ 0, 0,
	    sys_nosys },			/* 138 = unimplemented setfsuid */
	{ 0, 0,
	    sys_nosys },			/* 139 = unimplemented getfsuid */
	{ 5, s(struct linux_sys_llseek_args),
	    linux_sys_llseek },			/* 140 = llseek */
	{ 3, s(struct linux_sys_getdents_args),
	    linux_sys_getdents },		/* 141 = getdents */
	{ 5, s(struct linux_sys_select_args),
	    linux_sys_select },			/* 142 = select */
	{ 2, s(struct sys_flock_args),
	    sys_flock },			/* 143 = flock */
	{ 3, s(struct linux_sys_msync_args),
	    linux_sys_msync },			/* 144 = msync */
	{ 3, s(struct sys_readv_args),
	    sys_readv },			/* 145 = readv */
	{ 3, s(struct sys_writev_args),
	    sys_writev },			/* 146 = writev */
	{ 1, s(struct sys_getsid_args),
	    sys_getsid },			/* 147 = getsid */
	{ 1, s(struct linux_sys_fdatasync_args),
	    linux_sys_fdatasync },		/* 148 = fdatasync */
	{ 1, s(struct linux_sys___sysctl_args),
	    linux_sys___sysctl },		/* 149 = __sysctl */
	{ 2, s(struct sys_mlock_args),
	    sys_mlock },			/* 150 = mlock */
	{ 2, s(struct sys_munlock_args),
	    sys_munlock },			/* 151 = munlock */
	{ 0, 0,
	    sys_nosys },			/* 152 = unimplemented mlockall */
	{ 0, 0,
	    sys_nosys },			/* 153 = unimplemented munlockall */
	{ 0, 0,
	    sys_nosys },			/* 154 = unimplemented sched_setparam */
	{ 0, 0,
	    sys_nosys },			/* 155 = unimplemented sched_getparam */
	{ 0, 0,
	    sys_nosys },			/* 156 = unimplemented sched_setscheduler */
	{ 0, 0,
	    sys_nosys },			/* 157 = unimplemented sched_getscheduler */
	{ 0, 0,
	    sys_nosys },			/* 158 = unimplemented sched_yield */
	{ 0, 0,
	    sys_nosys },			/* 159 = unimplemented sched_get_priority_max */
	{ 0, 0,
	    sys_nosys },			/* 160 = unimplemented sched_get_priority_min */
	{ 0, 0,
	    sys_nosys },			/* 161 = unimplemented sched_rr_get_interval */
	{ 2, s(struct sys_nanosleep_args),
	    sys_nanosleep },			/* 162 = nanosleep */
	{ 4, s(struct linux_sys_mremap_args),
	    linux_sys_mremap },			/* 163 = mremap */
	{ 0, 0,
	    sys_nosys },			/* 164 = unimplemented setresuid */
	{ 0, 0,
	    sys_nosys },			/* 165 = unimplemented getresuid */
	{ 0, 0,
	    sys_nosys },			/* 166 = unimplemented vm86 */
	{ 0, 0,
	    sys_nosys },			/* 167 = unimplemented query_module */
	{ 3, s(struct sys_poll_args),
	    sys_poll },				/* 168 = poll */
	{ 0, 0,
	    sys_nosys },			/* 169 = unimplemented nfsservctl */
	{ 0, 0,
	    sys_nosys },			/* 170 = unimplemented setresgid */
	{ 0, 0,
	    sys_nosys },			/* 171 = unimplemented setresgid */
	{ 0, 0,
	    sys_nosys },			/* 172 = unimplemented prctl */
	{ 1, s(struct linux_sys_rt_sigreturn_args),
	    linux_sys_rt_sigreturn },		/* 173 = rt_sigreturn */
	{ 4, s(struct linux_sys_rt_sigaction_args),
	    linux_sys_rt_sigaction },		/* 174 = rt_sigaction */
	{ 4, s(struct linux_sys_rt_sigprocmask_args),
	    linux_sys_rt_sigprocmask },		/* 175 = rt_sigprocmask */
	{ 2, s(struct linux_sys_rt_sigpending_args),
	    linux_sys_rt_sigpending },		/* 176 = rt_sigpending */
	{ 0, 0,
	    sys_nosys },			/* 177 = unimplemented rt_sigtimedwait */
	{ 3, s(struct linux_sys_rt_queueinfo_args),
	    linux_sys_rt_queueinfo },		/* 178 = rt_queueinfo */
	{ 2, s(struct linux_sys_rt_sigsuspend_args),
	    linux_sys_rt_sigsuspend },		/* 179 = rt_sigsuspend */
	{ 4, s(struct sys_pread_args),
	    sys_pread },			/* 180 = pread */
	{ 4, s(struct sys_pwrite_args),
	    sys_pwrite },			/* 181 = pwrite */
	{ 3, s(struct linux_sys_chown_args),
	    linux_sys_chown },			/* 182 = chown */
	{ 2, s(struct sys___getcwd_args),
	    sys___getcwd },			/* 183 = __getcwd */
	{ 0, 0,
	    sys_nosys },			/* 184 = unimplemented capget */
	{ 0, 0,
	    sys_nosys },			/* 185 = unimplemented capset */
};

