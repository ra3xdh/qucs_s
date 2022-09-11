<Qucs Schematic 0.0.17>
<Properties>
  <View=68,40,560,310,1.72561,0,0>
  <Grid=5,5,0>
  <DataSet=timer_trig.dat>
  <DataDisplay=timer_trig.dpl>
  <OpenDisplay=1>
  <Script=timer_trig.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -30 30 10 0 #000080 2 1>
  <Line -20 -60 0 110 #000080 2 1>
  <Line -20 -60 90 55 #000080 2 1>
  <Line -20 50 90 -55 #000080 2 1>
  <Line 70 -5 10 0 #000080 2 1>
  <.PortSym 80 -5 3 180>
  <Line -30 -40 10 0 #000080 2 1>
  <.PortSym -30 30 2 0>
  <.PortSym -30 -40 1 0>
  <Line -10 10 20 0 #00007f 2 1>
  <Line 10 10 0 -20 #00007f 2 1>
  <Line 10 -10 20 0 #00007f 2 1>
  <Text -10 -25 12 #000000 0 "TRIG">
  <.ID 5 39 SUB>
  <Text -15 -45 16 #000000 0 "+">
  <Text -15 20 20 #000000 0 "-">
</Symbol>
<Components>
  <Port Pcomp_vp1 1 140 140 -23 12 0 0 "1" 0 "analog" 0>
  <Port Pcomp_vn1 1 140 200 -23 12 0 0 "2" 0 "analog" 0>
  <GND * 1 120 80 0 0 0 0>
  <Idc I1 1 160 110 18 -26 1 3 "500 nA" 1>
  <R R4 1 350 165 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 410 245 0 0 0 0>
  <Port comp_vout1 1 440 165 4 -28 0 2 "3" 0 "analog" 0>
  <OpAmp OP1 1 240 165 -26 42 0 0 "1e6" 1 "1 V" 1>
  <C C1 1 410 210 17 -26 0 1 "1 nF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <140 140 160 140 "" 0 0 0 "">
  <140 200 175 200 "" 0 0 0 "">
  <160 140 175 140 "" 0 0 0 "">
  <120 80 160 80 "" 0 0 0 "">
  <175 145 210 145 "" 0 0 0 "">
  <175 140 175 145 "" 0 0 0 "">
  <175 185 210 185 "" 0 0 0 "">
  <175 185 175 200 "" 0 0 0 "">
  <280 165 320 165 "" 0 0 0 "">
  <380 165 410 165 "" 0 0 0 "">
  <410 240 410 245 "" 0 0 0 "">
  <410 165 440 165 "" 0 0 0 "">
  <410 165 410 180 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
