<Qucs Schematic 25.1.2>
<Properties>
  <View=-734,-554,2525,1284,0.706633,406,70>
  <Grid=10,10,1>
  <DataSet=chebyshev1_5th.dat>
  <DataDisplay=chebyshev1_5th.dpl>
  <OpenDisplay=0>
  <Script=chebyshev1_5th.m>
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
  <GND * 1 70 110 0 0 0 0>
  <GND * 1 190 120 0 0 0 0>
  <GND * 1 330 120 0 0 0 0>
  <GND * 1 470 120 0 0 0 0>
  <L L1 1 260 0 -26 10 0 0 "86.83 nH" 1 "" 0>
  <L L2 1 400 0 -26 10 0 0 "86.83 nH" 1 "" 0>
  <C C1 1 190 90 17 -26 0 1 "67.96 pF" 1 "" 0 "neutral" 0>
  <C C3 1 470 90 17 -26 0 1 "67.96 pF" 1 "" 0 "neutral" 0>
  <C C2 1 330 90 17 -26 0 1 "95.52 pF" 1 "" 0 "neutral" 0>
  <GND * 1 590 120 0 0 0 0>
  <Pac P1 1 70 80 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <Pac P2 1 590 90 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <.SP SP1 1 110 270 0 61 0 0 "lin" 1 "1 MHz" 1 "150 MHz" 1 "300" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 300 280 -23 14 0 0 "S11_dB=dB(S[1,1])" 1 "S21_dB=dB(S[2,1])" 1 "yes" 0>
</Components>
<Wires>
  <70 0 70 50 "" 0 0 0 "">
  <70 0 190 0 "" 0 0 0 "">
  <190 0 230 0 "" 0 0 0 "">
  <190 0 190 60 "" 0 0 0 "">
  <290 0 330 0 "" 0 0 0 "">
  <330 0 370 0 "" 0 0 0 "">
  <330 0 330 60 "" 0 0 0 "">
  <430 0 470 0 "" 0 0 0 "">
  <470 0 470 60 "" 0 0 0 "">
  <590 0 590 60 "" 0 0 0 "">
  <470 0 590 0 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 770 215 398 325 3 #c0c0c0 1 00 1 0 2e+07 1.5e+08 0 -50 5 5 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"S11_dB" #0000ff 1 3 0 0 0>
	<"S21_dB" #ff0000 1 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 430 270 18 #000000 0 "5th Order Low Pass Chebyshev Type 1 Filter \nEnd of pass band:  100MHz ">
</Paintings>
