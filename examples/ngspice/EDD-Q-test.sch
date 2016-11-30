<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-251,922,588,1,0,0>
  <Grid=10,10,1>
  <DataSet=EDD-Q-test.dat>
  <DataDisplay=EDD-Q-test.dpl>
  <OpenDisplay=1>
  <Script=EDD-Q-test.m>
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
  <GND * 1 100 220 0 0 0 0>
  <R R1 1 240 130 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 100 190 18 -26 0 1 "1 V" 1>
  <.AC AC1 1 100 280 0 43 0 0 "lin" 1 "1 Hz" 1 "10 kHz" 1 "200" 1 "no" 0>
  <GND * 1 310 240 0 0 0 0>
  <.DC DC1 1 260 280 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <EDD D1 1 310 200 36 -26 1 1 "explicit" 0 "1" 0 "0" 1 "-(1e-6*V1+(0.5e-6*V1^2)/2+(0.2e-6*V1^3)/3)" 1>
  <Vac V2 1 180 130 -26 -58 0 2 "1 V" 1 "300 Hz" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <270 130 310 130 "out" 320 100 15 "">
  <310 130 310 170 "" 0 0 0 "">
  <310 230 310 240 "" 0 0 0 "">
  <100 130 100 160 "" 0 0 0 "">
  <100 130 150 130 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 476 138 369 225 3 #c0c0c0 1 10 1 1 1 10000 1 -0.0749954 0.5 1.09772 1 -1 0.5 1 315 0 225 "" "" "">
	<"out.v" #0000ff 0 3 0 0 0>
	<"xyce/ac.V(OUT)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 488 490 350 211 3 #c0c0c0 1 10 1 0 1 0 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.v(out)" #0000ff 0 3 0 0 0>
	<"xyce/ac.V(OUT)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 -40 12 #000000 0 "This is a nonlinear capacitance example for \nNgspice and Xyce. It is shown how to use charge\nequations with EDDs. Simulate it with Qucs, Ngspice,\nand Xyce and compare results.">
</Paintings>
