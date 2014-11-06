## Process this file with automake to produce Makefile.in
#
# qucs/diagrams/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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

# rules for translations
TRANSLATIONS = qucs_de.ts qucs_pl.ts qucs_fr.ts qucs_it.ts qucs_ro.ts \
  qucs_pt_BR.ts qucs_pt_PT.ts qucs_es.ts qucs_jp.ts qucs_hu.ts qucs_he.ts \
  qucs_sv.ts qucs_tr.ts qucs_uk.ts qucs_ru.ts qucs_cs.ts qucs_ca.ts \
  qucs_ar.ts qucs_kk.ts
GENTRANSLATIONS = qtgeneric_untranslated.ts qtgeneric_de.ts qtgeneric_fr.ts \
  qtgeneric_es.ts qtgeneric_ro.ts qtgeneric_jp.ts qtgeneric_hu.ts \
  qtgeneric_it.ts qtgeneric_he.ts qtgeneric_sv.ts qtgeneric_pl.ts \
  qtgeneric_tr.ts qtgeneric_uk.ts qtgeneric_ru.ts qtgeneric_cs.ts \
  qtgeneric_ca.ts qtgeneric_ar.ts qtgeneric_kk.ts
TRANSFILES = $(TRANSLATIONS:.ts=.qm)

qucs_.ts: $(qucs_SOURCES)
	@echo "Updating translations...";
	@list='$(TRANSLATIONS)'; for file in $$list; do \
	  $(LUPDATE) `$(PATHXFORM) $(srcdir)/*.cpp $(srcdir)/paintings/*.cpp \
	  $(srcdir)/diagrams/*.cpp $(srcdir)/components/*.cpp \
	  $(srcdir)/dialogs/*.cpp  $(top_srcdir)/qucs-help/*.cpp \
	  $(top_srcdir)/qucs-edit/*.cpp $(top_srcdir)/qucs-filter/*.cpp \
	  $(top_srcdir)/qucs-transcalc/*.cpp $(top_srcdir)/qucs-lib/*.cpp \
	  $(top_srcdir)/qucs-attenuator/*.cpp` \
	  -locations relative -ts $(srcdir)/$$file; \
	done

.ts.qm:
	@qucsfile=$<; \
	genfile=`echo $< | sed -e 's/qucs_\(.*\)\.ts/qtgeneric_\1\.ts/'`; \
	ts_file=`echo $< | sed -e 's/\(.*\)qucs_\(.*\)\.ts/tmp_\2\.ts/'`; \
	qm_file=`echo $< | sed -e 's/\(.*\)qucs_\(.*\)\.ts/tmp_\2\.qm/'`; \
	if test -f $$genfile; then \
	  echo "Incorporating '$$genfile'..."; \
	  grep -v "</TS>" $$qucsfile > $$ts_file; \
	  grep -v "<\!DOCTYPE TS><TS>" $$genfile >> $$ts_file; \
	else \
	  cp -f $$qucsfile $$ts_file; \
	fi; \
	echo "$(LRELEASE) $$qucsfile"; \
	$(LRELEASE) $$ts_file; test -f $$qm_file && mv -f $$qm_file $@; \
	rm -f $$ts_file

# installation of translations
langdatadir = ${prefix}/share/qucs/lang
langdata_DATA = $(TRANSFILES)

CLEANFILES = *.qm
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = ""
