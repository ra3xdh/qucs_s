<Qucs Schematic 0.0.19>
<Properties>
  <View=-104,-70,906,888,1,0,0>
  <Grid=10,10,1>
  <DataSet=sensitivityACandDC.dat>
  <DataDisplay=sensitivityACandDC.dpl>
  <OpenDisplay=1>
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
  <GND * 1 210 260 0 0 0 0>
  <GND * 1 -30 270 0 0 0 0>
  <S4Q_V V1 1 -30 200 18 -26 0 1 "dc 5 ac 5" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 100 260 0 0 0 0>
  <R R1 1 210 150 15 -26 0 1 "1000" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 210 230 15 -26 0 1 "1000" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 100 230 17 -26 0 1 "1u" 1 "" 0 "neutral" 0>
  <.SENS SENS1 1 360 120 0 71 0 0 "v(dv)" 1 "R1" 1 "100" 1 "1000" 1 "100" 1>
  <.SENS_AC SENS2 1 610 550 0 71 0 0 "v(dv)" 1 "lin" 1 "1 Hz" 1 "1000 Hz" 1 "100" 1>
</Components>
<Wires>
  <210 180 210 200 "dv" 270 180 7 "">
  <-30 120 210 120 "" 0 0 0 "">
  <-30 120 -30 170 "" 0 0 0 "">
  <-30 230 -30 270 "" 0 0 0 "">
  <100 180 100 200 "" 0 0 0 "">
  <100 180 210 180 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 15 550 531 206 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/r1" #0000ff 0 3 0 0 0>
  </Rect>
  <Tab 600 503 244 297 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 10 315 0 225 "" "" "">
	<"ngspice/r1" #0000ff 0 3 1 0 0>
	<"ngspice/r2" #0000ff 0 3 0 0 0>
  </Tab>
  <Rect 30 810 508 193 3 #c0c0c0 1 00 1 0 200 1000 1 -0.000125001 0.001 0.00137501 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/ac.c1" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 10 12 #000000 0 "This is example of sensitivity analysis (DC and AC) usage \nwith Ngspice. It simulates an influence of resistor tolerance \non divider output voltage">
</Paintings>
