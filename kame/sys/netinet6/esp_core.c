/*	$KAME: esp_core.c,v 1.28 2000/08/28 14:26:31 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include "opt_inet.h"
#include "opt_inet6.h"
#endif
#ifdef __NetBSD__
#include "opt_inet.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/syslog.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#ifdef INET6
#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>
#include <netinet/icmp6.h>
#endif

#include <netinet6/ipsec.h>
#include <netinet6/ah.h>
#include <netinet6/esp.h>
#include <net/pfkeyv2.h>
#include <netkey/keydb.h>
#include <crypto/des/des.h>
#include <crypto/blowfish/blowfish.h>
#include <crypto/cast128/cast128.h>
#ifndef IPSEC_ESP_TWOFISH
#include <crypto/rijndael/rijndael.h>
#else
#include <crypto/twofish/twofish.h>
#endif

#include <net/net_osdep.h>

#define USE_BLOCKCRYPT

static int esp_crypto_sanity __P((const struct esp_algorithm *,
	struct secasvar *, int));
static int esp_null_mature __P((struct secasvar *));
static int esp_null_decrypt __P((struct mbuf *, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_null_encrypt __P((struct mbuf *, size_t, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_descbc_mature __P((struct secasvar *));
static int esp_descbc_ivlen __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_descbc_decrypt __P((struct mbuf *, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_descbc_encrypt __P((struct mbuf *, size_t, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_des_schedule __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_cbc_mature __P((struct secasvar *));
#if 0
static int esp_blowfish_cbc_decrypt __P((struct mbuf *, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_blowfish_cbc_encrypt __P((struct mbuf *, size_t,
	size_t, struct secasvar *, const struct esp_algorithm *, int));
#endif
static int esp_blowfish_schedule __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_blowfish_blockdecrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
static int esp_blowfish_blockencrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
#ifndef USE_BLOCKCRYPT
static int esp_cast128cbc_decrypt __P((struct mbuf *, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_cast128cbc_encrypt __P((struct mbuf *, size_t, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
#endif
static int esp_cast128_schedule __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_cast128_blockdecrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
static int esp_cast128_blockencrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
#ifndef USE_BLOCKCRYPT
static int esp_3descbc_decrypt __P((struct mbuf *, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_3descbc_encrypt __P((struct mbuf *, size_t, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
#endif
static int esp_3des_schedule __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_3des_blockdecrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
static int esp_3des_blockencrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
#ifndef IPSEC_ESP_TWOFISH
static int esp_rijndael_schedule __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_rijndael_blockdecrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
static int esp_rijndael_blockencrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
#else
static int esp_twofish_schedule __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_twofish_blockdecrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
static int esp_twofish_blockencrypt __P((const struct esp_algorithm *,
	struct secasvar *, u_int8_t *, u_int8_t *));
#endif
static int esp_common_ivlen __P((const struct esp_algorithm *,
	struct secasvar *));
static int esp_cbc_decrypt __P((struct mbuf *, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static int esp_cbc_encrypt __P((struct mbuf *, size_t, size_t,
	struct secasvar *, const struct esp_algorithm *, int));
static void esp_increment_iv __P((struct secasvar *));

#define MAXIVLEN	16

const struct esp_algorithm *
esp_algorithm_lookup(idx)
	int idx;
{
	static struct esp_algorithm esp_algorithms[] = {
		{ 8, -1, esp_descbc_mature, 64, 64, sizeof(des_key_schedule),
			"des-cbc",
			esp_descbc_ivlen, esp_descbc_decrypt,
			esp_descbc_encrypt, esp_des_schedule, },
		{ 8, 8, esp_cbc_mature, 192, 192, sizeof(des_key_schedule) * 3,
			"3des-cbc",
			esp_common_ivlen,
#ifdef USE_BLOCKCRYPT
			esp_cbc_decrypt, esp_cbc_encrypt,
#else
			esp_3descbc_decrypt, esp_3descbc_encrypt,
#endif
			esp_3des_schedule,
			esp_3des_blockdecrypt, esp_3des_blockencrypt, },
		{ 1, 0, esp_null_mature, 0, 2048, 0, "null",
			esp_common_ivlen, esp_null_decrypt,
			esp_null_encrypt, NULL, },
		/*
		 * XXX it seems that old blowfish code had a bug.
		 * hardcode to esp_cbc_{en,de}crypt.
		 */
		{ 8, 8, esp_cbc_mature, 40, 448, sizeof(BF_KEY), "blowfish-cbc",
			esp_common_ivlen, esp_cbc_decrypt,
			esp_cbc_encrypt, esp_blowfish_schedule,
			esp_blowfish_blockdecrypt, esp_blowfish_blockencrypt, },
		{ 8, 8, esp_cbc_mature, 40, 128, sizeof(u_int32_t) * 32,
			"cast128-cbc",
			esp_common_ivlen,
#ifdef USE_BLOCKCRYPT
			esp_cbc_decrypt, esp_cbc_encrypt,
#else
			esp_cast128cbc_decrypt, esp_cast128cbc_encrypt,
#endif
			esp_cast128_schedule,
			esp_cast128_blockdecrypt, esp_cast128_blockencrypt, },
#ifndef IPSEC_ESP_TWOFISH
		{ 16, 16, esp_cbc_mature, 128, 256, sizeof(keyInstance) * 2,
			"rijndael-cbc",
			esp_common_ivlen, esp_cbc_decrypt,
			esp_cbc_encrypt, esp_rijndael_schedule,
			esp_rijndael_blockdecrypt, esp_rijndael_blockencrypt },
