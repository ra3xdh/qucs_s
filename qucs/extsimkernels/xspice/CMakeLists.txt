
SET(RULES_MK
cmlib.linux.rules.mk
cmlib.mingw32.rules.mk
)

# installation of transmission lines
INSTALL( FILES ${RULES_MK} DESTINATION share/${QUCS_NAME}/xspice_cmlib )
INSTALL(DIRECTORY include DESTINATION share/${QUCS_NAME}/xspice_cmlib )

