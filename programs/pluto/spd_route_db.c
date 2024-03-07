/* Connection database indexed by serialno, for libreswan
 *
 * Copyright (C) 2020 Andrew Cagney <cagney@gnu.org>
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

#include "spd_route_db.h"
#include "log.h"
#include "hash_table.h"
#include "connections.h"

/*
 * SPD_ROUTE database.
 */

static size_t jam_spd_route(struct jambuf *buf, const struct spd_route *sr)
{
	size_t s = 0;
	s += jam_connection(buf, sr->connection);
	s += jam_string(buf, " ");
	s += jam_selector_pair(buf, &sr->local->client, &sr->remote->client);
	return s;
}

static hash_t hash_spd_route_remote_client(const ip_selector *sr)
{
	return hash_thing(sr->bytes, zero_hash);
}

HASH_TABLE(spd_route, remote_client, .remote->client, STATE_TABLE_SIZE);

HASH_DB(spd_route, &spd_route_remote_client_hash_table);

REHASH_DB_ENTRY(spd_route, remote_client, .remote->client);

static struct list_head *spd_route_filter_head(struct spd_route_filter *filter)
{
	/* select list head */
	if (filter->remote_client_range != NULL) {
		selector_buf sb;
		dbg("FOR_EACH_SPD_ROUTE[remote_client_range=%s]... in "PRI_WHERE,
		    str_selector(filter->remote_client_range, &sb), pri_where(filter->where));
		hash_t hash = hash_spd_route_remote_client(filter->remote_client_range);
		return hash_table_bucket(&spd_route_remote_client_hash_table, hash);
	}

	/* else other queries? */
	dbg("FOR_EACH_SPD_ROUTE_... in "PRI_WHERE, pri_where(filter->where));
	return &spd_route_db_list_head;
}

static bool matches_spd_route_filter(struct spd_route *spd, struct spd_route_filter *filter)
{
	if (filter->remote_client_range != NULL &&
	    !selector_range_eq_selector_range(*filter->remote_client_range, spd->remote->client)) {
		return false;
	}
	return true;
}

bool next_spd_route(enum chrono order, struct spd_route_filter *filter)
{
	if (filter->internal == NULL) {
		/*
		 * Advance to first entry of the circular list (if the
		 * list is entry it ends up back on HEAD which has no
		 * data).
		 */
		filter->internal = spd_route_filter_head(filter)->head.next[order];
	}
	/* Walk list until an entry matches */
	filter->spd = NULL;
	for (struct list_entry *entry = filter->internal;
	     list_entry_data(entry) != NULL /* head has DATA == NULL */;
	     entry = entry->next[order]) {
		struct spd_route *spd = list_entry_data(entry);
		if (matches_spd_route_filter(spd, filter)) {
			/* save connection; but step off current entry */
			filter->internal = entry->next[order];
			filter->count++;
			LDBGP_JAMBUF(DBG_BASE, &global_logger, buf) {
				jam_string(buf, "  found ");
				jam_spd_route(buf, spd);
			}
			filter->spd = spd;
			return true;
		}
	}
	dbg("  matches: %d", filter->count);
	return false;
}
