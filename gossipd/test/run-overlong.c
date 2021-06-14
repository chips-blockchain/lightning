#include "../routing.c"
#include "../gossip_store.c"
#include <common/json_stream.h>
#include <common/setup.h>
#include <stdio.h>

void status_fmt(enum log_level level UNUSED,
		const struct node_id *node_id,
		const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
}

/* AUTOGENERATED MOCKS START */
/* Generated stub for cupdate_different */
bool cupdate_different(struct gossip_store *gs UNNEEDED,
		       const struct half_chan *hc UNNEEDED,
		       const u8 *cupdate UNNEEDED)
{ fprintf(stderr, "cupdate_different called!\n"); abort(); }
/* Generated stub for fmt_wireaddr_without_port */
char *fmt_wireaddr_without_port(const tal_t *ctx UNNEEDED, const struct wireaddr *a UNNEEDED)
{ fprintf(stderr, "fmt_wireaddr_without_port called!\n"); abort(); }
/* Generated stub for fromwire_wireaddr_array */
struct wireaddr *fromwire_wireaddr_array(const tal_t *ctx UNNEEDED, const u8 *ser UNNEEDED)
{ fprintf(stderr, "fromwire_wireaddr_array called!\n"); abort(); }
/* Generated stub for json_add_member */
void json_add_member(struct json_stream *js UNNEEDED,
		     const char *fieldname UNNEEDED,
		     bool quote UNNEEDED,
		     const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "json_add_member called!\n"); abort(); }
/* Generated stub for json_member_direct */
char *json_member_direct(struct json_stream *js UNNEEDED,
			 const char *fieldname UNNEEDED, size_t extra UNNEEDED)
{ fprintf(stderr, "json_member_direct called!\n"); abort(); }
/* Generated stub for json_object_end */
void json_object_end(struct json_stream *js UNNEEDED)
{ fprintf(stderr, "json_object_end called!\n"); abort(); }
/* Generated stub for json_object_start */
void json_object_start(struct json_stream *ks UNNEEDED, const char *fieldname UNNEEDED)
{ fprintf(stderr, "json_object_start called!\n"); abort(); }
/* Generated stub for memleak_add_helper_ */
void memleak_add_helper_(const tal_t *p UNNEEDED, void (*cb)(struct htable *memtable UNNEEDED,
						    const tal_t *)){ }
/* Generated stub for nannounce_different */
bool nannounce_different(struct gossip_store *gs UNNEEDED,
			 const struct node *node UNNEEDED,
			 const u8 *nannounce UNNEEDED)
{ fprintf(stderr, "nannounce_different called!\n"); abort(); }
/* Generated stub for notleak_ */
void *notleak_(const void *ptr UNNEEDED, bool plus_children UNNEEDED)
{ fprintf(stderr, "notleak_ called!\n"); abort(); }
/* Generated stub for peer_supplied_good_gossip */
void peer_supplied_good_gossip(struct peer *peer UNNEEDED, size_t amount UNNEEDED)
{ fprintf(stderr, "peer_supplied_good_gossip called!\n"); abort(); }
/* Generated stub for private_channel_announcement */
const u8 *private_channel_announcement(const tal_t *ctx UNNEEDED,
				       const struct short_channel_id *scid UNNEEDED,
				       const struct node_id *local_node_id UNNEEDED,
				       const struct node_id *remote_node_id UNNEEDED,
				       const u8 *features UNNEEDED)
{ fprintf(stderr, "private_channel_announcement called!\n"); abort(); }
/* Generated stub for sanitize_error */
char *sanitize_error(const tal_t *ctx UNNEEDED, const u8 *errmsg UNNEEDED,
		     struct channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "sanitize_error called!\n"); abort(); }
/* Generated stub for status_failed */
void status_failed(enum status_failreason code UNNEEDED,
		   const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "status_failed called!\n"); abort(); }
