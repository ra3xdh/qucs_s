# Install examples

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/qucs/examples
	if [[ ! -d examples ]] ; then $(MKDIR_P) examples; echo -e "This distribution contains no examples, please copy them manually from the qucs git repository:\ngit clone git://git.code.sf.net/p/qucs/git qucs">examples/README; fi
	for examplefile in examples/* ; do \
		if test -f $$examplefile; then \
			$(INSTALL_DATA) $$examplefile $(DESTDIR)$(datadir)/qucs/$$examplefile; \
		fi \
	done
	

