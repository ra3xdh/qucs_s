<Qucs Schematic 0.0.3>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=Chebyshev1_5th.dat>
  <DataDisplay=Chebyshev1_5th.dpl>
  <OpenDisplay=1>
</Properties>
<Components>
  <GND * 1 70 150 0 0 0 0>
  <GND * 1 190 190 0 0 0 0>
  <GND * 1 290 190 0 0 0 0>
  <GND * 1 390 190 0 0 0 0>
  <L L1 1 240 70 -26 10 0 0 "86.83 nH" 1>
  <L L2 1 340 70 -26 10 0 0 "86.83 nH" 1>
  <C C1 1 190 160 17 -26 0 1 "67.96 pF" 1>
  <C C3 1 390 160 17 -26 0 1 "67.96 pF" 1>
  <C C2 1 290 160 17 -26 0 1 "95.52 pF" 1>
  <GND * 1 510 150 0 0 0 0>
  <Pac P1 1 70 120 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0>
  <Pac P2 1 510 120 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0>
  <.SP SP1 1 110 270 -50 35 0 0 "lin" 1 "1 MHz" 1 "150 MHz" 1 "300" 1 "no" 0 "1" 0 "2" 0>
  <Eqn Eqn1 1 220 290 -23 14 0 0 "S11_dB=dB(S[1,1])" 1 "S21_dB=dB(S[2,1])" 1 "yes" 0>
</Components>
<Wires>
  <70 70 70 90 "" 0 0 0>
  <70 70 190 70 "" 0 0 0>
  <190 70 210 70 "" 0 0 0>
  <190 70 190 130 "" 0 0 0>
  <270 70 290 70 "" 0 0 0>
  <290 70 310 70 "" 0 0 0>
  <290 70 290 130 "" 0 0 0>
  <370 70 390 70 "" 0 0 0>
  <390 70 390 130 "" 0 0 0>
  <510 70 510 90 "" 0 0 0>
  <390 70 510 70 "" 0 0 0>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 310 250 18 #000000 0 "5th Order Low Pass Chebyshev Type 1 Filter \nEnd of pass band:  100MHz ">
</Paintings>
