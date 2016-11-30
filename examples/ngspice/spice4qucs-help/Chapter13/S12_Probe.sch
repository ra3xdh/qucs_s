<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=S12_Probe.dat>
  <DataDisplay=S12_Probe.dpl>
  <OpenDisplay=1>
  <Script=S12_Probe.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Rectangle -40 -40 80 70 #aa5500 3 1 #c0c0c0 1 0>
  <Line -60 20 20 0 #000080 2 1>
  <Line -40 10 80 0 #000000 3 1>
  <Line 32 -16 4 2 #000000 2 1>
  <Line -36 -16 4 -2 #000000 2 1>
  <Line -32 -18 6 -2 #000000 2 1>
  <Line -26 -20 8 -2 #000000 2 1>
  <Line -18 -22 6 -2 #000000 2 1>
  <Line -12 -24 8 -1 #000000 2 1>
  <Line 8 -24 8 2 #000000 2 1>
  <Line 16 -22 6 2 #000000 2 1>
  <Line 22 -20 6 2 #000000 2 1>
  <Line 28 -18 4 2 #000000 2 1>
  <Line 36 -14 0 0 #000000 0 1>
  <Line 36 -14 4 2 #000000 2 1>
  <Line 0 -25 -4 0 #000000 2 1>
  <Line -1 -25 9 1 #000000 2 1>
  <Line -36 -16 -4 2 #000000 2 1>
  <Ellipse 0 10 0 0 #000000 0 1 #c0c0c0 1 1>
  <Ellipse -1 9 2 2 #000000 0 1 #000000 1 1>
  <Line 0 10 -14 -37 #ff0000 3 1>
  <Text -37 7 12 #aa5500 0 "In">
  <Text 11 8 12 #aa5500 0 "S12">
  <Text -37 -35 10 #000000 0 "0">
  <Text 31 -34 10 #000000 0 "1">
  <Line 40 20 20 0 #000080 2 1>
  <.PortSym -60 20 1 0>
  <.PortSym 60 20 2 180>
  <.ID -40 34 S12_Probe "1=Z0=50==">
</Symbol>
<Components>
  <GND * 1 570 380 0 0 1 2>
  <GND * 1 630 380 0 0 1 2>
  <VCVS SRC1 1 600 350 -23 69 1 2 "2" 1 "0" 0>
  <GND * 1 530 380 0 0 1 2>
  <Port nP 1 660 320 4 -34 0 2 "1" 0 "analog" 0>
  <Port nS12 1 480 320 -23 12 0 0 "2" 0 "analog" 0>
  <R R1 1 530 350 -68 11 0 1 "{Z0}" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <630 320 660 320 "" 0 0 0 "">
  <480 320 530 320 "" 0 0 0 "">
  <530 320 570 320 "" 0 0 0 "">
  <630 320 630 320 "nP" 620 270 0 "">
  <480 320 480 320 "nS12" 480 270 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
