<Qucs Schematic 0.0.17>
<Properties>
  <View=143,214,453,404,2.73871,0,0>
  <Grid=5,5,0>
  <DataSet=timer_amp.dat>
  <DataDisplay=timer_amp.dpl>
  <OpenDisplay=1>
  <Script=timer_amp.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 99 SUB>
  <Line -20 50 90 -50 #000080 2 1>
  <Line -20 50 0 -100 #000080 2 1>
  <Line -20 -50 90 50 #000080 2 1>
  <Line 70 0 20 0 #00007f 2 1>
  <.PortSym 90 0 3 180>
  <Line -25 30 -15 0 #00007f 2 1>
  <Line -25 30 5 0 #00007f 2 1>
  <Line -20 -30 -20 0 #00007f 2 1>
  <.PortSym -40 -30 1 0>
  <.PortSym -40 30 2 0>
  <Text -15 20 20 #000000 0 "-">
  <Text -15 -45 20 #000000 0 "+">
  <Text -10 -5 12 #000000 0 "x 3.3 AMP">
</Symbol>
<Components>
  <GND * 1 310 325 0 0 0 0>
  <Port P_vout 1 365 265 4 12 1 2 "3" 0 "analog" 0>
  <R R2 1 335 265 -26 15 0 0 "7" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port Pamp_P 1 210 265 -23 12 0 0 "1" 0 "analog" 0>
  <Port Pamp_N 1 210 325 -23 12 0 0 "2" 0 "analog" 0>
  <VCVS SRC1 1 270 290 -26 34 0 0 "3.7" 1 "0" 1>
</Components>
<Wires>
  <210 260 240 260 "" 0 0 0 "">
  <210 260 210 265 "" 0 0 0 "">
  <300 260 300 265 "" 0 0 0 "">
  <300 265 305 265 "" 0 0 0 "">
  <300 320 310 320 "" 0 0 0 "">
  <310 320 310 325 "" 0 0 0 "">
  <210 320 240 320 "" 0 0 0 "">
  <210 320 210 325 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
