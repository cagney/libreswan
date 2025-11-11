/* base of ip structures, for libreswan
 *
 * Copyright (C) 2025  Andrew Cagney.
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
 */

#include "ip_base.h"
#include "jambuf.h"
#include "ip_info.h"
#include "lswlog.h"

size_t jam_ip_invalid(struct jambuf *buf,
		      const char *what,
		      const struct ip_base *ip,
		      const struct ip_info **afi)
{
	(*afi) = NULL;	/* unconfuse gcc-14.3.1-2.el10 */

	if (ip == NULL) {
		return jam(buf, "<null-%s>", what);
	}

	/*
	 * When log-ip=no, give little away; its intended as a backup
	 * for a missing sensitive call.
	 */
	if (ip->tainted && !log_ip) {
		return jam(buf, "<tainted-%s>", what);
	}

	/*
	 * Unset IPs can still have a valid .version.  But truely
	 * unset IPs cannot.
	 */
	(*afi) = ip_version_info(ip->version);
	if ((*afi) == NULL) {
		/* should be unset as well */
		if (ip->is_set) {
			return jam(buf, "<set-yet-unset-%s>", what);
		}
		return jam(buf, "<unset-%s>", what);
	}

	if (!ip->is_set) {
		size_t s = 0;
		s += jam_string(buf, "<unset");
		s += jam_string(buf, "-");
		s += jam_string(buf, (*afi)->ip_name);
		s += jam_string(buf, "-");
		s += jam_string(buf, what);
		s += jam_string(buf, ">");
		return s;
	}

	if ((*afi)->af == AF_UNSPEC) {
		return jam(buf, "<unspecified-%s>", what);
	}

	if (LDBGP(DBG_TAINT, &global_logger) && ip->tainted) {
		return jam(buf, "<%s-%s-%d>",
			   (*afi)->ip_name, what, ip->tainted);
	}

	return 0;
}

size_t jam_ip_sensitive(struct jambuf *buf,
		      const char *what,
		      const struct ip_base *ip,
		      const struct ip_info **afi)
{
	/*
	 * Check for sensitive first; so that tainted only kicks in
	 * when a sensitive call is missing.
	 */
	if (!log_ip) {
		size_t s = 0;
		s += jam_string(buf, "<");
		s += jam_string(buf, what);
		s += jam_string(buf, ">");
		return s;
	}

	size_t s = jam_ip_invalid(buf, what, ip, afi);
	if (s > 0) {
		return s;
	}

	return 0;
}
