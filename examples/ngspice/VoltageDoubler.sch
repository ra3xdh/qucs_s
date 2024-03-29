<Qucs Schematic 1.0.0>
<Properties>
  <View=0,-271,1941,1583,0.787232,0,0>
  <Grid=10,10,1>
  <DataSet=VoltageDoubler.dat>
  <DataDisplay=VoltageDoubler.dpl>
  <OpenDisplay=0>
  <Script=VoltageDoubler.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
  <.ID -30 -26 SUB>
</Symbol>
<Components>
  <GND * 1 180 240 0 0 0 0>
  <Vac V1 1 180 190 -96 -26 1 1 "1 V" 1 "500 MHz" 1 "0" 0 "0" 0>
  <GND * 1 400 240 0 0 0 0>
  <Diode D1 1 400 190 18 -9 0 3 "3e-6 A" 0 "1" 0 "180 fF" 0 "0.5" 0 "0.35 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "25.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "3.8" 0 "0.3 mA" 0 "26.85" 0 "2.0" 0 "0.69" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <C C2 1 540 190 17 -26 0 1 "1 nF" 1 "" 0 "neutral" 0>
  <GND * 1 540 240 0 0 0 0>
  <R R2 1 660 190 15 -26 0 1 "1 MOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 660 240 0 0 0 0>
  <.TR TR1 1 170 320 0 83 0 0 "lin" 1 "10 us" 1 "10.01 us" 1 "1000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-12" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <C C1 1 270 140 -26 -55 1 0 "1 nF" 1 "" 0 "neutral" 0>
  <Diode D2 1 470 140 -10 -34 0 2 "3e-6 A" 0 "1" 0 "180 fF" 0 "0.5" 0 "0.35 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "25.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "3.8" 0 "0.3 mA" 0 "26.85" 0 "2.0" 0 "0.69" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
</Components>
<Wires>
  <180 220 180 240 "" 0 0 0 "">
  <300 140 400 140 "Vmid" 380 100 68 "">
  <180 140 180 160 "Vin" 140 120 10 "">
  <180 140 240 140 "" 0 0 0 "">
  <400 220 400 240 "" 0 0 0 "">
  <400 140 400 160 "" 0 0 0 "">
  <540 220 540 240 "" 0 0 0 "">
  <540 140 540 160 "" 0 0 0 "">
  <540 140 660 140 "" 0 0 0 "">
  <660 140 660 160 "" 0 0 0 "">
  <660 220 660 240 "" 0 0 0 "">
  <400 140 440 140 "" 0 0 0 "">
  <500 140 540 140 "" 0 0 0 "">
  <660 140 660 140 "Vout" 670 110 0 "">
</Wires>
<Diagrams>
  <Rect 900 509 738 469 3 #c0c0c0 1 00 1 1e-05 1e-09 1.001e-05 1 -1.2 0.2 1.2 0 0 0.5 2 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(vin)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vout)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
