/*-
 * Copyright (c) 2004 Pawel Jakub Dawidek <pjd@FreeBSD.org>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/geom/mirror/g_mirror.h,v 1.8.2.2 2004/10/15 06:10:12 pjd Exp $
 */

#ifndef	_G_MIRROR_H_
#define	_G_MIRROR_H_

#include <sys/endian.h>
#include <sys/md5.h>

#define	G_MIRROR_CLASS_NAME	"MIRROR"

#define	G_MIRROR_MAGIC		"GEOM::MIRROR"
#define	G_MIRROR_VERSION	1

#define	G_MIRROR_BALANCE_NONE		0
#define	G_MIRROR_BALANCE_ROUND_ROBIN	1
#define	G_MIRROR_BALANCE_LOAD		2
#define	G_MIRROR_BALANCE_SPLIT		3
#define	G_MIRROR_BALANCE_PREFER		4
#define	G_MIRROR_BALANCE_MIN		G_MIRROR_BALANCE_NONE
#define	G_MIRROR_BALANCE_MAX		G_MIRROR_BALANCE_PREFER

#define	G_MIRROR_DISK_FLAG_DIRTY		0x0000000000000001ULL
#define	G_MIRROR_DISK_FLAG_SYNCHRONIZING	0x0000000000000002ULL
#define	G_MIRROR_DISK_FLAG_FORCE_SYNC		0x0000000000000004ULL
#define	G_MIRROR_DISK_FLAG_INACTIVE		0x0000000000000008ULL
#define	G_MIRROR_DISK_FLAG_HARDCODED		0x0000000000000010ULL
#define	G_MIRROR_DISK_FLAG_MASK		(G_MIRROR_DISK_FLAG_DIRTY |	\
					 G_MIRROR_DISK_FLAG_SYNCHRONIZING | \
					 G_MIRROR_DISK_FLAG_FORCE_SYNC | \
					 G_MIRROR_DISK_FLAG_INACTIVE)

#define	G_MIRROR_DEVICE_FLAG_NOAUTOSYNC	0x0000000000000001ULL
#define	G_MIRROR_DEVICE_FLAG_MASK	(G_MIRROR_DEVICE_FLAG_NOAUTOSYNC)

#ifdef _KERNEL
extern u_int g_mirror_debug;

#define	G_MIRROR_DEBUG(lvl, ...)	do {				\
	if (g_mirror_debug >= (lvl)) {					\
		printf("GEOM_MIRROR");					\
		if (g_mirror_debug > 0)					\
			printf("[%u]", lvl);				\
		printf(": ");						\
		printf(__VA_ARGS__);					\
		printf("\n");						\
	}								\
} while (0)
#define	G_MIRROR_LOGREQ(lvl, bp, ...)	do {				\
	if (g_mirror_debug >= (lvl)) {					\
		printf("GEOM_MIRROR");					\
		if (g_mirror_debug > 0)					\
			printf("[%u]", lvl);				\
		printf(": ");						\
		printf(__VA_ARGS__);					\
		printf(" ");						\
		g_print_bio(bp);					\
		printf("\n");						\
	}								\
} while (0)

#define	G_MIRROR_BIO_FLAG_REGULAR	0x01
#define	G_MIRROR_BIO_FLAG_SYNC		0x02

/*
 * Informations needed for synchronization.
 */
struct g_mirror_disk_sync {
	struct g_consumer *ds_consumer;	/* Consumer connected to our mirror. */
	off_t		 ds_offset;	/* Offset of next request to send. */
	off_t		 ds_offset_done; /* Offset of already synchronized
					   region. */
	off_t		 ds_resync;	/* Resynchronize from this offset. */
	u_int		 ds_syncid;	/* Disk's synchronization ID. */
	u_char		*ds_data;
};

/*
 * Informations needed for synchronization.
 */
struct g_mirror_device_sync {
	struct g_geom	*ds_geom;	/* Synchronization geom. */
	u_int		 ds_ndisks;	/* Number of disks in SYNCHRONIZING
					   state. */
};

#define	G_MIRROR_DISK_STATE_NONE		0
#define	G_MIRROR_DISK_STATE_NEW			1
#define	G_MIRROR_DISK_STATE_ACTIVE		2
#define	G_MIRROR_DISK_STATE_STALE		3
#define	G_MIRROR_DISK_STATE_SYNCHRONIZING	4
#define	G_MIRROR_DISK_STATE_DISCONNECTED	5
#define	G_MIRROR_DISK_STATE_DESTROY		6
struct g_mirror_disk {
	uint32_t	 d_id;		/* Disk ID. */
	struct g_consumer *d_consumer;	/* Consumer. */
	struct g_mirror_softc	*d_softc; /* Back-pointer to softc. */
	int		 d_state;	/* Disk state. */
	u_int		 d_priority;	/* Disk priority. */
	struct bintime	 d_delay;	/* Disk delay. */
	struct bintime	 d_last_used;	/* When disk was last used. */
	uint64_t	 d_flags;	/* Additional flags. */
	struct g_mirror_disk_sync d_sync;/* Sync information. */
	LIST_ENTRY(g_mirror_disk) d_next;
};
#define	d_name	d_consumer->provider->name

