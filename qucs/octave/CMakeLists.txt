# octave scripts

SET(BASICS
getQucsVariable.m	plotCartesian2D.m	plotFFT2V.m
loadQucsDataSet.m	plotCartesian2D2V.m	plotTimeFFT.m
loadQucsVariable.m	plotFFT.m		showQucsDataSet.m
)

INSTALL(FILES ${BASICS} DESTINATION share/${QUCS_NAME}/octave/)

