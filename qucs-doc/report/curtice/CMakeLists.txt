
add_latex_document(
    curtice.tex
    INPUTS content.tex ../basics.tex ../reportbook.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/curtice.pdf DESTINATION share/qucs/docs/report)
