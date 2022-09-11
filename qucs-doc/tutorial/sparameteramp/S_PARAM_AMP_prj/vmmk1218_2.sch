<Qucs Schematic 0.0.18>
<Properties>
  <View=0,-120,890,1532,1,84,0>
  <Grid=10,10,1>
  <DataSet=vmmk1218_2.dat>
  <DataDisplay=vmmk1218_2.dpl>
  <OpenDisplay=1>
  <Script=vmmk1218_2.m>
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
  <SPfile X1 1 350 120 -26 -53 0 0 "VMMK1218_2V20mA.s2p" 1 "rectangular" 0 "linear" 0 "open" 0 "2" 0>
  <MVIA MS4 1 300 300 -123 0 1 2 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <MVIA MS1 1 300 210 -123 0 1 2 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <TLIN Line2 1 350 270 20 -26 0 1 "60 Ohm" 1 "0.636 mm" 1 "0 dB" 0 "26.85" 0>
  <MVIA MS6 1 540 210 20 0 0 0 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <MVIA MS8 1 540 300 20 0 0 0 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <TLIN Line1 1 350 180 20 -26 0 1 "60 Ohm" 1 "0.317 mm" 1 "0 dB" 0 "26.85" 0>
  <.SP SP3 0 120 700 0 63 0 0 "lin" 1 "100MHz" 1 "2000MHz" 1 "200" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.SP SP2 1 120 560 0 63 0 0 "const" 1 "0.03 GHz" 0 "12 GHz" 0 "[11GHz]" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn2 1 330 560 -28 15 0 0 "GA=GaCircle(S,linspace(1,20,20))" 1 "GP=GpCircle(S,linspace(1,20,20))" 1 "SL=StabCircleL(S)" 1 "SG=StabCircleS(S)" 1 "NF=NoiseCircle(Sopt,Fmin,Rn,[Fmin,1.2, 1.25, 1.3,1.4,1.5])" 1 "yes" 0>
  <TLIN Line6 1 270 120 -26 -68 0 2 "50 Ohm" 1 "2.7 mm" 1 "0 dB" 0 "26.85" 0>
  <TLIN Line5 1 220 50 -86 -26 1 1 "50 Ohm" 1 "2.57 mm" 1 "0 dB" 0 "26.85" 0>
  <Eqn Eqn1 1 320 380 -28 15 0 0 "K=StabFactor(S)" 1 "S11db=dB(S[1,1])" 1 "S21db=dB(S[2,1])" 1 "S22db=dB(S[2,2])" 1 "Fmindb=w2dbm(Fmin)-30" 1 "Fdb=w2dbm(F)-30" 1 "yes" 0>
  <.SP SP1 0 120 380 0 63 0 0 "lin" 1 "0.03 GHz" 1 "30 GHz" 1 "200" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <TLIN Line3 1 490 180 20 -26 1 3 "60 Ohm" 0 "0.317 mm" 0 "0 dB" 0 "26.85" 0>
  <TLIN Line4 1 490 270 20 -26 0 1 "60 Ohm" 0 "0.636 mm" 0 "0 dB" 0 "26.85" 0>
  <SUBST RO4350_1 1 650 420 -30 24 0 0 "3.48" 1 "0.254 mm" 1 "35 um" 1 "0.004" 1 "0.022e-6" 1 "1.4e-6" 1>
  <Pac P2 1 740 150 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 740 170 0 0 0 0>
  <TLIN Line8 0 690 50 20 -26 1 3 "50 Ohm" 1 "2.85 mm" 1 "0 dB" 0 "26.85" 0>
  <TLIN Line7 1 570 120 -26 -68 0 2 "50 Ohm" 1 "1.0 mm" 1 "0 dB" 0 "26.85" 0>
  <TLIN Line9 2 630 120 -26 20 1 2 "50 Ohm" 1 "3.0 mm" 1 "0 dB" 0 "26.85" 0>
</Components>
<Wires>
  <320 210 350 210 "" 0 0 0 "">
  <350 210 350 240 "" 0 0 0 "">
  <320 300 350 300 "" 0 0 0 "">
  <350 150 490 150 "" 0 0 0 "">
  <490 210 520 210 "" 0 0 0 "">
  <490 210 490 240 "" 0 0 0 "">
  <490 300 520 300 "" 0 0 0 "">
  <300 120 320 120 "" 0 0 0 "">
  <140 120 220 120 "" 0 0 0 "">
  <220 120 240 120 "" 0 0 0 "">
  <220 80 220 120 "" 0 0 0 "">
  <740 170 740 180 "" 0 0 0 "">
  <690 120 740 120 "" 0 0 0 "">
  <690 80 690 120 "" 0 0 0 "">
  <380 120 540 120 "" 0 0 0 "">
  <660 120 690 120 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 380 550 12 #ff0000 0 "Deactivate, if simulation other than\nSP2 is used">
  <Text 170 -30 14 #000000 0 "Stage-1 as Small Signal Amp with Matching Network For 50Ω">
  <Text 560 0 12 #ff0000 0 "Switch, if match for 50Ω or 1mm-line\nshall be used">
  <Arrow 620 50 10 50 20 8 #000000 0 1 0>
  <Arrow 630 50 40 10 20 8 #000000 0 1 0>
</Paintings>
