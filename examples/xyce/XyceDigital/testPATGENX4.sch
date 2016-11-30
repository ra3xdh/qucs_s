<Qucs Schematic 0.0.19>
<Properties>
  <View=-40,10,1480,1380,1,0,120>
  <Grid=10,10,1>
  <DataSet=testPATGENX4.dat>
  <DataDisplay=testPATGENX4.dpl>
  <OpenDisplay=1>
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
  <SpLib X4 1 640 210 -52 84 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "A2DBRIDGEX4" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X3 1 440 200 -169 87 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "PATGENX8" 1 "auto" 1 "PulseFreq=10000.0 ScaleFactor=5" 1>
  <.TR TR1 1 840 180 0 77 0 0 "lin" 1 "0" 1 "0.2m" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <480 260 580 260 "" 0 0 0 "">
  <480 240 580 240 "nA2" 530 220 36 "">
  <480 220 580 220 "nA1" 530 200 20 "">
  <480 200 580 200 "" 0 0 0 "">
  <700 200 700 200 "nD0" 730 170 0 "">
  <700 220 700 220 "nD1" 730 190 0 "">
  <700 240 700 240 "nD2" 730 210 0 "">
  <700 260 700 260 "nD3" 730 230 0 "">
  <480 200 480 200 "nA0" 510 180 0 "">
  <480 260 480 260 "nA3" 510 240 0 "">
</Wires>
<Diagrams>
  <Rect 130 472 1108 60 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(ND0)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 579 1109 59 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(ND1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Time 130 1173 1130 265 3 #c0c0c0 1 00 1 477 1 10 1 0 1 1 1 0 1 950 315 0 225 "" "" "">
	<"xyce/tran.V(NA0)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(NA1)" #ff0000 0 3 0 0 0>
	<"xyce/tran.V(NA2)" #ff00ff 0 3 0 0 0>
	<"xyce/tran.V(NA3)" #00ff00 0 3 0 0 0>
	<"xyce/tran.V(ND0)" #00ffff 0 3 0 0 0>
	<"xyce/tran.V(ND1)" #ffff00 0 3 0 0 0>
	<"xyce/tran.V(ND2)" #777777 0 3 0 0 0>
	<"xyce/tran.V(ND3)" #000000 0 3 0 0 0>
  </Time>
  <Rect 130 719 1118 60 3 #c0c0c0 1 00 1 0 5e-05 0.001 0 0 1 1 1 -1 2 1 315 0 225 "" "" "">
	<"xyce/tran.V(ND2)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 848 1124 68 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(ND3)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Line 0 490 0 360 #000000 3 1>
  <Line 1440 510 0 360 #000000 3 1>
  <Line 440 30 360 0 #000000 3 1>
  <Line 520 1360 360 0 #000000 3 1>
</Paintings>
