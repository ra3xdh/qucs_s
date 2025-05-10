<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-120,1503,1165,1,0,180>
  <Grid=10,10,1>
  <DataSet=testDCMESFETCL1.dat>
  <DataDisplay=testDCMESFETCL1.dpl>
  <OpenDisplay=1>
  <Script=testDCMESFETCL1.m>
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
  <IProbe PrIds 1 670 180 -26 16 1 2>
  <GND * 1 250 300 0 0 0 0>
  <S4Q_V V2 1 250 270 18 -26 0 1 "-1" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 580 280 0 0 0 0>
  <Lib MESFETCL1 1 580 240 -54 84 0 0 "MESFETs" 0 "MESFETCL1" 0 "3e-3" 1 "0.05" 1 "2.25" 1 "-1.8" 1 "1" 1 "5.1" 1 "1.3" 1 "1.3" 1 "1e-15" 1 "10.0" 1 "1.0" 1 "0.1e-9" 1 "0.1e-9" 1 "0.1e-9" 1 "1e-12" 1 "1e-12" 1 "1e-12" 1 "1e-3" 1>
  <.SW SW1 1 690 290 0 65 0 0 "DC1" 1 "lin" 1 "V1" 1 "-10" 1 "10" 1 "121" 1 "false" 0>
  <.SW SW2 1 840 290 0 65 0 0 "SW1" 1 "lin" 1 "V2" 1 "-5" 1 "0.5" 1 "14" 1 "false" 0>
  <GND * 1 720 240 0 0 0 0>
  <S4Q_V V1 1 720 210 18 -26 0 1 "3" 1 "" 0 "" 0 "" 0 "" 0>
  <.DC DC1 1 790 170 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <IProbe PrIgs 1 380 240 -26 16 0 0>
</Components>
<Wires>
  <580 180 580 200 "" 0 0 0 "">
  <580 180 640 180 "" 0 0 0 "">
  <700 180 720 180 "" 0 0 0 "">
  <410 240 540 240 "" 0 0 0 "">
  <250 240 350 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 140 671 368 331 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"ngspice/i(vprids)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 700 700 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"ngspice/i(vprigs)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
