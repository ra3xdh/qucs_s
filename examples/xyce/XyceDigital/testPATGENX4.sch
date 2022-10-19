<Qucs Schematic 0.0.24>
<Properties>
  <View=-87,10,1480,1380,1,0,288>
  <Grid=10,10,1>
  <DataSet=testPATGENX4.dat>
  <DataDisplay=testPATGENX4.dpl>
  <OpenDisplay=0>
  <Script=testPATGENX4.m>
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
  <.TR TR1 1 1070 230 0 77 0 0 "lin" 1 "0" 1 "0.2m" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <SpLib X1 1 140 160 -112 96 0 0 "C:/Program Files (x86)/Qucs-S/share/qucs-s/library/XyceDigital.lib" 0 "PATGENX8" 1 "auto" 1 "PulseFreq=10000.0 ScaleFactor=5" 1>
  <SpLib X2 1 340 170 20 88 0 0 "C:/Program Files (x86)/Qucs-S/share/qucs-s/library/XyceDigital.lib" 0 "A2DBRIDGEX4" 1 "auto" 1 "ScaleFactor=5" 1>
  <GND * 1 580 290 0 0 0 0>
  <GND * 1 660 290 0 0 0 0>
  <GND * 1 740 290 0 0 0 0>
  <GND * 1 820 290 0 0 0 0>
  <R_SPICE R1 1 580 250 15 -26 0 1 "4.7k" 1 "" 0 "" 0 "" 0 "" 0>
  <R_SPICE R2 1 660 250 15 -26 0 1 "4.7k" 1 "" 0 "" 0 "" 0 "" 0>
  <R_SPICE R3 1 740 250 15 -26 0 1 "4.7k" 1 "" 0 "" 0 "" 0 "" 0>
  <R_SPICE R4 1 820 250 15 -26 0 1 "4.7k" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <180 220 280 220 "" 0 0 0 "">
  <180 200 280 200 "nA2" 230 180 36 "">
  <180 180 280 180 "nA1" 230 160 20 "">
  <180 160 280 160 "" 0 0 0 "">
  <580 280 580 290 "" 0 0 0 "">
  <660 280 660 290 "" 0 0 0 "">
  <820 280 820 290 "" 0 0 0 "">
  <740 280 740 290 "" 0 0 0 "">
  <180 160 180 160 "nA0" 210 140 0 "">
  <180 220 180 220 "nA3" 210 200 0 "">
  <400 160 400 160 "nD0" 430 130 0 "">
  <400 180 400 180 "nD1" 430 150 0 "">
  <400 200 400 200 "nD2" 430 170 0 "">
  <400 220 400 220 "nD3" 430 190 0 "">
  <580 220 580 220 "nD0" 610 190 0 "">
  <660 220 660 220 "nD1" 690 190 0 "">
  <820 220 820 220 "nD3" 850 190 0 "">
  <740 220 740 220 "nD2" 770 190 0 "">
</Wires>
<Diagrams>
  <Rect 130 579 1109 59 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(ND1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 452 1108 60 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(ND0)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 699 1118 60 3 #c0c0c0 1 00 1 0 5e-05 0.001 0 0 1 1 1 -1 2 1 315 0 225 "" "" "">
	<"xyce/tran.V(ND2)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 818 1124 68 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(ND3)" #0000ff 2 3 0 0 0>
  </Rect>
  <Time 130 1143 1127 274 3 #c0c0c0 1 00 1 0 1 13 1 0 1 1 1 0 1 950 315 0 225 "" "" "">
	<"xyce/tran.V(NA0)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(NA1)" #ff0000 0 3 0 0 0>
	<"xyce/tran.V(NA2)" #ff00ff 0 3 0 0 0>
	<"xyce/tran.V(NA3)" #00ff00 0 3 0 0 0>
	<"xyce/tran.V(ND0)" #00ffff 0 3 0 0 0>
	<"xyce/tran.V(ND1)" #ffff00 0 3 0 0 0>
	<"xyce/tran.V(ND2)" #777777 0 3 0 0 0>
	<"xyce/tran.V(ND3)" #000000 0 3 0 0 0>
  </Time>
</Diagrams>
<Paintings>
  <Line 0 490 0 360 #000000 3 1>
  <Line 1440 510 0 360 #000000 3 1>
  <Line 440 30 360 0 #000000 3 1>
  <Line 520 1360 360 0 #000000 3 1>
</Paintings>
