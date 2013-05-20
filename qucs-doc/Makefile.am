# Install examples

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/qucs/qucs-doc
	for docfile in *.pdf ; do \
		if test -f $$docfile; then \
			$(INSTALL_DATA) $$docfile $(DESTDIR)$(datadir)/qucs/qucs-doc/$$docfile; \
		fi \
	done
	