#define	G_MIRROR_EVENT_DONTWAIT	0x1
#define	G_MIRROR_EVENT_WAIT	0x2
#define	G_MIRROR_EVENT_DEVICE	0x4
#define	G_MIRROR_EVENT_DONE	0x8
struct g_mirror_event {
	struct g_mirror_disk	*e_disk;
	int			 e_state;
	int			 e_flags;
	int			 e_error;
	TAILQ_ENTRY(g_mirror_event) e_next;
};

#define	G_MIRROR_DEVICE_FLAG_DESTROY	0x0100000000000000ULL
#define	G_MIRROR_DEVICE_FLAG_WAIT	0x0200000000000000ULL

#define	G_MIRROR_DEVICE_STATE_STARTING		0
#define	G_MIRROR_DEVICE_STATE_RUNNING		1

#define	G_MIRROR_BUMP_ON_FIRST_WRITE		1
#define	G_MIRROR_BUMP_IMMEDIATELY		2
struct g_mirror_softc {
	u_int		sc_state;	/* Device state. */
	uint32_t	sc_slice;	/* Slice size. */
	uint8_t		sc_balance;	/* Balance algorithm. */
	uint64_t	sc_mediasize;	/* Device size. */
	uint32_t	sc_sectorsize;	/* Sector size. */
	uint64_t	sc_flags;	/* Additional flags. */

	struct g_geom	*sc_geom;
	struct g_provider *sc_provider;

	uint32_t	sc_id;		/* Mirror unique ID. */

	struct bio_queue_head sc_queue;
	struct mtx	 sc_queue_mtx;
	struct proc	*sc_worker;

	LIST_HEAD(, g_mirror_disk) sc_disks;
	u_int		sc_ndisks;	/* Number of disks. */
	struct g_mirror_disk *sc_hint;

	u_int		sc_syncid;	/* Synchronization ID. */
	int		sc_bump_syncid;
	struct g_mirror_device_sync sc_sync;

	TAILQ_HEAD(, g_mirror_event) sc_events;
	struct mtx	sc_events_mtx;

	struct callout	sc_callout;
};
#define	sc_name	sc_geom->name

u_int g_mirror_ndisks(struct g_mirror_softc *sc, int state);
int g_mirror_destroy(struct g_mirror_softc *sc, boolean_t force);
int g_mirror_event_send(void *arg, int state, int flags);
struct g_mirror_metadata;
void g_mirror_fill_metadata(struct g_mirror_softc *sc,
    struct g_mirror_disk *disk, struct g_mirror_metadata *md);
void g_mirror_update_metadata(struct g_mirror_disk *disk);

g_ctl_req_t g_mirror_config;
#endif	/* _KERNEL */

struct g_mirror_metadata {
	char		md_magic[16];	/* Magic value. */
	uint32_t	md_version;	/* Version number. */
	char		md_name[16];	/* Mirror name. */
	uint32_t	md_mid;		/* Mirror unique ID. */
	uint32_t	md_did;		/* Disk unique ID. */
	uint8_t		md_all;		/* Number of disks in mirror. */
	uint32_t	md_syncid;	/* Synchronization ID. */
	uint8_t		md_priority;	/* Disk priority. */
	uint32_t	md_slice;	/* Slice size. */
	uint8_t		md_balance;	/* Balance type. */
	uint64_t	md_mediasize;	/* Size of the smallest
					   disk in mirror. */
	uint32_t	md_sectorsize;	/* Sector size. */
	uint64_t	md_sync_offset;	/* Synchronized offset. */
	uint64_t	md_mflags;	/* Additional mirror flags. */
	uint64_t	md_dflags;	/* Additional disk flags. */
	char		md_provider[16]; /* Hardcoded provider. */
	u_char		md_hash[16];	/* MD5 hash. */
};
static __inline void
mirror_metadata_encode(struct g_mirror_metadata *md, u_char *data)
{
	MD5_CTX ctx;

	bcopy(md->md_magic, data, 16);
	le32enc(data + 16, md->md_version);
	bcopy(md->md_name, data + 20, 16);
	le32enc(data + 36, md->md_mid);
	le32enc(data + 40, md->md_did);
	*(data + 44) = md->md_all;
	le32enc(data + 45, md->md_syncid);
	*(data + 49) = md->md_priority;
	le32enc(data + 50, md->md_slice);
	*(data + 54) = md->md_balance;
	le64enc(data + 55, md->md_mediasize);
	le32enc(data + 63, md->md_sectorsize);
	le64enc(data + 67, md->md_sync_offset);
	le64enc(data + 75, md->md_mflags);
	le64enc(data + 83, md->md_dflags);
	bcopy(md->md_provider, data + 91, 16);
	MD5Init(&ctx);
	MD5Update(&ctx, data, 107);
	MD5Final(md->md_hash, &ctx);
	bcopy(md->md_hash, data + 107, 16);
}
static __inline int
mirror_metadata_decode(const u_char *data, struct g_mirror_metadata *md)
{
	MD5_CTX ctx;

	bcopy(data, md->md_magic, 16);
	md->md_version = le32dec(data + 16);
	bcopy(data + 20, md->md_name, 16);
	md->md_mid = le32dec(data + 36);
	md->md_did = le32dec(data + 40);
	md->md_all = *(data + 44);
	md->md_syncid = le32dec(data + 45);
	md->md_priority = *(data + 49);
	md->md_slice = le32dec(data + 50);
	md->md_balance = *(data + 54);
	md->md_mediasize = le64dec(data + 55);
	md->md_sectorsize = le32dec(data + 63);
	md->md_sync_offset = le64dec(data + 67);
	md->md_mflags = le64dec(data + 75);
	md->md_dflags = le64dec(data + 83);
	bcopy(data + 91, md->md_provider, 16);
	bcopy(data + 107, md->md_hash, 16);
	MD5Init(&ctx);
	MD5Update(&ctx, data, 107);
	MD5Final(md->md_hash, &ctx);
	if (bcmp(md->md_hash, data + 107, 16) != 0)
		return (EINVAL);
	return (0);
}

