
add_subdirectory(activefilter)
add_subdirectory(bjtbias)
add_subdirectory(coupler)
add_subdirectory(dcstatic)
add_subdirectory(digital)
add_subdirectory(equations)
add_subdirectory(ffmodels)
add_subdirectory(functions)
add_subdirectory(getstarted)
add_subdirectory(linblocks)
add_subdirectory(lna)
add_subdirectory(modelbjt)
add_subdirectory(opamp)
add_subdirectory(poweramp)
add_subdirectory(RFResModels)
add_subdirectory(sparameteramp)
add_subdirectory(spicetoqucs)
add_subdirectory(textmode)
add_subdirectory(timer555)

add_latex_document(
    workbook.tex
    INPUTS titlepage.tex basics.tex intro.tex general.tex datasheet.tex
    DEPENDS
        getstarted
        datasheet.tex
        dcstatic
        digital
        ffmodels
        opamp
        timer555
        spicetoqucs
        bjtbias
        modelbjt
        poweramp
        lna
        sparameteramp
        coupler
        functions
        equations
        RFResModels
        textmode
        activefilter_tut
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/workbook.pdf DESTINATION share/qucs/docs/tutorial)
