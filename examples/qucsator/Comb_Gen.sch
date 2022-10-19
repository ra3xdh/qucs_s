<Qucs Schematic 0.0.17>
<Properties>
  <View=656,223,1620,788,1.0177,0,0>
  <Grid=10,10,1>
  <DataSet=Comb_Gen.dat>
  <DataDisplay=Comb_Gen.dpl>
  <OpenDisplay=1>
  <Script=Comb_Gen.m>
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
  <GND * 1 710 410 0 0 0 0>
  <R R2 5 770 340 -39 -28 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vac V1 5 710 380 19 -8 0 1 "5 V" 1 "100 MHz" 1 "0" 0 "0" 0>
  <GND * 1 890 410 0 0 0 0>
  <R R5 5 890 380 -37 58 0 1 "150" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C3 5 830 340 -26 17 0 0 "220 pF" 1 "" 0 "neutral" 0>
  <GND * 1 1110 410 0 0 0 0>
  <Lib D3 5 1110 380 13 10 0 1 "LEDs" 0 "green" 0>
  <GND * 1 1000 410 0 0 0 0>
  <C C1 5 1000 380 9 11 0 1 "1.2 pF" 1 "" 0 "neutral" 0>
  <C C2 5 1210 340 -28 -32 0 0 "0.8 pF" 1 "" 0 "neutral" 0>
  <R R3 5 1260 380 22 -10 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 1260 410 0 0 0 0>
  <Eqn Eqn1 1 760 450 -30 16 0 0 "Spectrum_dBm=16+dB(Time2Freq(Vout.Vt,time))" 1 "VSWR=max(VSWR1,VSWR2)" 1 "VSWR1=(.5*rms(Vsource.Vt)/rms(Vin.Vt))" 1 "VSWR2=1/VSWR1" 1 "yes" 0>
  <R R4 5 950 340 -15 -29 0 0 "5" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L2 5 1060 340 -29 -30 0 0 "8 nH" 1 "" 0>
  <L L3 5 1150 340 -29 -30 0 0 "4 nH" 1 "" 0>
  <.TR TR1 1 1140 460 0 67 0 0 "lin" 0 "0" 0 "1e-6" 1 "4000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <710 340 710 350 "" 0 0 0 "">
  <710 340 740 340 "" 0 0 0 "">
  <890 340 890 350 "" 0 0 0 "">
  <860 340 890 340 "" 0 0 0 "">
  <1110 340 1110 350 "" 0 0 0 "">
  <1000 340 1000 350 "" 0 0 0 "">
  <1260 340 1260 350 "" 0 0 0 "">
  <1240 340 1260 340 "" 0 0 0 "">
  <980 340 1000 340 "" 0 0 0 "">
  <890 340 920 340 "" 0 0 0 "">
  <1090 340 1110 340 "" 0 0 0 "">
  <1000 340 1030 340 "" 0 0 0 "">
  <1110 340 1120 340 "" 0 0 0 "">
  <800 340 800 340 "Vin" 830 280 0 "">
  <710 340 710 340 "Vsource" 720 280 0 "">
  <1260 340 1260 340 "Vout" 1290 290 0 "">
</Wires>
<Diagrams>
  <Rect 1340 580 240 160 3 #c0c0c0 1 00 0 0 5e+08 2e+09 0 -50 10 0.1 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"Spectrum_dBm" #0000ff 1 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 730 570 12 #ff0000 0 "Not a practical circuit due to diode package parasitics.  Purpose is to show\nhow light-emitting diodes (LED) can be used as "varactors" (voltage variable\ncapacitors) in low frequency applications as a non-linear element.\n\nAn actual circuit employing a packaged LED would probably not produce significant\nharmonics beyond a few hundred MHz.  Matching elements shown were chosen to\nobtain a fairly flat comb across the simulation frequency.  The purpose of the 5 ohm\nand 150 ohm resistors is to present a better VSWR to the driving source.\n\nThe display includes output spectrum and input VSWR at the drive frequency.">
</Paintings>