#else
		{ 16, 16, esp_cbc_mature, 128, 256, sizeof(keyInstance) * 2,
			"twofish-cbc",
			esp_common_ivlen, esp_cbc_decrypt,
			esp_cbc_encrypt, esp_twofish_schedule,
			esp_twofish_blockdecrypt, esp_twofish_blockencrypt },
#endif
	};

	switch (idx) {
	case SADB_EALG_DESCBC:
		return &esp_algorithms[0];
	case SADB_EALG_3DESCBC:
		return &esp_algorithms[1];
	case SADB_EALG_NULL:
		return &esp_algorithms[2];
	case SADB_X_EALG_BLOWFISHCBC:
		return &esp_algorithms[3];
	case SADB_X_EALG_CAST128CBC:
		return &esp_algorithms[4];
#ifndef IPSEC_ESP_TWOFISH
	case SADB_X_EALG_RIJNDAELCBC:
#else
	case SADB_X_EALG_TWOFISHCBC:
#endif
		return &esp_algorithms[5];
	default:
		return NULL;
	}
}

int
esp_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{

	if (_KEYBITS(sav->key_enc) < algo->keymin ||
	    _KEYBITS(sav->key_enc) > algo->keymax) {
		ipseclog((LOG_ERR,
		    "esp_schedule %s: unsupported key length %d: "
		    "needs %d to %d bits\n", algo->name, _KEYBITS(sav->key_enc),
		    algo->keymin, algo->keymax));
		return EINVAL;
	}

	if (!algo->schedule || algo->schedlen == 0)
		return 0;
	if (!sav->sched || sav->schedlen != algo->schedlen)
		panic("invalid sav->schedlen in esp_schedule");
	return (*algo->schedule)(algo, sav);
}

/*
 * default sanity check for algo->{de,en}crypt
 */
static int
esp_crypto_sanity(algo, sav, ivlen)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	int ivlen;
{

	if (sav->ivlen != ivlen ||
	    (algo->ivlenval >= 0 && algo->ivlenval != ivlen)) {
		ipseclog((LOG_ERR, "esp_decrypt %s: bad ivlen %d/%d\n",
		    algo->name, ivlen, sav->ivlen));
		return EINVAL;
	}
	if (!sav->sched || sav->schedlen != algo->schedlen) {
		ipseclog((LOG_ERR,
		    "esp_decrypt %s: no intermediate key\n", algo->name));
		return EINVAL;
	}

	return 0;
}

/*
 * mbuf assumption: foo_encrypt() assumes that IV part is placed in a single
 * mbuf, not across multiple mbufs.
 */

static int
esp_null_mature(sav)
	struct secasvar *sav;
{

	/* anything is okay */
	return 0;
}

