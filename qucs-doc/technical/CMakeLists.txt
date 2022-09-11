

set(FIGS
    acstrafo.fig
    actrafo.fig
    beuler.fig
    biast.fig
    bondwire.fig
    bridgecircuit.fig
    cccs.fig
    ccvs.fig
    coax.fig
    convprob1.fig
    convprob2.fig
    convprob3.fig
    coupler.fig
    cpconform.fig
    cpgap.fig
    cpline.fig
    cpopen.fig
    cpscheme.fig
    cpshort.fig
    cpstep.fig
    dcalgo.fig
    dcdiode.fig
    dcjfet.fig
    dcmosfet.fig
    dcsgp.fig
    dcsgp_spice.fig
    differential.fig
    diode.fig
    feuler.fig
    fft.fig
    fourportab.fig
    gyrator.fig
    hb_concept.fig
    hbn_concept.fig
    jfet.fig
    lssp.fig
    mosband.fig
    mosfet.fig
    mosphysical.fig
    mostypes.fig
    mscorner.fig
    mscoupled.fig
    mscoupledphys.fig
    mscross.fig
    msgap.fig
    msgapphys.fig
    msline.fig
    mstee.fig
    mstee2.fig
    msvia.fig
    nconnect.fig
    nconnect2.fig
    nconnect3.fig
    newton.fig
    newtonbad.fig
    newtondiode.fig
    nitrafo.fig
    noisediode.fig
    noisejfet.fig
    noisemosfet.fig
    noisesgp.fig
    opamp.fig
    picircuit.fig
    sconnectgraph.fig
    sconnectloop.fig
    sconnectpath.fig
    sgp.fig
    siconnectgraph.fig
    siconnectloop.fig
    siconnectpath.fig
    sparam.fig
    spdiode.fig
    spjfet.fig
    spmosfet.fig
    spnfile.fig
    spsgp.fig
    spsgp_spice.fig
    stiffstable.fig
    tcircuit.fig
    tline.fig
    tline4p.fig
    transcap.fig
    transind.fig
    trapez.fig
    twisted.fig
    twoportab.fig
    twoportcascade.fig
    twoportch.fig
    twoportiv.fig
    twoportpp.fig
    twoportps.fig
    twoportsp.fig
    twoportss.fig
    vam.fig
    vccs.fig
    vcvs.fig
    vpm.fig
    xcircuit.fig
)

set(PSFILES
    CYexample.eps
    CascadedLCsch.eps
    CircularLoop.eps
    DoubleStubOpen.eps
    DoubleStubOpenBalanced.eps
    DoubleStubShort.eps
    DoubleStubShortBalanced.eps
    Lsection.eps
    MNAexample.eps
    MNAnoise1.eps
    MNAnoise2.eps
    MNAsingular.eps
    NLexample.eps
    PlanarSpiralInductors.eps
    PrintedInductorEC.eps
    QuarterWavelengthFilter.eps
    Real_c.eps
    Real_ec.eps
    Real_r.eps
    ReflectionCoeffsTaper.eps
    SingleStubOpen.eps
    SingleStubOpenBalanced.eps
    SingleStubShort.eps
    SingleStubShortBalanced.eps
    SmithCascadedLCsections.eps
    SmithChart-Lsection.eps
    SmithChartTL.eps
    Smithl8l4.eps
    TLpiece.eps
    Unoise.eps
    capq.eps
    circularwaveguide.eps
    indq.eps
    rectwaveguide.eps
    )

set(CHAPTERS
    acanalysis.tex
    acnoise.tex
    coplanar.tex
    dcanalysis.tex
    devices.tex
    grammar-common.tex
    grammar-data.tex
    grammar-qucs.tex
    hbanalysis.tex
    hbnoise.tex
    lineardevices.tex
    maths.tex
    microstrip.tex
    noisewaves.tex
    sparameter.tex
    synthesis.tex
    transient.tex
    transline.tex
    )


# generate .fig --> .eps
set(fig_gen)
foreach(file ${FIGS})
  get_filename_component(_name ${file} NAME_WE)
  set(infile  "${CMAKE_CURRENT_SOURCE_DIR}/${file}" )
  set(outfile "${_name}.eps" )
  add_custom_command(
     OUTPUT ${outfile}
     COMMAND fig2dev -L eps ${infile} ${outfile}
     DEPENDS ${infile})
  list(APPEND fig_gen ${outfile})
endforeach()


# generate figures from python script
set(pyFigs
    coplanarzl.pdf
    mscomparezl.pdf
    mszl.pdf
    radius.pdf
    stabbashford.pdf
    stabgear.pdf
    stabgearex.pdf
    stabmoulton.pdf)

add_custom_command(
   OUTPUT ${pyFigs}
   COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/figures.py
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/figures.py)

# Compile LaTeX document
# If fig2eps and m2eps were known formatds they could be on IMAGE.
# Put then now as dependency to force the conversion to eps.
add_latex_document(
  technical.tex
  INPUTS ${CHAPTERS}
  IMAGES ${PSFILES}
  BIBFILES literature.bib
  DEPENDS ${fig_gen} ${pyFigs}
)

# Handle install
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/technical.pdf DESTINATION share/qucs/docs/technical)
