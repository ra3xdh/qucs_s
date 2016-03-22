

SET(HTML index.html short.html start.html mathfunc.html subcircuit.html
  programs.html internal.html characters.html matching.html start_digi.html
  start_opt.html octave.html)

SET(PNGS qucsmain.png paste.png wire.png select.png contab.png subcircuit.png
  qucsdigi.png optimization1.png optimization2.png optimization3.png
  optimization4.png optimization5.png optimization6.png optimization7.png
  optimization8.png optimization9.png optimization10.png )


# installation of documentation
INSTALL(FILES ${HTML} ${PNGS} DESTINATION share/qucs/docs/en)

