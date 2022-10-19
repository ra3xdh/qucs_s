<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-1,1420,872,1,0,0>
  <Grid=10,10,1>
  <DataSet=Xyce2ToneTest.dat>
  <DataDisplay=Xyce2ToneTest.dpl>
  <OpenDisplay=1>
  <Script=Xyce2ToneTest.m>
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
  <GND * 1 640 100 0 0 0 0>
  <VProbe Pr1 1 660 90 -16 28 0 3>
  <GND * 1 160 220 0 0 0 0>
  <GND * 1 560 240 0 0 0 0>
  <IProbe Pr2 1 560 200 -41 -26 0 3>
  <R_SPICE R1 1 400 80 -26 -61 0 2 "0.1" 1 "" 0 "" 0 "" 0 "" 0>
  <DIODE_SPICE D1 1 560 120 43 72 0 3 "DMOD" 1 ".model DMOD D(is=1e-15 n=1 cj0=1p rs=0.1 bv=0.7)" 1 "" 0 "" 0 "" 0>
  <S4Q_V V1 1 160 130 18 -26 0 1 "dc 0 ac 0 sin(0 0.8 0.95e6 0 0)" 1 "" 0 "" 0 "" 0 "" 0>
  <S4Q_V V2 1 160 190 18 -26 0 1 "dc 0 ac 0 sin(0 0.8 1.05e6 0 0)" 1 "" 0 "" 0 "" 0 "" 0>
  <.HB HB1 1 860 80 0 77 0 0 "0.95e6 1.05e6" 1 "3,3" 1 "1 pA" 0 "1 uV" 0 "0.001" 0 "150" 0>
</Components>
<Wires>
  <560 80 640 80 "" 0 0 0 "">
  <560 80 560 90 "" 0 0 0 "">
  <560 230 560 240 "" 0 0 0 "">
  <560 150 560 170 "" 0 0 0 "">
  <160 80 160 100 "" 0 0 0 "">
  <160 80 370 80 "n1" 230 100 39 "">
  <430 80 560 80 "" 0 0 0 "">
  <560 80 560 80 "nd" 490 100 0 "">
</Wires>
<Diagrams>
  <Rect 160 473 871 166 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/PR1.Vb" #0000ff 2 3 0 6 0>
  </Rect>
  <Rect 160 731 867 181 3 #c0c0c0 1 00 1 -3.15e+06 2e+06 3.15e+06 1 -0.0742266 0.2 0.458508 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/PR2.Ib" #0000ff 2 3 0 6 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
