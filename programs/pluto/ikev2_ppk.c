/*
 * Helper function for dealing with post-quantum preshared keys
 *
 * Copyright (C) 2017 Vukasin Karadzic <vukasin.karadzic@gmail.com>
 * Copyright (C) 2019 D. Hugh Redelmeier <hugh@mimosa.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <https://www.gnu.org/licenses/gpl2.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */



#include "defs.h"

#include "id.h"
#include "connections.h"        /* needs id.h */
#include "state.h"
#include "keys.h" /* needs state.h */
#include "demux.h"
#include "packet.h"
#include "ikev2_prf.h"

#include "ike_alg.h"
#include "crypt_symkey.h"
#include "ikev2.h"
#include "ikev2_ppk.h"
#include "ike_alg_hash.h"
#include "crypt_mac.h"
#include "ikev2_auth.h"
#include "log.h"
#include "ikev2_psk.h"

/*
 * Used by initiator, to properly construct struct from chunk_t we got
 * from .secrets
 */

struct ppk_id_payload ppk_id_payload(enum ikev2_ppk_id_type type,
				     const shunk_t ppk_id,
				     struct logger *logger)
{
	struct ppk_id_payload payload = {
		.type = type,
		.ppk_id = ppk_id,
	};
	if (LDBGP(DBG_CRYPT, logger)) {
		LDBG_log(logger, "ppk type: %d", (int) payload.type);
		LDBG_log(logger, "ppk_id from payload:");
		LDBG_hunk(logger, payload.ppk_id);
	}
	return payload;
}

/*
 * used by initiator to make chunk_t from ppk_id payload
 * for sending it in PPK_ID Notify Payload over the wire
 */
bool emit_unified_ppk_id(const struct ppk_id_payload *payl, struct pbs_out *outs)
{
	uint8_t type = PPK_ID_FIXED;
	if (!pbs_out_thing(outs, type, "PPK_ID_FIXED")) {
		/* already logged */
		return false;
	}
	return pbs_out_hunk(outs, payl->ppk_id, "PPK_ID");
}

/*
 * used by responder, for extracting PPK_ID from IKEv2 Notify PPK_ID
 * Payload, we store PPK_ID and its type in payl
 */
bool extract_v2N_ppk_identity(const struct pbs_in *notify_pbs,
			      struct ppk_id_payload *payl,
			      struct ike_sa *ike)
{
	diag_t d;
	struct pbs_in pbs = *notify_pbs;

	/* read in and verify the first (type) byte */

	uint8_t id_byte;
	d = pbs_in_thing(&pbs, id_byte, "PPK_ID type");
	if (d != NULL) {
		llog(RC_LOG, ike->sa.logger, "reading PPK ID: %s", str_diag(d));
		pfree_diag(&d);
		return false;

	}

	/* XXX: above+below could be turned into a descr? */
	enum ikev2_ppk_id_type id_type = id_byte;
	switch (id_type) {
	case PPK_ID_FIXED:
		dbg("PPK_ID of type PPK_ID_FIXED.");
		break;
	case PPK_ID_OPAQUE:
	default:
	{
		name_buf eb;
		llog_sa(RC_LOG, ike, "PPK_ID type %d (%s) not supported",
			id_type, str_enum_short(&ikev2_ppk_id_type_names, id_type, &eb));
		return false;
	}
	}

	shunk_t ppk_id = pbs_in_left(&pbs);
	if (LDBGP(DBG_BASE, ike->sa.logger)) {
		LDBG_log(ike->sa.logger, "extracted PPK_ID:");
		LDBG_hunk(ike->sa.logger, ppk_id);
	}

	if (ppk_id.len == 0) {
		llog_sa(RC_LOG, ike, "PPK ID data must be at least 1 byte");
		return false;
	}

	if (ppk_id.len > PPK_ID_MAXLEN) {
		llog_sa(RC_LOG, ike, "PPK ID %zu byte length exceeds %u",
			ppk_id.len, PPK_ID_MAXLEN);
		return false;
	}

	(*payl) = ppk_id_payload(id_type, ppk_id, ike->sa.logger);
	return true;
}

