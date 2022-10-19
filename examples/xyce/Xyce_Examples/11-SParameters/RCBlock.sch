<Qucs Schematic 0.0.23>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=RCBlock.dat>
  <DataDisplay=RCBlock.dpl>
  <OpenDisplay=1>
  <Script=RCBlock.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 44 SUB>
  <.PortSym -30 -30 1 0>
  <.PortSym 30 -30 2 0>
  <.PortSym -30 30 3 0>
  <Line -20 -40 40 0 #000080 2 1>
  <Line 20 -40 0 80 #000080 2 1>
  <Line -20 40 40 0 #000080 2 1>
  <Line -20 -40 0 80 #000080 2 1>
  <Line -30 -30 10 0 #000080 2 1>
  <Line 20 -30 10 0 #000080 2 1>
  <Line -30 30 10 0 #000080 2 1>
</Symbol>
<Components>
  <R_SPICE R1 1 360 200 -26 -53 0 2 "20" 1 "" 0 "" 0 "" 0 "" 0>
  <C_SPICE C1 1 360 290 -26 -55 0 2 "1p" 1 "" 0 "" 0 "" 0 "" 0>
  <Port OUT 1 630 240 4 12 1 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <Port IN 1 200 240 -23 12 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port GND 1 560 400 -23 12 0 0 "3" 1 "analog" 0 "v" 0 "" 0>
  <C_SPICE Cg1 1 540 270 17 -26 0 1 "1p" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <200 200 200 240 "" 0 0 0 "">
  <200 200 330 200 "" 0 0 0 "">
  <200 290 330 290 "" 0 0 0 "">
  <390 200 390 240 "" 0 0 0 "">
  <390 240 540 240 "" 0 0 0 "">
  <390 240 390 290 "" 0 0 0 "">
  <200 240 200 290 "" 0 0 0 "">
  <540 240 630 240 "" 0 0 0 "">
  <560 300 560 400 "" 0 0 0 "">
  <540 300 560 300 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
