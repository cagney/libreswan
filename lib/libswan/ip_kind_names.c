/*
 * Libreswan config file parser (keywords.c)
 * Copyright (C) 2003-2006 Michael Richardson <mcr@xelerance.com>
 * Copyright (C) 2007-2010 Paul Wouters <paul@xelerance.com>
 * Copyright (C) 2012 Paul Wouters <paul@libreswan.org>
 * Copyright (C) 2013-2019 Paul Wouters <pwouters@redhat.com>
 * Copyright (C) 2013-2019 D. Hugh Redelmeier <hugh@mimosa.com>
 * Copyright (C) 2013 David McCullough <ucdevel@gmail.com>
 * Copyright (C) 2013-2016 Antony Antony <antony@phenome.org>
 * Copyright (C) 2016-2022 Andrew Cagney
 * Copyright (C) 2017 Mayank Totale <mtotale@gmail.com>
 * Copyright (C) 2020 Yulia Kuzovkova <ukuzovkova@gmail.com>
 * Copyright (C) 2020 Nupur Agrawal <nupur202000@gmail.com>
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

#include "ip_kind.h"
#include "sparse_names.h"

/*
 * XXX: this table is only searched when the KEY starts with %, hence
 * the <> names will never match a lookup-by-name.
 */
const struct sparse_names ip_kind_names = {
	.list = {
		SPARSE("%defaultroute",  IP_KIND_DEFAULTROUTE),
		SPARSE("%any",           IP_KIND_ANY),
		SPARSE("%oppo",          IP_KIND_OPPO),
		SPARSE("%opportunistic", IP_KIND_OPPO),
		SPARSE("%opportunisticgroup", IP_KIND_OPPOGROUP),
		SPARSE("%oppogroup",     IP_KIND_OPPOGROUP),
		SPARSE("%group",         IP_KIND_GROUP),
		SPARSE("%direct",        IP_KIND_DIRECT),
		SPARSE("<hostname>",	 IP_KIND_HOSTNAME),
		SPARSE("<unset>",	 0),
		SPARSE_NULL
	},
};
