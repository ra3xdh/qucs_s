<Qucs Schematic 0.0.19> 
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=single_phase_transformer.dat>
  <DataDisplay=single_phase_transformer.dpl>
  <OpenDisplay=1>
  <Script=single_phase_transformer.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line 60 -60 0 0 #000000 0 1>
  <Line 60 -10 0 0 #000000 0 1>
  <Line 80 40 -40 0 #00007f 2 1>
  <.PortSym 80 40 5 180>
  <Line 40 -30 10 0 #ff0000 3 1>
  <Line 50 -30 0 10 #ff0000 3 1>
  <Line 50 -20 -10 0 #ff0000 3 1>
  <Line 40 -20 0 10 #ff0000 3 1>
  <Line 40 -10 10 0 #ff0000 3 1>
  <Line 50 -10 0 10 #ff0000 3 1>
  <Line 50 0 -10 0 #ff0000 3 1>
  <Line 40 0 0 10 #ff0000 3 1>
  <Line 40 10 10 0 #ff0000 3 1>
  <Line 50 10 0 10 #ff0000 3 1>
  <Line 50 20 -10 0 #ff0000 3 1>
  <Line 60 70 0 0 #000000 0 1>
  <Line 60 120 0 0 #000000 0 1>
  <Line 40 70 10 0 #ff0000 3 1>
  <Line 50 70 0 10 #ff0000 3 1>
  <Line 50 80 -10 0 #ff0000 3 1>
  <Line 40 80 0 10 #ff0000 3 1>
  <Line 40 90 10 0 #ff0000 3 1>
  <Line 50 90 0 10 #ff0000 3 1>
  <Line 50 100 -10 0 #ff0000 3 1>
  <Line 40 100 0 10 #ff0000 3 1>
  <Line 40 110 10 0 #ff0000 3 1>
  <Line 50 110 0 10 #ff0000 3 1>
  <Line 50 120 -10 0 #ff0000 3 1>
  <Line 40 70 0 -10 #ff0000 3 1>
  <Line 40 20 0 40 #ff0000 3 1>
  <Line 40 -30 0 -20 #ff0000 3 1>
  <Line 40 -50 40 0 #000080 2 1>
  <.PortSym 80 -50 3 180>
  <Line 40 120 0 20 #ff0000 3 1>
  <Line 40 140 40 0 #000080 2 1>
  <.PortSym 80 140 4 180>
  <Line 30 -30 0 150 #000000 2 1>
  <Line 24 -30 0 150 #000000 2 1>
  <Line 18 -30 0 150 #000000 2 1>
  <.PortSym -50 140 2 0>
  <Line 20 10 0 0 #000000 0 1>
  <Line 20 60 0 0 #000000 0 1>
  <Line 0 10 10 0 #ff0000 3 1>
  <Line 10 10 0 10 #ff0000 3 1>
  <Line 10 20 -10 0 #ff0000 3 1>
  <Line 0 20 0 10 #ff0000 3 1>
  <Line 0 30 10 0 #ff0000 3 1>
  <Line 10 30 0 10 #ff0000 3 1>
  <Line 10 40 -10 0 #ff0000 3 1>
  <Line 0 40 0 10 #ff0000 3 1>
  <Line 0 50 10 0 #ff0000 3 1>
  <Line 10 50 0 10 #ff0000 3 1>
  <Line 10 60 -10 0 #ff0000 3 1>
  <Line 0 10 0 -60 #ff0000 3 1>
  <Line 0 60 0 80 #ff0000 3 1>
  <Line -50 140 50 0 #000080 2 1>
  <Ellipse 4 -2 8 8 #ff0000 1 1 #ff0000 1 1>
  <Ellipse 44 -42 8 8 #ff0000 1 1 #ff0000 1 1>
  <Ellipse 44 58 8 8 #ff0000 1 1 #ff0000 1 1>
  <.ID -28 152 TRAN "1=L1=1m==" "1=L2=1m==" "1=L3=1m==" "1=K12=0.999==" "1=K13=0.999==" "1=K23=0.999==">
  <Line -40 -50 40 0 #000080 2 1>
  <.PortSym -40 -50 1 0>
</Symbol>
<Components>
  <Port P3 1 440 140 -10 -53 0 3 "3" 0 "analog" 0>
  <Port P5 1 440 230 4 -29 0 2 "5" 0 "analog" 0>
  <Port P4 1 440 320 -10 35 0 1 "4" 0 "analog" 0>
  <Port P2 1 390 320 -11 33 0 1 "2" 0 "analog" 0>
  <Port P1 1 390 140 -10 -51 0 3 "1" 0 "analog" 0>
  <L_SPICE L1 1 390 230 -118 -22 1 3 "L1=L1" 1 "=" 0 "=" 0 "=" 0 "=" 0>
  <L_SPICE L2 1 440 170 10 -26 1 3 "L2=L2" 1 "=" 0 "=" 0 "=" 0 "=" 0>
  <L_SPICE L3 1 440 290 10 -26 1 3 "L3=L3" 1 "=" 0 "=" 0 "=" 0 "=" 0>
  <K_SPICE K13 1 410 400 -26 17 0 0 "L1=L1" 1 "L3=L3" 1 "K13" 1>
  <K_SPICE K12 1 330 400 -26 17 0 0 "L1=L1" 1 "L2=L2" 1 "K12" 1>
  <K_SPICE K23 1 480 400 -26 17 0 0 "L2=L2" 1 "L3=L3" 1 "K23" 1>
</Components>
<Wires>
  <440 200 440 230 "" 0 0 0 "">
  <440 230 440 260 "" 0 0 0 "">
  <390 260 390 320 "" 0 0 0 "">
  <390 140 390 200 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
