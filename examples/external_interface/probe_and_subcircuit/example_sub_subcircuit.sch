<Qucs Schematic 0.0.18>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=example_sub_subcircuit.dat>
  <DataDisplay=example_sub_subcircuit.dpl>
  <OpenDisplay=1>
  <Script=example_sub_subcircuit.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym 40 20 1 0>
  <.PortSym 40 60 2 0>
</Symbol>
<Components>
  <Port P1 1 130 90 -23 12 0 0 "1" 1 "analog" 0>
  <R R1 1 200 260 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port P2 1 130 430 -23 12 0 0 "2" 1 "analog" 0>
  <Sub SUB1 1 200 140 21 -26 1 3 "example_subcircuit" 1>
  <Sub SUB2 1 200 370 21 -26 0 1 "example_subcircuit" 1>
</Components>
<Wires>
  <130 90 200 90 "" 0 0 0 "">
  <200 90 200 110 "" 0 0 0 "">
  <200 170 200 230 "" 0 0 0 "">
  <200 290 200 340 "" 0 0 0 "">
  <200 400 200 430 "" 0 0 0 "">
  <130 430 200 430 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
