
add_latex_document(
    bjtbias.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bjtbias.pdf DESTINATION share/qucs/docs/tutorial)
