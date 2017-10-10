<Qucs Schematic 0.0.19>
<Properties>
  <View=0,220,995,963,1,0,0>
  <Grid=10,10,1>
  <DataSet=TestFNEDDDC,sch.dat>
  <DataDisplay=TestFNEDDDC,sch.dpl>
  <OpenDisplay=1>
  <Script=TestFNEDDDC,sch.m>
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
  <GND * 1 180 340 0 0 0 0>
  <IProbe PrId 1 350 280 -26 16 0 0>
  <S4Q_V V1 1 180 310 18 -26 0 1 "dc 0 ac 0" 1 "" 0 "" 0 "" 0 "" 0>
  <.DC DC1 1 220 380 0 61 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R_SPICE R1 1 540 310 15 -26 0 1 "0.1" 1 "" 0 "" 0 "" 0 "" 0>
  <.SW SW1 1 230 510 0 107 0 0 "DC1" 1 "lin" 1 "V1" 1 "-6" 1 "6" 1 "81" 1 "false" 0>
  <Lib SUB1 1 540 420 117 -177 0 3 "AnalogueCM" 0 "FowlerNDiodeXSPICE" 0 "1" 1 "50e-6" 1 "50e-6" 1 "1e10" 1 "1e10" 1 "1e-10" 1 "1e-10" 1 "1.0" 1 "1.0" 1 "1.0" 1 "1.0" 1 "1.0" 1 "0.1" 1 "1e-10" 1 "1.0e-8" 1>
  <GND * 1 540 480 0 0 0 0>
</Components>
<Wires>
  <380 280 540 280 "n2" 450 240 93 "">
  <180 280 320 280 "n1" 260 240 67 "">
  <540 340 540 380 "" 0 0 0 "">
  <540 380 540 380 "nd" 500 360 0 "">
</Wires>
<Diagrams>
  <Rect 490 810 240 160 3 #c0c0c0 1 00 1 -3 1 3 1 nan 1 3e-08 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/TestFNEDDDC:i(vprid)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
