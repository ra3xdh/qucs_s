<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=S21_Probe.dat>
  <DataDisplay=S21_Probe.dpl>
  <OpenDisplay=1>
  <Script=S21_Probe.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Rectangle -60 -140 80 70 #aa5500 3 1 #c0c0c0 1 0>
  <Line -80 -80 20 0 #000080 2 1>
  <Line -60 -90 80 0 #000000 3 1>
  <Line 12 -116 4 2 #000000 2 1>
  <Line -56 -116 4 -2 #000000 2 1>
  <Line -52 -118 6 -2 #000000 2 1>
  <Line -46 -120 8 -2 #000000 2 1>
  <Line -38 -122 6 -2 #000000 2 1>
  <Line -32 -124 8 -1 #000000 2 1>
  <Line -12 -124 8 2 #000000 2 1>
  <Line -4 -122 6 2 #000000 2 1>
  <Line 2 -120 6 2 #000000 2 1>
  <Line 8 -118 4 2 #000000 2 1>
  <Line 16 -114 0 0 #000000 0 1>
  <Line 16 -114 4 2 #000000 2 1>
  <Line -20 -125 -4 0 #000000 2 1>
  <Line -21 -125 9 1 #000000 2 1>
  <Line -56 -116 -4 2 #000000 2 1>
  <Ellipse -20 -90 0 0 #000000 0 1 #c0c0c0 1 1>
  <Ellipse -21 -91 2 2 #000000 0 1 #000000 1 1>
  <Line -20 -90 -14 -37 #ff0000 3 1>
  <Text -57 -93 12 #aa5500 0 "In">
  <Text -9 -92 12 #aa5500 0 "S21">
  <Text -57 -135 10 #000000 0 "0">
  <Text 11 -134 10 #000000 0 "1">
  <.PortSym -80 -80 1 0>
  <Line 20 -80 20 0 #000080 2 1>
  <.PortSym 40 -80 2 180>
  <.ID -60 -66 S21_Probe "1=Z0=50==">
</Symbol>
<Components>
  <VCVS SRC1 1 550 290 -20 58 0 0 "2" 1 "0" 0>
  <GND * 1 610 320 0 0 0 0>
  <GND * 1 580 320 0 0 0 0>
  <GND * 1 520 320 0 0 0 0>
  <Port nP 1 470 260 -23 12 0 0 "1" 0 "analog" 0>
  <Port nS21 1 670 260 4 12 1 2 "2" 0 "analog" 0>
  <R R1 1 610 290 15 -26 0 1 "{Z0}" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <580 260 610 260 "" 0 0 0 "">
  <470 260 520 260 "" 0 0 0 "">
  <610 260 670 260 "" 0 0 0 "">
  <470 260 470 260 "nP" 470 220 0 "">
  <670 260 670 260 "nS21" 630 220 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
