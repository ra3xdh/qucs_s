<Qucs Schematic 0.0.19>
<Properties>
  <View=-104,-450,882,1617,0.767173,0,53>
  <Grid=10,10,1>
  <DataSet=bfp450_1a_match.dat>
  <DataDisplay=bfp450_1a_match.dpl>
  <OpenDisplay=1>
  <Script=bfp450_1a_match.m>
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
  <GND * 1 310 110 0 0 0 0>
  <L L3 1 250 80 -26 10 0 0 "3.39628nH" 1 "" 0>
  <R R4 2 530 80 -26 -47 0 2 "3.5Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C10 1 430 80 -52 10 1 2 "41.1255pF" 1 "" 0 "neutral" 0>
  <R R2 2 170 80 -26 15 0 0 "0.20 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C5 1 80 80 -26 -49 0 2 "100n" 1 "" 0 "neutral" 0>
  <Pac P1 1 -10 110 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 -10 140 0 0 0 0>
  <C C7 1 630 80 -26 -49 0 2 "100n" 1 "" 0 "neutral" 0>
  <Pac P5 1 700 110 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 700 140 0 0 0 0>
  <GND * 1 120 250 0 0 0 0>
  <R R1 2 120 220 15 -26 0 1 "0.2Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C3 1 120 160 17 -26 0 1 "5.68261pF" 1 "" 0 "neutral" 0>
  <GND * 1 580 230 0 0 0 0>
  <R R3 2 580 200 15 -26 0 1 "3Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L L13 1 580 140 -87 -26 0 3 "9.54728nH" 1 "" 0>
  <.SP SP2 1 0 310 0 74 0 0 "const" 1 "0.03 GHz" 0 "12 GHz" 0 "[900MHz]" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 200 310 -28 15 0 0 "K=StabFactor(S)" 1 "S11dB=dB(S[1,1])" 1 "S21dB=dB(S[2,1])" 1 "S22dB=dB(S[2,2])" 1 "Fmindb=w2dbm(Fmin)-30" 1 "Fdb=w2dbm(F)-30" 1 "yes" 0>
  <Eqn Eqn2 1 420 310 -28 15 0 0 "GA=GaCircle(S,linspace(50,200,16))" 1 "GP=GpCircle(S,linspace(50,200,16))" 1 "SL=StabCircleL(S)" 1 "SG=StabCircleS(S)" 1 "NF=NoiseCircle(Sopt,Fmin,Rn,[Fmin,1.4,1.5,2,2.5,3])" 1 "yes" 0>
  <SPfile X1 1 310 80 -26 -53 0 0 "BFP450_3V040M.s2p" 1 "rectangular" 0 "linear" 0 "open" 0 "2" 0>
</Components>
<Wires>
  <560 80 580 80 "" 0 0 0 "">
  <340 80 400 80 "" 0 0 0 "">
  <460 80 500 80 "" 0 0 0 "">
  <200 80 220 80 "" 0 0 0 "">
  <120 80 140 80 "" 0 0 0 "">
  <110 80 120 80 "" 0 0 0 "">
  <-10 80 50 80 "" 0 0 0 "">
  <660 80 700 80 "" 0 0 0 "">
  <120 80 120 130 "" 0 0 0 "">
  <580 80 600 80 "" 0 0 0 "">
  <580 80 580 110 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 140 -90 14 #000000 0 "Small Signal Simulation for Transistor with \nMatching Networks.\n(try out resistors to observe stability bahaviour)">
</Paintings>
