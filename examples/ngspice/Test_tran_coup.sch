<Qucs Schematic 0.0.19>
<Properties>
  <View=0,69,1700,794,1,0,0>
  <Grid=10,10,1>
  <DataSet=Test_tran_coup.dat>
  <DataDisplay=Test_tran_coup.dpl>
  <OpenDisplay=1>
  <Script=Test_tran_pos_coup.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 -16 SUB>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
</Symbol>
<Components>
  <R R3 1 700 180 15 -26 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 700 210 0 0 0 0>
  <GND * 1 320 210 0 0 0 0>
  <GND * 1 550 220 0 0 0 0>
  <GND * 1 490 220 0 0 0 0>
  <Vac V1 1 320 180 18 -26 0 1 "1 V" 1 "50Hz" 1 "0" 0 "0" 0>
  <R R2 1 620 150 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 690 400 0 0 0 0>
  <GND * 1 490 410 0 0 0 0>
  <GND * 1 550 410 0 0 0 0>
  <R R6 1 690 370 15 -26 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R5 1 620 340 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.TR TR1 1 800 240 0 77 0 0 "lin" 1 "0" 1 "100 ms" 1 "1001" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R1 1 440 150 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 440 340 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Sub TRAN3 1 510 130 -20 114 0 0 "Transformer_positive_coupling.sch" 0 "0.999" 1 "0.5m" 1 "0.5m" 1>
  <Sub TRAN2 1 510 320 -20 114 0 0 "Transformer_opposite_coupling.sch" 0 "0.999" 1 "0.5m" 1 "0.5m" 1>
</Components>
<Wires>
  <320 150 400 150 "Nsig" 350 110 20 "">
  <550 150 590 150 "" 0 0 0 "">
  <650 150 700 150 "N4" 650 120 25 "">
  <550 340 590 340 "" 0 0 0 "">
  <650 340 690 340 "" 0 0 0 "">
  <470 150 490 150 "" 0 0 0 "">
  <400 150 410 150 "" 0 0 0 "">
  <470 340 490 340 "" 0 0 0 "">
  <400 340 410 340 "" 0 0 0 "">
  <400 150 400 340 "" 0 0 0 "">
  <690 340 690 340 "N6" 670 310 0 "">
</Wires>
<Diagrams>
  <Rect 690 690 240 160 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/tran.v(n6)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(nsig)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 370 690 240 160 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/tran.v(n4)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(nsig)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
