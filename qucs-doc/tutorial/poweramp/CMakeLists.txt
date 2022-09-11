
add_latex_document(
    poweramp.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/poweramp.pdf DESTINATION share/qucs/docs/tutorial)
