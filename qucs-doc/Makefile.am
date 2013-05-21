# Install examples

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/qucs/docs; \
	for DOC_SUBDIR in report technical tutorial ; do \
		$(MKDIR_P) $(DESTDIR)$(datadir)/qucs/docs/$$DOC_SUBDIR; \
		for docfile in $$DOC_SUBDIR/*.pdf ; do \
			if test -f $$docfile; then \
				$(INSTALL_DATA) $$docfile $(DESTDIR)$(datadir)/qucs/docs/$$docfile; \
			fi \ 
		done \
	done
	

