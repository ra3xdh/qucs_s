<Qucs Schematic 0.0.19>
<Properties>
  <View=-104,-430,850,1317,0.767173,0,0>
  <Grid=10,10,1>
  <DataSet=bfp450_2.dat>
  <DataDisplay=bfp450_2.dpl>
  <OpenDisplay=1>
  <Script=bfp450_2.m>
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
  <SPfile X1 1 310 80 -26 -53 0 0 "BFP450_3V040M.s2p" 1 "rectangular" 0 "linear" 0 "open" 0 "2" 0>
  <GND * 1 310 110 0 0 0 0>
  <L L3 1 250 80 -26 10 0 0 "3.39nH" 1 "" 0>
  <L L10 1 280 10 -87 -26 0 3 "56nH" 1 "" 0>
  <R R7 1 410 -100 15 -26 0 1 "50Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 530 80 -26 -47 0 2 "3.5Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C10 1 430 80 -52 10 1 2 "41.1255pF" 1 "" 0 "neutral" 0>
  <C C5 1 80 80 -26 -49 0 2 "100n" 1 "" 0 "neutral" 0>
  <Pac P1 1 -10 110 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 -10 140 0 0 0 0>
  <C C7 1 630 80 -26 -49 0 2 "100n" 1 "" 0 "neutral" 0>
  <GND * 1 120 250 0 0 0 0>
  <R R1 1 120 220 15 -26 0 1 "0.2Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C3 1 120 160 17 -26 0 1 "5.68261pF" 1 "" 0 "neutral" 0>
  <GND * 1 580 230 0 0 0 0>
  <R R3 1 580 200 15 -26 0 1 "3Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L L13 1 580 140 -87 -26 0 3 "9.54728nH" 1 "" 0>
  <L L14 1 380 -170 -87 -26 0 3 "1000nH" 1 "" 0>
  <GND * 1 380 -200 0 0 0 2>
  <L L9 1 410 -40 -87 -26 0 3 "50nH" 1 "" 0>
  <GND * 1 560 -160 0 0 0 1>
  <C C8 1 530 -160 -52 10 1 2 "1.6pF" 1 "" 0 "neutral" 0>
  <Pac P5 1 690 110 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 690 140 0 0 0 0>
  <GND * 1 220 -90 0 0 0 1>
  <R R10 0 190 -90 -26 15 0 0 "4733 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 230 -170 0 0 0 1>
  <GND * 1 10 -170 0 0 0 3>
  <GND * 1 120 -230 0 0 0 2>
  <C C6 1 120 -200 -62 -26 0 3 "220n" 1 "" 0 "neutral" 0>
  <R R8 1 40 -170 -26 15 0 0 "87 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R9 1 200 -170 -26 15 0 0 "213 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 170 80 -26 -47 1 0 "0.20 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.SP SP1 1 -10 300 0 74 0 0 "lin" 1 "0.03 GHz" 1 "12 GHz" 1 "2000" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 160 310 -28 15 0 0 "K=StabFactor(S)" 1 "S11dB=dB(S[1,1])" 1 "S21dB=dB(S[2,1])" 1 "S22dB=dB(S[2,2])" 1 "Fmindb=w2dbm(Fmin)-30" 1 "Fdb=w2dbm(F)-30" 1 "yes" 0>
  <.SP SP2 0 290 270 0 74 0 0 "const" 1 "0.03 GHz" 0 "12 GHz" 0 "[900MHz]" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn2 0 480 270 -28 15 0 0 "GA=GaCircle(S,linspace(50,200,16))" 1 "GP=GpCircle(S,linspace(50,200,16))" 1 "SL=StabCircleL(S)" 1 "SG=StabCircleS(S)" 1 "NF=NoiseCircle(Sopt,Fmin,Rn,[Fmin,1.4,1.5,2,2.5,3])" 1 "yes" 0>
</Components>
<Wires>
  <280 40 280 80 "" 0 0 0 "">
  <560 80 580 80 "" 0 0 0 "">
  <340 80 380 80 "" 0 0 0 "">
  <460 80 500 80 "" 0 0 0 "">
  <200 80 220 80 "" 0 0 0 "">
  <120 80 140 80 "" 0 0 0 "">
  <110 80 120 80 "" 0 0 0 "">
  <-10 80 50 80 "" 0 0 0 "">
  <120 80 120 130 "" 0 0 0 "">
  <580 80 600 80 "" 0 0 0 "">
  <580 80 580 110 "" 0 0 0 "">
  <380 -70 410 -70 "" 0 0 0 "">
  <380 -140 380 -70 "" 0 0 0 "">
  <410 -160 500 -160 "" 0 0 0 "">
  <410 -160 410 -130 "" 0 0 0 "">
  <380 80 400 80 "" 0 0 0 "">
  <380 -10 380 80 "" 0 0 0 "">
  <380 -10 410 -10 "" 0 0 0 "">
  <660 80 690 80 "" 0 0 0 "">
  <120 -20 280 -20 "" 0 0 0 "">
  <120 -90 120 -20 "" 0 0 0 "">
  <120 -90 160 -90 "" 0 0 0 "">
  <120 -170 120 -90 "" 0 0 0 "">
  <70 -170 120 -170 "" 0 0 0 "">
  <120 -170 170 -170 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 230 20 12 #000000 0 "0.87V">
  <Text 420 -210 12 #000000 0 "This is the 3V supply">
  <Text 150 -240 12 #000000 0 "Voltage divider to \ndivide 3V to 0.87V">
  <Text 30 -9 12 #000000 90 "Alternatively \ndrop to 0.87V\nby passing \n0.45mA">
  <Text 100 -330 14 #000000 0 "Transistor Amplifier with Matching Networks\nand Biasing.">
  <Text 540 260 12 #ff0000 0 "Deactivate, if simulation other than\nSP2 is used">
</Paintings>
