<Qucs Schematic 0.0.21>
<Properties>
  <View=0,146,1033,870,1,0,0>
  <Grid=10,10,1>
  <DataSet=BJT_dc.dat>
  <DataDisplay=BJT_dc.dpl>
  <OpenDisplay=0>
  <Script=BJT_dc.m>
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
  <GND * 1 270 320 0 0 0 0>
  <GND * 1 420 320 0 0 0 0>
  <IProbe Pr1 1 340 200 -26 16 1 2>
  <Vdc V1 1 420 290 18 -26 0 1 "5" 1>
  <_BJT T1 1 270 290 12 -32 0 0 "npn" 0 "1e-14" 0 "1" 0 "1" 0 "0.3" 0 "0" 0 "100" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 1 "3" 0 "0" 0 "0" 0 "3" 0 "1" 0 "10" 0 "25e-12" 0 "0.75" 0 "0.33" 0 "8e-12" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "400e-12" 0 "3" 0 "0.0" 0 "2" 0 "100e-9" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <GND * 1 170 390 0 0 0 0>
  <Idc I1 1 170 340 -48 -26 1 1 "5u" 1>
  <.DC DC1 1 60 450 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 230 450 0 71 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "5" 1 "250" 1 "false" 0>
  <.SW SW2 1 370 450 0 71 0 0 "SW1" 1 "lin" 1 "I1" 1 "20u" 1 "200u" 1 "5" 1 "false" 0>
</Components>
<Wires>
  <270 200 270 260 "" 0 0 0 "">
  <270 200 310 200 "" 0 0 0 "">
  <420 200 420 260 "" 0 0 0 "">
  <370 200 420 200 "" 0 0 0 "">
  <170 290 240 290 "" 0 0 0 "">
  <170 290 170 310 "" 0 0 0 "">
  <170 370 170 390 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 610 442 386 242 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 530 500 12 #000000 0 "This is an example of DC sweep usage with \nNgspice. It shows how to sweep model \nparameters with Ngspice. Simulate it with Ngspice\n and obtain IV-chart of BJT.\n\nNOTE: This Example could be run only with Ngspice">
</Paintings>
