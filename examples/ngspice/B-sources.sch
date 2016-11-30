<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-13,909,677,1,0,120>
  <Grid=10,10,1>
  <DataSet=B-sources.dat>
  <DataDisplay=B-sources.dpl>
  <OpenDisplay=1>
  <Script=4.m>
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
  <GND * 1 210 220 0 0 0 0>
  <GND * 1 100 220 0 0 0 0>
  <GND * 1 110 400 0 0 0 0>
  <Vac V1 1 100 190 18 -26 0 1 "1 V" 1 "10 kHz" 0 "0" 0 "0" 0>
  <Vac V2 1 110 370 18 -26 0 1 "1 V" 1 "7 kHz" 0 "0" 0 "0" 0>
  <.TR TR1 1 220 290 0 71 0 0 "lin" 1 "0" 1 "1 ms" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <B B1 1 210 190 18 -26 0 1 "V=V(in1)*V(in2)" 1>
</Components>
<Wires>
  <100 130 100 160 "" 0 0 0 "">
  <100 130 110 130 "" 0 0 0 "">
  <110 310 110 340 "" 0 0 0 "">
  <110 310 120 310 "" 0 0 0 "">
  <210 140 210 160 "" 0 0 0 "">
  <210 140 260 140 "mul" 270 110 25 "">
  <110 130 110 130 "in1" 140 100 0 "">
  <120 310 120 310 "in2" 150 280 0 "">
</Wires>
<Diagrams>
  <Rect 390 230 256 161 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/tran.v(mul)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 390 464 254 164 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/tran.V(MUL)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 90 470 12 #000000 0 "This is a voltage multiplier that \nuses B-type spice sources.\nNOTE: This Example could be run \nonly with Ngspice/Xyce">
</Paintings>
