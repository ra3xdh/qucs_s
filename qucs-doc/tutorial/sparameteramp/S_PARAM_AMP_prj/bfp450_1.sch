<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,922,1130,1,0,0>
  <Grid=10,10,1>
  <DataSet=bfp450_1.dat>
  <DataDisplay=bfp450_1.dpl>
  <OpenDisplay=1>
  <Script=bfp450_1.m>
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
  <Pac P1 1 140 150 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 140 180 0 0 0 0>
  <.SP SP1 1 110 250 0 71 0 0 "lin" 1 "0.03 GHz" 1 "12 GHz" 1 "200" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.SP SP2 0 110 430 0 71 0 0 "const" 1 "0.03 GHz" 0 "12 GHz" 0 "[900MHz]" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.SP SP3 0 110 570 0 71 0 0 "lin" 1 "100MHz" 1 "2000MHz" 1 "200" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 310 250 -28 15 0 0 "K=StabFactor(S)" 1 "Fmindb=w2dbm(Fmin)-30" 1 "Fdb=w2dbm(F)-30" 1 "yes" 0>
  <Eqn Eqn2 0 320 430 -28 15 0 0 "GA=GaCircle(S,linspace(50,200,16))" 1 "GP=GpCircle(S,linspace(50,200,16))" 1 "SL=StabCircleL(S)" 1 "SG=StabCircleS(S)" 1 "NF=NoiseCircle(Sopt,Fmin,Rn,[Fmin,1.4,1.5,2,2.5,3])" 1 "yes" 0>
  <Pac P2 1 570 150 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 570 180 0 0 0 0>
  <GND * 1 350 150 0 0 0 0>
  <SPfile X1 1 350 120 -26 -53 0 0 "BFP450_3V040M.s2p" 1 "rectangular" 0 "linear" 0 "open" 0 "2" 0>
</Components>
<Wires>
  <380 120 570 120 "" 0 0 0 "">
  <140 120 320 120 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 390 410 12 #ff0000 0 "Deactivate, if simulation other than\nSP2 is used">
  <Text 190 30 14 #000000 0 "Check of S-Parameters of Bare Transistor">
</Paintings>
