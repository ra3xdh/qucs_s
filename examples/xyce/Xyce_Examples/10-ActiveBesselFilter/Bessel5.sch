<Qucs Schematic 0.0.23>
<Properties>
  <View=0,-120,2760,796,1,1358,0>
  <Grid=10,10,1>
  <DataSet=Bessel5.dat>
  <DataDisplay=Bessel5.dpl>
  <OpenDisplay=0>
  <Script=Bessel5.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Vac V1 1 380 380 18 -26 0 1 "1 V" 1 "1 kHz" 0 "0" 0 "0" 0>
  <GND * 1 380 410 0 0 0 0>
  <GND * 1 510 430 0 0 0 0>
  <GND * 1 670 400 0 0 0 0>
  <OpAmp OP1 1 700 320 -26 -70 1 0 "1e6" 1 "15 V" 0>
  <C C1 1 510 400 17 -26 0 1 "10.000nF" 1 "" 0 "neutral" 0>
  <C C2 1 630 230 17 -26 0 1 "1488.217pF" 1 "" 0 "neutral" 0>
  <R R1 1 510 230 15 -26 0 1 "13.693k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 460 300 -26 15 0 0 "13.693k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 560 300 -26 15 0 0 "6.846k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 1020 430 0 0 0 0>
  <GND * 1 1180 400 0 0 0 0>
  <OpAmp OP2 1 1210 320 -26 -70 1 0 "1e6" 1 "15 V" 0>
  <C C3 1 1020 400 17 -26 0 1 "10.000nF" 1 "" 0 "neutral" 0>
  <C C4 1 1140 230 17 -26 0 1 "3936.111pF" 1 "" 0 "neutral" 0>
  <R R4 1 1020 230 15 -26 0 1 "9.496k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R5 1 970 300 -26 15 0 0 "9.496k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R6 1 1070 300 -26 15 0 0 "4.748k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <OpAmp OP3 1 1610 210 -26 42 0 0 "1e6" 1 "15 V" 0>
  <GND * 1 1510 320 0 0 0 0>
  <GND * 1 1560 420 0 0 0 0>
  <R R7 1 1560 390 15 -26 0 1 "1000.000k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R8 1 1440 240 -75 -52 1 0 "4.364k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R9 1 1650 310 -26 15 1 2 "0.000k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C5 1 1510 290 26 -45 1 1 "10.000nF" 1 "" 0 "neutral" 0>
  <.XYCESCR XYCESCR1 1 950 510 0 51 0 0 ".ac lin 501 1 10K\n.PRINT AC format=raw file=ac.txt V(OUT) {db(V(OUT)/V(IN))}" 1 "" 0 "ac.txt" 0>
</Components>
<Wires>
  <380 300 430 300 "in" 430 270 22 "">
  <380 300 380 350 "" 0 0 0 "">
  <490 300 510 300 "" 0 0 0 "">
  <510 300 530 300 "" 0 0 0 "">
  <510 260 510 300 "" 0 0 0 "">
  <510 300 510 370 "" 0 0 0 "">
  <670 340 670 400 "" 0 0 0 "">
  <510 180 510 200 "" 0 0 0 "">
  <510 180 630 180 "" 0 0 0 "">
  <630 180 630 200 "" 0 0 0 "">
  <590 300 630 300 "" 0 0 0 "">
  <630 300 670 300 "" 0 0 0 "">
  <630 260 630 300 "" 0 0 0 "">
  <630 180 780 180 "" 0 0 0 "">
  <740 320 780 320 "" 0 0 0 "">
  <780 180 780 320 "" 0 0 0 "">
  <940 300 940 320 "" 0 0 0 "">
  <780 320 940 320 "" 0 0 0 "">
  <1000 300 1020 300 "" 0 0 0 "">
  <1020 300 1040 300 "" 0 0 0 "">
  <1020 260 1020 300 "" 0 0 0 "">
  <1020 300 1020 370 "" 0 0 0 "">
  <1180 340 1180 400 "" 0 0 0 "">
  <1020 180 1020 200 "" 0 0 0 "">
  <1020 180 1140 180 "" 0 0 0 "">
  <1140 180 1140 200 "" 0 0 0 "">
  <1100 300 1140 300 "" 0 0 0 "">
  <1140 300 1180 300 "" 0 0 0 "">
  <1140 260 1140 300 "" 0 0 0 "">
  <1140 180 1290 180 "" 0 0 0 "">
  <1250 320 1290 320 "" 0 0 0 "">
  <1290 180 1290 320 "" 0 0 0 "">
  <1320 240 1410 240 "" 0 0 0 "">
  <1320 240 1320 320 "" 0 0 0 "">
  <1290 320 1320 320 "" 0 0 0 "">
  <1470 240 1510 240 "" 0 0 0 "">
  <1650 210 1700 210 "out" 1720 180 39 "">
  <1680 310 1700 310 "" 0 0 0 "">
  <1700 210 1700 310 "" 0 0 0 "">
  <1510 240 1510 260 "" 0 0 0 "">
  <1510 190 1510 240 "" 0 0 0 "">
  <1510 190 1580 190 "" 0 0 0 "">
  <1560 230 1560 310 "" 0 0 0 "">
  <1560 230 1580 230 "" 0 0 0 "">
  <1560 310 1620 310 "" 0 0 0 "">
  <1560 310 1560 360 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 1780 461 680 401 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.2 1 1 -1 0.2 1 315 0 225 "" "K" "">
	<"xyce/ac.{DB(V(OUT)/V(IN))}" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 90 510 12 #000000 0 "The schematic here was produced by the qucs tool "qucsactivefilter."\n\nParameters that generated this were all of the defaults of the qucsactivefilter program, with the exception of the \napproximation type, which was changed to "Bessel."  Once these parameters are set,\none simply presses the "Calculate and copy to clipboard" button, then pastes the contents\nof the clipboard into a new qucs schematic panel. \n\nqucsactive filter will produce a schematic that includes a DC Simulation object, an AC Simulation Object,\nand an Equation block.  Unfortunately, the contents of the Equation block are ignored\nwhen using Xyce as the simulator, because Xyce does not support equation blocks.\n\nThis schematic has been modified to replace the AC Simulation and Equation objects with an\nequivalent Xyce Script object.\n">
</Paintings>
