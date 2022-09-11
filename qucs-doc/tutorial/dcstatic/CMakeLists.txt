
add_latex_document(
    dcstatic.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dcstatic.pdf DESTINATION share/qucs/docs/tutorial)