static int
esp_null_decrypt(m, off, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;		/* offset to ESP header */
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{

	return 0; /* do nothing */
}

static int
esp_null_encrypt(m, off, plen, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;	/* offset to ESP header */
	size_t plen;	/* payload length (to be encrypted) */
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{

	return 0; /* do nothing */
}

static int
esp_descbc_mature(sav)
	struct secasvar *sav;
{
	const struct esp_algorithm *algo;

	if (!(sav->flags & SADB_X_EXT_OLD) && (sav->flags & SADB_X_EXT_IV4B)) {
		ipseclog((LOG_ERR, "esp_cbc_mature: "
		    "algorithm incompatible with 4 octets IV length\n"));
		return 1;
	}

	if (!sav->key_enc) {
		ipseclog((LOG_ERR, "esp_descbc_mature: no key is given.\n"));
		return 1;
	}

	algo = esp_algorithm_lookup(sav->alg_enc);
	if (!algo) {
		ipseclog((LOG_ERR,
		    "esp_descbc_mature: unsupported algorithm.\n"));
		return 1;
	}

	if (_KEYBITS(sav->key_enc) < algo->keymin ||
	    _KEYBITS(sav->key_enc) > algo->keymax) {
		ipseclog((LOG_ERR,
		    "esp_descbc_mature: invalid key length %d.\n",
		    _KEYBITS(sav->key_enc)));
		return 1;
	}

	/* weak key check */
	if (des_is_weak_key((C_Block *)_KEYBUF(sav->key_enc))) {
		ipseclog((LOG_ERR,
		    "esp_descbc_mature: weak key was passed.\n"));
		return 1;
	}

	return 0;
}

static int
esp_descbc_ivlen(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{

	if (!sav)
		return 8;
	if ((sav->flags & SADB_X_EXT_OLD) && (sav->flags & SADB_X_EXT_IV4B))
		return 4;
	if (!(sav->flags & SADB_X_EXT_OLD) && (sav->flags & SADB_X_EXT_DERIV))
		return 4;
	return 8;
}

static int
esp_descbc_decrypt(m, off, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;		/* offset to ESP header */
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff = 0;
	size_t bodyoff = 0;
	u_int8_t *iv;
	size_t plen;
	u_int8_t tiv[8];
	int derived;
	int error;

	derived = 0;
	/* sanity check */
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	if (sav->flags & SADB_X_EXT_OLD) {
		/* RFC 1827 */
		ivoff = off + sizeof(struct esp);
		bodyoff = off + sizeof(struct esp) + ivlen;
		derived = 0;
	} else {
		/* RFC 2406 */
		if (sav->flags & SADB_X_EXT_DERIV) {
			/*
			 * draft-ietf-ipsec-ciph-des-derived-00.txt
			 * uses sequence number field as IV field.
			 * This draft has been deleted, but you can get from
			 * ftp://ftp.kame.net/pub/internet-drafts/.
			 */
			ivoff = off + sizeof(struct esp);
			bodyoff = off + sizeof(struct esp) + sizeof(u_int32_t);
			ivlen = sizeof(u_int32_t);
			derived = 1;
		} else {
			ivoff = off + sizeof(struct newesp);
			bodyoff = off + sizeof(struct newesp) + ivlen;
			derived = 0;
		}
	}
	if (ivlen == 4) {
		iv = &tiv[0];
		m_copydata(m, ivoff, 4, &tiv[0]);
		bcopy(&tiv[0], &tiv[4], 4);
		tiv[4] ^= 0xff;
		tiv[5] ^= 0xff;
		tiv[6] ^= 0xff;
		tiv[7] ^= 0xff;
	} else if (ivlen == 8) {
		iv = &tiv[0];
		m_copydata(m, ivoff, 8, &tiv[0]);
	} else {
		ipseclog((LOG_ERR, "esp_descbc_decrypt: unsupported ivlen %d\n",
		    ivlen));
		m_freem(m);
		return EINVAL;
	}

	plen = m->m_pkthdr.len;
	if (plen < bodyoff)
		panic("esp_descbc_decrypt: too short packet: len=%lu",
		    (u_long)plen);
	plen -= bodyoff;

	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_descbc_decrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}

	error = des_cbc_encrypt(m, bodyoff, plen,
	    *(des_key_schedule *)sav->sched, (C_Block *)iv, DES_DECRYPT);

	/* for safety */
	bzero(&tiv[0], sizeof(tiv));

	if (error)
		m_freem(m);
	return error;
}

static int
esp_descbc_encrypt(m, off, plen, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;	/* offset to ESP header */
	size_t plen;	/* payload length (to be decrypted) */
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff = 0;
	size_t bodyoff = 0;
	u_int8_t *iv;
	u_int8_t tiv[8];
	int derived;
	int error;

	derived = 0;

	/* sanity check */
	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_descbc_encrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	if (sav->flags & SADB_X_EXT_OLD) {
		/* RFC 1827 */
		/*
		 * draft-ietf-ipsec-ciph-des-derived-00.txt
		 * uses sequence number field as IV field.
		 */
		ivoff = off + sizeof(struct esp);
		bodyoff = off + sizeof(struct esp) + ivlen;
		derived = 0;
	} else {
		/* RFC 2406 */
		if (sav->flags & SADB_X_EXT_DERIV) {
			/*
			 * draft-ietf-ipsec-ciph-des-derived-00.txt
			 * uses sequence number field as IV field.
			 */
			ivoff = off + sizeof(struct esp);
			bodyoff = off + sizeof(struct esp) + sizeof(u_int32_t);
			ivlen = sizeof(u_int32_t);
			derived = 1;
		} else {
			ivoff = off + sizeof(struct newesp);
			bodyoff = off + sizeof(struct newesp) + ivlen;
			derived = 0;
		}
	}

	if (m->m_pkthdr.len < bodyoff)
		panic("assumption failed: mbuf too short");
	if (ivlen == 4) {
		if (derived)
			m_copydata(m, ivoff, ivlen, &tiv[0]);
		else {
			m_copyback(m, ivoff, ivlen, sav->iv);
			bcopy(sav->iv, &tiv[0], 4);
		}
		bcopy(&tiv[0], &tiv[4], 4);
		tiv[4] ^= 0xff;
		tiv[5] ^= 0xff;
		tiv[6] ^= 0xff;
		tiv[7] ^= 0xff;
		iv = &tiv[0];
	} else if (ivlen == 8) {
		m_copyback(m, ivoff, ivlen, sav->iv);
		iv = sav->iv;
	} else {
		ipseclog((LOG_ERR,
		    "esp_descbc_encrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}

	error = des_cbc_encrypt(m, bodyoff, plen,
	    *(des_key_schedule *)sav->sched, (C_Block *)iv, DES_ENCRYPT);

	esp_increment_iv(sav);

	/* for safety */
	bzero(&tiv[0], sizeof(tiv));

	if (error)
		m_freem(m);
	return error;
}

static int
esp_des_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{

	if (des_key_sched((C_Block *)_KEYBUF(sav->key_enc),
	    *(des_key_schedule *)sav->sched))
		return EINVAL;
	else
		return 0;
}

static int
esp_cbc_mature(sav)
	struct secasvar *sav;
{
	int keylen;
	const struct esp_algorithm *algo;

	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_cbc_mature: algorithm incompatible with esp-old\n"));
		return 1;
	}
	if (sav->flags & SADB_X_EXT_DERIV) {
		ipseclog((LOG_ERR,
		    "esp_cbc_mature: algorithm incompatible with derived\n"));
		return 1;
	}

	if (!sav->key_enc) {
		ipseclog((LOG_ERR, "esp_cbc_mature: no key is given.\n"));
		return 1;
	}

	algo = esp_algorithm_lookup(sav->alg_enc);
	if (!algo) {
		ipseclog((LOG_ERR,
		    "esp_cbc_mature %s: unsupported algorithm.\n", algo->name));
		return 1;
	}

	keylen = sav->key_enc->sadb_key_bits;
	if (keylen < algo->keymin || algo->keymax < keylen) {
		ipseclog((LOG_ERR,
		    "esp_cbc_mature %s: invalid key length %d.\n",
		    algo->name, sav->key_enc->sadb_key_bits));
		return 1;
	}
	switch (sav->alg_enc) {
	case SADB_EALG_3DESCBC:
		/* weak key check */
		if (des_is_weak_key((C_Block *)_KEYBUF(sav->key_enc))
		 || des_is_weak_key((C_Block *)(_KEYBUF(sav->key_enc) + 8))
		 || des_is_weak_key((C_Block *)(_KEYBUF(sav->key_enc) + 16))) {
			ipseclog((LOG_ERR,
			    "esp_cbc_mature %s: weak key was passed.\n",
			    algo->name));
			return 1;
		}
		break;
	case SADB_X_EALG_BLOWFISHCBC:
	case SADB_X_EALG_CAST128CBC:
	case SADB_X_EALG_TWOFISHCBC:
		break;
	case SADB_X_EALG_RIJNDAELCBC:
		/* allows specific key sizes only */
		if (!(keylen == 128 || keylen == 192 || keylen == 256)) {
			ipseclog((LOG_ERR,
			    "esp_cbc_mature %s: invalid key length %d.\n",
			    algo->name, keylen));
			return 1;
		}
		break;
	}

	return 0;
}

#if 0
static int
esp_blowfish_cbc_decrypt(m, off, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;		/* offset to ESP header */
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff;
	size_t bodyoff;
	u_int8_t *iv;
	u_int8_t tiv[8];
	size_t plen;
	int error;

	/* sanity check */
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_blowfish_cbc_decrypt: unsupported ESP version\n"));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != 8) {
		ipseclog((LOG_ERR,
		    "esp_blowfish_cbc_decrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;
	iv = &tiv[0];
	m_copydata(m, ivoff, 8, &tiv[0]);

	plen = m->m_pkthdr.len;
	if (plen < bodyoff)
		panic("esp_blowfish_cbc_decrypt: too short packet: len=%lu",
		    (u_long)plen);
	plen -= bodyoff;

	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_blowfish_cbc_decrypt: "
			"payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}

	error = BF_cbc_encrypt_m(m, bodyoff, plen, (BF_KEY *)sav->sched, iv,
	    BF_DECRYPT);

	/* for safety */
	bzero(&tiv[0], sizeof(tiv));

	if (error)
		m_freem(m);
	return error;
}

static int
esp_blowfish_cbc_encrypt(m, off, plen, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;	/* offset to ESP header */
	size_t plen;	/* payload length (to be decrypted) */
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff;
	size_t bodyoff;
	u_int8_t *iv;
	int error;

	/* sanity check */
	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_blowfish_cbc_encrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_blowfish_cbc_encrypt: unsupported ESP version\n"));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != 8) {
		ipseclog((LOG_ERR,
		    "esp_blowfish_cbc_encrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;

	if (m->m_pkthdr.len < bodyoff)
		panic("assumption failed: mbuf too short");
	m_copyback(m, ivoff, ivlen, (caddr_t)sav->iv);
	iv = sav->iv;

	error = BF_cbc_encrypt_m(m, bodyoff, plen, (BF_KEY *)sav->sched, iv,
	    BF_ENCRYPT);

	esp_increment_iv(sav);

	if (error)
		m_freem(m);
	return error;
}
#endif

static int
esp_blowfish_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{

	BF_set_key((BF_KEY *)sav->sched, _KEYLEN(sav->key_enc),
	    _KEYBUF(sav->key_enc));
	return 0;
}

static int
esp_blowfish_blockdecrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{

	/* assumption: d has a good alignment */
	bcopy(s, d, sizeof(BF_LONG) * 2);
	BF_encrypt((BF_LONG *)d, (BF_KEY *)sav->sched, BF_DECRYPT);
	return 0;
}

static int
esp_blowfish_blockencrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{

	/* assumption: d has a good alignment */
	bcopy(s, d, sizeof(BF_LONG) * 2);
	BF_encrypt((BF_LONG *)d, (BF_KEY *)sav->sched, BF_ENCRYPT);
	return 0;
}

#ifndef USE_BLOCKCRYPT
static int
esp_cast128cbc_decrypt(m, off, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff;
	size_t bodyoff;
	u_int8_t iv[8];
	size_t plen;
	int error;

	/* sanity check */
	if (_KEYBITS(sav->key_enc) < algo->keymin ||
	    _KEYBITS(sav->key_enc) > algo->keymax) {
		ipseclog((LOG_ERR,
		    "esp_cast128cbc_decrypt: unsupported key length %d: "
		    "needs %d to %d bits\n", _KEYBITS(sav->key_enc),
		    algo->keymin, algo->keymax));
		m_freem(m);
		return EINVAL;
	}
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_cast128cbc_decrypt: unsupported ESP version\n"));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != 8) {
		ipseclog((LOG_ERR,
		    "esp_cast128cbc_decrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;

	/* copy mbuf's IV into iv */
	m_copydata(m, ivoff, 8, iv);

	plen = m->m_pkthdr.len;
	if (plen < bodyoff) {
		panic("esp_cast128cbc_decrypt: too short packet: len=%lu\n",
		    (u_long)plen);
	}
	plen -= bodyoff;

	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_cast128cbc_decrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}

	/* decrypt */
	error = cast128_cbc_process(m, bodyoff, plen, (u_int32_t *)sav->sched,
	    iv, _KEYLEN(sav->key_enc), CAST128_DECRYPT);

	if (error)
		m_freem(m);
	return error;
}

static int
esp_cast128cbc_encrypt(m, off, plen, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;
	size_t plen;
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff;
	size_t bodyoff;
	u_int8_t *iv;
	int error;

	/* sanity check */
	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_cast128cbc_encrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}
	if (_KEYBITS(sav->key_enc) < algo->keymin ||
	    _KEYBITS(sav->key_enc) > algo->keymax) {
		ipseclog((LOG_ERR,
		    "esp_cast128cbc_encrypt: unsupported key length %d: "
		    "needs %d to %d bits\n", _KEYBITS(sav->key_enc),
		    algo->keymin, algo->keymax));
		m_freem(m);
		return EINVAL;
	}
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_cast128cbc_encrypt: unsupported ESP version\n"));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != 8) {
		ipseclog((LOG_ERR,
		    "esp_cast128cbc_encrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;

	if (m->m_pkthdr.len < bodyoff)
		panic("assumption failed: mbuf too short");
	m_copyback(m, ivoff, ivlen, sav->iv);
	iv = sav->iv;

	/* encrypt */
	error = cast128_cbc_process(m, bodyoff, plen, (u_int32_t *)sav->sched,
	    iv, _KEYLEN(sav->key_enc), CAST128_ENCRYPT);

	esp_increment_iv(sav);

	if (error)
		m_freem(m);
	return error;
}
#endif

static int
esp_cast128_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{

	set_cast128_subkey((u_int32_t *)sav->sched, _KEYBUF(sav->key_enc));
	return 0;
}

static int
esp_cast128_blockdecrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{

	if (_KEYLEN(sav->key_enc) <= 80 / 8)
		cast128_decrypt_round12(d, s, (u_int32_t *)sav->sched);
	else
		cast128_decrypt_round16(d, s, (u_int32_t *)sav->sched);
	return 0;
}

static int
esp_cast128_blockencrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{

	if (_KEYLEN(sav->key_enc) <= 80 / 8)
		cast128_encrypt_round12(d, s, (u_int32_t *)sav->sched);
	else
		cast128_encrypt_round16(d, s, (u_int32_t *)sav->sched);
	return 0;
}

#ifndef USE_BLOCKCRYPT
static int
esp_3descbc_decrypt(m, off, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff;
	size_t bodyoff;
	u_int8_t *iv;
	size_t plen;
	u_int8_t tiv[8];
	int error;

	/* sanity check */
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_3descbc_decrypt: unsupported ESP version\n"));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != 8) {
		ipseclog((LOG_ERR,
		    "esp_3descbc_decrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;
	iv = &tiv[0];
	m_copydata(m, ivoff, 8, &tiv[0]);

	plen = m->m_pkthdr.len;
	if (plen < bodyoff)
		panic("esp_3descbc_decrypt: too short packet: len=%lu",
		   (u_long)plen);

	plen -= bodyoff;

	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_3descbc_decrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}

	/* decrypt packet */
	des_3cbc_process(m, bodyoff, plen, (des_key_schedule *)sav->sched,
	    (C_Block *)iv, DES_DECRYPT);

	/* for safety */
	bzero(&tiv[0], sizeof(tiv));

	return 0;
}

static int
esp_3descbc_encrypt(m, off, plen, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;
	size_t plen;
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	size_t ivoff;
	size_t bodyoff;
	u_int8_t *iv;
	int error;

	/* sanity check */
	if (plen % 8) {
		ipseclog((LOG_ERR, "esp_3descbc_encrypt: "
		    "payload length must be multiple of 8\n"));
		m_freem(m);
		return EINVAL;
	}
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR,
		    "esp_3descbc_encrypt: unsupported ESP version\n"));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != 8) {
		ipseclog((LOG_ERR,
		    "esp_3descbc_encrypt: unsupported ivlen %d\n", ivlen));
		m_freem(m);
		return EINVAL;
	}
	error = esp_crypto_sanity(algo, sav, ivlen);
	if (error) {
		m_freem(m);
		return error;
	}

	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;

	if (m->m_pkthdr.len < bodyoff)
		panic("assumption failed: mbuf too short");
	m_copyback(m, ivoff, ivlen, sav->iv);
	iv = sav->iv;

	/* encrypt packet */
	des_3cbc_process(m, bodyoff, plen, (des_key_schedule *)sav->sched,
	    (C_Block *)iv, DES_ENCRYPT);

	esp_increment_iv(sav);

	return 0;
}
#endif

static int
esp_3des_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{
	int error;
	des_key_schedule *p;
	int i;
	char *k;

	p = (des_key_schedule *)sav->sched;
	k = _KEYBUF(sav->key_enc);
	for (i = 0; i < 3; i++) {
		error = des_key_sched((C_Block *)(k + 8 * i), p[i]);
		if (error)
			return EINVAL;
	}
	return 0;
}

static int
esp_3des_blockdecrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	des_key_schedule *p;

	/* assumption: d has a good alignment */
	p = (des_key_schedule *)sav->sched;
	bcopy(s, d, sizeof(DES_LONG) * 2);
	des_encrypt((DES_LONG *)d, p[2], DES_DECRYPT);
	des_encrypt((DES_LONG *)d, p[1], DES_ENCRYPT);
	des_encrypt((DES_LONG *)d, p[0], DES_DECRYPT);
	return 0;
}

static int
esp_3des_blockencrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	des_key_schedule *p;

	/* assumption: d has a good alignment */
	p = (des_key_schedule *)sav->sched;
	bcopy(s, d, sizeof(DES_LONG) * 2);
	des_encrypt((DES_LONG *)d, p[0], DES_ENCRYPT);
	des_encrypt((DES_LONG *)d, p[1], DES_DECRYPT);
	des_encrypt((DES_LONG *)d, p[2], DES_ENCRYPT);
	return 0;
}

#ifndef IPSEC_ESP_TWOFISH
/* as rijndael uses assymetric scheduled keys, we need to do it twice. */
static int
esp_rijndael_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{
	keyInstance *p;

	p = (keyInstance *)sav->sched;
	if (rijndael_makeKey(&p[0], DIR_DECRYPT, _KEYLEN(sav->key_enc),
	    _KEYBUF(sav->key_enc)) < 0)
		return -1;
	if (rijndael_makeKey(&p[1], DIR_ENCRYPT, _KEYLEN(sav->key_enc),
	    _KEYBUF(sav->key_enc)) < 0)
		return -1;
	return 0;
}

static int
esp_rijndael_blockdecrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	cipherInstance c;
	keyInstance *p;

	/* does not take advantage of CBC mode support */
	bzero(&c, sizeof(c));
	if (rijndael_cipherInit(&c, MODE_CBC, NULL) < 0)
		return -1;
	p = (keyInstance *)sav->sched;
	if (rijndael_blockDecrypt(&c, &p[0], s, algo->padbound, d) < 0)
		return -1;
	return 0;
}

static int
esp_rijndael_blockencrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	cipherInstance c;
	keyInstance *p;

	/* does not take advantage of CBC mode support */
	bzero(&c, sizeof(c));
	if (rijndael_cipherInit(&c, MODE_CBC, NULL) < 0)
		return -1;
	p = (keyInstance *)sav->sched;
	if (rijndael_blockEncrypt(&c, &p[1], s, algo->padbound, d) < 0)
		return -1;
	return 0;
}
#else
/*
 * twofish actually do not use assymetric scheduled keys, however, AES C
 * syggests assymetric key setup.
 */
static int
esp_twofish_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{
	keyInstance *p;

	p = (keyInstance *)sav->sched;
	if (twofish_makeKey(&p[0], DIR_DECRYPT, _KEYLEN(sav->key_enc),
	    _KEYBUF(sav->key_enc)) < 0)
		return -1;
	if (twofish_makeKey(&p[1], DIR_ENCRYPT, _KEYLEN(sav->key_enc),
	    _KEYBUF(sav->key_enc)) < 0)
		return -1;
	return 0;
}

static int
esp_twofish_blockdecrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	cipherInstance c;
	keyInstance *p;

	/* does not take advantage of CBC mode support */
	bzero(&c, sizeof(c));
	if (twofish_cipherInit(&c, MODE_CBC, NULL) < 0)
		return -1;
	p = (keyInstance *)sav->sched;
	if (twofish_blockDecrypt(&c, &p[0], s, algo->padbound, d) < 0)
		return -1;
	return 0;
}

static int
esp_twofish_blockencrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	cipherInstance c;
	keyInstance *p;

	/* does not take advantage of CBC mode support */
	bzero(&c, sizeof(c));
	if (twofish_cipherInit(&c, MODE_CBC, NULL) < 0)
		return -1;
	p = (keyInstance *)sav->sched;
	if (twofish_blockEncrypt(&c, &p[1], s, algo->padbound, d) < 0)
		return -1;
	return 0;
}
#endif

static int
esp_common_ivlen(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{

	if (!algo)
		panic("esp_common_ivlen: unknown algorithm");
	return algo->ivlenval;
}

static int
esp_cbc_decrypt(m, off, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	struct mbuf *s;
	struct mbuf *d, *d0, *dp;
	int soff, doff;	/*offset from the head of chain, to head of this mbuf */
	int sn, dn;	/*offset from the head of the mbuf, to meat */
	size_t ivoff, bodyoff;
	u_int8_t iv[MAXIVLEN], *ivp;
	u_int8_t sbuf[MAXIVLEN], *sp;
	u_int8_t *p, *q;
	struct mbuf *scut;
	int scutoff;
	int i;

	if (m->m_pkthdr.len < off) {
		ipseclog((LOG_ERR, "esp_cbc_decrypt %s: bad len %d/%d\n",
		    algo->name, m->m_pkthdr.len, off));
		m_freem(m);
		return EINVAL;
	}
	if ((m->m_pkthdr.len - off) % algo->padbound) {
		ipseclog((LOG_ERR, "esp_cbc_decrypt %s: "
		    "payload length must be multiple of %d\n",
		    algo->name, algo->padbound));
		m_freem(m);
		return EINVAL;
	}
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR, "esp_cbc_decrypt %s: "
		    "unsupported ESP version\n", algo->name));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != algo->padbound || ivlen != sav->ivlen) {
		ipseclog((LOG_ERR, "esp_cbc_decrypt %s: "
		    "unsupported ivlen %d\n", algo->name, ivlen));
		m_freem(m);
		return EINVAL;
	}

	s = m;
	d = d0 = dp = NULL;
	soff = doff = sn = dn = 0;
	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;
	m_copydata(m, ivoff, ivlen, iv);
	ivp = sp = NULL;

	/* skip bodyoff */
	while (soff < bodyoff) {
		if (soff + s->m_len > bodyoff) {
			sn = bodyoff - soff;
			break;
		}

		soff += s->m_len;
		s = s->m_next;
	}
	scut = s;
	scutoff = sn;

	/* skip over empty mbuf */
	while (s && s->m_len == 0)
		s = s->m_next;

	while (soff < m->m_pkthdr.len) {
		/* source */
		if (sn + ivlen <= s->m_len) {
			/* body is continuous */
			sp = mtod(s, u_int8_t *) + sn;
		} else {
			/* body is non-continuous */
			m_copydata(s, sn, ivlen, sbuf);
			sp = sbuf;
		}

		/* destination */
		if (!d || dn + ivlen > d->m_len) {
			if (d)
				dp = d;
			MGET(d, M_DONTWAIT, MT_DATA);
			i = m->m_pkthdr.len - (soff + sn);
			if (d && i > MLEN) {
				MCLGET(d, M_DONTWAIT);
				if ((d->m_flags & M_EXT) == 0) {
					m_free(d);
					d = NULL;
				}
			}
			if (!d) {
				m_freem(m);
				if (d0)
					m_freem(d0);
				return ENOBUFS;
			}
			if (!d0)
				d0 = d;
			if (dp)
				dp->m_next = d;
			d->m_len = 0;
			d->m_len = (M_TRAILINGSPACE(d) / ivlen) * ivlen;
			if (d->m_len > i)
				d->m_len = i;
			dn = 0;
		}

		/* decrypt */
		(*algo->blockdecrypt)(algo, sav, sp, mtod(d, u_int8_t *) + dn);

		/* xor */
		p = ivp ? ivp : iv;
		q = mtod(d, u_int8_t *) + dn;
		for (i = 0; i < ivlen; i++)
			q[i] ^= p[i];

		/* next iv */
		if (sp == sbuf) {
			bcopy(sbuf, iv, sizeof(iv));
			ivp = NULL;
		} else
			ivp = sp;

		sn += ivlen;
		dn += ivlen;

		while (s && sn >= s->m_len) {
			sn -= s->m_len;
			soff += s->m_len;
			s = s->m_next;
		}

		/* skip over empty mbuf */
		while (s && s->m_len == 0)
			s = s->m_next;
	}

	m_freem(scut->m_next);
	scut->m_len = scutoff;
	scut->m_next = d0;

	/* just in case */
	bzero(iv, sizeof(iv));
	bzero(sbuf, sizeof(sbuf));

	return 0;
}

