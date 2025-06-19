/* ip address type, for libreswan
 *
 * Copyright (C) 1998, 1999, 2000  Henry Spencer.
 * Copyright (C) 1999, 2000, 2001  Richard Guy Briggs
 * Copyright (C) 2019 Andrew Cagney <cagney@gnu.org>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <https://www.gnu.org/licenses/lgpl-2.1.txt>.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 */

#ifndef IP_KIND_H
#define IP_KIND_H

/*
 * %kinds, not all may be valid.
 */

enum ip_kind {
	IP_KIND_DEFAULTROUTE = 1,
	IP_KIND_ANY,
	IP_KIND_IFACE,
	IP_KIND_OPPO,
	IP_KIND_OPPOGROUP,
	IP_KIND_GROUP,
	IP_KIND_HOSTNAME,            	/* host_addr invalid, only string */
	IP_KIND_DIRECT,
};

extern const struct sparse_names ip_kind_names;

#endif
