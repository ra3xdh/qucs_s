<Qucs Schematic 0.0.17>
<Properties>
  <View=-20,78,848,615,0.87788,0,0>
  <Grid=10,10,1>
  <DataSet=opa227.dat>
  <DataDisplay=opa227.dpl>
  <OpenDisplay=0>
  <Script=opa227.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 44 OPA227_>
  <Line -20 -40 40 0 #000080 2 1>
  <Line 20 -40 0 80 #000080 2 1>
  <Line -20 40 40 0 #000080 2 1>
  <Line -20 -40 0 80 #000080 2 1>
  <.PortSym 30 0 3 0>
  <Line 20 0 10 0 #000080 2 1>
  <.PortSym -30 -30 2 0>
  <Line -30 -30 10 0 #000080 2 1>
  <Line -30 30 10 0 #000080 2 1>
  <.PortSym -30 30 1 0>
  <Text -10 -40 12 #000000 0 "-">
  <Text -10 20 12 #000000 0 "+\n">
</Symbol>
<Components>
  <VCVS SRC2 1 530 190 -26 34 0 0 "1" 1 "0" 0>
  <GND * 1 410 220 0 0 0 0>
  <R R1 1 360 160 -26 15 0 0 "1 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 440 190 17 -26 0 1 "1.98944 mF" 1 "" 0 "neutral" 0>
  <VCVS SRC1 1 260 190 -26 34 0 0 "1e8" 1 "0" 0>
  <Port P1 1 160 160 -23 12 0 0 "1" 1 "analog" 0>
  <Port P3 1 610 160 4 12 1 2 "3" 1 "analog" 0>
  <Port P2 1 160 220 -23 12 0 0 "2" 1 "analog" 0>
  <GND * 1 600 220 0 0 0 0>
</Components>
<Wires>
  <390 160 440 160 "" 0 0 0 "">
  <410 220 440 220 "" 0 0 0 "">
  <560 160 610 160 "" 0 0 0 "">
  <290 220 410 220 "" 0 0 0 "">
  <290 160 330 160 "" 0 0 0 "">
  <440 220 500 220 "" 0 0 0 "">
  <440 160 500 160 "" 0 0 0 "">
  <160 160 230 160 "" 0 0 0 "">
  <160 220 230 220 "" 0 0 0 "">
  <560 220 600 220 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