static int
esp_cbc_encrypt(m, off, plen, sav, algo, ivlen)
	struct mbuf *m;
	size_t off;
	size_t plen;
	struct secasvar *sav;
	const struct esp_algorithm *algo;
	int ivlen;
{
	struct mbuf *s;
	struct mbuf *d, *d0, *dp;
	int soff, doff;	/*offset from the head of chain, to head of this mbuf */
	int sn, dn;	/*offset from the head of the mbuf, to meat */
	size_t ivoff, bodyoff;
	u_int8_t iv[MAXIVLEN], *ivp;
	u_int8_t sbuf[MAXIVLEN], *sp;
	u_int8_t *p, *q;
	struct mbuf *scut;
	int scutoff;
	int i;

	if (m->m_pkthdr.len < off) {
		ipseclog((LOG_ERR, "esp_cbc_encrypt %s: bad len %d/%d\n",
		    algo->name, m->m_pkthdr.len, off));
		m_freem(m);
		return EINVAL;
	}
	if ((m->m_pkthdr.len - off) % algo->padbound) {
		ipseclog((LOG_ERR, "esp_cbc_encrypt %s: "
		    "payload length must be multiple of %d\n",
		    algo->name, algo->padbound));
		m_freem(m);
		return EINVAL;
	}
	if (sav->flags & SADB_X_EXT_OLD) {
		ipseclog((LOG_ERR, "esp_cbc_encrypt %s: "
		    "unsupported ESP version\n", algo->name));
		m_freem(m);
		return EINVAL;
	}
	if (ivlen != algo->padbound || ivlen != sav->ivlen) {
		ipseclog((LOG_ERR, "esp_cbc_encrypt %s: "
		    "unsupported ivlen %d\n", algo->name, ivlen));
		m_freem(m);
		return EINVAL;
	}

	s = m;
	d = d0 = dp = NULL;
	soff = doff = sn = dn = 0;
	ivoff = off + sizeof(struct newesp);
	bodyoff = off + sizeof(struct newesp) + ivlen;
	/* initialize iv - maybe it is better to overwrite dest, not source */
	m_copyback(m, ivoff, ivlen, sav->iv);
	bcopy(sav->iv, iv, ivlen);
	ivp = sp = NULL;

	/* skip bodyoff */
	while (soff < bodyoff) {
		if (soff + s->m_len > bodyoff) {
			sn = bodyoff - soff;
			break;
		}

		soff += s->m_len;
		s = s->m_next;
	}
	scut = s;
	scutoff = sn;

	/* skip over empty mbuf */
	while (s && s->m_len == 0)
		s = s->m_next;

	while (soff < m->m_pkthdr.len) {
		/* source */
		if (sn + ivlen <= s->m_len) {
			/* body is continuous */
			sp = mtod(s, u_int8_t *) + sn;
		} else {
			/* body is non-continuous */
			m_copydata(s, sn, ivlen, sbuf);
			sp = sbuf;
		}

		/* destination */
		if (!d || dn + ivlen > d->m_len) {
			if (d)
				dp = d;
			MGET(d, M_DONTWAIT, MT_DATA);
			i = m->m_pkthdr.len - (soff + sn);
			if (d && i > MLEN) {
				MCLGET(d, M_DONTWAIT);
				if ((d->m_flags & M_EXT) == 0) {
					m_free(d);
					d = NULL;
				}
			}
			if (!d) {
				m_freem(m);
				if (d0)
					m_freem(d0);
				return ENOBUFS;
			}
			if (!d0)
				d0 = d;
			if (dp)
				dp->m_next = d;
			d->m_len = 0;
			d->m_len = (M_TRAILINGSPACE(d) / ivlen) * ivlen;
			if (d->m_len > i)
				d->m_len = i;
			dn = 0;
		}

		/* xor */
		p = ivp ? ivp : iv;
		q = sp;
		for (i = 0; i < ivlen; i++)
			q[i] ^= p[i];

		/* encrypt */
		(*algo->blockencrypt)(algo, sav, sp, mtod(d, u_int8_t *) + dn);

		/* next iv */
		ivp = mtod(d, u_int8_t *) + dn;

		sn += ivlen;
		dn += ivlen;

		while (s && sn >= s->m_len) {
			sn -= s->m_len;
			soff += s->m_len;
			s = s->m_next;
		}

		/* skip over empty mbuf */
		while (s && s->m_len == 0)
			s = s->m_next;
	}

	m_freem(scut->m_next);
	scut->m_len = scutoff;
	scut->m_next = d0;

	/* just in case */
	bzero(iv, sizeof(iv));
	bzero(sbuf, sizeof(sbuf));

	esp_increment_iv(sav);

	return 0;
}

