<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-13,800,569,1,0,0>
  <Grid=10,10,1>
  <DataSet=quarz.dat>
  <DataDisplay=quarz.dpl>
  <OpenDisplay=1>
  <Script=quarz.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Название>
  <FrameText1=Чертил:>
  <FrameText2=Дата:>
  <FrameText3=Версия:>
</Properties>
<Symbol>
  <Line 10 0 30 0 #000000 0 1>
  <Line -40 0 30 0 #000000 0 1>
  <.PortSym -40 0 1 0>
  <.PortSym 40 0 2 180>
  <Line -10 -20 0 40 #000000 0 1>
  <Line 10 -20 0 40 #000000 0 1>
  <Rectangle -5 -30 10 60 #000000 0 1 #c0c0c0 1 0>
  <.ID -40 34 ZQ "1=f=8863k=Crystal Frequency=" "1=Lq=0.01406=Crystal Inductance=" "1=Cs=6.5p=Crystal Capacitance=">
</Symbol>
<Components>
  <R R1 1 320 280 -26 15 0 0 "50 MOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port P1 1 170 140 -23 12 0 0 "1" 1 "analog" 0>
  <Port P2 1 510 140 4 12 1 2 "2" 1 "analog" 0>
  <C C2 1 310 190 -26 17 0 0 "Cs" 1 "" 0 "neutral" 0>
  <R R2 1 380 90 -26 15 0 0 "2 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L L1 1 320 90 -26 -52 1 0 "Lq" 1 "" 0>
  <Eqn Eqn1 1 150 340 -38 17 0 0 "Cq=1/(4*pi^2*f^2*Lq)" 1 "yes" 0>
  <C C1 1 260 90 -26 17 0 0 "Cq" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <210 190 280 190 "" 0 0 0 "">
  <210 90 230 90 "" 0 0 0 "">
  <410 90 450 90 "" 0 0 0 "">
  <340 190 450 190 "" 0 0 0 "">
  <210 280 290 280 "" 0 0 0 "">
  <210 190 210 280 "" 0 0 0 "">
  <350 280 450 280 "" 0 0 0 "">
  <450 190 450 280 "" 0 0 0 "">
  <210 90 210 140 "" 0 0 0 "">
  <210 140 210 190 "" 0 0 0 "">
  <170 140 210 140 "" 0 0 0 "">
  <450 90 450 140 "" 0 0 0 "">
  <450 140 450 190 "" 0 0 0 "">
  <450 140 510 140 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
