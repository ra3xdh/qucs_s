include_directories(${qucs-core_SOURCE_DIR} ${qucs-core_CURRENT_SOURCE_DIR}
                    ${qucs-core_SOURCE_DIR}/src/math)

set(COMPONENTS_SRC
    amplifier.cpp
    attenuator.cpp
    biastee.cpp
    capacitor.cpp
    capq.cpp
    cccs.cpp
    ccvs.cpp
    circline.cpp
    circulator.cpp
    coaxline.cpp
    coupler.cpp
    cross.cpp
    ctline.cpp
    dcblock.cpp
    dcfeed.cpp
    ecvs.cpp
    taperedline.cpp
    ground.cpp
    gyrator.cpp
    hybrid.cpp
    iac.cpp
    idc.cpp
    iexp.cpp
    ifile.cpp
    iinoise.cpp
    inductor.cpp
    indq.cpp
    inoise.cpp
    iprobe.cpp
    ipulse.cpp
    irect.cpp
    isolator.cpp
    itrafo.cpp
    ivnoise.cpp
    mutual.cpp
    mutual2.cpp
    mutualx.cpp
    opamp.cpp
    open.cpp
    pac.cpp
    phaseshifter.cpp
    rectline.cpp
    relais.cpp
    resistor.cpp
    rfedd.cpp
    rlcg.cpp
    short.cpp
    spfile.cpp
    spembed.cpp
    spdeembed.cpp
    strafo.cpp
    tee.cpp
    tline.cpp
    tline4p.cpp
    trafo.cpp
    tswitch.cpp
    twistedpair.cpp
    vac.cpp
    vam.cpp
    vccs.cpp
    vcvs.cpp
    vdc.cpp
    vexp.cpp
    vfile.cpp
    vnoise.cpp
    vpm.cpp
    vprobe.cpp
    vpulse.cpp
    vrect.cpp
    vvnoise.cpp
    wprobe.cpp)

set(HEADERS component.h component_id.h components.h)

add_library(coreComponents OBJECT ${COMPONENTS_SRC})

install(FILES ${HEADERS} DESTINATION include/qucs-core)
