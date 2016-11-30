<Qucs Schematic 0.0.19>
<Properties>
  <View=513,16,1093,949,1,0,60>
  <Grid=10,10,1>
  <DataSet=S22_Probe.dat>
  <DataDisplay=S22_Probe.dpl>
  <OpenDisplay=1>
  <Script=S22_Probe.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Rectangle -20 -40 80 70 #aa0000 3 1 #c0c0c0 1 0>
  <Line -40 20 20 0 #000080 2 1>
  <Line 60 20 20 0 #000080 2 1>
  <Line -20 10 80 0 #000000 3 1>
  <Line 52 -16 4 2 #000000 2 1>
  <Line -16 -16 4 -2 #000000 2 1>
  <Line -12 -18 6 -2 #000000 2 1>
  <Line -6 -20 8 -2 #000000 2 1>
  <Line 2 -22 6 -2 #000000 2 1>
  <Line 8 -24 8 -1 #000000 2 1>
  <Line 28 -24 8 2 #000000 2 1>
  <Line 36 -22 6 2 #000000 2 1>
  <Line 42 -20 6 2 #000000 2 1>
  <Line 48 -18 4 2 #000000 2 1>
  <Line 56 -14 0 0 #000000 0 1>
  <Line 56 -14 4 2 #000000 2 1>
  <Line 20 -25 -4 0 #000000 2 1>
  <Line 19 -25 9 1 #000000 2 1>
  <Line -16 -16 -4 2 #000000 2 1>
  <Ellipse 20 10 0 0 #000000 0 1 #c0c0c0 1 1>
  <Ellipse 19 9 2 2 #000000 0 1 #000000 1 1>
  <Line 20 10 -14 -37 #ff0000 3 1>
  <Text -17 7 12 #aa0000 0 "In">
  <Text 31 8 12 #aa0000 0 "S22">
  <Text -17 -35 10 #000000 0 "0">
  <Text 51 -34 10 #000000 0 "1">
  <.PortSym -40 20 1 0>
  <.PortSym 80 20 2 180>
  <.ID -20 44 S22_Probe "1=Z0=50==">
</Symbol>
<Components>
  <GND * 1 640 360 0 0 0 0>
  <GND * 1 580 360 0 0 0 0>
  <VCVS SRC1 1 610 330 -16 53 0 0 "2" 1 "0" 0>
  <Port nP 1 580 300 -23 12 0 0 "1" 0 "analog" 0>
  <GND * 1 780 360 0 0 0 0>
  <Port nS22 1 810 300 4 -34 0 2 "2" 0 "analog" 0>
  <R R1 1 780 330 15 -26 0 1 "{Z0}" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <S4Q_V V1 1 670 300 -26 -64 0 2 "dc 0 ac 1" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <700 300 780 300 "" 0 0 0 "">
  <780 300 810 300 "nS22" 770 240 17 "">
  <580 300 580 300 "nP" 570 250 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
