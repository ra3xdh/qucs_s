
set(FIGS
    ./pics/mcoupled.fig
)

# convert .fig --> .eps
foreach(fig ${FIGS})
  set(infile "${CMAKE_CURRENT_SOURCE_DIR}/${fig}" )
  string(REGEX REPLACE ".fig" "" baseName ${fig})
  set(outfile "${CMAKE_CURRENT_BINARY_DIR}/${baseName}.eps" )
  add_custom_command(OUTPUT ${outfile}
     COMMAND fig2dev -L eps ${infile} ${outfile}
     DEPENDS ${infile})
  set(fig2eps ${fig2eps} ${outfile})
endforeach()

add_latex_document(
    coupler.tex
    INPUTS content.tex ../basics.tex ../titlepage.tex
    INCLUDE_DIRECTORIES ..
    IMAGE_DIRS pics
    DEPENDS ${fig2eps}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/coupler.pdf DESTINATION share/qucs/docs/tutorial)
