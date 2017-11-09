<Qucs Schematic 0.0.19>
<Properties>
  <View=-43,70,1738,1266,1,43,0>
  <Grid=10,10,1>
  <DataSet=Test_chip_res_basic                                                                .dat>
  <DataDisplay=Test_chip_res_basic                                                                .dpl>
  <OpenDisplay=1>
  <Script=Test_chip_res_basic                                                                .m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 -16 SUB>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
</Symbol>
<Components>
  <S4Q_V V1 1 270 250 -136 -22 0 1 "DC 0.0 AC 1" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 270 280 0 0 0 0>
  <GND * 1 490 440 0 0 0 0>
  <IProbe InR1 1 430 220 -10 -63 0 0>
  <GND * 1 620 360 0 0 0 0>
  <IProbe InR2 1 570 220 -10 -63 0 0>
  <GND * 1 760 360 0 0 0 0>
  <IProbe InR3 1 710 220 -10 -63 0 0>
  <GND * 1 880 360 0 0 0 0>
  <IProbe InR4 1 830 220 -10 -63 0 0>
  <GND * 1 1010 360 0 0 0 0>
  <IProbe InR5 1 960 220 -10 -63 0 0>
  <GND * 1 1150 360 0 0 0 0>
  <IProbe InR6 1 1100 220 -10 -63 0 0>
  <Eqn Eqn1 1 170 520 -72 21 0 0 "Z1=V(nr1)/i(Vinr1)" 1 "Z2=V(nr2)/i(Vinr2)" 1 "Z3=V(nr3)/i(Vinr3)" 1 "Z4=V(nr4)/i(Vinr4)" 1 "Z5=V(nr5)/i(Vinr5)" 1 "Z6=V(nr6)/i(Vinr6)" 1 "yes" 0>
  <.AC AC1 1 110 320 0 61 0 0 "log" 1 "1 Hz" 1 "10e10" 1 "10001" 1 "no" 0>
  <Lib R_chip_EC1 1 470 300 -194 135 0 3 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFChipResPgen" 0 "1k" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.25e-3" 1 "2.0e-3" 1 "0.5e-3" 1>
  <Lib R_CHIP9 1 920 300 -81 100 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFChipRes" 0 "50" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "0.316n" 1 "50.3f" 1>
  <Lib R_CHIP7 1 660 300 -81 100 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFChipRes" 0 "200" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "0.316n" 1 "50.3f" 1>
  <Lib R_CHIP8 1 800 300 -81 100 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFChipRes" 0 "100" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "0.316n" 1 "50.3f" 1>
  <Lib R_CHIP10 1 1050 300 -81 100 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFChipRes" 0 "10" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "0.316n" 1 "50.3f" 1>
  <Lib R_CHIP11 1 1190 300 -81 100 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFChipRes" 0 "1" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "0.316n" 1 "50.3f" 1>
</Components>
<Wires>
  <270 220 370 220 "" 0 0 0 "">
  <460 220 490 220 "" 0 0 0 "">
  <370 220 400 220 "" 0 0 0 "">
  <370 150 370 220 "" 0 0 0 "">
  <370 150 530 150 "" 0 0 0 "">
  <530 150 530 220 "" 0 0 0 "">
  <530 220 540 220 "" 0 0 0 "">
  <600 220 620 220 "" 0 0 0 "">
  <670 150 670 220 "" 0 0 0 "">
  <670 220 680 220 "" 0 0 0 "">
  <740 220 760 220 "" 0 0 0 "">
  <790 150 790 220 "" 0 0 0 "">
  <790 220 800 220 "" 0 0 0 "">
  <860 220 880 220 "" 0 0 0 "">
  <920 150 920 220 "" 0 0 0 "">
  <920 220 930 220 "" 0 0 0 "">
  <990 220 1010 220 "" 0 0 0 "">
  <1060 150 1060 220 "" 0 0 0 "">
  <1060 220 1070 220 "" 0 0 0 "">
  <1130 220 1150 220 "" 0 0 0 "">
  <530 150 670 150 "" 0 0 0 "">
  <670 150 790 150 "" 0 0 0 "">
  <920 150 1060 150 "" 0 0 0 "">
  <790 150 920 150 "" 0 0 0 "">
  <460 220 460 220 "nR1" 470 180 0 "">
  <760 220 760 220 "nR3" 760 180 0 "">
  <880 220 880 220 "nR4" 880 180 0 "">
  <1010 220 1010 220 "nR5" 1010 180 0 "">
  <1150 220 1150 220 "nR6" 1150 180 0 "">
  <410 360 410 360 "nCp" 370 320 0 "">
  <410 280 410 280 "nLs" 361 240 0 "">
  <620 220 620 220 "nR2" 620 180 0 "">
</Wires>
<Diagrams>
  <Tab 431 666 670 52 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 10011 315 0 225 "" "" "">
	<"ngspice/Test_chip_res_basic:ac.v(nls)" #0000ff 0 3 0 0 0>
	<"ngspice/Test_chip_res_basic:ac.v(ncp)" #0000ff 0 6 0 0 0>
  </Tab>
</Diagrams>
<Paintings>
</Paintings>
