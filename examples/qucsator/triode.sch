<Qucs Schematic 0.0.15>
<Properties>
  <View=-177,-148,553,365,1.33973,0,0>
  <Grid=10,10,1>
  <DataSet=triode.dat>
  <DataDisplay=triode.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line 30 0 -70 0 #00007f 2 2>
  <Line -40 0 -20 0 #00007f 2 1>
  <Line 0 -20 0 -40 #00007f 2 1>
  <Line 0 20 0 40 #00007f 2 1>
  <Ellipse -40 -40 80 80 #00007f 2 1 #c0c0c0 1 0>
  <Line -20 -20 40 0 #00007f 4 1>
  <Line -20 20 40 0 #00007f 4 1>
  <.PortSym -60 0 2 0>
  <.PortSym 0 60 3 0>
  <.PortSym 0 -60 1 0>
  <.ID 50 -16 SUB "1=u=70=Amplification factor=" "0=k=1=Amplification factor=" "0=RGK=5=Grid resistance when larger than Cathode=" "0=Cak=4 pF=Capacitance between Anode and Cathode=" "0=Cag=4 pF=Capacitance between Anode and Grid=" "0=Cgk=2.7 pF=Capacitance between Grid and Cathode=">
</Symbol>
<Components>
  <Port G 1 -110 20 -23 12 0 0 "2" 0 "analog" 0>
  <Port A 1 -110 70 -23 12 0 0 "1" 0 "analog" 0>
  <Port K 1 -110 290 -23 12 0 0 "3" 0 "analog" 0>
  <EDD D1 1 100 190 96 -26 0 3 "explicit" 0 "3" 0 "V1 > 0 ? V1/RGK : 0" 1 "Cgk*V1" 1 "(V1 + V2/u) >= 0 ? ((V1 + V2/u)^(3/2))/k : 0" 1 "Cak*V2" 1 "0" 1 "Cag*V3" 1>
</Components>
<Wires>
  <-110 20 40 20 "" 0 0 0 "">
  <100 220 100 290 "" 0 0 0 "">
  <100 290 160 290 "" 0 0 0 "">
  <160 220 160 290 "" 0 0 0 "">
  <160 20 160 160 "" 0 0 0 "">
  <-110 70 -20 70 "" 0 0 0 "">
  <100 70 100 160 "" 0 0 0 "">
  <-110 290 100 290 "" 0 0 0 "">
  <40 20 160 20 "" 0 0 0 "">
  <40 20 40 160 "" 0 0 0 "">
  <-20 70 100 70 "" 0 0 0 "">
  <-20 70 -20 220 "" 0 0 0 "">
  <-20 220 40 220 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
