<Qucs Schematic 0.0.18>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=resistor.dat>
  <DataDisplay=resistor.dpl>
  <OpenDisplay=0>
  <Script=TR_resistor.m>
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
  <GND * 1 660 370 0 0 0 0>
  <.TR TR1 1 260 300 0 51 0 0 "lin" 1 "0" 1 "1 ms" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R2 1 610 270 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R1 1 530 270 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 450 320 18 -26 0 1 "10 V" 1 "1kHz" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <450 350 450 360 "" 0 0 0 "">
  <450 360 660 360 "" 0 0 0 "">
  <660 360 660 370 "" 0 0 0 "">
  <660 270 660 360 "" 0 0 0 "">
  <450 270 450 290 "" 0 0 0 "">
  <450 270 500 270 "Vin" 500 240 23 "">
  <560 270 580 270 "Vout" 600 240 9 "">
  <640 270 660 270 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 430 590 240 160 3 #c0c0c0 1 00 1 0 0.0002 0.001 1 -0.12 0.1 0.12 1 -6 5 6 315 0 225 "" "" "">
	<"Vin.Vt" #0000ff 0 3 0 0 0>
	<"Vout.Vt" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
