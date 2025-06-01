<Qucs Schematic 25.1.2>
<Properties>
  <View=77,-52,1454,757,1.14161,0,0>
  <Grid=10,10,1>
  <DataSet=core_test.dat>
  <DataDisplay=core_test.dpl>
  <OpenDisplay=0>
  <Script=core_test.m>
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
  <GND * 1 420 310 0 0 0 0>
  <GND * 1 420 540 0 0 0 0>
  <GND * 1 300 310 0 0 0 0>
  <GND * 1 310 530 0 0 0 0>
  <R R1 1 370 200 -26 -59 1 0 "20" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 310 500 15 -26 0 1 "20" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 300 280 18 -26 0 1 "50" 1 "10 kHz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <.TR TR1 1 120 210 0 70 0 0 "lin" 1 "0" 1 "500us" 1 "2000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <WINDING W1 1 420 270 35 -44 0 0 "N87_1" 1 "25" 1 "0.1" 1>
  <WINDING W2 1 420 500 42 -41 0 0 "N87_1" 1 "25" 1 "0.1" 1>
  <CORE N87_1 1 620 220 -40 35 0 0 "27.2116" 1 "26.565" 1 "0.01" 1 "395.888k" 1 "0.0001" 0 "0.0311018" 1 "1.4e-05" 1 "0.0" 1 "ring" 1 "12.7" 0 "7.1" 0 "5" 0 "1.0" 0 "1.0" 0 "1.0" 0 "false" 0>
</Components>
<Wires>
  <420 430 420 460 "" 0 0 0 "">
  <310 430 420 430 "out" 430 400 24 "">
  <310 430 310 470 "" 0 0 0 "">
  <420 200 420 230 "" 0 0 0 "">
  <400 200 420 200 "in" 440 170 14 "">
  <300 200 300 250 "" 0 0 0 "">
  <300 200 340 200 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 852 519 561 353 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(in)" #0000ff 1 3 0 0 0>
	<"ngspice/tran.v(out)" #ff0000 1 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 210 50 10 #000000 0 "Transformer with two windings and N87 ferrite ring core:\n\nNp = 25 turns\nNs = 25 turns">
</Paintings>
