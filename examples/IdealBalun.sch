<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=IdealBalun.dat>
  <DataDisplay=IdealBalun.dpl>
  <OpenDisplay=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -20 -70 70 0 #000080 2 1>
  <Line -20 -70 0 100 #000080 2 1>
  <.PortSym -30 -60 1 0>
  <Line -30 -60 10 0 #000080 2 1>
  <.PortSym -30 -20 2 0>
  <Line -30 -20 10 0 #000080 2 1>
  <Line -20 20 -10 0 #000080 2 1>
  <.PortSym -30 20 5 0>
  <.ID -10 34 SUB>
  <Text -20 -70 9 #000000 0 "Vdiff">
  <Text -20 -30 9 #000000 0 "VComm">
  <Text -20 10 9 #000000 0 "GND">
  <Line 50 -70 0 100 #000080 2 1>
  <Line 50 -60 10 0 #000080 2 1>
  <Line 50 20 10 0 #000080 2 1>
  <.PortSym 60 20 3 180>
  <Text 20 10 9 #000000 0 "VNeg">
  <.PortSym 60 -60 4 180>
  <Line -20 30 70 0 #000080 2 1>
  <Text 20 -70 9 #000000 0 "VPos">
</Symbol>
<Components>
  <sTr Tr1 1 380 210 -29 78 1 2 "0.5" 1 "0.5" 1>
  <Port P3 1 460 280 4 12 1 2 "3" 1 "analog" 0>
  <Port P4 1 460 140 4 12 1 2 "4" 1 "analog" 0>
  <Port P5 1 340 390 12 4 0 1 "5" 1 "analog" 0>
  <Port P2 1 260 360 -23 12 0 0 "2" 1 "analog" 0>
  <Port P1 1 260 180 -23 12 0 0 "1" 1 "analog" 0>
</Components>
<Wires>
  <410 220 430 220 "" 0 0 0 "">
  <410 200 430 200 "" 0 0 0 "">
  <430 200 430 220 "" 0 0 0 "">
  <410 280 460 280 "" 0 0 0 "">
  <410 140 460 140 "" 0 0 0 "">
  <340 240 350 240 "" 0 0 0 "">
  <340 240 340 390 "" 0 0 0 "">
  <430 220 430 360 "" 0 0 0 "">
  <260 360 430 360 "" 0 0 0 "">
  <260 180 350 180 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
