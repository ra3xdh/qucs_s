<Qucs Schematic 0.0.12>
<Properties>
  <View=-426,-90,646,756,0.925532,0,0>
  <Grid=10,10,1>
  <DataSet=SmithChartTest.dat>
  <DataDisplay=SmithChartTest.sch>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By: SunnySan>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac P1 1 -140 230 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 -140 260 0 0 0 0>
  <L L1 1 10 230 8 -26 0 1 "196.6nH" 1 "" 0>
  <C C1 1 -20 230 -8 46 0 1 "644.1pF" 1 "" 0 "neutral" 0>
  <GND * 1 10 260 0 0 0 0>
  <L L2 1 120 150 -26 -44 0 0 "791.1nH" 1 "" 0>
  <C C2 1 60 150 -26 10 0 0 "160.1pF" 1 "" 0 "neutral" 0>
  <L L3 1 150 230 8 -26 0 1 "196.6nH" 1 "" 0>
  <C C3 1 120 230 -8 46 0 1 "644.1pF" 1 "" 0 "neutral" 0>
  <GND * 1 150 260 0 0 0 0>
  <Pac P2 1 260 230 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 260 260 0 0 0 0>
  <.SP SP2 1 -320 160 0 79 0 0 "log" 1 "1MHz" 1 "80MHz" 1 "461" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn3 1 430 290 -28 15 0 0 "dBS21=dB(S[2,1])" 1 "ZofS22=rtoz(S[2,2])" 1 "yes" 0>
</Components>
<Wires>
  <-140 150 -140 200 "" 0 0 0 "">
  <-140 150 10 150 "" 0 0 0 "">
  <10 150 10 200 "" 0 0 0 "">
  <150 150 150 200 "" 0 0 0 "">
  <10 150 30 150 "" 0 0 0 "">
  <-20 200 10 200 "" 0 0 0 "">
  <-20 260 10 260 "" 0 0 0 "">
  <120 200 150 200 "" 0 0 0 "">
  <120 260 150 260 "" 0 0 0 "">
  <260 150 260 200 "" 0 0 0 "">
  <150 150 260 150 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Smith -350 620 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"S[2,2]" #ff0000 0 3 0 0 0>
	  <Mkr 1.41295e+07 170 -180 3 0 0>
  </Smith>
  <Tab 220 660 386 210 3 #c0c0c0 1 00 1 277 1 1 1 0 1 1 1 0 1 461 315 0 225 "" "" "">
	<"S[2,2]" #0000ff 0 3 1 0 0>
	<"ZofS22" #0000ff 0 3 0 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text -100 350 12 #000000 0 "Chebyshev band-pass filter\n10MHz...20MHz, PI-type,\nimpedance matching 50 Ohm">
  <Text -120 510 12 #5500ff 0 "example test (in the middle of the chart)\nLook at S22 at Freq 1.41e+07\nS[2,2] is almost zero\nbut ZofS22 normalised is almost 1 \n(multiple by Z0 for the real impedance,\n50 ohms in this case)\nZofS22 can be read on the Smith chart\nor on the right table">
  <Text -110 -50 14 #5500ff 0 "Smith Chart example test with a band pass filter">
  <Text -350 -10 12 #000000 0 "The Smith Chart can be used to measure the impedance corresponding to S22 (if you \nknow how to use a Smith Chart)\nOr you can introduce the equation "rtoz()" to obtain the corresponding impedances.\nR to Z function will transform the Reflection to an impedance Z\n\nThanks to the QUCS team">
</Paintings>
