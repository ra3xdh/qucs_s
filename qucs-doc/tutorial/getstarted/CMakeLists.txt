
add_latex_document(
    getstarted.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/getstarted.pdf DESTINATION share/qucs/docs/tutorial)
