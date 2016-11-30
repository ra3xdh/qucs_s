<Qucs Schematic 0.0.14>
<Properties>
  <View=0,-41,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=shot.dat>
  <DataDisplay=shot.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
  <Line -20 20 40 -40 #000080 2 1>
  <Line -20 0 20 -20 #000080 2 1>
  <Line 0 20 20 -20 #000080 2 1>
  <Line 0 -20 0 -10 #000080 2 1>
  <Line 0 30 0 -10 #000080 2 1>
  <Line -40 20 5 -10 #ff5500 2 1>
  <Line -45 10 5 10 #ff5500 2 1>
  <Line -20 -10 10 -10 #000080 2 1>
  <Line 10 20 10 -10 #000080 2 1>
  <Line -10 20 30 -30 #000080 2 1>
  <Line -20 10 30 -30 #000080 2 1>
  <Line -40 -30 0 60 #ff5500 2 1>
  <.PortSym -40 30 2 0>
  <.PortSym -40 -30 1 0>
  <.PortSym 0 -30 3 270>
  <.ID 30 -26 SHOT>
  <.PortSym 0 30 4 90>
  <Text -60 -5 6 #000000 0 "DC">
</Symbol>
<Components>
  <Vnoise V1 1 500 210 19 -26 0 1 "Vnoise" 1 "0" 1 "1" 1 "0" 1>
  <CCVS SRC1 1 150 230 -26 34 0 0 "1" 1 "0" 0>
  <GND * 1 380 300 0 0 0 0>
  <GND * 1 500 300 0 0 0 0>
  <Port INEG1 1 120 300 -23 28 0 1 "2" 1 "analog" 0>
  <Port INPOS1 1 320 160 -23 -61 0 3 "3" 1 "analog" 0>
  <Port INNEG1 1 320 300 -26 27 0 1 "4" 1 "analog" 0>
  <GND * 1 260 300 0 0 0 0>
  <Eqn Eqn1 1 510 30 -31 17 0 0 "Vnoise=2*q" 1 "yes" 0>
  <Port IPOS1 1 120 100 -22 -62 0 3 "1" 1 "analog" 0>
  <DCFeed L1 1 120 150 20 -26 0 1 "1 uH" 0>
  <EDD D1 1 320 230 -78 -211 0 3 "explicit" 0 "3" 0 "0" 1 "0" 0 "sqrt(V3+1e-20)*(V1)" 1 "0" 0 "0" 1 "0" 0>
</Components>
<Wires>
  <180 200 260 200 "vin" 230 160 26 "">
  <180 260 260 260 "" 0 0 0 "">
  <120 260 120 300 "" 0 0 0 "">
  <320 160 320 200 "" 0 0 0 "">
  <320 260 320 300 "" 0 0 0 "">
  <500 160 500 180 "" 0 0 0 "">
  <380 160 500 160 "vnoise" 440 130 45 "">
  <380 160 380 200 "" 0 0 0 "">
  <380 260 380 300 "" 0 0 0 "">
  <500 240 500 300 "" 0 0 0 "">
  <260 260 260 300 "" 0 0 0 "">
  <120 100 120 120 "" 0 0 0 "">
  <120 180 120 200 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
