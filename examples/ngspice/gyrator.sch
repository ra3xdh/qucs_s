<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-120,1179,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=gyrator.dat>
  <DataDisplay=gyrator.dpl>
  <OpenDisplay=0>
  <Script=gyrator.m>
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
  <C C1 1 410 160 -26 17 0 0 "200 uF" 1 "" 0 "neutral" 0>
  <R R1 1 310 160 -26 15 0 0 "20k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <_BJT T1 1 360 120 -29 -51 0 1 "npn" 0 "1e-16" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "250" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <Vdc V1 1 160 290 -73 -26 1 1 "10 V" 1>
  <Vac V2 1 160 210 -64 -26 1 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 160 340 0 0 0 0>
  <.DC DC1 1 120 390 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R Load 1 520 210 15 -26 0 1 "1000 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 520 260 0 0 0 0>
  <.TR TR1 1 490 390 0 68 0 0 "lin" 1 "0" 1 "40us" 1 "101" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.AC AC1 1 300 390 0 40 0 0 "log" 1 "10 kHz" 1 "100 kHz" 1 "10" 1 "no" 0>
</Components>
<Wires>
  <340 160 360 160 "" 0 0 0 "">
  <360 160 380 160 "" 0 0 0 "">
  <360 150 360 160 "" 0 0 0 "">
  <160 240 160 260 "" 0 0 0 "">
  <160 160 260 160 "" 0 0 0 "">
  <160 160 160 180 "" 0 0 0 "">
  <160 320 160 340 "" 0 0 0 "">
  <440 160 460 160 "" 0 0 0 "">
  <520 160 520 180 "" 0 0 0 "">
  <520 240 520 260 "" 0 0 0 "">
  <260 120 330 120 "" 0 0 0 "">
  <260 160 280 160 "" 0 0 0 "">
  <260 120 260 160 "" 0 0 0 "">
  <390 120 460 120 "" 0 0 0 "">
  <460 160 520 160 "" 0 0 0 "">
  <460 120 460 160 "" 0 0 0 "">
  <160 160 160 160 "Input" 110 130 0 "">
  <520 160 520 160 "Output" 530 130 0 "">
</Wires>
<Diagrams>
  <Rect 740 261 299 261 3 #c0c0c0 1 00 1 10000 20000 100000 1 0 0.5 2 1 0.0476176 5e-06 0.0476369 315 0 225 "" "" "">
	<"ngspice/ac.v(input)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(output)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 740 621 299 261 3 #c0c0c0 1 00 1 0 1e-05 4e-05 1 8.80642 0.5 11.1994 1 8.43728 0.02 8.55135 315 0 225 "" "" "">
	<"ngspice/tran.v(input)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(output)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 190 0 12 #000000 0 "This circuit is a primitive gyrator. It simulates an inductor\nby taking a capacitor and an transistor inverter. At the\noutput there should be only little ac voltage.">
</Paintings>
