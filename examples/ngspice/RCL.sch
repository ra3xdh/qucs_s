<Qucs Schematic 0.0.19>
<Properties>
  <View=0,48,1022,1477,1,0,600>
  <Grid=10,10,1>
  <DataSet=RCL.dat>
  <DataDisplay=RCL.dpl>
  <OpenDisplay=1>
  <Script=ngsp-RCL.m>
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
  <GND * 1 30 310 0 0 0 0>
  <Vac V1 1 30 280 18 -26 0 1 "0.6 V" 1 "7.5 MHz" 0 "0" 0 "0" 0>
  <IProbe Pr1 1 60 200 -26 16 0 0>
  <L L1 1 170 200 -26 10 0 0 "10u" 1 "" 0>
  <R R1 1 350 270 15 -26 0 1 "30" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 270 200 -26 17 0 0 "40p" 1 "" 0 "neutral" 0>
  <GND * 1 350 300 0 0 0 0>
  <.TR TR1 1 260 480 0 71 0 0 "lin" 1 "0" 1 "1 us" 1 "201" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.AC AC1 1 60 480 0 43 0 0 "lin" 1 "1 MHz" 1 "10 MHz" 1 "1000" 1 "no" 0>
</Components>
<Wires>
  <30 200 30 250 "" 0 0 0 "">
  <90 200 140 200 "vIn" 160 120 5 "">
  <200 200 240 200 "" 0 0 0 "">
  <300 200 350 200 "vR" 350 150 30 "">
  <350 200 350 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 490 568 409 211 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.v(vr)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 80 830 259 189 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/ac.V(VR)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 490 1141 306 231 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/ac.I(PR1)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 80 1419 410 229 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/ac.I(PR1)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 478 291 413 217 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"RCL_ngspice:ac.v(vr)" #0000ff 0 3 0 0 0>
	<"RCL_ngspice:ac.i(pr1)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 458 830 447 185 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/tran.V(VIN)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(VR)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 90 1144 307 234 3 #c0c0c0 1 00 1 1e+06 2e+06 1e+07 1 -0.00183133 0.01 0.0219832 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 80 330 12 #000000 0 "It is a simple series RCL circuit. \nYou can simulate it with Ngspice and Xyce. \nAC and TRAN analysis are allowed.\n\nNOTE: This Example could be run with Qucs and\n Ngspice/Xyce">
</Paintings>
