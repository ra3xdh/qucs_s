# XML component libraries
SET(XML_COMPONENTS
    Lumped_Components.xml
    Probes.xml
    Sources.xml
)

# Install individual files
INSTALL(FILES ${XML_COMPONENTS} DESTINATION share/${QUCS_NAME}/components/)

# Install folders
INSTALL(DIRECTORY Lumped_Component_Symbols Nonlinear_Components_Symbols
        DESTINATION share/${QUCS_NAME}/components/
        FILES_MATCHING PATTERN "*.sym"
)
