<Qucs Schematic 24.1.0>
<Properties>
  <View=-391,-130,738,545,1,0,0>
  <Grid=10,10,1>
  <DataSet=Statz_sub.dat>
  <DataDisplay=Statz_sub.dpl>
  <OpenDisplay=1>
  <Script=Statz_sub.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym -30 0 2 0>
  <.ID 20 -16 Z>
  <.PortSym 0 -30 1 0>
  <.PortSym 0 30 3 0>
  <Line -10 -15 0 30 #800000 3 1>
  <Line -10 -10 10 0 #800000 3 1>
  <Line -10 10 10 0 #800000 3 1>
  <Line -4 24 8 -4 #000080 2 1>
  <Line -16 -5 5 5 #800000 3 1>
  <Line -16 5 5 -5 #800000 3 1>
  <Line -20 0 10 0 #800000 3 1>
  <Line -30 0 10 0 #000080 3 1>
  <Line 0 10 0 10 #800000 3 1>
  <Line 0 20 0 10 #000080 3 1>
  <Line 0 -10 0 -10 #800000 3 1>
  <Line 0 -20 0 -10 #000080 3 1>
</Symbol>
<Components>
  <Port D 1 100 140 -46 -10 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port G 1 200 140 33 -11 1 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <Port S 1 100 200 -46 -10 0 0 "3" 1 "analog" 0 "v" 0 "" 0>
  <SpLib X1 1 150 170 -19 54 0 0 "Statz_dc_mod.cir" 1 "STATZ" 1 "auto" 0 "" 0>
</Components>
<Wires>
  <100 140 120 140 "" 0 0 0 "">
  <100 200 120 200 "" 0 0 0 "">
  <180 140 200 140 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 50 80 12 #aa0000 0 "Correct model file location!">
</Paintings>
