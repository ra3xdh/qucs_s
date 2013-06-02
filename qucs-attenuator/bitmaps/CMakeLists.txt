
SET(XPMS
)

# toolbar images
SET(PNGS
att_pi.png
att_tee.png
att_bridge.png
)

# application images
SET(ICONS
)

INSTALL(FILES ${XPMS} DESTINATION shared/qucs/bitmaps)
INSTALL(FILES ${PNGS} DESTINATION shared/qucs/bitmaps)
INSTALL(FILES ${ICONS} DESTINATION shared/qucs/bitmaps)

