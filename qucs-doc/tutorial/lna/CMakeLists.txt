
add_latex_document(
    lna.tex
    INPUTS content.tex bfg403.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lna.pdf DESTINATION share/qucs/docs/tutorial)
