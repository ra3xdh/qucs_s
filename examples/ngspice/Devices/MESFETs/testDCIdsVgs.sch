<Qucs Schematic 0.0.19>
<Properties>
  <View=-72,39,1281,953,1,0,0>
  <Grid=10,10,1>
  <DataSet=testDCIdsVgs.dat>
  <DataDisplay=testDCIdsVgs.dpl>
  <OpenDisplay=1>
  <Script=testDCIdsVgs.m>
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
  <GND * 1 780 240 0 0 0 0>
  <IProbe PrIds 1 670 180 -26 16 1 2>
  <IProbe PrIgs 1 450 240 -26 16 0 0>
  <S4Q_V V1 1 780 210 18 -26 0 1 "3" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 580 280 0 0 0 0>
  <.SW SW2 1 1010 160 0 65 0 0 "SW1" 1 "lin" 1 "V2" 1 "-3" 1 "0" 1 "8" 1 "false" 0>
  <Lib MESFETCL1 1 580 240 -384 -136 0 0 "MESFETs" 0 "MESFETCL1" 0 "3e-3" 1 "0.05" 1 "2.25" 1 "-1.8" 1 "1" 1 "5.1" 1 "1.3" 1 "1.3" 1 "1e-15" 1 "10.0" 1 "1.0" 1 "0.1e-9" 1 "0.1e-9" 1 "0.1e-9" 1 "1e-12" 1 "1e-12" 1 "1e-12" 1 "1e-3" 1>
  <.SW SW1 1 860 170 0 65 0 0 "DC1" 1 "lin" 1 "V1" 1 "1" 1 "11" 1 "6" 1 "false" 0>
  <.DC DC1 1 620 260 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 340 300 0 0 0 0>
  <S4Q_V V2 1 340 270 18 -26 0 1 "-1" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <700 180 780 180 "" 0 0 0 "">
  <480 240 540 240 "" 0 0 0 "">
  <580 180 580 200 "" 0 0 0 "">
  <580 180 640 180 "" 0 0 0 "">
  <340 240 420 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect3D 590 838 492 428 3 #c0c0c0 1 00 1 1 1 9.33335 1 -3.3 0.5 0.3 1 -0.0013953 0.002 0.0153483 315 0 314 "V(nds) V" "" "">
	<"ngspice/i(vprids)" #0000ff 2 3 0 0 1>
  </Rect3D>
  <Rect 200 851 368 331 3 #c0c0c0 1 00 1 1 1 9.33335 1 -0.0013953 0.002 0.0153483 1 -1 0.5 1 315 0 225 "V(nds) V" "" "">
	<"ngspice/i(vprids)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
