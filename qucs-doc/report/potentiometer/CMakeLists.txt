
add_latex_document(
    potentiometer.tex
    INPUTS content.tex ../basics.tex ../reportbook.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/potentiometer.pdf DESTINATION share/qucs/docs/report)
