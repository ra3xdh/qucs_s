
add_latex_document(
    activefilter.tex
    INPUTS content.tex ../basics.tex ../reportbook.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
    BIBFILES activefilter.bib
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/activefilter.pdf DESTINATION share/qucs/docs/report)
