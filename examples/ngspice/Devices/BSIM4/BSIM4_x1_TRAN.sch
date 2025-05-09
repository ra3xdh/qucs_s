<Qucs Schematic 0.0.24>
<Properties>
  <View=-61,-90,1380,880,0.853821,0,0>
  <Grid=10,10,1>
  <DataSet=BSIM4_x1_TRAN.dat>
  <DataDisplay=BSIM4_x1_TRAN.dpl>
  <OpenDisplay=0>
  <Script=BSIM4_x1_TRAN.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=>
  <FrameText1=>
  <FrameText2=>
  <FrameText3=>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 100 280 0 0 0 0>
  <GND * 1 320 100 0 0 0 0>
  <GND * 1 220 260 0 0 0 0>
  <Vrect V2 1 100 230 -114 -64 0 1 "1.5V" 1 "100 ns" 1 "100 ns" 1 "10 ns" 1 "10 ns" 1 "10 ns" 1>
  <.DC DC1 1 150 330 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR2 1 350 330 0 75 0 0 "lin" 1 "0" 1 "1 us" 1 "1000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vdc V1 1 280 80 -15 -56 0 2 "1.5 v" 1>
  <Sub SUB1 1 200 180 -128 -90 0 0 "BSIM4_inv.sch" 1>
  <GND * 1 340 260 0 0 0 0>
  <R R1 1 340 220 15 -26 0 1 "1MEG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <100 260 100 280 "" 0 0 0 "">
  <310 80 320 80 "" 0 0 0 "">
  <320 80 320 100 "" 0 0 0 "">
  <220 80 250 80 "" 0 0 0 "">
  <220 80 220 120 "" 0 0 0 "">
  <100 180 100 200 "" 0 0 0 "">
  <100 180 160 180 "in" 90 150 10 "">
  <220 240 220 260 "" 0 0 0 "">
  <280 180 340 180 "out" 310 150 22 "">
  <340 180 340 190 "" 0 0 0 "">
  <340 250 340 260 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 530 400 803 358 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "Time (s)" "" "">
	<"ngspice/tran.v(in)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(out)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 30 -10 14 #ff0000 0 "BSIM 4 inverter transient">
</Paintings>
