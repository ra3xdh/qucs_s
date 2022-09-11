<Qucs Schematic 0.0.18>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=bfp450_ac0.dat>
  <DataDisplay=bfp450_ac0.dpl>
  <OpenDisplay=1>
  <Script=bfp450_ac0.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <.DC DC1 1 70 410 0 39 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Vdc V1 1 40 180 18 -26 0 1 "UBE" 1>
  <IProbe Pr2 1 100 150 -26 16 0 0>
  <GND * 1 280 240 0 0 0 0>
  <SPICE X1 1 280 180 -26 -83 1 2 "BFP450_spice.cir" 1 "_net1,_net2,_net3" 0 "yes" 0 "none" 0>
  <GND * 1 310 210 0 0 0 0>
  <Vdc V2 1 510 180 18 -26 0 1 "UCE" 1>
  <GND * 1 510 210 0 0 0 0>
  <Vac V3 1 40 240 18 -26 0 1 "0.0001 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <R R1 1 40 300 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 40 330 0 0 0 0>
  <IProbe Pr1 1 410 150 -26 16 1 2>
  <R R2 1 470 150 -26 -47 0 2 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Eqn Eqn1 1 370 410 -28 15 0 0 "S11Z=(-50+VBE.v/Pr2.i)/(50+VBE.v/Pr2.i)" 1 "UCE=5" 1 "S21=VCE.v/VBE.v" 1 "UBE=0.9" 1 "yes" 0>
  <.AC AC1 1 70 520 0 39 0 0 "lin" 1 "100MHz" 1 "2 GHz" 1 "200" 1 "no" 0>
</Components>
<Wires>
  <40 150 70 150 "" 0 0 0 "">
  <130 150 250 150 "VBE" 210 120 52 "">
  <500 150 510 150 "" 0 0 0 "">
  <310 150 380 150 "VCE" 370 120 29 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
