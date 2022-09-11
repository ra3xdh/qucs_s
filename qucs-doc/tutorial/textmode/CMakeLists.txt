
# workaround figures with same name
set(PICS
    ./pics/rc_ac.eps
    ./pics/rc_ac_circuit.eps
    #./pics/rc_ac_circuit.svg
    ./pics/rc_ac_python.eps
    ./pics/rc_ac_sweep_python.eps
    ./pics/rc_ac_tr.eps
)

add_latex_document(
    textmode.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    #IMAGE_DIRS pics
    IMAGES ${PICS}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/textmode.pdf DESTINATION share/qucs/docs/tutorial)
