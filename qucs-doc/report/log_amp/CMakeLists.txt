
add_latex_document(
    log_amp.tex
    INPUTS content.tex ../basics.tex ../reportbook.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/log_amp.pdf DESTINATION share/qucs/docs/report)
