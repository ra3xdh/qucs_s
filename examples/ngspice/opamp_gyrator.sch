<Qucs Schematic 1.0.0>
<Properties>
  <View=0,-120,1257,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=opamp_gyrator.dat>
  <DataDisplay=opamp_gyrator.dpl>
  <OpenDisplay=0>
  <Script=opamp_gyrator.m>
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
  <C C1 1 210 150 -26 17 0 0 "1 uF" 1 "" 0 "neutral" 0>
  <OpAmp OP1 1 290 130 0 26 0 0 "1e6" 1 "15 V" 0>
  <Pac P2 1 420 130 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <R R4 1 530 130 15 -26 0 1 "500k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 640 130 15 -26 0 1 "10 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 5 640 250 0 0 0 0>
  <.DC DC1 1 60 360 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SP SP1 1 230 360 0 65 0 0 "log" 1 "1 Hz" 1 "10 kHz" 1 "101" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <R R2 1 260 220 21 -16 1 1 "500k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 5 260 260 0 0 0 0>
  <GND * 5 530 180 0 0 0 0>
  <GND * 5 420 180 0 0 0 0>
  <GND * 5 70 210 0 0 0 0>
  <L L1 1 640 210 10 -26 0 1 "5 H" 1 "" 0>
  <Pac P1 1 70 160 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <R R1 1 210 110 -16 -49 1 0 "10" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <240 110 260 110 "" 0 0 0 "">
  <240 150 260 150 "" 0 0 0 "">
  <420 80 420 100 "" 0 0 0 "">
  <640 80 640 100 "" 0 0 0 "">
  <420 80 530 80 "" 0 0 0 "">
  <530 80 640 80 "" 0 0 0 "">
  <530 80 530 100 "" 0 0 0 "">
  <160 150 180 150 "" 0 0 0 "">
  <160 110 180 110 "" 0 0 0 "">
  <160 110 160 150 "" 0 0 0 "">
  <260 150 260 190 "" 0 0 0 "">
  <260 250 260 260 "" 0 0 0 "">
  <530 160 530 180 "" 0 0 0 "">
  <420 160 420 180 "" 0 0 0 "">
  <640 160 640 180 "" 0 0 0 "">
  <640 240 640 250 "" 0 0 0 "">
  <70 190 70 210 "" 0 0 0 "">
  <70 110 160 110 "" 0 0 0 "">
  <70 110 70 130 "" 0 0 0 "">
  <330 130 340 130 "" 0 0 0 "">
  <340 60 340 130 "" 0 0 0 "">
  <260 60 260 110 "" 0 0 0 "">
  <260 60 340 60 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 780 548 437 475 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 105 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.v(s_1_1)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(s_2_2)" #0000ff 0 3 0 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 480 50 14 #000000 0 "equivalent circuit">
  <Text 390 350 12 #000000 0 "As can be seen: With the use of\nan OpAmps, a capacitor can\nrebuild a very large inductor.">
  <Text 400 420 14 #000000 0 "L = R1 * R2 * C">
  <Text 70 50 14 #000000 0 "Gyrator">
</Paintings>