/*
 * increment iv.
 */
static void
esp_increment_iv(sav)
	struct secasvar *sav;
{
	u_int8_t *x;
	u_int8_t y;
	int i;

#if !(defined(__FreeBSD__) && __FreeBSD__ >= 3)
	y = time.tv_sec & 0xff;
#else
	y = time_second & 0xff;
#endif
	if (!y) y++;
	x = (u_int8_t *)sav->iv;
	for (i = 0; i < sav->ivlen; i++) {
		*x = (*x + y) & 0xff;
		x++;
	}
}

/*------------------------------------------------------------*/

/* does not free m0 on error */
int
esp_auth(m0, skip, length, sav, sum)
	struct mbuf *m0;
	size_t skip;	/* offset to ESP header */
	size_t length;	/* payload length */
	struct secasvar *sav;
	u_char *sum;
{
	struct mbuf *m;
	size_t off;
	struct ah_algorithm_state s;
	u_char sumbuf[AH_MAXSUMSIZE];
	const struct ah_algorithm *algo;
	size_t siz;
	int error;

	/* sanity checks */
	if (m0->m_pkthdr.len < skip) {
		ipseclog((LOG_DEBUG, "esp_auth: mbuf length < skip\n"));
		return EINVAL;
	}
	if (m0->m_pkthdr.len < skip + length) {
		ipseclog((LOG_DEBUG,
		    "esp_auth: mbuf length < skip + length\n"));
		return EINVAL;
	}
	/*
	 * length of esp part (excluding authentication data) must be 4n,
	 * since nexthdr must be at offset 4n+3.
	 */
	if (length % 4) {
		ipseclog((LOG_ERR, "esp_auth: length is not multiple of 4\n"));
		return EINVAL;
	}
	if (!sav) {
		ipseclog((LOG_DEBUG, "esp_auth: NULL SA passed\n"));
		return EINVAL;
	}
	algo = ah_algorithm_lookup(sav->alg_auth);
	if (!algo) {
		ipseclog((LOG_ERR,
		    "esp_auth: bad ESP auth algorithm passed: %d\n",
		    sav->alg_auth));
		return EINVAL;
	}

	m = m0;
	off = 0;

	siz = (((*algo->sumsiz)(sav) + 3) & ~(4 - 1));
	if (sizeof(sumbuf) < siz) {
		ipseclog((LOG_DEBUG,
		    "esp_auth: AH_MAXSUMSIZE is too small: siz=%lu\n",
		    (u_long)siz));
		return EINVAL;
	}

	/* skip the header */
	while (skip) {
		if (!m)
			panic("mbuf chain?");
		if (m->m_len <= skip) {
			skip -= m->m_len;
			m = m->m_next;
			off = 0;
		} else {
			off = skip;
			skip = 0;
		}
	}

	error = (*algo->init)(&s, sav);
	if (error)
		return error;

	while (0 < length) {
		if (!m)
			panic("mbuf chain?");

		if (m->m_len - off < length) {
			(*algo->update)(&s, mtod(m, u_char *) + off,
				m->m_len - off);
			length -= m->m_len - off;
			m = m->m_next;
			off = 0;
		} else {
			(*algo->update)(&s, mtod(m, u_char *) + off, length);
			break;
		}
	}
	(*algo->result)(&s, sumbuf);
	bcopy(sumbuf, sum, siz);	/*XXX*/
	
	return 0;
}
