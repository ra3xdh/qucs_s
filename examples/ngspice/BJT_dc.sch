<Qucs Schematic 0.0.24>
<Properties>
  <View=0,0,1033,870,1,0,60>
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
  <GND * 1 420 320 0 0 0 0>
  <IProbe Pr1 1 340 200 -10 16 1 2>
  <_BJT T1 1 270 290 17 -22 0 0 "npn" 0 "1e-14" 0 "1" 0 "1" 0 "0.3" 0 "0" 0 "100" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 1 "3" 0 "0" 0 "0" 0 "3" 0 "1" 0 "10" 0 "25e-12" 0 "0.75" 0 "0.33" 0 "8e-12" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "400e-12" 0 "3" 0 "0.0" 0 "2" 0 "100e-9" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <.DC DC1 1 60 420 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 250 370 0 75 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "5" 1 "250" 1 "false" 0>
  <GND * 1 180 380 0 0 0 0>
  <Vdc V1 1 420 270 19 -22 0 1 "5" 1>
  <GND * 1 270 330 0 0 0 0>
  <.SW SW2 1 390 370 0 75 0 0 "SW1" 1 "lin" 1 "T1.Bf" 1 "20" 1 "200" 1 "5" 1 "false" 1>
  <Idc I1 1 180 330 -53 -21 0 1 "5u" 1>
</Components>
<Wires>
  <270 200 270 260 "" 0 0 0 "">
  <270 200 310 200 "" 0 0 0 "">
  <180 290 240 290 "" 0 0 0 "">
  <180 290 180 300 "" 0 0 0 "">
  <180 360 180 380 "" 0 0 0 "">
  <420 300 420 320 "" 0 0 0 "">
  <370 200 420 200 "" 0 0 0 "">
  <420 200 420 240 "" 0 0 0 "">
  <270 320 270 330 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 640 432 386 242 3 #c0c0c0 1 00 1 0 1 5 1 2.5e-6 2e-12 2.50001e-6 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 30 20 12 #000000 0 "This is an example of DC sweep usage with \nNgspice. It shows how to sweep model \nparameters with Ngspice. Simulate it with Ngspice\n and obtain IV-chart of BJT.\n\nNOTE: This Example could be run only with Ngspice">
</Paintings>
