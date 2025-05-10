<Qucs Schematic 1.0.0>
<Properties>
  <View=-51,-120,1367,864,1,0,120>
  <Grid=10,10,1>
  <DataSet=par_sweep_test.dat>
  <DataDisplay=par_sweep_test.dpl>
  <OpenDisplay=0>
  <Script=par_sweep_test.m>
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
  <IProbe Pr1 1 140 120 -10 -60 0 0>
  <L L1 1 250 120 -24 16 0 0 "10u" 1 "" 0>
  <C C1 1 310 120 -22 15 0 0 "40p" 1 "" 0 "neutral" 0>
  <GND * 1 360 220 0 0 0 0>
  <R R1 1 360 170 15 -20 0 1 "30" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 80 170 -78 -26 1 1 "0.6 V" 1 "7.5 MHz" 0 "0" 0 "0" 0>
  <GND * 1 80 220 0 0 0 0>
  <.AC AC1 1 70 290 0 43 0 0 "lin" 1 "1 MHz" 1 "10 MHz" 1 "200" 1 "no" 0>
  <.TR TR1 1 70 450 0 71 0 0 "lin" 1 "0" 1 "1 us" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.SW SW1 1 240 290 0 71 0 0 "AC1" 1 "lin" 1 "R1" 1 "5 Ohm" 1 "50 Ohm" 1 "5" 1 "false" 0>
</Components>
<Wires>
  <170 120 220 120 "vIn" 200 80 15 "">
  <340 120 360 120 "vR" 380 80 20 "">
  <360 120 360 140 "" 0 0 0 "">
  <360 200 360 220 "" 0 0 0 "">
  <80 200 80 220 "" 0 0 0 "">
  <80 120 80 140 "" 0 0 0 "">
  <80 120 110 120 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 460 640 402 190 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.v(vr)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 467 372 377 272 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"xyce/ac.V(VR)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 970 358 370 260 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(vr)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 180 -100 12 #000000 0 "This examples shows how to use \nparameter sweep with passive circuit and AC and TRAN\nsimulations. For Ngspice both AC and TRAN are \nsupporrted. For Xyce there is no warranty of proper \nTRAN+ParSweep operation.\n\nNOTE: This Example could be run only with Ngspice/Xyce">
</Paintings>
