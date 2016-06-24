

SET(HTML characters.html	internal.html	mathfunc.html	short.html	start_digi.html	subcircuit.html
index.html	matching.html	programs.html	start.html	start_opt.html)

SET(PNGS contab.png		optimization3.png	optimization6.png	paste.png		select.png
optimization1.png	optimization4.png	optimization7.png	qucsdigi.png		subcircuit.png
optimization2.png	optimization5.png	optimization8.png	qucsmain.png		wire.png)

# installation of documentation
INSTALL(FILES ${HTML} ${PNGS} DESTINATION share/${QUCS_NAME}/docs/pt)

