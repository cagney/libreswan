/* SA ID, for libreswan
 *
 * Copyright (C) 2019 Andrew Cagney <cagney@gnu.org>
 * Copyright (C) 2000, 2001  Henry Spencer.
 * Copyright (C) 2012 David McCullough <david_mccullough@mcafee.com>
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

#include "ip_said.h"
#include "ip_info.h"

#include "jambuf.h"

const ip_said unset_said;

ip_said said_from_raw(where_t where UNUSED,
		      const struct ip_info *afi,
		      const struct ip_bytes dst,
		      const struct ip_protocol *protocol,
		      ip_port port,
		      ipsec_spi_t spi)
{
	ip_said said = {
		.ip.is_set = true,
		.ip.version = afi->ip.version,
		.dst = dst,
		.ipproto = protocol->ipproto,
		.spi = spi,
		.hport = port.hport,
	};
	return said;

}

ip_said said_from_address_protocol_spi(const ip_address address,
				       const struct ip_protocol *protocol,
				       ipsec_spi_t spi)
{
	const struct ip_info *afi = address_info(address);
	if (afi == NULL) {
		/* NULL+unset+unknown */
		return unset_said;
	}

	return said_from_raw(HERE, afi, address.bytes,
			     protocol, unset_port, spi);
}

ip_said said_from_address_protocol_port_spi(const ip_address address,
					    const struct ip_protocol *protocol,
					    ip_port port,
					    ipsec_spi_t spi)
{
	const struct ip_info *afi = address_info(address);
	if (afi == NULL) {
		/* NULL+unset+unknown */
		return unset_said;
	}

	return said_from_raw(HERE, afi, address.bytes,
			     protocol, port, spi);
}

bool said_is_unset(const ip_said *said)
{
	return ip_is_unset(said);
}

/*
 * convert SA to text "ah507@1.2.3.4"
 */

size_t jam_said(struct jambuf *buf, const ip_said *said)
{
	const struct ip_info *afi;
	size_t s = jam_invalid_ip(buf, "said", said, &afi);
	if (s > 0) {
		return s;
	}

	const struct ip_protocol *proto = protocol_from_ipproto(said->ipproto);

	if (proto == &ip_protocol_ipip/*TUN*/ &&
	    said->spi == PASSTHROUGHSPI &&
	    /* any zero */ thingeq(said->dst, unset_ip_bytes)) {
		return jam_string(buf, (afi == &ipv4_info ? PASSTHROUGH4NAME :
					afi == &ipv6_info ? PASSTHROUGH6NAME :
					"<unknown-said-version>"));;
	}

	/* general case needed */

	s += jam_string(buf, proto->prefix != NULL ? proto->prefix : proto->name);
	/* .SPI */
	s += jam_char(buf, (afi == &ipv4_info ? '.' :
			    afi == &ipv6_info ? ':' :
			    '?'));
	s += jam(buf, "%x", ntohl(said->spi));;
	s += jam_char(buf, '@');
	s += afi->jam.address(buf, afi, &said->dst);
	return s;
}

const char *str_said(const ip_said *said, said_buf *buf)
{
	struct jambuf b = ARRAY_AS_JAMBUF(buf->buf);
	jam_said(&b, said);
	return buf->buf;
}

const struct ip_info *said_type(const ip_said *said)
{
	/* may return NULL */
	return ip_type(said);
}

const struct ip_info *said_info(const ip_said said)
{
	/* may return NULL */
	return ip_info(said);
}

ip_address said_address(const ip_said said)
{
	const struct ip_info *afi = said_info(said);
	if (afi == NULL) {
		/* NULL+unset+unknown */
		return unset_address; /* empty_address? */
	}

	return address_from_raw(HERE, afi, said.dst);
}

const struct ip_protocol *said_protocol(const ip_said said)
{
	if (said_is_unset(&said)) {
		return NULL;
	}

	return protocol_from_ipproto(said.ipproto);
}
