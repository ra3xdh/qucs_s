
#
# Handle Qucs translations

# TODO
# - evaluate need to merge the qtgeneric_XX.ts into the final .qm (see translations/Makefile.am)

#
# Qucs translations
#
file (GLOB TRANSLATIONS_FILES qucs_*.ts)

# WARNING: make clean might delete the source .ts files! Danger!
option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts" "OFF")

IF (UPDATE_TRANSLATIONS)
  QT6_CREATE_TRANSLATION ( QM_FILES ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
ELSE (UPDATE_TRANSLATIONS)
  QT6_ADD_TRANSLATION ( QM_FILES  ${TRANSLATIONS_FILES})
ENDIF (UPDATE_TRANSLATIONS)

ADD_CUSTOM_TARGET (translations ALL DEPENDS ${QM_FILES})

IF (UPDATE_TRANSLATIONS)
  ADD_CUSTOM_TARGET(translations_src ALL DEPENDS ${Tano_QMS_SRC})
ENDIF (UPDATE_TRANSLATIONS)

#
# Qt translation
#
#FILE(GLOB QtTRANSLATIONS qtgeneric_*.ts)

#
# install translations
INSTALL( FILES ${QM_FILES} DESTINATION share/${QUCS_NAME}/lang)

# prevent the generated files from being deleted during make clean
#set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM true)
