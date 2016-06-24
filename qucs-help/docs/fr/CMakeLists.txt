

SET(HTML index.html	mathfunc.html	short.html	start.html	subcircuit.html)

SET(PNGS contab.png	paste.png	qucsmain.png	select.png	subcircuit.png	wire.png)

# installation of documentation
INSTALL(FILES ${HTML} ${PNGS} DESTINATION share/${QUCS_NAME}/docs/fr)

