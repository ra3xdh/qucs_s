<Qucs Schematic 0.0.14>
<Properties>
  <View=0,0,800,905,1,0,0>
  <Grid=10,10,1>
  <DataSet=flicker.dat>
  <DataDisplay=flicker.dpl>
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
  <.PortSym -40 -30 1 270>
  <.PortSym -40 30 2 90>
  <.PortSym 0 -30 3 270>
  <.PortSym 0 30 4 90>
  <.ID 30 -26 FLICKER "1=Kf=0.0=flicker noise coefficient" "1=Af=1.0=flicker noise exponent" "1=Ffe=1.0=flicker noise frequency exponent">
</Symbol>
<Components>
  <Vnoise V1 1 460 230 19 -26 0 1 "Kf" 1 "Ffe" 1 "1" 1 "0" 1>
  <CCVS SRC1 1 110 250 -26 34 0 0 "1" 1 "0" 0>
  <GND * 1 340 320 0 0 0 0>
  <GND * 1 460 320 0 0 0 0>
  <Port INEG1 1 80 320 -23 28 0 1 "2" 1 "analog" 0>
  <Port INNEG1 1 280 320 -26 27 0 1 "4" 1 "analog" 0>
  <GND * 1 220 320 0 0 0 0>
  <EDD D1 1 280 250 -78 -211 0 3 "explicit" 0 "3" 0 "0" 1 "0" 0 "(V3+1e-20)^(0.5*Af)*(V1)" 1 "0" 0 "0" 1 "0" 0>
  <DCFeed L1 1 80 170 20 -26 0 1 "1 uH" 0>
  <Port IPOS1 1 80 120 -22 -62 0 3 "1" 1 "analog" 0>
  <Port INPOS1 1 280 180 -23 -61 0 3 "3" 1 "analog" 0>
</Components>
<Wires>
  <140 220 220 220 "" 0 0 0 "">
  <140 280 220 280 "" 0 0 0 "">
  <80 280 80 320 "" 0 0 0 "">
  <280 280 280 320 "" 0 0 0 "">
  <460 180 460 200 "" 0 0 0 "">
  <340 180 460 180 "" 0 0 0 "">
  <340 180 340 220 "" 0 0 0 "">
  <340 280 340 320 "" 0 0 0 "">
  <460 260 460 320 "" 0 0 0 "">
  <220 280 220 320 "" 0 0 0 "">
  <80 200 80 220 "" 0 0 0 "">
  <80 120 80 140 "" 0 0 0 "">
  <280 180 280 220 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
