<Qucs Schematic 1.0.0>
<Properties>
  <View=-124,28,1469,1020,1,0,0>
  <Grid=10,10,1>
  <DataSet=RC_filter_FFT.dat>
  <DataDisplay=RC_filter_FFT.dpl>
  <OpenDisplay=0>
  <Script=RC filter FFT.m>
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
  <C C1 1 200 210 27 -23 0 1 "10nF" 1 "" 0 "neutral" 0>
  <GND * 1 200 260 0 0 0 0>
  <Vrect V1 1 80 210 -90 -49 1 1 "1 V" 1 ".5 ms" 1 ".5 ms" 1 "1 ns" 1 "1 ns" 1 "0 ns" 0>
  <GND * 1 80 260 0 0 0 0>
  <R R1 1 140 150 -28 -51 1 0 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.TR TR1 1 -30 320 0 71 0 0 "lin" 1 "0" 1 "10 ms" 1 "1000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.FFT FFT1 1 -30 510 0 75 0 0 "1MHz" 1 "1kHz" 1 "hanning" 1 "2" 0>
  <NutmegEq NutmegEq1 1 0 690 -30 18 0 0 "fft" 1 "S=db(v(out))" 1>
</Components>
<Wires>
  <200 240 200 260 "" 0 0 0 "">
  <80 240 80 260 "" 0 0 0 "">
  <200 150 200 180 "" 0 0 0 "">
  <170 150 200 150 "" 0 0 0 "">
  <80 150 80 180 "" 0 0 0 "">
  <80 150 110 150 "in" 50 120 0 "">
  <200 150 200 150 "out" 210 120 0 "">
</Wires>
<Diagrams>
  <Rect 380 311 329 231 3 #c0c0c0 1 00 1 0 0.0002 0.001 1 -1.19998 1 1.19998 1 -8.49897 5 5 315 0 225 0 0 0 "" "" "">
	<"ngspice/RC_filter_FFT:tran.v(in)" #0000ff 0 3 0 0 0>
	<"ngspice/RC_filter_FFT:tran.v(out)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 374 704 574 284 3 #c0c0c0 1 00 1 0 1e+08 1e+09 0 -150 25 0 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/RC_filter_FFT:ac.s" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
