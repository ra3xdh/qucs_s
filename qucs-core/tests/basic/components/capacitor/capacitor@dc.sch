<Qucs Schematic 0.0.15>
<Properties>
  <View=0,-115,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=test.dat>
  <DataDisplay=test.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Titre>
  <FrameText1=Auteur :>
  <FrameText2=Date :>
  <FrameText3=Version :>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Vdc V1 1 120 130 18 -26 0 1 "1 V" 1>
  <.DC DC1 1 150 250 0 79 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 120 200 0 0 0 0>
  <IProbe I 1 150 40 -26 16 0 0>
  <R R1 1 230 40 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <VProbe U 1 280 -40 28 -31 0 0>
  <GND * 1 290 -10 0 0 0 0>
  <C C1 1 340 40 -26 17 0 0 "1 F" 1 "" 0 "neutral" 0>
  <Eqn Eqn1 1 470 60 -33 19 0 0 "assertI=assert(abs(I.I)<1e-16)" 1 "assertV=assert(abs(U.V-1.0)<1e-16)" 1 "yes" 0>
</Components>
<Wires>
  <120 200 400 200 "" 0 0 0 "">
  <120 160 120 200 "" 0 0 0 "">
  <120 40 120 100 "" 0 0 0 "">
  <180 40 200 40 "" 0 0 0 "">
  <270 -20 270 40 "" 0 0 0 "">
  <290 -20 290 -10 "" 0 0 0 "">
  <260 40 270 40 "" 0 0 0 "">
  <400 40 400 200 "" 0 0 0 "">
  <370 40 400 40 "" 0 0 0 "">
  <270 40 310 40 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
