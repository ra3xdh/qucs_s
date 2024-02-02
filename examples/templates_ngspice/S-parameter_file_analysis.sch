<Qucs Schematic 24.1.0>
<Properties>
  <View=-17,-126,1556,780,0.745707,0,1>
  <Grid=10,10,1>
  <DataSet=S-parameter_file_analysis.dat>
  <DataDisplay=S-parameter_file_analysis.dpl>
  <OpenDisplay=0>
  <Script=S-parameter_file_analysis.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Datum:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 5 250 140 0 0 0 0>
  <Pac P2 1 400 150 20 -25 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 5 400 200 0 0 0 0>
  <Pac P1 1 120 150 -97 -31 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 5 120 200 0 0 0 0>
  <.SP SP1 1 70 270 0 80 0 0 "lin" 1 "445kHz" 1 "465kHz" 1 "101" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <NutmegEq NutmegEq1 1 290 270 -27 16 0 0 "sp" 1 "S11_dB=dB(s_1_1)" 1 "S12_dB=dB(s_1_2)" 1 "S21_dB=dB(s_2_1)" 1 "S22_dB=dB(s_2_2)" 1>
  <NutmegEq NutmegEq2 1 290 430 -27 16 0 0 "sp" 1 "Phase_rad=cph(s_2_1)" 1 "Phase_deg=180/pi*Phase_rad" 1 "Group_Delay=-1*deriv(cph(S_2_1))/2/pi" 1>
  <SPfile X1 1 250 100 -26 -59 0 0 "FL455kHz.s2p" 1 "rectangular" 0 "linear" 0 "open" 0 "2" 0>
</Components>
<Wires>
  <400 180 400 200 "" 0 0 0 "">
  <120 180 120 200 "" 0 0 0 "">
  <250 130 250 140 "" 0 0 0 "">
  <400 100 400 120 "" 0 0 0 "">
  <280 100 400 100 "" 0 0 0 "">
  <120 100 120 120 "" 0 0 0 "">
  <120 100 220 100 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 650 300 346 273 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.s21_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.s11_db" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 650 660 348 277 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.phase_deg" #0000ff 0 3 0 0 0>
	<"ngspice/ac.group_delay" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 1110 300 346 273 3 #c0c0c0 1 00 1 445000 5000 465000 1 -87.4291 20 6.33641 1 -40 10 2.39487 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.s12_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.s22_db" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 60 -30 14 #000000 0 "S-parameter simulation of FL455kHz.s2p\nFix path to FL455kHz.s2p file">
</Paintings>
