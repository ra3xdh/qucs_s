<Qucs Schematic 0.0.19>
<Properties>
  <View=-2170,-550,1527,1645,0.762028,1260,667>
  <Grid=10,10,1>
  <DataSet=Test_RATF_EquivCircD.dat>
  <DataDisplay=Test_RATF_EquivCircD.dpl>
  <OpenDisplay=1>
  <Script=Test_RATF_EquivCircD.m>
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
  <IProbe Pr1 1 430 220 -10 -63 0 0>
  <GND * 1 460 610 0 0 0 0>
  <IProbe Pr2 1 550 220 -26 16 0 0>
  <Lib R_ATF_EC1 1 440 400 -243 -88 0 3 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialResPgen" 0 "1k" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "8e-3" 1 "0.3e-3" 1 "8e-3" 1 "6.3e-3" 1 "1.2e-3" 1 "1.6e-3" 1 "9.9" 1 "3.1" 1>
  <.AC AC1 1 -110 230 0 62 0 0 "log" 1 "1 Hz" 1 "1e10" 1 "1001" 1 "no" 0>
  <GND * 1 580 320 0 0 0 0>
  <IProbe Pr3 1 660 220 -26 16 0 0>
  <GND * 1 690 320 0 0 0 0>
  <Lib RFALC1 1 580 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "500" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <Lib RFALC2 1 690 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "200" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <IProbe Pr4 1 770 220 -26 16 0 0>
  <GND * 1 800 320 0 0 0 0>
  <Lib RFALC3 1 800 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "100" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <IProbe Pr5 1 890 220 -26 16 0 0>
  <GND * 1 920 320 0 0 0 0>
  <IProbe Pr6 1 1000 220 -26 16 0 0>
  <GND * 1 1030 320 0 0 0 0>
  <IProbe Pr7 1 1110 220 -26 16 0 0>
  <GND * 1 1140 320 0 0 0 0>
  <Lib RFALC4 1 920 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "50" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <Lib RFALC5 1 1030 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "10" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <Lib RFALC6 1 1140 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "5" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <IProbe Pr8 1 1220 220 -26 16 0 0>
  <GND * 1 1250 320 0 0 0 0>
  <Lib RFALC7 1 1250 260 -42 94 0 1 "/home/mike/.qucs/user_lib/RFLumpedComponents" 0 "RFAxialRes" 0 "1" 1 "0.0" 1 "0.0" 1 "26.58" 1 "26.58" 1 "1.98n" 1 "7.7n" 1 "1.5n" 1 "8.39e-14" 1 "0.194p" 1>
  <Eqn Eqn1 1 30 310 -31 19 0 0 "z1000=v(nr1)/i(vpr1)" 1 "z500=v(nr2)/i(vpr2)" 1 "z200=v(nr3)/i(vpr3)" 1 "z100=v(nr4)/i(vpr4)" 1 "z50=v(nr5)/i(vpr5)" 1 "z10=v(nr6)/i(vpr6)" 1 "z5=v(nr7)/i(vpr7)" 1 "z1=v(nr8)/i(vpr8)" 1 "yes" 0>
</Components>
<Wires>
  <370 220 400 220 "" 0 0 0 "">
  <370 150 370 220 "" 0 0 0 "">
  <270 220 370 220 "" 0 0 0 "">
  <370 150 520 150 "" 0 0 0 "">
  <520 150 520 220 "" 0 0 0 "">
  <460 600 460 610 "" 0 0 0 "">
  <630 150 630 220 "" 0 0 0 "">
  <520 150 630 150 "" 0 0 0 "">
  <740 150 740 220 "" 0 0 0 "">
  <630 150 740 150 "" 0 0 0 "">
  <860 150 860 220 "" 0 0 0 "">
  <970 150 970 220 "" 0 0 0 "">
  <860 150 970 150 "" 0 0 0 "">
  <1080 150 1080 220 "" 0 0 0 "">
  <970 150 1080 150 "" 0 0 0 "">
  <740 150 860 150 "" 0 0 0 "">
  <1190 150 1190 220 "" 0 0 0 "">
  <1080 150 1190 150 "" 0 0 0 "">
  <460 220 460 220 "nR1" 470 180 0 "">
  <580 220 580 220 "nR2" 580 160 0 "">
  <380 420 380 420 "nCp" 320 400 0 "">
  <380 380 380 380 "nLs" 331 370 0 "">
  <380 340 380 340 "nL1" 341 320 0 "">
  <380 475 380 475 "nL2" 331 450 0 "">
  <380 510 380 510 "nCpad" 320 490 0 "">
  <690 220 690 220 "nR3" 690 160 0 "">
  <-2070 -430 -2070 -430 "nCp" -2120 -440 0 "">
  <-2070 -470 -2070 -470 "nLs" -2119 -480 0 "">
  <-2070 -510 -2070 -510 "nL1" -2119 -530 0 "">
  <-2070 -375 -2070 -375 "nL2" -2119 -390 0 "">
  <-2070 -340 -2070 -340 "nCpad" -2130 -320 0 "">
  <800 220 800 220 "nR4" 800 160 0 "">
  <920 220 920 220 "nR5" 920 160 0 "">
  <1030 220 1030 220 "nR6" 1030 160 0 "">
  <1140 220 1140 220 "nR7" 1140 160 0 "">
  <1250 220 1250 220 "nR8" 1250 160 0 "">
</Wires>
<Diagrams>
  <Tab -50 711 838 60 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1011 315 0 225 "" "" "">
	<"ngspice/ac.v(nl1)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(nls)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(ncp)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(nl2)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(ncpad)" #0000ff 0 3 0 0 0>
  </Tab>
  <Rect -70 1331 1377 551 3 #c0c0c0 1 11 1 1 1 1e+10 0 1 1 100000 1 -1 0.2 1 315 0 225 "Frequency (Hz)" "" "">
	<"ngspice/ac.z1000" #0000ff 2 3 0 0 0>
	<"ngspice/ac.z200" #ff0000 2 3 0 0 0>
	<"ngspice/ac.z100" #005500 2 3 0 0 0>
	<"ngspice/ac.z500" #ff00ff 2 3 0 0 0>
	<"ngspice/ac.z50" #aa00ff 2 3 0 0 0>
	<"ngspice/ac.z10" #ffaa00 2 3 0 0 0>
	<"ngspice/ac.z5" #00557f 2 3 0 0 0>
	<"ngspice/ac.z1" #55557f 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
