<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-199,1054,775,1,0,0>
  <Grid=10,10,1>
  <DataSet=sensitivityDC.dat>
  <DataDisplay=sensitivityDC.dpl>
  <OpenDisplay=1>
  <Script=sensitivityDC.m>
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
  <GND * 1 380 270 0 0 0 0>
  <GND * 1 140 280 0 0 0 0>
  <S4Q_V V1 1 140 210 18 -26 0 1 "dc 5 ac 5" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 270 270 0 0 0 0>
  <R R1 1 380 160 15 -26 0 1 "1000" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 380 240 15 -26 0 1 "1000" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 270 240 17 -26 0 1 "1u" 1 "" 0 "neutral" 0>
  <.SENS_XYCE SENS1 1 530 130 0 71 0 0 "v(dv)" 1 "R1:R" 1 "V1" 1 "1" 1 "5" 1 "1" 1>
</Components>
<Wires>
  <380 190 380 210 "dv" 440 190 7 "">
  <140 130 380 130 "" 0 0 0 "">
  <140 130 140 180 "" 0 0 0 "">
  <140 240 140 280 "" 0 0 0 "">
  <270 190 270 210 "" 0 0 0 "">
  <270 190 380 190 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 130 580 300 200 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/d{v(dv)}/d(R1_R)_Adj" #0000ff 0 3 1 0 0>
  </Tab>
  <Rect 541 677 356 296 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/d{v(dv)}/d(R1_R)_Adj" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 320 30 12 #000000 0 "This is example of the DC sensitivity analysis with Xyce. \nThe influence of resistor tolerance on divider output voltage\nvalue is analysed.">
</Paintings>
