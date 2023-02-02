<Qucs Schematic 1.0.0>
<Properties>
  <View=0,-180,1221,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=tunn.dat>
  <DataDisplay=tunn.dpl>
  <OpenDisplay=1>
  <Script=tunn.m>
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
  <GND * 1 330 170 0 0 0 0>
  <.DC DC1 1 240 240 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 410 240 0 75 0 0 "DC1" 1 "lin" 1 "V1" 1 "-0.05" 1 "0.4" 1 "100" 1 "false" 0>
  <.TR TR1 1 560 240 0 75 0 0 "lin" 1 "0" 1 "1 ms" 1 "11" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vdc V1 1 150 180 18 -26 0 1 "1" 1>
  <GND * 1 150 210 0 0 0 0>
  <IProbe Pr1 1 200 120 -26 16 0 0>
  <tunnel tunnel1 1 280 120 70 0 0 0 "1e-12" 0 "1e-5" 0 "1e-6" 0 "0.1" 1 "0.4" 1 "5" 0 "0.01p" 0 "300" 0>
  <GND * 1 340 30 0 0 0 0>
  <IProbe Pr2 1 210 -20 -26 16 0 0>
  <tunnel tunnel2 1 290 -20 70 0 0 0 "1e-12" 0 "1e-5" 0 "1e-6" 0 "0.1" 1 "0.2" 1 "5" 0 "0.01p" 0 "300" 0>
</Components>
<Wires>
  <330 120 330 170 "" 0 0 0 "">
  <310 120 330 120 "" 0 0 0 "">
  <150 120 170 120 "" 0 0 0 "">
  <150 120 150 150 "" 0 0 0 "">
  <230 120 250 120 "" 0 0 0 "">
  <340 -20 340 30 "" 0 0 0 "">
  <320 -20 340 -20 "" 0 0 0 "">
  <240 -20 260 -20 "" 0 0 0 "">
  <150 -20 150 120 "" 0 0 0 "">
  <150 -20 180 -20 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 552 178 469 338 3 #c0c0c0 1 00 1 -0.05 0.05 0.4 1 -2.55566e-5 5e-6 1.34857e-5 1 -1 0.2 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
	<"ngspice/i(pr2)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
