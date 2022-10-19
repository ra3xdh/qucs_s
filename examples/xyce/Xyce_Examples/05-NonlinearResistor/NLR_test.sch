<Qucs Schematic 0.0.19>
<Properties>
  <View=-70,-57,1006,800,1,0,57>
  <Grid=10,10,1>
  <DataSet=NLR_test.dat>
  <DataDisplay=NLR_test.dpl>
  <OpenDisplay=0>
  <Script=NLR_test.m>
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
  <GND * 1 260 280 0 0 0 0>
  <S4Q_V Vmon 1 460 270 -26 -54 0 2 "DC 0" 1 "" 0 "" 0 "" 0 "" 0>
  <L L1 1 400 270 -26 10 0 0 "15 mH" 1 "0" 0>
  <GND * 1 770 270 0 0 0 0>
  <Sub SUB1 1 630 270 -26 18 0 0 "NLR_subc.sch" 0 ".15" 1 "6" 1 "4" 1>
  <.TR TR1 1 210 520 0 57 0 0 "lin" 1 "0" 1 "1 ms" 1 "11" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <C C1 1 340 270 -26 -53 0 2 "400uF" 1 "400V" 0 "neutral" 0>
  <R R1 1 520 270 -26 15 0 0 "4 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <260 270 310 270 "" 0 0 0 "">
  <260 270 260 280 "" 0 0 0 "">
  <660 270 770 270 "" 0 0 0 "">
  <550 270 600 270 "" 0 0 0 "">
  <430 270 430 270 "node2" 430 180 0 "">
  <370 270 370 270 "node1" 360 210 0 "">
  <550 270 550 270 "node3" 560 220 0 "">
</Wires>
<Diagrams>
  <Rect 510 640 240 160 3 #c0c0c0 1 00 1 0 0.0002 0.001 1 -440 200 40 1 -1 1 1 315 0 225 "TIME" "" "">
	<"xyce/tran.V(NODE1)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(NODE2)" #ff0000 0 3 0 0 0>
	<"xyce/tran.V(NODE3)" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 40 12 #000000 0 "This is the top-level schematic demonstrating the use of subcircuit parameters in Qucs-s.\n\nIt corresponds to the NL_RESISTOR/nlrcs10 test case in the Xyce test suite.\n\nThe SUB1 subcircuit takes three input parameters.  These parameters must be defined in the SYMBOL for the subcircuit.\n">
</Paintings>
