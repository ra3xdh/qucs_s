<Qucs Schematic 0.0.24>
<Properties>
  <View=573,240,1906,960,1.09602,0,0>
  <Grid=10,10,1>
  <DataSet=Comb_Gen.dat>
  <DataDisplay=Comb_Gen.dpl>
  <OpenDisplay=0>
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
  <C C3 5 830 340 -26 17 0 0 "220 pF" 1 "" 0 "neutral" 0>
  <R R4 5 950 340 -15 -29 0 0 "5" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vac V1 5 700 390 -101 -20 1 1 "5 V" 1 "100 MHz" 1 "0" 0 "0" 0>
  <GND * 1 700 440 0 0 0 0>
  <R R5 5 900 390 15 -10 0 1 "150" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 900 440 0 0 0 0>
  <GND * 1 1000 440 0 0 0 0>
  <C C1 5 1000 390 17 -11 0 1 "1.2 pF" 1 "" 0 "neutral" 0>
  <R R2 5 750 340 -18 -35 0 0 "50" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 1260 430 0 0 0 0>
  <GND * 1 1110 440 0 0 0 0>
  <Lib D3 5 1110 390 -13 10 1 1 "LEDs" 0 "green" 0>
  <R R3 5 1260 390 17 -11 0 1 "50" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C2 5 1210 340 -27 -36 0 0 "0.8pF" 1 "" 0 "neutral" 0>
  <L L3 5 1150 340 -26 -31 0 0 "4nH" 1 "" 0>
  <L L2 5 1060 340 -26 -32 0 0 "8nH" 1 "" 0>
  <.TR TR1 1 720 530 0 68 0 0 "lin" 0 "0" 1 "1e-6" 1 "4000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <NutmegEq NutmegEq1 1 1040 540 -30 18 0 0 "fft" 1 "S=db(v(out))" 1>
  <.FFT FFT1 1 870 530 0 75 0 0 "1GHz" 1 "1MHz" 1 "hanning" 1 "2" 0>
</Components>
<Wires>
  <860 340 900 340 "" 0 0 0 "">
  <980 340 1000 340 "" 0 0 0 "">
  <1090 340 1110 340 "" 0 0 0 "">
  <1000 340 1030 340 "" 0 0 0 "">
  <1110 340 1120 340 "" 0 0 0 "">
  <700 420 700 440 "" 0 0 0 "">
  <900 420 900 440 "" 0 0 0 "">
  <900 340 920 340 "" 0 0 0 "">
  <900 340 900 360 "" 0 0 0 "">
  <1000 340 1000 360 "" 0 0 0 "">
  <1000 420 1000 440 "" 0 0 0 "">
  <780 340 800 340 "" 0 0 0 "">
  <700 340 700 360 "" 0 0 0 "">
  <700 340 720 340 "" 0 0 0 "">
  <1110 340 1110 360 "" 0 0 0 "">
  <1110 420 1110 440 "" 0 0 0 "">
  <1240 340 1260 340 "" 0 0 0 "">
  <1260 340 1260 360 "" 0 0 0 "">
  <1260 420 1260 430 "" 0 0 0 "">
  <800 340 800 340 "Vin" 810 300 0 "">
  <700 340 700 340 "Vsource" 630 310 0 "">
  <1260 340 1260 340 "out" 1270 310 0 "">
</Wires>
<Diagrams>
  <Rect 1380 664 481 324 3 #c0c0c0 1 00 1 0 5e+08 2e+09 1 -50 10 0.1 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"ngspice/ac.s" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 700 730 12 #ff0000 0 "Not a practical circuit due to diode package parasitics.  Purpose is to show\nhow light-emitting diodes (LED) can be used as "varactors" (voltage variable\ncapacitors) in low frequency applications as a non-linear element.\n\nAn actual circuit employing a packaged LED would probably not produce significant\nharmonics beyond a few hundred MHz.  Matching elements shown were chosen to\nobtain a fairly flat comb across the simulation frequency.  The purpose of the 5 ohm\nand 150 ohm resistors is to present a better VSWR to the driving source.\n\nThe display includes output spectrum and input VSWR at the drive frequency.">
</Paintings>
