/*
 * Copyright (C) 2014,2016 Andrew Cagney <cagney@gnu.org>
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

struct cbc_test_vector {
	const char *description;
	/* mumble something about algorithm setting here. */
	const char *key;
	const char *iv;
	const char *plaintext;
	const char *ciphertext;
};

extern const struct cbc_test_vector aes_cbc_test_vectors[];
extern const struct cbc_test_vector camellia_cbc_test_vectors[];

bool test_cbc_vectors(const struct ike_alg *alg,
		      struct logger *logger);
