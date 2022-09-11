
add_latex_document(
    spicetoqucs.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spicetoqucs.pdf DESTINATION share/qucs/docs/tutorial)
