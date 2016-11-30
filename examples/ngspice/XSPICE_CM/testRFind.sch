<Qucs Schematic 0.0.19>
<Properties>
  <View=-56,6,1607,1192,1,111,0>
  <Grid=10,10,1>
  <DataSet=testlspice2g6AC.dat>
  <DataDisplay=testlspice2g6AC.dpl>
  <OpenDisplay=1>
  <Script=testlspice2g6AC.m>
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
  <IProbe Pr1 1 440 180 -26 16 0 0>
  <GND * 1 560 330 0 0 0 0>
  <.AC AC1 1 640 180 0 40 0 0 "log" 1 "1 Hz" 1 "100GHz" 1 "141" 1 "no" 0>
  <.FOURIER FOUR1 1 980 180 0 65 0 0 "TR1" 1 "10" 1 "100kHz" 1 "i(vpr1) v(nl)" 1>
  <.DC DC1 1 260 200 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 190 240 0 0 0 0>
  <NutmegEq NutmegEq1 1 330 340 66 -9 0 0 "ac" 1 "zin=v(nl)/i(vpr1)" 1>
  <Sub SUB1 1 590 270 555 -115 0 1 "lspice2g6.sch" 0 "370u" 1 "0.0" 1 "0.0" 1 "0.0" 1 "0.0" 1 "0.0" 1 "26.85" 1 "2" 1 "6.02e-12" 1 "363k" 1>
  <S4Q_V V1 1 190 210 -25 60 0 1 "dc 1 ac 1 sin(1 5 100k 0 0)" 1 "" 0 "" 0 "" 0 "" 0>
  <.TR TR1 1 820 180 0 65 0 0 "lin" 1 "0" 1 "500us" 1 "30001" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 1 "0" 1>
</Components>
<Wires>
  <470 180 560 180 "" 0 0 0 "">
  <190 180 410 180 "" 0 0 0 "">
  <560 180 560 180 "nl" 480 190 0 "">
</Wires>
<Diagrams>
  <Rect 240 637 1055 210 3 #c0c0c0 1 11 1 1 1 1e+11 1 -6145.15 20000 67598.5 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.zin" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 560 873 738 176 3 #c0c0c0 1 00 1 0 0.0001 0.0015 1 -0.2 1 2.2 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/tran.v(nl)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 250 860 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"ngspice/tran.i(pr1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 810 1110 457 162 3 #c0c0c0 1 00 1 0 100000 900000 1 -0.1 0.5 1.1 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/magnitude(v(nl))" #ff0000 2 3 0 6 0>
  </Rect>
  <Rect 310 1110 377 162 3 #c0c0c0 1 00 1 0 200000 900000 1 -0.1 0.5 1.1 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/magnitude(i(vpr1))" #ff0000 2 3 0 6 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