/*
 * used by responder, in IKE INTERMEDIATE exchange, for extracting
 * PPK_ID (variable length) and PPK confirmation (8 octets) from IKEv2
 * PPK_IDENTITY_KEY Notify Payload. We store PPK_ID and associated
 * PPK confirmation chunk in payl.
 *                      1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * ~                             PPK_ID                            ~
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * +                        PPK Confirmation                       +
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
bool extract_v2N_ppk_id_key(const struct pbs_in *notify_pbs,
			    struct ppk_id_key_payload *payl,
			    struct ike_sa *ike)
{
	diag_t d;
	struct pbs_in pbs = *notify_pbs;
	zero(payl);

	uint8_t id_byte;
	d = pbs_in_thing(&pbs, id_byte, "PPK_ID type");
	if (d != NULL) {
		llog(RC_LOG, ike->sa.logger,
		     "%s", str_diag(d));
		return false;

	}

	/* XXX: above+below could be turned into a descr? */
	enum ikev2_ppk_id_type id_type = id_byte;
	switch (id_type) {
	case PPK_ID_FIXED:
		dbg("PPK_ID of type PPK_ID_FIXED.");
		break;
	case PPK_ID_OPAQUE:
	default:
	{
		name_buf eb;
		llog_sa(RC_LOG, ike, "PPK_ID type %d (%s) not supported",
			id_type, str_enum_short(&ikev2_ppk_id_type_names, id_type, &eb));
		return false;
	}
	}

	payl->ppk_id_payl.type = id_type;

	shunk_t id_and_confirmation = pbs_in_left(&pbs);
	if (id_and_confirmation.len < PPK_CONFIRMATION_LEN) {
		llog(RC_LOG, ike->sa.logger, "ID+CONFIRMATION must be at least %u bytes, was %zu bytes",
		     PPK_CONFIRMATION_LEN, id_and_confirmation.len);
		return false;
	}

	size_t ppk_id_len = id_and_confirmation.len - PPK_CONFIRMATION_LEN;
	shunk_t ppk_id = hunk_slice(id_and_confirmation, 0, ppk_id_len);
	if (LDBGP(DBG_BASE, ike->sa.logger)) {
		LDBG_log(ike->sa.logger, "extracted PPK_ID:");
		LDBG_hunk(ike->sa.logger, ppk_id);
	}

	shunk_t ppk_confirmation = hunk_slice(id_and_confirmation, ppk_id_len, id_and_confirmation.len);
	if (LDBGP(DBG_CRYPT, ike->sa.logger)) {
		LDBG_log(ike->sa.logger, "extracted PPK Confirmation:");
		LDBG_hunk(ike->sa.logger, ppk_confirmation);
	}
	if (!PEXPECT(ike->sa.logger, ppk_confirmation.len == PPK_CONFIRMATION_LEN)) {
		/* above math screwed up */
		return false;
	}

	/* clone ppk id and ppk confirmation data */
	payl->ppk_id_payl = ppk_id_payload(id_type, ppk_id, ike->sa.logger);
	payl->ppk_confirmation = ppk_confirmation;

	return true;
}

static bool ikev2_calculate_hash(struct ike_sa *ike,
				 const struct crypt_mac *idhash,
				 struct pbs_out *a_pbs,
				 chunk_t *no_ppk_auth, /* optional output */
				 const struct hash_desc *hash_algo,
				 const struct pubkey_signer *signer)
{
	struct logger *logger = ike->sa.logger;
	const struct pubkey_type *type = &pubkey_type_rsa;
	statetime_t start = statetime_start(&ike->sa);
	const struct connection *c = ike->sa.st_connection;

	const struct secret_pubkey_stuff *pks = get_local_private_key(c, type,
								      ike->sa.logger);
	if (pks == NULL) {
		llog_sa(RC_LOG, ike, "No %s private key found", type->name);
		return false; /* failure: no key to use */
	}

	struct crypt_mac hash = v2_calculate_sighash(ike, idhash, hash_algo,
						     LOCAL_PERSPECTIVE);
	passert(hash.len <= sizeof(hash.ptr/*array*/));

	if (LDBGP(DBG_CRYPT, logger)) {
		LDBG_log_hunk(logger, "v2rsa octets:", *idhash);
	}

	/* now generate signature blob */
	statetime_t sign_time = statetime_start(&ike->sa);
	struct hash_signature sig;
	sig = signer->sign_hash(pks, idhash->ptr, idhash->len,
				hash_algo, ike->sa.logger);
	statetime_stop(&sign_time, "%s() calling sign_hash_RSA()", __func__);
	if (sig.len == 0)
		return false;

	if (no_ppk_auth != NULL) {
		*no_ppk_auth = clone_hunk(sig, "NO_PPK_AUTH chunk");
		if (LDBGP(DBG_PRIVATE, logger) || LDBGP(DBG_CRYPT, logger)) {
			LDBG_log_hunk(logger, "NO_PPK_AUTH payload:", *no_ppk_auth);
		}
	} else {
		if (!out_hunk(sig, a_pbs, "rsa signature"))
			return false;
	}

	statetime_stop(&start, "%s()", __func__);
	return true;
}

