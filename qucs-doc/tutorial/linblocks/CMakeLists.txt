
add_latex_document(
    linblocks.tex
    INPUTS ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
    IMAGES theproblem.eps
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/linblocks.pdf DESTINATION share/qucs/docs/tutorial)
