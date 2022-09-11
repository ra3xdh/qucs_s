
add_latex_document(
    activefilter.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
    TARGET_NAME activefilter_tut # avoid target collision
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/activefilter.pdf DESTINATION share/qucs/docs/tutorial)
