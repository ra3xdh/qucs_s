<Qucs Schematic 0.0.18>
<Properties>
  <View=0,-240,859,1472,1,84,180>
  <Grid=10,10,1>
  <DataSet=f36163_1.dat>
  <DataDisplay=f36163_1.dpl>
  <OpenDisplay=1>
  <Script=f36163_1.m>
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
  <Pac P1 1 140 290 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 660 290 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <SPfile X1 1 390 260 -26 -53 0 0 "f361632c.s2p" 1 "rectangular" 0 "linear" 0 "open" 0 "2" 0>
  <GND * 1 660 320 0 0 0 0>
  <GND * 1 140 320 0 0 0 0>
  <Eqn Eqn1 1 400 500 -24 15 0 0 "S11dB=dB(S[1,1])" 1 "S21dB=dB(S[2,1])" 1 "S22dB=dB(S[2,2])" 1 "FdB=w2dbm(F)-30" 1 "K=StabFactor(S)" 1 "yes" 0>
  <.SP SP2 0 80 680 0 63 0 0 "const" 1 "0.03 GHz" 0 "12 GHz" 0 "[11GHz]" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn2 0 390 690 -28 15 0 0 "GA=GaCircle(S,linspace(1,15,15))" 1 "GP=GpCircle(S,linspace(1,15,15))" 1 "SL=StabCircleL(S)" 1 "SG=StabCircleS(S)" 1 "NF=NoiseCircle(Sopt,Fmin,Rn,[Fmin,1.27, 1.28, 1.3,1.4,1.5])" 1 "yes" 0>
  <TLIN Line1 1 450 260 -26 20 0 0 "50 Ohm" 1 "1 mm" 1 "0 dB" 0 "26.85" 0>
  <TLIN Line2 1 330 260 -26 20 0 0 "50 Ohm" 1 "1 mm" 1 "0 dB" 0 "26.85" 0>
  <SUBST RO4350_1 1 640 540 -30 24 0 0 "3.48" 1 "0.254 mm" 1 "35 um" 1 "0.004" 1 "0.022e-6" 1 "1.4e-6" 1>
  <MVIA MS1 1 480 410 20 0 0 0 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <TLIN Line3 1 460 380 20 -26 0 1 "56 Ohm" 0 "1.2 mm" 0 "0 dB" 0 "26.85" 0>
  <MVIA MS2 1 430 410 20 0 0 0 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <TLIN Line4 1 410 380 20 -26 0 1 "56 Ohm" 0 "1.2 mm" 0 "0 dB" 0 "26.85" 0>
  <MVIA MS3 1 390 410 20 0 0 0 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <TLIN Line5 1 370 380 20 -26 0 1 "56 Ohm" 0 "1.2 mm" 0 "0 dB" 0 "26.85" 0>
  <MVIA MS4 1 350 410 20 0 0 0 "RO4350_1" 1 "0.254 mm" 1 "26.85" 0>
  <TLIN Line6 1 330 380 20 -26 0 1 "56 Ohm" 0 "1.2 mm" 0 "0 dB" 0 "26.85" 0>
  <.SP SP1 1 90 470 0 63 0 0 "lin" 1 "0.01 GHz" 1 "18 GHz" 1 "200" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <140 260 300 260 "" 0 0 0 "">
  <480 260 660 260 "" 0 0 0 "">
  <390 290 390 350 "" 0 0 0 "">
  <370 350 390 350 "" 0 0 0 "">
  <330 350 370 350 "" 0 0 0 "">
  <410 350 460 350 "" 0 0 0 "">
  <390 350 410 350 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 410 840 12 #ff0000 0 "Deactivate, if simulation other than\nSP2 is used">
  <Text 200 148 14 #000000 0 "S-Parameters of Stage-2 FET">
</Paintings>
