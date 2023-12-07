# Build documentation, for Libreswan
#
# Copyright (C) 2015,2016,2023 Andrew Cagney
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.  See <https://www.gnu.org/licenses/gpl2.txt>.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.

# Note: this .mk file is included by the top-level Makefile,
# manpages.mk is included by sub-Makefile.
#
# Note: In the rules below % is the full path, not just the directory.

MANPAGES = $(shell find programs configs -name '*.[1-8].xml' -print)
DOCUMENTATION = $(patsubst %.xml, $(top_builddir)/%.man, $(MANPAGES))


.PHONY: documentation
documentation:
	$(MAKE) $(DOCUMENTATION)

# Given the file MANPAGE.[0-9].{xml,tmp}, generate a list of
# <refname/> entries, including the section number.

refnames = $(top_srcdir)/packaging/utils/refnames.sh

.PHONY: refnames
refnames:
	for m in $(MANPAGES) ; do \
		for r in $$($(refnames) $$m) ; do \
			n=$$(basename $$m .xml) ; \
			if test "$$n" != "$$r" ; then \
				echo "$$n" "$$r" ; \
			fi \
		done \
	done

.PHONY: suffixes
suffixes:
	echo $(sort $(suffix $(basename $(MANPAGES))))

# Default rule for creating the TRANSFORMED_MANPAGES.

define transform-manpage
	$(TRANSFORM_VARIABLES) -i $(1)

endef

$(top_builddir)/%.man: %.xml | $(top_builddir)/manpages
	mkdir -p $(dir $@)
	$(XMLTO) man $< -o $(top_builddir)/manpages
	$(foreach refname, $(shell $(refnames) $<), $(call transform-manpage, $(top_builddir)/manpages/$(refname)))
	touch $@

$(top_builddir)/manpages:
	mkdir $@

XMLTO ?= xmlto

# $(MANDIR$(suffix $(MANPAGE))) will expand one of the below, roughly:
# 3 is libraries; 5 is file formats; 7 is overviews; 8 is system
# programs.
MANDIR.3 ?= $(MANDIR)/man3
MANDIR.5 ?= $(MANDIR)/man5
MANDIR.7 ?= $(MANDIR)/man7
MANDIR.8 ?= $(MANDIR)/man8

# pre-populate directories needed to install documentation

define install-documentation-mandirs
	set -eu ; mkdir -p $(DESTDIR)$(MANDIR$(1))

endef

# Man pages to build, since the list of generated man pages isn't
# predictable (see refnames.sh) use a fake target to mark that each
# page has been generated.

define install-documentation
	mkdir -p $(DESTDIR)$(MANDIR$(suffix $(1)))
	$(INSTALL) $(INSTMANFLAGS) $(top_builddir)/manpages/$(1) $(DESTDIR)$(MANDIR$(suffix $(1)))

endef

install-documentation: documentation
	$(foreach manpage, $(MANPAGES), \
		$(foreach refname, $(shell $(refnames) $(manpage)), \
			$(call install-documentation,$(refname))))
