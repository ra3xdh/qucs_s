<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=S11_Probe.dat>
  <DataDisplay=S11_Probe.dpl>
  <OpenDisplay=1>
  <Script=S11_Probe.m>
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
  <Line 40 20 20 0 #000080 2 1>
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
  <Text 11 8 12 #aa5500 0 "S11">
  <Text -37 -35 10 #000000 0 "0">
  <Text 31 -34 10 #000000 0 "1">
  <.PortSym -60 20 1 0>
  <.PortSym 60 20 2 180>
  <.ID -40 34 S11_Probe "1=Z0=50==">
</Symbol>
<Components>
  <GND * 1 610 310 0 0 0 0>
  <GND * 1 500 310 0 0 0 0>
  <GND * 1 440 310 0 0 0 0>
  <VCVS SRC1 1 470 280 -21 51 0 0 "2" 1 "0" 0>
  <Port nS11 1 700 250 4 -35 0 2 "2" 0 "analog" 0>
  <S4Q_V V1 1 550 250 -26 -64 0 2 "DC 0 AC 1" 1 "" 0 "" 0 "" 0 "" 0>
  <Port nP 1 400 250 -23 12 0 0 "1" 0 "analog" 0>
  <R R1 1 610 280 15 -26 0 1 "{Z0}" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <400 250 440 250 "" 0 0 0 "">
  <580 250 610 250 "" 0 0 0 "">
  <500 250 520 250 "" 0 0 0 "">
  <610 250 700 250 "nS11" 640 200 57 "">
  <400 250 400 250 "nP" 390 200 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
