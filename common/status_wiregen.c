/* This file was generated by generate-wire.py */
/* Do not modify this file! Modify the .csv file it was generated from. */
/* Original template can be found at tools/gen/impl_template */

#include <common/status_wiregen.h>
#include <assert.h>
#include <ccan/array_size/array_size.h>
#include <ccan/mem/mem.h>
#include <ccan/tal/str/str.h>
#include <common/utils.h>
#include <stdio.h>

#ifndef SUPERVERBOSE
#define SUPERVERBOSE(...)
#endif


const char *status_wire_name(int e)
{
	static char invalidbuf[sizeof("INVALID ") + STR_MAX_CHARS(e)];

	switch ((enum status_wire)e) {
	case WIRE_STATUS_LOG: return "WIRE_STATUS_LOG";
	case WIRE_STATUS_IO: return "WIRE_STATUS_IO";
	case WIRE_STATUS_FAIL: return "WIRE_STATUS_FAIL";
	case WIRE_STATUS_PEER_CONNECTION_LOST: return "WIRE_STATUS_PEER_CONNECTION_LOST";
	case WIRE_STATUS_PEER_BILLBOARD: return "WIRE_STATUS_PEER_BILLBOARD";
	case WIRE_STATUS_VERSION: return "WIRE_STATUS_VERSION";
	}

	snprintf(invalidbuf, sizeof(invalidbuf), "INVALID %i", e);
	return invalidbuf;
}

bool status_wire_is_defined(u16 type)
{
	switch ((enum status_wire)type) {
	case WIRE_STATUS_LOG:;
	case WIRE_STATUS_IO:;
	case WIRE_STATUS_FAIL:;
	case WIRE_STATUS_PEER_CONNECTION_LOST:;
	case WIRE_STATUS_PEER_BILLBOARD:;
	case WIRE_STATUS_VERSION:;
	      return true;
	}
	return false;
}





/* WIRE: STATUS_LOG */
u8 *towire_status_log(const tal_t *ctx, enum log_level level, const struct node_id *peer, const wirestring *entry)
{
	u8 *p = tal_arr(ctx, u8, 0);

	towire_u16(&p, WIRE_STATUS_LOG);
	towire_log_level(&p, level);
	if (!peer)
		towire_bool(&p, false);
	else {
		towire_bool(&p, true);
		towire_node_id(&p, peer);
	}
	towire_wirestring(&p, entry);

	return memcheck(p, tal_count(p));
}
bool fromwire_status_log(const tal_t *ctx, const void *p, enum log_level *level, struct node_id **peer, wirestring **entry)
{
	const u8 *cursor = p;
	size_t plen = tal_count(p);

	if (fromwire_u16(&cursor, &plen) != WIRE_STATUS_LOG)
		return false;
 	*level = fromwire_log_level(&cursor, &plen);
 	if (!fromwire_bool(&cursor, &plen))
		*peer = NULL;
	else {
		*peer = tal(ctx, struct node_id);
		fromwire_node_id(&cursor, &plen, *peer);
	}
 	*entry = fromwire_wirestring(ctx, &cursor, &plen);
	return cursor != NULL;
}

/* WIRE: STATUS_IO */
u8 *towire_status_io(const tal_t *ctx, enum log_level iodir, const struct node_id *peer, const wirestring *who, const u8 *data)
{
	u16 len = tal_count(data);
	u8 *p = tal_arr(ctx, u8, 0);

	towire_u16(&p, WIRE_STATUS_IO);
	towire_log_level(&p, iodir);
	if (!peer)
		towire_bool(&p, false);
	else {
		towire_bool(&p, true);
		towire_node_id(&p, peer);
	}
	towire_wirestring(&p, who);
	towire_u16(&p, len);
	towire_u8_array(&p, data, len);

	return memcheck(p, tal_count(p));
}
bool fromwire_status_io(const tal_t *ctx, const void *p, enum log_level *iodir, struct node_id **peer, wirestring **who, u8 **data)
{
	u16 len;

	const u8 *cursor = p;
	size_t plen = tal_count(p);

	if (fromwire_u16(&cursor, &plen) != WIRE_STATUS_IO)
		return false;
 	*iodir = fromwire_log_level(&cursor, &plen);
 	if (!fromwire_bool(&cursor, &plen))
		*peer = NULL;
	else {
		*peer = tal(ctx, struct node_id);
		fromwire_node_id(&cursor, &plen, *peer);
	}
 	*who = fromwire_wirestring(ctx, &cursor, &plen);
 	len = fromwire_u16(&cursor, &plen);
 	// 2nd case data
	*data = len ? tal_arr(ctx, u8, len) : NULL;
	fromwire_u8_array(&cursor, &plen, *data, len);
	return cursor != NULL;
}

