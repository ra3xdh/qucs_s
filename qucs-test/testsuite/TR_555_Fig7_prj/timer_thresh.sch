<Qucs Schematic 0.0.17>
<Properties>
  <View=-7,-10,480,300,1.74333,0,0>
  <Grid=5,5,0>
  <DataSet=timer_thresh.dat>
  <DataDisplay=timer_thresh.dpl>
  <OpenDisplay=1>
  <Script=timer_thresh.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym -30 -30 1 0>
  <.PortSym -30 30 2 0>
  <Line -30 -30 10 0 #000080 2 1>
  <Line -30 30 10 0 #000080 2 1>
  <Line -20 60 100 -60 #000080 2 1>
  <Line -20 -60 100 60 #000080 2 1>
  <Line 80 0 10 0 #000080 2 1>
  <.PortSym 90 0 3 180>
  <Line -10 20 20 0 #00007f 2 1>
  <Line 10 20 0 -20 #00007f 2 1>
  <Line 10 0 20 0 #00007f 2 1>
  <Text -5 -20 12 #000000 0 "THRESH">
  <.ID 5 49 SUB>
  <Text -15 -40 16 #000000 0 "+">
  <Line -20 -60 0 120 #000080 2 1>
  <Text -15 25 20 #000000 0 "-">
</Symbol>
<Components>
  <Port PinP 1 60 120 -23 12 0 0 "1" 1 "analog" 0>
  <Port PinN 1 60 180 -23 12 0 0 "2" 1 "analog" 0>
  <GND * 1 245 35 0 0 0 0>
  <Idc I1 1 160 60 18 -26 0 1 "0.1 uA" 1>
  <R R4 1 320 150 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port POUT 1 390 150 4 -42 0 2 "3" 1 "analog" 0>
  <GND * 1 370 235 0 0 0 0>
  <OpAmp OP1 1 225 150 -26 42 0 0 "1e6" 1 "1 V" 1>
  <C C1 1 370 205 17 -26 0 1 "1 nF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <60 120 160 120 "" 0 0 0 "">
  <245 30 245 35 "" 0 0 0 "">
  <160 30 245 30 "" 0 0 0 "">
  <160 90 160 120 "" 0 0 0 "">
  <160 120 170 120 "" 0 0 0 "">
  <265 150 290 150 "" 0 0 0 "">
  <350 150 370 150 "" 0 0 0 "">
  <370 150 390 150 "" 0 0 0 "">
  <370 150 370 175 "" 0 0 0 "">
  <170 130 195 130 "" 0 0 0 "">
  <170 120 170 130 "" 0 0 0 "">
  <60 180 195 180 "" 0 0 0 "">
  <195 170 195 180 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
