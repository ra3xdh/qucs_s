<Qucs Schematic 0.0.19> 
<Properties>
  <View=350,-48,803,402,1,0,0>
  <Grid=10,10,1>
  <DataSet=Transformer_positive_coupling.dat>
  <DataDisplay=Transformer_positive_coupling.dpl>
  <OpenDisplay=1>
  <Script=Transformer_positive_coupling.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line 10 30 0 0 #000000 0 1>
  <Line 10 80 0 0 #000000 0 1>
  <Line 30 30 -10 0 #ff0000 3 1>
  <Line 20 30 0 10 #ff0000 3 1>
  <Line 20 40 10 0 #ff0000 3 1>
  <Line 30 40 0 10 #ff0000 3 1>
  <Line 30 50 -10 0 #ff0000 3 1>
  <Line 20 50 0 10 #ff0000 3 1>
  <Line 20 60 10 0 #ff0000 3 1>
  <Line 30 60 0 10 #ff0000 3 1>
  <Line 30 70 -10 0 #ff0000 3 1>
  <Line 20 70 0 10 #ff0000 3 1>
  <Line 20 80 10 0 #ff0000 3 1>
  <Line 30 30 0 -10 #ff0000 3 1>
  <Line 30 80 0 10 #ff0000 3 1>
  <Line -10 30 10 0 #ff0000 3 1>
  <Line 0 30 0 10 #ff0000 3 1>
  <Line 0 40 -10 0 #ff0000 3 1>
  <Line -10 40 0 10 #ff0000 3 1>
  <Line -10 50 10 0 #ff0000 3 1>
  <Line 0 50 0 10 #ff0000 3 1>
  <Line 0 60 -10 0 #ff0000 3 1>
  <Line -10 60 0 10 #ff0000 3 1>
  <Line -10 70 10 0 #ff0000 3 1>
  <Line 0 70 0 10 #ff0000 3 1>
  <Line 0 80 -10 0 #ff0000 3 1>
  <Line -10 30 0 -10 #ff0000 3 1>
  <Line -10 80 0 10 #ff0000 3 1>
  <Line 15 30 0 50 #000000 2 1>
  <Line 6 30 0 50 #000000 2 1>
  <Line 11 30 0 50 #000000 2 1>
  <Line -20 20 10 0 #000080 2 1>
  <Line 30 20 10 0 #000080 2 1>
  <Line -20 90 10 0 #000080 2 1>
  <Line 30 90 10 0 #00007f 2 1>
  <.PortSym 40 20 3 180>
  <.PortSym 40 90 4 180>
  <.ID -20 114 TRAN "1=K=0.999==" "1=L1=0.5m==" "1=L2=0.5m==">
  <Ellipse -6 18 8 8 #ff0000 1 1 #ff0000 1 1>
  <Ellipse 18 18 8 8 #ff0000 1 1 #ff0000 1 1>
  <.PortSym -20 20 2 0>
  <.PortSym -20 90 1 0>
</Symbol>
<Components>
  <Port PL2plus 1 670 220 35 -12 1 2 "3" 0 "analog" 0>
  <Port PL2neg 1 670 280 33 -11 1 2 "4" 0 "analog" 0>
  <Port PL1plus 1 480 220 -91 -10 0 0 "2" 0 "analog" 0>
  <Port PL1neg 1 480 280 -87 -6 0 0 "1" 0 "analog" 0>
  <L_SPICE L1 1 570 250 -93 -26 0 3 "L1=L1" 1 "=" 0 "=" 0 "=" 0 "=" 0>
  <L_SPICE L2 1 620 250 10 -26 1 3 "L2=L2" 1 "=" 0 "=" 0 "=" 0 "=" 0>
  <K_SPICE K1 1 593 290 -26 17 0 0 "L1=L1" 1 "L2=L2" 1 "K" 1>
</Components>
<Wires>
  <620 220 670 220 "" 0 0 0 "">
  <620 280 670 280 "" 0 0 0 "">
  <480 220 570 220 "PL1plus" 510 180 19 "">
  <480 280 570 280 "PL1neg" 470 300 22 "">
  <670 220 670 220 "PL2plus" 620 190 0 "">
  <670 280 670 280 "PL2neg" 630 300 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
