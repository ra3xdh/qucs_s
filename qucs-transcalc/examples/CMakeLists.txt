
SET(EXAMPLES
hp.trc
ustrip50fr4.trc
ustrip_jansen.trc
cstrip_kj_1.trc
cstrip_kj_2.trc)

# installation of transmission lines
INSTALL( FILES ${EXAMPLES} DESTINATION share/${QUCS_NAME}/tline )