/* Generated stub for towire_warningfmt */
u8 *towire_warningfmt(const tal_t *ctx UNNEEDED,
		      const struct channel_id *channel UNNEEDED,
		      const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "towire_warningfmt called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

#if DEVELOPER
/* Generated stub for memleak_remove_htable */
void memleak_remove_htable(struct htable *memtable UNNEEDED, const struct htable *ht UNNEEDED)
{ fprintf(stderr, "memleak_remove_htable called!\n"); abort(); }
/* Generated stub for memleak_remove_intmap_ */
void memleak_remove_intmap_(struct htable *memtable UNNEEDED, const struct intmap *m UNNEEDED)
{ fprintf(stderr, "memleak_remove_intmap_ called!\n"); abort(); }
#endif

/* NOOP for new_reltimer_ */
struct oneshot *new_reltimer_(struct timers *timers UNNEEDED,
			      const tal_t *ctx UNNEEDED,
			      struct timerel expire UNNEEDED,
			      void (*cb)(void *) UNNEEDED, void *arg UNNEEDED)
{
	return NULL;
}

static void node_id_from_privkey(const struct privkey *p, struct node_id *id)
{
	struct pubkey k;
	pubkey_from_privkey(p, &k);
	node_id_from_pubkey(id, &k);
}

#define NUM_NODES (ROUTING_MAX_HOPS + 1)

/* We create an arrangement of nodes, each node N connected to N+1 and
 * to node 1.  The cost for each N to N+1 route is 1, for N to 1 is
 * 2^N.  That means it's always cheapest to go the longer route */
int main(int argc, char *argv[])
{
	struct routing_state *rstate;
	struct node_id ids[NUM_NODES];
	struct chan **route;
	struct amount_msat last_fee;

	common_setup(argv[0]);

	for (size_t i = 0; i < NUM_NODES; i++) {
		struct privkey tmp;
		memset(&tmp, i+1, sizeof(tmp));
		node_id_from_privkey(&tmp, &ids[i]);
	}
	/* We are node 0 */
	rstate = new_routing_state(tmpctx, &ids[0], NULL, NULL, NULL,
				   false, false);

	for (size_t i = 0; i < NUM_NODES; i++) {
		struct chan *chan;
		struct half_chan *hc;
		struct short_channel_id scid;

		new_node(rstate, &ids[i]);

		if (i == 0)
			continue;
		if (!mk_short_channel_id(&scid, i, i-1, 0))
			abort();
		chan = new_chan(rstate, &scid, &ids[i], &ids[i-1],
				AMOUNT_SAT(1000000), NULL);

		hc = &chan->half[node_id_idx(&ids[i-1], &ids[i])];
		hc->bcast.index = 1;
		hc->base_fee = 1;
		hc->proportional_fee = 0;
		hc->delay = 0;
		hc->channel_flags = node_id_idx(&ids[i-1], &ids[i]);
		hc->htlc_minimum = AMOUNT_MSAT(0);
		hc->htlc_maximum = AMOUNT_MSAT(1000000 * 1000);
		SUPERVERBOSE("Joining %s to %s, fee %u",
			     type_to_string(tmpctx, struct node_id, &ids[i-1]),
			     type_to_string(tmpctx, struct node_id, &ids[i]),
			     (int)hc->base_fee);

		if (i <= 2)
			continue;
		if (!mk_short_channel_id(&scid, i, 1, 0))
			abort();
		chan = new_chan(rstate, &scid, &ids[i], &ids[1],
				AMOUNT_SAT(1000000), NULL);
		hc = &chan->half[node_id_idx(&ids[1], &ids[i])];
		hc->bcast.index = 1;
		hc->base_fee = 1 << i;
		hc->proportional_fee = 0;
		hc->delay = 0;
		hc->channel_flags = node_id_idx(&ids[1], &ids[i]);
		hc->htlc_minimum = AMOUNT_MSAT(0);
		hc->htlc_maximum = AMOUNT_MSAT(1000000 * 1000);
		SUPERVERBOSE("Joining %s to %s, fee %u",
			     type_to_string(tmpctx, struct node_id, &ids[1]),
			     type_to_string(tmpctx, struct node_id, &ids[i]),
			     (int)hc->base_fee);
	}

	for (size_t i = ROUTING_MAX_HOPS; i > 1; i--) {
		struct amount_msat fee;
		SUPERVERBOSE("%s -> %s:",
			     type_to_string(tmpctx, struct node_id, &ids[0]),
			     type_to_string(tmpctx, struct node_id, &ids[NUM_NODES-1]));

		route = find_route(tmpctx, rstate, &ids[0], &ids[NUM_NODES-1],
				   AMOUNT_MSAT(1000), 0, 0.0, NULL,
				   i, &fee);
		assert(route);
		assert(tal_count(route) == i);
		if (i != ROUTING_MAX_HOPS)
			assert(amount_msat_greater(fee, last_fee));
		last_fee = fee;
	}

	common_shutdown();
	return 0;
}
