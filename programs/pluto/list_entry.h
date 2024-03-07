/* double linked list, for libreswan
 *
 * Copyright (C) 2015-2019 Andrew Cagney <cagney@gnu.org>
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

#ifndef LIST_ENTRY_H
#define LIST_ENTRY_H

/*
 * Description of a list entry, used for logging.
 */

struct list_info {
	const char *name;
	size_t (*jam)(struct jambuf *buf, const void *data);
	ptrdiff_t offset;
};

#define LIST_INFO(STRUCT, FIELD, INFO, JAM)				\
									\
	static size_t jam_##INFO(struct jambuf *buf, const void *data)	\
	{								\
		const struct STRUCT *s = data;				\
		return JAM(buf, s);						\
	}								\
									\
	static const struct list_info INFO = {				\
		.name = #STRUCT " " #FIELD,				\
		.jam = jam_##INFO,					\
		.offset = offsetof(struct STRUCT, FIELD),		\
	}

/*
 * Double linked list entry.
 *
 * Since these are stored directly in the list object there is less
 * memory management overhead.

 * Each list is represented as a doubly-linked cycle, with a
 * distinguished element we call the head.  The head is the
 * only element with .data == NULL.  This acts as a sentinel.
 *
 * For a list_head.head, before initialization, the list head's .newer and
 * .older must be NULL.  After initialization .newer points to the oldest
 * list_entry (list_head.head, if the list is empty) and .older points
 * to the newest.  This seems contradictory, but it serves to bend time
 * into a cycle.
 *
 * For a regular list_entry, .newer and .older are NULL when the the entry
 * is detached from any list.  Otherwise both must be non-NULL.
 *
 * Currently all elements of a list must have identical .info values.
 * This could easily be changed if we needed heterogeneous lists.
 */

enum chrono {
	OLD2NEW, NEW2OLD,
};

#define CHRONO_ROOF 2

struct list_entry {
	struct list_entry *next[CHRONO_ROOF];
	const struct list_info *info;
};

void jam_list_entry(struct jambuf *buf, const struct list_entry *entry);

/*
 * Double linked list HEAD.
 *
 * INIT_LIST_HEAD() is intended for static structures.  To use it at
 * runtime, prefix the macro with (struct list_head).
 */

struct list_head {
	const struct list_info *info;
	struct list_entry head;
};

#define INIT_LIST_HEAD(LIST_HEAD_PTR, LIST_INFO_PTR)			\
	{								\
		.head = {						\
			.next[NEW2OLD] = &(LIST_HEAD_PTR)->head,	\
			.next[OLD2NEW] = &(LIST_HEAD_PTR)->head,	\
		},							\
		.info = (LIST_INFO_PTR),				\
	}

/*
 * detached_list_entry: test whether an entry is on any list.
 * insert_list_entry: Insert an entry at the front of the list.
 * remove_list_entry: remove the entry from anywhere in the list.
 * The macros *OLD2NEW() and *NEW2OLD(), below, expose the ordering.
 *
 * These operations are O(1).
 */

void init_list_entry(const struct list_info *info, void *data, struct list_entry *entry);

struct list_entry *data_list_entry(const struct list_info *info, void *data);
void *list_entry_data(const struct list_entry *entry);

bool detached_list_entry(const struct list_entry *entry);

void insert_list_entry(struct list_head *list,
		       struct list_entry *entry);
void remove_list_entry(struct list_entry *entry);

/*
 * Iterate through all the entries in the list in either old-to-new or
 * new-to-old order.
 *
 * So that the current entry can be deleted, the E##entry pointer
 * advanced before entering the loop body.
 *
 * Since a non-empty list loops back to HEAD, HEAD's .info==NULL acts
 * as the sentinel.  When this is reached, DATA is left with a NULL
 * value.
 */

#define FOR_EACH_LIST_ENTRY_(DATA, HEAD, NEXT)				\
									\
	/* entry = either first entry or back at head (.info==NULL) */	\
	for (struct list_entry *entry_ = (HEAD)->head.next[NEXT];	\
	     entry_ != NULL && entry_->info != NULL; entry_ = NULL)	\
									\
		/* DATA = ENTRY->data; ENTRY = ENTRY->NEXT */		\
		for (DATA = list_entry_data(entry_),			\
			     entry_ = entry_->next[NEXT];		\
		     DATA != NULL;					\
		     DATA = (entry_->info != NULL ? list_entry_data(entry_) : NULL), \
			     entry_ = entry_->next[NEXT])

#define FOR_EACH_LIST_ENTRY_OLD2NEW(DATA, HEAD)		\
	FOR_EACH_LIST_ENTRY_(DATA, HEAD, OLD2NEW)

#define FOR_EACH_LIST_ENTRY_NEW2OLD(DATA, HEAD)		\
	FOR_EACH_LIST_ENTRY_(DATA, HEAD, NEW2OLD)

#define NEXT_LIST_ENTRY(LIST, DATA, FIELD, NEXT)			\
	({								\
		struct list_entry *next_;				\
		if (DATA == NULL) {					\
			next_ = (LIST)->head.next[NEXT];		\
		} else {						\
			next_ = DATA->FIELD.next[NEXT];			\
		}							\
		(typeof(DATA)) (next_ == NULL ? NULL : list_entry_data(next_)); \
	})

#endif
