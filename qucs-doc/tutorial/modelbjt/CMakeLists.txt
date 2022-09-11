
add_latex_document(
    modelbjt.tex
    INPUTS content.tex  bfg425s2p.tex bfg425.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/modelbjt.pdf DESTINATION share/qucs/docs/tutorial)
