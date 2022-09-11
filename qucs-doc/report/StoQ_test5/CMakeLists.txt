
add_latex_document(
    StoQ_test5.tex
    INPUTS content.tex ../basics.tex ../reportbook.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/StoQ_test5.pdf DESTINATION share/qucs/docs/report)
