
add_latex_document(
    timer555.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/timer555.pdf DESTINATION share/qucs/docs/tutorial)
