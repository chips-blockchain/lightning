#include <ccan/array_size/array_size.h>
#include <ccan/str/hex/hex.h>
#include <common/utils.h>
#include <stdio.h>
#include <wally_core.h>

static const char *reason;
#undef SUPERVERBOSE
#define SUPERVERBOSE(r) do { reason = (r); } while(0)

#include <bitcoin/pubkey.c>
#include <common/amount.c>
#include <common/bigsize.c>
#include <common/node_id.c>
#include <common/setup.h>

#if EXPERIMENTAL_FEATURES
#include <wire/peer_exp_wiregen.c>
#else
#include <wire/peer_wiregen.c>
#endif
#include <wire/fromwire.c>
#include <wire/towire.c>
#include <wire/tlvstream.c>

/* AUTOGENERATED MOCKS START */
/* Generated stub for fromwire_channel_id */
void fromwire_channel_id(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
			 struct channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "fromwire_channel_id called!\n"); abort(); }
/* Generated stub for towire_channel_id */
void towire_channel_id(u8 **pptr UNNEEDED, const struct channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "towire_channel_id called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */


/* BOLT #1
### TLV Decoding Failures

The following TLV streams in any namespace should trigger a decoding failure:

1. Invalid stream: 0xfd
2. Reason: type truncated

1. Invalid stream: 0xfd01
2. Reason: type truncated

1. Invalid stream: 0xfd0001 00
2. Reason: not minimally encoded type

1. Invalid stream: 0xfd0101
2. Reason: missing length

1. Invalid stream: 0x0f fd
2. Reason: (length truncated)

1. Invalid stream: 0x0f fd26
2. Reason: (length truncated)

1. Invalid stream: 0x0f fd2602
2. Reason: missing value

1. Invalid stream: 0x0f fd0001 00
2. Reason: not minimally encoded length

1. Invalid stream: 0x0f fd0201 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
2. Reason: value truncated

The following TLV streams in either namespace should trigger a
decoding failure:

1. Invalid stream: 0x12 00
2. Reason: unknown even type.

1. Invalid stream: 0xfd0102 00
2. Reason: unknown even type.

1. Invalid stream: 0xfe01000002 00
2. Reason: unknown even type.

1. Invalid stream: 0xff0100000000000002 00
2. Reason: unknown even type.
*/

struct invalid_stream {
	const char *hex;
	const char *reason;
};

static struct invalid_stream invalid_streams_either[] = {
	{ "fd", "type truncated" },
	{ "fd01", "type truncated" },
	{ "fd0001 00", "not minimally encoded type" },
	{ "fd0101", "missing length" },
	{ "0f fd", "(length truncated)" },
	{ "0f fd26", "(length truncated)" },
	{ "0f fd2602", "missing value" },
	{ "0f fd0001 00", "not minimally encoded length" },
	{ "0f fd0201 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", "value truncated" },

	{ "12 00", "unknown even type." },
	{ "fd0102 00", "unknown even type." },
	{ "fe01000002 00", "unknown even type." },
	{ "ff0100000000000002 00", "unknown even type." },
};

/* BOLT #1:
 *
 * The following TLV streams in namespace `n1` should trigger a decoding
failure:

1. Invalid stream: 0x01 09 ffffffffffffffffff
2. Reason: greater than encoding length for `n1`s `tlv1`.

1. Invalid stream: 0x01 01 00
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 02 0001
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 03 000100
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 04 00010000
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 05 0001000000
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 06 000100000000
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 07 00010000000000
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x01 08 0001000000000000
2. Reason: encoding for `n1`s `tlv1`s `amount_msat` is not minimal

1. Invalid stream: 0x02 07 01010101010101
2. Reason: less than encoding length for `n1`s `tlv2`.

1. Invalid stream: 0x02 09 010101010101010101
2. Reason: greater than encoding length for `n1`s `tlv2`.

1. Invalid stream: 0x03 21 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb
2. Reason: less than encoding length for `n1`s `tlv3`.

1. Invalid stream: 0x03 29 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb0000000000000001
2. Reason: less than encoding length for `n1`s `tlv3`.

1. Invalid stream: 0x03 30 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb000000000000000100000000000001
2. Reason: less than encoding length for `n1`s `tlv3`.

1. Invalid stream: 0x03 31 043da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb00000000000000010000000000000002
2. Reason: `n1`s `node_id` is not a valid point.

1. Invalid stream: 0x03 32 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb0000000000000001000000000000000001
2. Reason: greater than encoding length for `n1`s `tlv3`.

1. Invalid stream: 0xfd00fe 00
2. Reason: less than encoding length for `n1`s `tlv4`.

1. Invalid stream: 0xfd00fe 01 01
2. Reason: less than encoding length for `n1`s `tlv4`.

1. Invalid stream: 0xfd00fe 03 010101
2. Reason: greater than encoding length for `n1`s `tlv4`.

1. Invalid stream: 0x00 00
2. Reason: unknown even field for `n1`s namespace.
*/

static struct invalid_stream invalid_streams_n1[] = {
	{ "01 09 ffffffffffffffffff", "greater than encoding length for `n1`s `tlv1`." },
	{ "01 01 00", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 02 0001", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 03 000100", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 04 00010000", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 05 0001000000", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 06 000100000000", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 07 00010000000000", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "01 08 0001000000000000", "encoding for `n1`s `tlv1`s `amount_msat` is not minimal" },
	{ "02 07 01010101010101", "less than encoding length for `n1`s `tlv2`." },
	{ "02 09 010101010101010101", "greater than encoding length for `n1`s `tlv2`." },
	{ "03 21 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb", "less than encoding length for `n1`s `tlv3`." },
	{ "03 29 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb0000000000000001", "less than encoding length for `n1`s `tlv3`." },
	{ "03 30 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb000000000000000100000000000001", "less than encoding length for `n1`s `tlv3`." },
	{ "03 31 043da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb00000000000000010000000000000002", "`n1`s `node_id` is not a valid point." },
	{ "03 32 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb0000000000000001000000000000000001", "greater than encoding length for `n1`s `tlv3`." },
	{ "fd00fe 00", "less than encoding length for `n1`s `tlv4`." },
	{ "fd00fe 01 01", "less than encoding length for `n1`s `tlv4`." },
	{ "fd00fe 03 010101", "greater than encoding length for `n1`s `tlv4`." },
	{ "00 00", "unknown even field for `n1`s namespace." },
};

/* BOLT #1:
### TLV Stream Decoding Failure

Any appending of an invalid stream to a valid stream should trigger
a decoding failure.

Any appending of a higher-numbered valid stream to a lower-numbered
valid stream should not trigger a decoding failure.

In addition, the following TLV streams in namespace `n1` should
trigger a decoding failure:

1. Invalid stream: 0x02 08 0000000000000226 01 01 2a
2. Reason: valid TLV records but invalid ordering

1. Invalid stream: 0x02 08 0000000000000231 02 08 0000000000000451
2. Reason: duplicate TLV type

1. Invalid stream: 0x1f 00 0f 01 2a
2. Reason: valid (ignored) TLV records but invalid ordering

1. Invalid stream: 0x1f 00 1f 01 2a
2. Reason: duplicate TLV type (ignored)
 */
static struct invalid_stream invalid_streams_n1_combo[] = {
	{ "02 08 0000000000000226 01 01 2a", "valid tlv records but invalid ordering" },
	{ "02 08 0000000000000231 02 08 0000000000000451", "duplicate tlv type" },
	{ "1f 00 0f 01 2a", "valid (ignored) tlv records but invalid ordering" },
	{ "1f 00 1f 01 2a", "duplicate tlv type (ignored)" }
};

/* BOLT #1:
The following TLV stream in namespace `n2` should trigger a decoding
failure:

1. Invalid stream: 0xffffffffffffffffff 00 00 00
2. Reason: valid TLV records but invalid ordering
*/
static struct invalid_stream invalid_streams_n2_combo[] = {
	{ "ffffffffffffffffff 00 00 00", "valid TLV records but invalid ordering" },
};

/* BOLT #1:
 *
### TLV Decoding Successes

The following TLV streams in either namespace should correctly decode,
and be ignored:

1. Valid stream: 0x
2. Explanation: empty message

1. Valid stream: 0x21 00
2. Explanation: Unknown odd type.

1. Valid stream: 0xfd0201 00
2. Explanation: Unknown odd type.

1. Valid stream: 0xfd00fd 00
2. Explanation: Unknown odd type.

1. Valid stream: 0xfd00ff 00
2. Explanation: Unknown odd type.

1. Valid stream: 0xfe02000001 00
2. Explanation: Unknown odd type.

1. Valid stream: 0xff0200000000000001 00
2. Explanation: Unknown odd type.

The following TLV streams in `n1` namespace should correctly decode,
with the values given here:

1. Valid stream: 0x01 00
2. Values: `tlv1` `amount_msat`=0

1. Valid stream: 0x01 01 01
2. Values: `tlv1` `amount_msat`=1

1. Valid stream: 0x01 02 0100
2. Values: `tlv1` `amount_msat`=256

1. Valid stream: 0x01 03 010000
2. Values: `tlv1` `amount_msat`=65536

1. Valid stream: 0x01 04 01000000
2. Values: `tlv1` `amount_msat`=16777216

1. Valid stream: 0x01 05 0100000000
2. Values: `tlv1` `amount_msat`=4294967296

1. Valid stream: 0x01 06 010000000000
2. Values: `tlv1` `amount_msat`=1099511627776

1. Valid stream: 0x01 07 01000000000000
2. Values: `tlv1` `amount_msat`=281474976710656

1. Valid stream: 0x01 08 0100000000000000
2. Values: `tlv1` `amount_msat`=72057594037927936

1. Valid stream: 0x02 08 0000000000000226
2. Values: `tlv2` `scid`=0x0x550

1. Valid stream: 0x03 31 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb00000000000000010000000000000002
2. Values: `tlv3` `node_id`=023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb `amount_msat_1`=1 `amount_msat_2`=2

1. Valid stream: 0xfd00fe 02 0226
2. Values: `tlv4` `cltv_delta`=550
*/

struct valid_stream {
	const char *hex;
	const struct tlv_n1 expect;
};

static u64 tlv1_0 = 0;
static u64 tlv1_1 = 1;
static u64 tlv1_256 = 256;
static u64 tlv1_65536 = 65536;
static u64 tlv1_16777216 = 16777216;
static u64 tlv1_4294967296 = 4294967296ULL;
static u64 tlv1_1099511627776 = 1099511627776UL;
static u64 tlv1_281474976710656 = 281474976710656ULL;
static u64 tlv1_72057594037927936 = 72057594037927936ULL;
static struct short_channel_id tlv2_0x0x550 = { .u64 = 0x000000000226 };
/* filled in at runtime. */
static struct tlv_n1_tlv3 tlv3_node_id;
static u16 tlv4_550 = 550;

static struct valid_stream valid_streams[] = {
	/* Valid but no (known) content. */
	{ "", {NULL, NULL, NULL, NULL} },
	{ "21 00", {NULL, NULL, NULL, NULL} },
	{ "fd0201 00", {NULL, NULL, NULL, NULL} },
	{ "fd00fd 00", {NULL, NULL, NULL, NULL} },
	{ "fd00ff 00", {NULL, NULL, NULL, NULL} },
	{ "fe02000001 00", {NULL, NULL, NULL, NULL} },
	{ "ff0200000000000001 00", {NULL, NULL, NULL, NULL} },

	/* TLV1 */
	{ "01 00", { .tlv1 = &tlv1_0 } },
	{ "01 01 01", { .tlv1 = &tlv1_1 } },
	{ "01 02 0100", { .tlv1 = &tlv1_256 } },
	{ "01 03 010000", { .tlv1 = &tlv1_65536 } },
	{ "01 04 01000000", { .tlv1 = &tlv1_16777216 } },
	{ "01 05 0100000000", { .tlv1 = &tlv1_4294967296 } },
	{ "01 06 010000000000", { .tlv1 = &tlv1_1099511627776 } },
	{ "01 07 01000000000000", { .tlv1 = &tlv1_281474976710656 } },
	{ "01 08 0100000000000000", { .tlv1 = &tlv1_72057594037927936 } },
	{ "02 08 0000000000000226", { .tlv2 = &tlv2_0x0x550 } },
	{ "03 31 023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb00000000000000010000000000000002", { .tlv3 = &tlv3_node_id } },
	{ "fd00fe 02 0226", { .tlv4 = &tlv4_550 } },
};

static bool tlv_n1_eq(const struct tlv_n1 *a, const struct tlv_n1 *b)
{
	if (a->tlv1) {
		if (!b->tlv1)
			return false;
		if (*a->tlv1 != *b->tlv1)
			return false;
	} else if (b->tlv1)
		return false;

	if (a->tlv2) {
		if (!b->tlv2)
			return false;
		if (!short_channel_id_eq(a->tlv2, b->tlv2))
			return false;
	} else if (b->tlv2)
		return false;

	if (a->tlv3) {
		if (!b->tlv3)
			return false;
		if (!pubkey_eq(&a->tlv3->node_id, &b->tlv3->node_id))
			return false;
		if (!amount_msat_eq(a->tlv3->amount_msat_1,
				    b->tlv3->amount_msat_1))
			return false;
		if (!amount_msat_eq(a->tlv3->amount_msat_2,
				    b->tlv3->amount_msat_2))
			return false;
	} else if (b->tlv3)
		return false;

	if (a->tlv4) {
		if (!b->tlv4)
			return false;
		if (*a->tlv4 != *b->tlv4)
			return false;
	} else if (b->tlv4)
		return false;

	return true;
}

/* hexstr is valid, so pull out first field */
static u64 pull_type(const char *hexstr)
{
	u8 v;

	hex_decode(hexstr, 2, &v, sizeof(v));
	switch (v) {
	case 0xfd: {
		u16 d;
		hex_decode(hexstr + 2, 2*sizeof(d), &d, sizeof(d));
		return be16_to_cpu(d);
	}
	case 0xfe: {
		u32 d;
		hex_decode(hexstr + 2, 2*sizeof(d), &d, sizeof(d));
		return be32_to_cpu(d);
	}
	case 0xff: {
		u64 d;
		hex_decode(hexstr + 2, 2*sizeof(d), &d, sizeof(d));
		return be64_to_cpu(d);
	}
	default:
		return v;
	}
}

static u8 *stream(const tal_t *ctx, const char *hex)
{
	size_t i, j;
	char *str = tal_arr(tmpctx, char, strlen(hex) + 1);

	/* Remove spaces and use normal helper */
	for (i = j = 0; i <= strlen(hex); i++) {
		if (hex[i] != ' ')
			str[j++] = hex[i];
	}
	return tal_hexdata(ctx, str, strlen(str));
}

static u8 *stream2(const tal_t *ctx, const char *hex1, const char *hex2)
{
	u8 *a = stream(tmpctx, hex1), *b = stream(tmpctx, hex2), *ret;

	ret = tal_dup_arr(ctx, u8, a, tal_count(a), tal_count(b));
	memcpy(ret + tal_count(a), b, tal_count(b));
	return ret;
}

static bool ignored_fields(const struct tlv_n1 *tlv_n1)
{
	return tlv_n1->tlv1 == NULL
		&& tlv_n1->tlv2 == NULL
		&& tlv_n1->tlv3 == NULL
		&& tlv_n1->tlv4 == NULL;
}

int main(int argc, char *argv[])
{
	common_setup(argv[0]);

	if (!pubkey_from_hexstr("023da092f6980e58d2c037173180e9a465476026ee50f96695963e8efe436f54eb", 66, &tlv3_node_id.node_id))
		abort();
	/* gcc 4.8 doesn't like this initialized statically. */
	tlv3_node_id.amount_msat_1 = AMOUNT_MSAT(1);
	tlv3_node_id.amount_msat_2 = AMOUNT_MSAT(2);

	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_either); i++) {
		struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
		struct tlv_n2 *tlv_n2 = tlv_n2_new(tmpctx);
		const u8 *p, *orig_p;
		size_t max;

		orig_p = stream(tmpctx, invalid_streams_either[i].hex);
		max = tal_count(orig_p);
		p = orig_p;
		assert((!fromwire_n1(&p, &max, tlv_n1) && !p) ||
		       !n1_is_valid(tlv_n1, NULL));
		assert(strstr(invalid_streams_either[i].reason, reason));
		max = tal_count(orig_p);
		p = orig_p;
		assert((!fromwire_n2(&p, &max, tlv_n2) && !p) ||
		       !n2_is_valid(tlv_n2, NULL));
		assert(strstr(invalid_streams_either[i].reason, reason));
	}

	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_n1); i++) {
		struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
		const u8 *p;
		size_t max;

		p = stream(tmpctx, invalid_streams_n1[i].hex);
		max = tal_count(p);
		assert((!fromwire_n1(&p, &max, tlv_n1) && !p) ||
		       !n1_is_valid(tlv_n1, NULL));
		assert(strstr(invalid_streams_n1[i].reason, reason));
	}

	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_n1_combo); i++) {
		struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
		const u8 *p;
		size_t max;

		p = stream(tmpctx, invalid_streams_n1_combo[i].hex);
		max = tal_count(p);
		assert((!fromwire_n1(&p, &max, tlv_n1) && !p) ||
		       !n1_is_valid(tlv_n1, NULL));
		assert(strstr(invalid_streams_n1_combo[i].reason, reason));
	}

	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_n2_combo); i++) {
		struct tlv_n2 *tlv_n2 = tlv_n2_new(tmpctx);
		const u8 *p;
		size_t max;

		p = stream(tmpctx, invalid_streams_n2_combo[i].hex);
		max = tal_count(p);
		assert((!fromwire_n2(&p, &max, tlv_n2) && !p) ||
		       !n2_is_valid(tlv_n2, NULL));
		assert(strstr(invalid_streams_n2_combo[i].reason, reason));
	}

	for (size_t i = 0; i < ARRAY_SIZE(valid_streams); i++) {
 		struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
		const u8 *orig_p, *p;
		u8 *p2;
		size_t max;

		orig_p = stream(tmpctx, valid_streams[i].hex);

		max = tal_count(orig_p);
		p = orig_p;
		assert(fromwire_n1(&p, &max, tlv_n1) &&
		       n1_is_valid(tlv_n1, NULL));
		assert(max == 0);
		assert(tlv_n1_eq(tlv_n1, &valid_streams[i].expect));

		/* Re-encoding should give the same results (except
		 * ignored fields tests!) */
		if (ignored_fields(tlv_n1))
			continue;

		p2 = tal_arr(tmpctx, u8, 0);
		towire_n1(&p2, tlv_n1);
		assert(memeq(p2, tal_count(p2), orig_p, tal_count(orig_p)));
	}

	/* BOLT #1:
	 *
	 * Any appending of an invalid stream to a valid stream should trigger
	 * a decoding failure.
	 */
	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_either); i++) {
		for (size_t j = 0; j < ARRAY_SIZE(valid_streams); j++) {
			struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
			struct tlv_n2 *tlv_n2 = tlv_n2_new(tmpctx);
			const u8 *orig_p, *p;
			size_t max;

			/* Append */
			orig_p = stream2(tmpctx, valid_streams[j].hex,
					 invalid_streams_either[i].hex);
			max = tal_count(orig_p);
			p = orig_p;
			assert((!fromwire_n1(&p, &max, tlv_n1) && !p) ||
			       !n1_is_valid(tlv_n1, NULL));
			max = tal_count(orig_p);
			p = orig_p;
			assert((!fromwire_n2(&p, &max, tlv_n2) && !p) ||
			       !n2_is_valid(tlv_n2, NULL));
		}
	}

	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_n1); i++) {
		for (size_t j = 0; j < ARRAY_SIZE(valid_streams); j++) {
			struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
			const u8 *p;
			size_t max;

			p = stream2(tmpctx, valid_streams[j].hex,
				    invalid_streams_n1[i].hex);
			max = tal_count(p);
			assert((!fromwire_n1(&p, &max, tlv_n1) && !p) ||
			       !n1_is_valid(tlv_n1, NULL));
		}
	}

	for (size_t i = 0; i < ARRAY_SIZE(invalid_streams_n1_combo); i++) {
		for (size_t j = 0; j < ARRAY_SIZE(valid_streams); j++) {
			struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
			const u8 *p;
			size_t max;

			p = stream2(tmpctx, valid_streams[j].hex,
				    invalid_streams_n1_combo[i].hex);
			max = tal_count(p);
			assert((!fromwire_n1(&p, &max, tlv_n1) && !p) ||
			       !n1_is_valid(tlv_n1, NULL));
		}
	}

	/* BOLT #1:
	 *
	 * Any appending of a higher-numbered valid stream to a lower-numbered
	 * valid stream should not trigger a decoding failure.
	 */
	for (size_t i = 0; i < ARRAY_SIZE(valid_streams); i++) {
		for (size_t j = i+1; j < ARRAY_SIZE(valid_streams); j++) {
			struct tlv_n1 *tlv_n1 = tlv_n1_new(tmpctx);
			const u8 *orig_p, *p;
			size_t max;
			bool expect_success;

			if (strlen(valid_streams[i].hex) == 0
			    || strlen(valid_streams[j].hex) == 0)
				continue;

			orig_p = stream2(tmpctx, valid_streams[i].hex,
					 valid_streams[j].hex);
			max = tal_count(orig_p);
			p = orig_p;

			/* This comparison works for our simple cases. */
			expect_success = pull_type(valid_streams[i].hex)
				< pull_type(valid_streams[j].hex);

			assert(fromwire_n1(&p, &max, tlv_n1) &&
			       n1_is_valid(tlv_n1, NULL) == expect_success);

			if (!expect_success)
				continue;

			/* Re-encoding should give the same results (except
			 * ignored fields tests!) */
			if (ignored_fields(&valid_streams[i].expect)
			    || ignored_fields(&valid_streams[j].expect))
				continue;

			u8 *p2 = tal_arr(tmpctx, u8, 0);
			towire_n1(&p2, tlv_n1);
			assert(memeq(orig_p, tal_count(orig_p),
				     p2, tal_count(p2)));
		}
	}
	common_shutdown();
}
