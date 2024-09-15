<Qucs Schematic 24.3.99>
<Properties>
  <View=-881,-113,2464,1738,0.943747,638,0>
  <Grid=10,10,1>
  <DataSet=testNAND2.dat>
  <DataDisplay=testNAND2.dpl>
  <OpenDisplay=0>
  <Script=testNAND2.m>
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
  <SpLib X7 1 230 210 -177 -141 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "PATGENX2" 1 "auto" 1 "PulseFreq=1e4 ScaleFactor=5" 1 "" 0>
  <SpLib X6 1 380 220 -24 -154 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "A2DBRIDGEX2" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <SpLib X8 1 670 220 53 -146 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "D2ABRIDGE" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <SpLib X5 1 530 220 15 -149 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "NAND2" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <GND * 1 760 300 0 0 0 0>
  <R_SPICE R1 1 760 260 15 -26 0 1 "47k" 1 "" 0 "" 0 "" 0 "" 0 "2" 0 "R" 1>
  <SpiceModel SpiceModel1 1 180 310 -29 17 0 0 ".model DMOD DIG ( RLOAD=1000 CLOAD=1e-12 DELAY=20e-9 CLO=1e-12 CHI=1e-12" 1 "+  S0RLO=5 S0RHI=5 S0TSW=5e-9 S0VLO=-1 S0VHI=0.16 S1RLO=200 S1RHI=5 S1TSW=5e-9 " 1 "+  S1VLO=0.52 S1VHI=1 )" 1 "" 0 "Line_5=" 0>
  <.TR TR1 1 1000 120 0 64 0 0 "lin" 1 "0" 1 "0.2 ms" 1 "101" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <270 230 330 230 "nA1" 290 240 21 "">
  <270 210 330 210 "nA0" 290 170 22 "">
  <570 220 620 220 "" 0 0 0 "">
  <450 230 490 230 "" 0 0 0 "">
  <450 210 490 210 "" 0 0 0 "">
  <760 220 760 230 "" 0 0 0 "">
  <740 220 760 220 "" 0 0 0 "">
  <760 290 760 300 "" 0 0 0 "">
  <740 220 740 220 "nAout" 770 180 0 "">
  <490 210 490 210 "nD0" 438 170 0 "">
  <490 230 490 230 "nD1" 448 240 0 "">
  <570 220 570 220 "nDout" 580 240 0 "">
</Wires>
<Diagrams>
  <Time 200 658 1191 218 3 #c0c0c0 1 00 1 454 1 14 1 0 1 1 1 0 1 468 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NA0)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(NA1)" #ff0000 0 3 0 0 0>
	<"xyce/tran.V(ND0)" #ff00ff 0 3 0 0 0>
	<"xyce/tran.V(ND1)" #00ff00 0 3 0 0 0>
	<"xyce/tran.V(NDOUT)" #00ffff 0 3 0 0 0>
	<"xyce/tran.V(NAOUT)" #ffff00 0 3 0 0 0>
  </Time>
  <Rect 200 764 1192 51 3 #c0c0c0 1 00 1 0 5e-06 5e-05 1 -2 5 7 1 -1 2 1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NA0)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 210 890 1181 63 3 #c0c0c0 1 00 1 0 2e-06 5e-05 1 -5e+07 2e+08 6e+08 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NA1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 210 1029 1186 79 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(ND0)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 210 1173 1185 93 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(ND1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 210 1340 1189 110 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NDOUT)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 210 1511 1189 101 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NAOUT)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Line 40 240 0 330 #000000 3 1>
  <Line 1540 280 0 330 #000000 3 1>
  <Line 615 1695 330 0 #000000 3 1>
  <Line 575 -55 330 0 #000000 3 1>
</Paintings>
