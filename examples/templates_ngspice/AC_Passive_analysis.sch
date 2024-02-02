<Qucs Schematic 24.1.0>
<Properties>
  <View=-73,-70,1584,913,0.834037,0,0>
  <Grid=10,10,1>
  <DataSet=AC_Passive_analysis.dat>
  <DataDisplay=AC_Passive_analysis.dpl>
  <OpenDisplay=0>
  <Script=AC_Passive_analysis.m>
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
  <GND * 1 100 180 0 0 0 0>
  <R R1 1 170 80 -32 -54 0 0 "800" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L1 1 270 80 -34 -56 0 0 "367 uH" 1 "" 0>
  <L L2 1 320 130 -88 -21 0 1 "384 uH" 1 "" 0>
  <GND * 1 320 180 0 0 0 0>
  <C C1 1 370 80 -40 -57 1 0 "204 pF" 1 "" 0 "neutral" 0>
  <R R2 1 420 130 18 -20 0 1 "200" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 420 180 0 0 0 0>
  <.AC AC1 1 100 290 -2 45 0 0 "lin" 1 "696 kHz" 1 "896 kHz" 1 "200" 1 "no" 0>
  <NutmegEq NutmegEq1 1 310 290 -27 19 0 0 "ac" 1 "Gain=(ac.v(out)/ac.v(in))" 1 "Gain_dB=dB(ac.v(out)/ac.v(in))" 1 "Phase_rad=cph(ac.v(out))" 1 "Phase_deg=180/pi*Phase_rad" 1 "Group_Delay=-1*deriv(cph(ac.v(out)))/2/pi" 1>
  <S4Q_V V1 1 100 130 -71 -19 1 1 "AC 1" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <200 80 240 80 "" 0 0 0 "">
  <320 80 320 100 "" 0 0 0 "">
  <300 80 320 80 "" 0 0 0 "">
  <320 160 320 180 "" 0 0 0 "">
  <320 80 340 80 "" 0 0 0 "">
  <400 80 420 80 "" 0 0 0 "">
  <420 80 420 100 "" 0 0 0 "">
  <420 160 420 180 "" 0 0 0 "">
  <100 160 100 180 "" 0 0 0 "">
  <100 80 140 80 "" 0 0 0 "">
  <100 80 100 100 "" 0 0 0 "">
  <420 80 420 80 "out" 440 50 0 "">
  <100 80 100 80 "in" 70 50 0 "">
</Wires>
<Diagrams>
  <Rect 680 350 520 370 3 #c0c0c0 1 00 1 696000 20000 900000 1 -13.658 0.2 -11.8944 1 -24.8581 10 52.5387 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.gain_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #ff0000 0 3 0 0 1>
	  <Mkr 796503 66 -116 3 0 0>
  </Rect>
  <Rect 680 810 520 370 3 #c0c0c0 1 00 1 696000 20000 900000 1 -13.658 0.2 -11.8942 1 -24.8581 10 52.5387 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.phase_deg" #0000ff 0 3 0 0 0>
	<"ngspice/ac.group_delay" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 120 -30 16 #000000 0 "Matching Network 12deg lead @ 796kHz">
</Paintings>
