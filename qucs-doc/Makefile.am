# Install examples

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/qucs/qucs-doc; \
	for DOC_SUBDIR in report technical tutorial ; do \
		$(MKDIR_P) $(DESTDIR)$(datadir)/qucs/qucs-doc/$$DOC_SUBDIR; \
		for docfile in $$DOC_SUBDIR/*.pdf ; do \
			if test -f $$docfile; then \
				$(INSTALL_DATA) $$docfile $(DESTDIR)$(datadir)/qucs/qucs-doc/$$docfile; \
			fi \ 
		done \
	done
	