bool ikev2_calc_no_ppk_auth(struct ike_sa *ike,
			    const struct crypt_mac *id_hash,
			    chunk_t *no_ppk_auth /* output */)
{
	struct connection *c = ike->sa.st_connection;
	enum keyword_auth authby = c->local->host.config->auth;

	free_chunk_content(no_ppk_auth);	/* in case it was occupied */

	switch (authby) {
	case AUTH_RSASIG:
	{
		const struct hash_desc *hash_algo = v2_auth_negotiated_signature_hash(ike);
		if (hash_algo == NULL) {
			if (c->config->sighash_policy == LEMPTY) {
				/* RSA with SHA1 without Digsig: no oid blob appended */
				if (!ikev2_calculate_hash(ike, id_hash, NULL, no_ppk_auth,
							  &ike_alg_hash_sha1,
							  &pubkey_signer_raw_pkcs1_1_5_rsa)) {
					return false;
				}
				return true;
			} else {
				llog_sa(RC_LOG, ike,
					  "no compatible hash algo");
				return false;
			}
		}

		shunk_t h = hash_algo->digital_signature_blob[DIGITAL_SIGNATURE_RSASSA_PSS_BLOB];
		if (h.len == 0) {
			llog_pexpect(ike->sa.logger, HERE,
				     "negotiated hash algorithm %s has no RSA ASN1 blob",
				     hash_algo->common.fqn);
			return false;
		}

		chunk_t hashval = NULL_HUNK;
		if (!ikev2_calculate_hash(ike, id_hash, NULL, &hashval,
					  hash_algo, &pubkey_signer_digsig_rsassa_pss)) {
			return false;
		}

		if (ike->sa.st_seen_hashnotify) {
			/*
			 * combine blobs to create no_ppk_auth:
			 * - ASN.1 algo blob
			 * - hashval
			 */
			int len = h.len + hashval.len;
			uint8_t *blobs = alloc_bytes(len,
				"bytes for blobs for AUTH_DIGSIG NO_PPK_AUTH");

			memcpy(&blobs[0], h.ptr, h.len);
			memcpy(&blobs[h.len], hashval.ptr, hashval.len);
			*no_ppk_auth = chunk2(blobs, len);
		}
		free_chunk_content(&hashval);
		return true;
	}
	case AUTH_PSK:
		/* store in no_ppk_auth */
		if (!ikev2_create_psk_auth(AUTH_PSK, ike, id_hash, no_ppk_auth)) {
			return false; /* was STF_INTERNAL_ERROR but don't tell */
		}
		return true;

	default:
		bad_case(authby);
	}
}

/* in X_no_ppk keys are stored keys that go into PRF, and we store result in sk_X */

static void ppk_recalc_one(PK11SymKey **sk /* updated */, PK11SymKey *ppk_key,
			   const struct prf_desc *prf_desc, const char *name,
			   struct logger *logger)
{
	PK11SymKey *t = ikev2_prfplus(prf_desc, ppk_key, *sk, prf_desc->prf_key_size, logger);
	symkey_delref(logger, name, sk);
	*sk = t;
	if (LDBGP(DBG_CRYPT, logger)) {
		chunk_t chunk_sk = chunk_from_symkey("sk_chunk", *sk, logger);
		LDBG_log_hunk(logger, "%s:", chunk_sk, name);
		free_chunk_content(&chunk_sk);
	}
}

void ppk_recalculate(shunk_t ppk, const struct prf_desc *prf_desc,
		     PK11SymKey **sk_d,	/* updated */
		     PK11SymKey **sk_pi,	/* updated */
		     PK11SymKey **sk_pr,	/* updated */
		     struct logger *logger)
{
	PK11SymKey *ppk_key = symkey_from_hunk("PPK Keying material", ppk, logger);

	if (LDBGP(DBG_CRYPT, logger)) {
		LDBG_log(logger, "starting to recalculate SK_d, SK_pi, SK_pr");
		LDBG_log_hunk(logger, "PPK:", ppk);
	}

	ppk_recalc_one(sk_d, ppk_key, prf_desc, "sk_d", logger);
	ppk_recalc_one(sk_pi, ppk_key, prf_desc, "sk_pi", logger);
	ppk_recalc_one(sk_pr, ppk_key, prf_desc, "sk_pr", logger);

	symkey_delref(logger, "PPK chunk", &ppk_key);
}
