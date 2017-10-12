<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-40,947,893,1,0,0>
  <Grid=10,10,1>
  <DataSet=sensitivityTRAN.dat>
  <DataDisplay=sensitivityTRAN.dpl>
  <OpenDisplay=1>
  <Script=sensitivityTRAN.m>
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
  <GND * 1 120 410 0 0 0 0>
  <GND * 1 250 410 0 0 0 0>
  <R R1 1 250 380 15 -26 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 120 380 17 -26 0 1 "1u" 1 "" 0 "neutral" 0>
  <SpiceIC SpiceIC1 1 380 310 -9 17 0 0 "v(cap)=1.0" 1>
  <.SENS_TR_XYCE SENS1 1 510 240 0 71 0 0 "v(cap)" 1 "R1:R,C1:C" 1 "direct" 1 "0" 1 "5m" 1 "5u" 1 "no" 1>
</Components>
<Wires>
  <120 310 120 350 "" 0 0 0 "">
  <120 310 250 310 "cap" 210 280 54 "">
  <250 310 250 350 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 104 710 254 191 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.{v(cap)}" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 466 726 288 212 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.2 1 1 -1 0.2 1 315 0 225 "" "" "">
	<"xyce/tran.d{v(cap)}/d(C1_C)_Dir" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 140 160 12 #000000 0 "This is example of transisent sensitivity analysis usage \nwith Xyce.  This example follows the Chapter 7.8.2 of the Xyce\nReference manual.">
</Paintings>
