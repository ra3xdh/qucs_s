## Process this file with automake to produce Makefile.in
# 
# doxygen/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.  
#

EXTRA_DIST = doxygen_main_page.c doxygen.cfg.in 

CLEANFILES = doxygen.cfg doxygen.log *~ *.rej *.orig
MAINTAINERCLEANFILES = Makefile.in *.pdf
DISTCLEANFILES = *.ps *.pdf doxygen.cfg doxygen.log

#docdir = ${prefix}/share/qucs/apidoc

all:

doxygen: doxygen.cfg
	@rm -rf html refman.pdf
	$(DOXYGEN) doxygen.cfg
#	$(MAKE) -C latex/ 
#	mv latex/refman.pdf ./refman.pdf

maintainer-clean-local:
	rm -rf html latex

#install-data-hook:
#	$(mkinstalldirs) "$(DESTDIR)$(docdir)/html"
#	-test -d "$(srcdir)/html" && \
#	cp -rp "$(srcdir)/html" "$(DESTDIR)$(docdir)"

#uninstall-hook:
#	rm -rf ${docdir}/html

## We borrow guile's convention and use @-...-@ as the substitution
## brackets here, instead of the usual @...@.  This prevents autoconf
## from substituting the values directly into the left-hand sides of
## the sed substitutions.
doxygen.cfg: doxygen.cfg.in Makefile
	rm -f $@.tmp
	sed < $< > $@.tmp \
            -e 's:@-top_srcdir-@:${top_srcdir}:g' \
            -e 's:@-enable_dot-@:${enable_dot}:g' \
            -e 's:@-version-@:${VERSION}:g' \
            -e 's:@-DOT-@:${DOT}:g'
	mv $@.tmp $@
