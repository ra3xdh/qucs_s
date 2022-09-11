
add_latex_document(
    functions.tex
    INPUTS content.tex appendix.ist math.tex use.tex electronics.tex intro.tex syntaxoverview.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/functions.pdf DESTINATION share/qucs/docs/tutorial)
