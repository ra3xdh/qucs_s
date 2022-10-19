<Qucs Schematic 1.0.0>
<Properties>
  <View=-104,-10,1575,881,1,0,60>
  <Grid=10,10,1>
  <DataSet=sensitivityACandDC.dat>
  <DataDisplay=sensitivityACandDC.dpl>
  <OpenDisplay=0>
  <Script=sensitivityACandDC.m>
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
  <R R1 1 200 150 15 -26 0 1 "1000" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 200 230 15 -26 0 1 "1000" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 200 280 0 0 0 0>
  <C C1 1 160 230 -56 -26 1 1 "1u" 1 "" 0 "neutral" 0>
  <GND * 1 160 280 0 0 0 0>
  <S4Q_V V1 1 80 190 -103 -26 1 1 "dc 5 ac 5" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 80 240 0 0 0 0>
  <.SENS_AC SENS2 1 -50 400 0 71 0 0 "v(dv)" 1 "lin" 1 "1 Hz" 1 "1000 Hz" 1 "100" 1>
  <.SENS SENS1 1 200 400 0 71 0 0 "v(dv)" 1 "R1" 1 "100" 1 "1000" 1 "100" 1>
</Components>
<Wires>
  <200 100 200 120 "" 0 0 0 "">
  <200 180 200 200 "dv" 230 180 7 "">
  <200 260 200 280 "" 0 0 0 "">
  <160 180 200 180 "" 0 0 0 "">
  <160 180 160 200 "" 0 0 0 "">
  <160 260 160 280 "" 0 0 0 "">
  <80 100 200 100 "" 0 0 0 "">
  <80 100 80 160 "" 0 0 0 "">
  <80 220 80 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 450 619 284 223 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 10 315 0 225 0 0 0 "" "" "">
	<"ngspice/r1" #0000ff 0 3 1 0 0>
	<"ngspice/r2" #0000ff 0 3 0 0 0>
  </Tab>
  <Rect 495 300 531 206 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 1 1 1 -1 1 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/r1" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 820 580 508 193 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.v(c1)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 10 12 #000000 0 "This is example of sensitivity analysis (DC and AC) usage \nwith Ngspice. It simulates an influence of resistor tolerance \non divider output voltage">
</Paintings>
