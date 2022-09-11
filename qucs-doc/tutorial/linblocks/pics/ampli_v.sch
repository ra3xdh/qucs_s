<Qucs Schematic 0.0.18>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=ampli_v.dat>
  <DataDisplay=ampli_v.dpl>
  <OpenDisplay=1>
  <Script=ampli_v.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -60 -30 10 0 #000080 2 1>
  <Line -60 30 10 0 #000080 2 1>
  <Line -50 -40 0 80 #000080 2 1>
  <Line 40 -40 0 80 #000080 2 1>
  <Line 40 -30 10 0 #000080 2 1>
  <Line 40 30 10 0 #000080 2 1>
  <Line -50 40 0 30 #000000 0 1>
  <Line -50 70 90 -30 #000000 0 1>
  <Line -50 -70 90 30 #000000 0 1>
  <.ID -30 74 AV "1=Rin=1e6=input resistance=" "1=Rout=10=output resistance=" "1=Av=1=voltage amplification=">
  <Line -50 -40 0 -30 #000000 0 1>
  <Text -40 20 12 #000000 0 "in_{ref}">
  <Text 10 -40 12 #000000 0 "out">
  <Text -40 -50 12 #000000 0 "in">
  <Text -10 10 12 #000000 0 "out_{ref}">
  <.PortSym -60 -30 1 0>
  <.PortSym -60 30 2 0>
  <.PortSym 50 30 4 180>
  <.PortSym 50 -30 3 180>
</Symbol>
<Components>
  <Port In 1 110 100 -23 12 0 0 "1" 1 "analog" 0>
  <Port In_ref 1 120 280 -23 12 0 0 "2" 1 "analog" 0>
  <Port Out 1 430 100 4 12 1 2 "3" 1 "analog" 0>
  <Port Out_ref 1 430 280 4 12 1 2 "4" 1 "analog" 0>
  <VCVS SRC1 1 270 190 -26 34 0 0 "Av" 1 "0" 0>
  <R R2 1 180 190 -98 -17 0 3 "Rin" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R1 1 370 100 -26 15 0 0 "Rout" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <300 100 340 100 "" 0 0 0 "">
  <300 100 300 160 "" 0 0 0 "">
  <300 220 300 280 "" 0 0 0 "">
  <300 280 430 280 "" 0 0 0 "">
  <400 100 430 100 "" 0 0 0 "">
  <110 100 180 100 "" 0 0 0 "">
  <180 100 180 160 "" 0 0 0 "">
  <180 220 180 280 "" 0 0 0 "">
  <120 280 180 280 "" 0 0 0 "">
  <180 220 240 220 "" 0 0 0 "">
  <180 160 240 160 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
