<Qucs Schematic 0.0.24>
<Properties>
  <View=-88,-61,1599,710,1,0,0>
  <Grid=10,10,1>
  <DataSet=RCL_resonance.dat>
  <DataDisplay=RCL_resonance.dpl>
  <OpenDisplay=0>
  <Script=RCL_resonance.m>
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
  <L L1 1 180 80 -26 10 0 0 "10u" 1 "" 0>
  <C C1 1 280 80 -26 17 0 0 "40p" 1 "" 0 "neutral" 0>
  <Vac V1 1 30 130 -78 -26 1 1 "0.6 V" 1 "7.5 MHz" 0 "0" 0 "0" 0>
  <GND * 1 30 180 0 0 0 0>
  <R R1 1 350 130 15 -26 0 1 "30" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 350 180 0 0 0 0>
  <IProbe Pr1 1 90 80 -11 -61 0 0>
  <.AC AC1 1 -50 420 0 45 0 0 "lin" 1 "1 MHz" 1 "16 MHz" 1 "101" 1 "no" 0>
  <.TR TR1 1 120 420 0 75 0 0 "lin" 1 "0" 1 "1 us" 1 "201" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Eqn Eqn1 1 280 430 -31 17 0 0 "Gain_dB=dB(out.v/in.v)" 1 "Phase=(cph(out.v)-cph(in.v))*180/pi" 1 "yes" 0>
</Components>
<Wires>
  <210 80 250 80 "" 0 0 0 "">
  <30 160 30 180 "" 0 0 0 "">
  <310 80 350 80 "out" 350 40 30 "">
  <350 80 350 100 "" 0 0 0 "">
  <350 160 350 180 "" 0 0 0 "">
  <120 80 150 80 "" 0 0 0 "">
  <30 80 30 100 "" 0 0 0 "">
  <30 80 60 80 "" 0 0 0 "">
  <30 80 30 80 "in" 0 50 0 "">
</Wires>
<Diagrams>
  <Rect 568 291 413 217 3 #c0c0c0 1 00 1 1e+6 2e+6 1e+7 1 -0.0549399 0.2 0.659495 1 -0.00183133 0.01 0.0219832 315 0 225 "" "" "">
	<"ngspice/ac.v(out)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.i(pr1)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 580 578 409 211 3 #c0c0c0 1 00 1 0 2e-7 1e-6 1 -0.473667 0.2 0.47597 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.v(out)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.i(pr1)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 30 250 12 #000000 0 "It is a simple series RCL circuit. \nYou can simulate it with Ngspice and Xyce. \nAC and TRAN analysis are allowed.\n\nNOTE: This Example could be run with Qucs and\n Ngspice/Xyce">
</Paintings>
