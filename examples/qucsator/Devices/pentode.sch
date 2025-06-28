<Qucs Schematic 0.0.15>
<Properties>
  <View=43,172,907,564,1.13194,0,0>
  <Grid=10,10,1>
  <DataSet=pentode.dat>
  <DataDisplay=pentode.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line 0 -20 0 -40 #00007f 2 1>
  <Line 0 20 0 40 #00007f 2 1>
  <Ellipse -40 -40 80 80 #00007f 2 1 #c0c0c0 1 0>
  <Line 30 10 -70 0 #00007f 2 2>
  <Line -40 10 -20 0 #00007f 2 1>
  <Line -20 -20 40 0 #00007f 4 1>
  <Line -20 20 40 0 #00007f 4 1>
  <Line 40 0 -70 0 #00007f 2 2>
  <Line 60 0 -20 0 #00007f 2 1>
  <Line 30 -10 -60 0 #00007f 2 2>
  <.PortSym -60 10 2 0>
  <.PortSym 0 60 3 0>
  <.PortSym 0 -60 1 0>
  <.ID -40 84 SUB "1=u=9.5=Amplification factor=" "0=k=300=Gate adjustment factor=" "0=kvb=200=pentode adjustment  factor=" "0=RGK=5=Grid resistance=" "0=kg2=300=Screen adjustment factor=" "0=Cak=8 pF=Capacitanc between Anode and Cathode=" "0=Cgk=9.3 pF=Capacitanc between Grid and Cathode=" "0=Cag=0.3 pF=Capacitanc between Anode and Grid=">
  <.PortSym 60 0 4 180>
</Symbol>
<Components>
  <Port G 1 110 220 -23 12 0 0 "2" 0 "analog" 0>
  <Port K 1 110 490 -23 12 0 0 "3" 0 "analog" 0>
  <Port A 1 110 270 -23 12 0 0 "1" 0 "analog" 0>
  <Port S 1 110 320 -23 12 0 0 "4" 0 "analog" 0>
  <EDD D1 1 290 390 126 -26 0 3 "explicit" 0 "4" 0 "V1 > 0 ? V1/RGK : 0" 1 "Cgk*V1" 1 "(V1 + V3/u) >= 0 ? ((V1 + V3/u)^(3/2))*(2/k)*arctan(V2/kvb) : 0" 1 "Cak*V2" 1 "(V1 + V3/u) >= 0 ? ((V1 + V3/u)^(3/2))/kg2 : 0" 1 "0" 1 "0" 1 "Cag*V4" 1>
</Components>
<Wires>
  <110 220 200 220 "" 0 0 0 "">
  <320 420 320 490 "" 0 0 0 "">
  <320 490 380 490 "" 0 0 0 "">
  <380 420 380 490 "" 0 0 0 "">
  <380 220 380 360 "" 0 0 0 "">
  <260 490 320 490 "" 0 0 0 "">
  <260 420 260 490 "" 0 0 0 "">
  <110 490 260 490 "" 0 0 0 "">
  <320 270 320 360 "" 0 0 0 "">
  <110 270 150 270 "" 0 0 0 "">
  <110 320 260 320 "" 0 0 0 "">
  <260 320 260 360 "" 0 0 0 "">
  <200 220 380 220 "" 0 0 0 "">
  <200 220 200 360 "" 0 0 0 "">
  <150 270 320 270 "" 0 0 0 "">
  <150 270 150 420 "" 0 0 0 "">
  <150 420 200 420 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
