<Qucs Schematic 0.0.15>
<Properties>
  <View=0,-115,1188,800,1,0,32>
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
  <GND * 1 120 200 0 0 0 0>
  <IProbe I 1 150 40 -26 16 0 0>
  <VProbe V 1 280 -40 28 -31 0 0>
  <GND * 1 290 -10 0 0 0 0>
  <C C1 1 340 40 -26 17 0 0 "100u" 1 "0" 0 "neutral" 0>
  <R R1 1 230 40 -26 15 0 0 "1k" 1 "25" 1 "0.0" 0 "0.0" 0 "25" 0 "european" 0>
  <.TR TR1 1 130 250 0 79 0 0 "lin" 1 "0" 1 "4s" 1 "20001" 1 "Trapezoidal" 0 "2" 0 "0.1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vrect V1 1 120 120 18 -26 0 1 "1 V" 1 "2s" 1 "2s" 1 "0.01 ns" 0 "0.01 ns" 0 "0 ns" 0>
  <Eqn Eqn1 1 510 50 -33 19 0 0 "R=1k" 1 "C=100u" 1 "TC=25" 1 "computed=(time<2)*(1-exp(-time/(R*C)))+(1-exp(-time/(R*C)))*(time>=2)*(exp(-(time-2.0)/(R*C)))" 1 "diff=V.Vt-computed" 0 "assert=abs(diff)<1e-6" 0 "yes" 1>
</Components>
<Wires>
  <120 200 400 200 "" 0 0 0 "">
  <180 40 200 40 "" 0 0 0 "">
  <270 -20 270 40 "" 0 0 0 "">
  <290 -20 290 -10 "" 0 0 0 "">
  <260 40 270 40 "" 0 0 0 "">
  <400 40 400 200 "" 0 0 0 "">
  <370 40 400 40 "" 0 0 0 "">
  <270 40 310 40 "" 0 0 0 "">
  <120 150 120 200 "" 0 0 0 "">
  <120 40 120 90 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