static __inline const char *
balance_name(u_int balance)
{
	static const char *algorithms[] = {
		[G_MIRROR_BALANCE_NONE] = "none",
		[G_MIRROR_BALANCE_ROUND_ROBIN] = "round-robin",
		[G_MIRROR_BALANCE_LOAD] = "load",
		[G_MIRROR_BALANCE_SPLIT] = "split",
		[G_MIRROR_BALANCE_PREFER] = "prefer",
		[G_MIRROR_BALANCE_MAX + 1] = "unknown"
	};

	if (balance > G_MIRROR_BALANCE_MAX)
		balance = G_MIRROR_BALANCE_MAX + 1;

	return (algorithms[balance]);
}

static __inline int
balance_id(const char *name)
{
	static const char *algorithms[] = {
		[G_MIRROR_BALANCE_NONE] = "none",
		[G_MIRROR_BALANCE_ROUND_ROBIN] = "round-robin",
		[G_MIRROR_BALANCE_LOAD] = "load",
		[G_MIRROR_BALANCE_SPLIT] = "split",
		[G_MIRROR_BALANCE_PREFER] = "prefer"
	};
	int n;

	for (n = G_MIRROR_BALANCE_MIN; n <= G_MIRROR_BALANCE_MAX; n++) {
		if (strcmp(name, algorithms[n]) == 0)
			return (n);
	}
	return (-1);
}

static __inline void
mirror_metadata_dump(const struct g_mirror_metadata *md)
{
	static const char hex[] = "0123456789abcdef";
	char hash[16 * 2 + 1];
	u_int i;

	printf("     magic: %s\n", md->md_magic);
	printf("   version: %u\n", (u_int)md->md_version);
	printf("      name: %s\n", md->md_name);
	printf("       mid: %u\n", (u_int)md->md_mid);
	printf("       did: %u\n", (u_int)md->md_did);
	printf("       all: %u\n", (u_int)md->md_all);
	printf("    syncid: %u\n", (u_int)md->md_syncid);
	printf("  priority: %u\n", (u_int)md->md_priority);
	printf("     slice: %u\n", (u_int)md->md_slice);
	printf("   balance: %s\n", balance_name((u_int)md->md_balance));
	printf(" mediasize: %jd\n", (intmax_t)md->md_mediasize);
	printf("sectorsize: %u\n", (u_int)md->md_sectorsize);
	printf("syncoffset: %jd\n", (intmax_t)md->md_sync_offset);
	printf("    mflags:");
	if (md->md_mflags == 0)
		printf(" NONE");
	else {
		if ((md->md_mflags & G_MIRROR_DEVICE_FLAG_NOAUTOSYNC) != 0)
			printf(" NOAUTOSYNC");
	}
	printf("\n");
	printf("    dflags:");
	if (md->md_dflags == 0)
		printf(" NONE");
	else {
		if ((md->md_dflags & G_MIRROR_DISK_FLAG_DIRTY) != 0)
			printf(" DIRTY");
		if ((md->md_dflags & G_MIRROR_DISK_FLAG_SYNCHRONIZING) != 0)
			printf(" SYNCHRONIZING");
		if ((md->md_dflags & G_MIRROR_DISK_FLAG_FORCE_SYNC) != 0)
			printf(" FORCE_SYNC");
		if ((md->md_dflags & G_MIRROR_DISK_FLAG_INACTIVE) != 0)
			printf(" INACTIVE");
	}
	printf("\n");
	printf("hcprovider: %s\n", md->md_provider);
	bzero(hash, sizeof(hash));
	for (i = 0; i < 16; i++) {
		hash[i * 2] = hex[md->md_hash[i] >> 4];
		hash[i * 2 + 1] = hex[md->md_hash[i] & 0x0f];
	}
	printf("  MD5 hash: %s\n", hash);
}
#endif	/* !_G_MIRROR_H_ */
