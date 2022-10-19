<Qucs Schematic 0.0.24>
<Properties>
  <View=-14,-193,863,667,1,0,29>
  <Grid=10,10,1>
  <DataSet=fet.dat>
  <DataDisplay=fet.dpl>
  <OpenDisplay=1>
  <Script=fet.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line 0 -30 0 40 #000080 4 1>
  <Arrow -20 0 20 0 10 6 #00007f 2 1 0>
  <Line 0 0 20 0 #000080 2 1>
  <Line 0 -20 40 0 #000080 2 1>
  <.PortSym -20 0 1 0>
  <Line 20 20 0 -20 #000080 2 1>
  <.PortSym 20 20 3 0>
  <EArc 15 -40 10 10 4169 3161 #00007f 1 1>
  <EArc 15 -50 10 10 4169 3161 #00007f 1 1>
  <EArc 15 -60 10 10 4169 3161 #00007f 1 1>
  <Line 20 -20 0 -10 #000080 2 1>
  <Line 20 -60 0 -10 #000080 2 1>
  <Ellipse 15 -80 10 10 #00007f 2 1 #c0c0c0 1 0>
  <.PortSym 40 -20 2 180>
  <.ID 50 4 SUB>
  <Text 0 -100 12 #ff0000 0 "+Vdd">
</Symbol>
<Components>
  <C C3 1 190 140 17 -25 0 1 "234 fF" 1 "" 0 "neutral" 0>
  <VCCS SRC1 1 300 140 -25 34 0 0 "0.069 S" 1 "1.69 ps" 1>
  <C C4 1 100 140 17 -25 0 1 "5 fF" 1 "" 0 "neutral" 0>
  <C C1 1 230 50 -25 17 0 0 "39 fF" 1 "" 0 "neutral" 0>
  <C C2 1 530 150 17 -25 0 1 "31 fF" 1 "" 0 "neutral" 0>
  <L L2 1 560 50 -25 10 0 0 "16.7 pH" 1 "" 0>
  <Port P2 1 590 50 4 -42 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <L L1 1 70 50 -25 10 0 0 "6 pH" 1 "" 0>
  <Port P1 1 40 50 -23 -42 1 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <R R1 1 140 50 -25 13 0 0 "0.9 Ohm" 1 "16.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 190 200 13 -25 0 1 "1.9" 1 "16.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 300 50 -25 13 0 0 "3.6 Ohm" 1 "16.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R6 1 480 50 -25 -43 1 0 "2.5" 1 "16.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C5 1 450 110 17 -25 0 1 "14 fF" 1 "" 0 "neutral" 0>
  <R R5 1 260 260 13 -25 0 1 "2.4 Ohm" 1 "16.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L3 1 260 320 10 -25 0 1 "6.4 pH" 1 "" 0>
  <Port P3 1 260 350 12 4 0 1 "3" 1 "analog" 0 "v" 0 "" 0>
  <R R3 1 360 140 13 -25 0 1 "312" 1 "6000" 1 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <190 50 190 110 "" 0 0 0 "">
  <190 110 270 110 "" 0 0 0 "">
  <330 110 360 110 "" 0 0 0 "">
  <330 170 360 170 "" 0 0 0 "">
  <190 170 270 170 "" 0 0 0 "">
  <170 50 190 50 "" 0 0 0 "">
  <100 50 110 50 "" 0 0 0 "">
  <100 50 100 110 "" 0 0 0 "">
  <190 230 260 230 "" 0 0 0 "">
  <100 290 260 290 "" 0 0 0 "">
  <100 170 100 290 "" 0 0 0 "">
  <190 50 200 50 "" 0 0 0 "">
  <260 50 270 50 "" 0 0 0 "">
  <330 50 360 50 "" 0 0 0 "">
  <360 50 450 50 "" 0 0 0 "">
  <360 50 360 110 "" 0 0 0 "">
  <360 170 360 230 "" 0 0 0 "">
  <510 50 530 50 "" 0 0 0 "">
  <530 180 530 290 "" 0 0 0 "">
  <530 50 530 120 "" 0 0 0 "">
  <450 140 450 170 "" 0 0 0 "">
  <360 170 450 170 "" 0 0 0 "">
  <450 50 450 80 "" 0 0 0 "">
  <260 290 530 290 "" 0 0 0 "">
  <260 230 360 230 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 370 340 12 #000000 0 "small-signal equivalent circuit of a microwave FET\nThe temperature of all resistors is set to 16.85�C\n(290K) which is the IEEE standard temperature for\nnoise figure measurement. This means their noise\nis thermal noise. An exception is R3 whose\nelectron gas is highly overheated (diffusion noise).">
  <Text 180 -20 16 #ff0000 0 "AC, SP analysis only. No DC">
</Paintings>