/* WIRE: STATUS_FAIL */
u8 *towire_status_fail(const tal_t *ctx, enum status_failreason failreason, const wirestring *desc)
{
	u8 *p = tal_arr(ctx, u8, 0);

	towire_u16(&p, WIRE_STATUS_FAIL);
	towire_status_failreason(&p, failreason);
	towire_wirestring(&p, desc);

	return memcheck(p, tal_count(p));
}
bool fromwire_status_fail(const tal_t *ctx, const void *p, enum status_failreason *failreason, wirestring **desc)
{
	const u8 *cursor = p;
	size_t plen = tal_count(p);

	if (fromwire_u16(&cursor, &plen) != WIRE_STATUS_FAIL)
		return false;
 	*failreason = fromwire_status_failreason(&cursor, &plen);
 	*desc = fromwire_wirestring(ctx, &cursor, &plen);
	return cursor != NULL;
}

/* WIRE: STATUS_PEER_CONNECTION_LOST */
u8 *towire_status_peer_connection_lost(const tal_t *ctx)
{
	u8 *p = tal_arr(ctx, u8, 0);

	towire_u16(&p, WIRE_STATUS_PEER_CONNECTION_LOST);

	return memcheck(p, tal_count(p));
}
bool fromwire_status_peer_connection_lost(const void *p)
{
	const u8 *cursor = p;
	size_t plen = tal_count(p);

	if (fromwire_u16(&cursor, &plen) != WIRE_STATUS_PEER_CONNECTION_LOST)
		return false;
	return cursor != NULL;
}

/* WIRE: STATUS_PEER_BILLBOARD */
u8 *towire_status_peer_billboard(const tal_t *ctx, bool perm, const wirestring *happenings)
{
	u8 *p = tal_arr(ctx, u8, 0);

	towire_u16(&p, WIRE_STATUS_PEER_BILLBOARD);
	towire_bool(&p, perm);
	towire_wirestring(&p, happenings);

	return memcheck(p, tal_count(p));
}
bool fromwire_status_peer_billboard(const tal_t *ctx, const void *p, bool *perm, wirestring **happenings)
{
	const u8 *cursor = p;
	size_t plen = tal_count(p);

	if (fromwire_u16(&cursor, &plen) != WIRE_STATUS_PEER_BILLBOARD)
		return false;
 	*perm = fromwire_bool(&cursor, &plen);
 	*happenings = fromwire_wirestring(ctx, &cursor, &plen);
	return cursor != NULL;
}

/* WIRE: STATUS_VERSION */
u8 *towire_status_version(const tal_t *ctx, const wirestring *version)
{
	u8 *p = tal_arr(ctx, u8, 0);

	towire_u16(&p, WIRE_STATUS_VERSION);
	towire_wirestring(&p, version);

	return memcheck(p, tal_count(p));
}
bool fromwire_status_version(const tal_t *ctx, const void *p, wirestring **version)
{
	const u8 *cursor = p;
	size_t plen = tal_count(p);

	if (fromwire_u16(&cursor, &plen) != WIRE_STATUS_VERSION)
		return false;
 	*version = fromwire_wirestring(ctx, &cursor, &plen);
	return cursor != NULL;
}
// SHA256STAMP:3164c82c28124ba916aebd075baa2315cd82cee0d785908da25c6aa6c5b11f22
