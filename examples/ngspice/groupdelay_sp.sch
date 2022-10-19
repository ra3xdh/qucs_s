<Qucs Schematic 0.0.24>
<Properties>
  <View=-15,-180,1748,1214,1,71,0>
  <Grid=10,10,1>
  <DataSet=groupdelay_sp.dat>
  <DataDisplay=groupdelay_sp.dpl>
  <OpenDisplay=0>
  <Script=groupdelay_sp.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Titel>
  <FrameText1=Gezeichnet von:>
  <FrameText2=Datum:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <.SP SP1 1 90 370 0 80 0 0 "log" 1 "1MHz" 1 "200MHz" 1 "458" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <NutmegEq NutmegEq1 1 370 290 -27 16 0 0 "sp" 1 "S11_dB=dB(s_1_1)" 1 "S12_dB=dB(s_1_2)" 1 "S21_dB=dB(s_2_1)" 1 "S22_dB=dB(s_2_2)" 1>
  <NutmegEq NutmegEq2 1 370 450 -27 16 0 0 "sp" 1 "GD=group_delay(s_2_1)" 1>
  <NutmegEq NutmegEq3 1 370 550 -27 16 0 0 "sp" 1 "Phase_rad=cph(s_2_1)" 1 "Phase_deg=180/pi*Phase_rad" 1>
  <NutmegEq NutmegEq4 1 370 670 -27 16 0 0 "sp" 1 "Group_Delay=-1*deriv(cph(S_2_1))/2/pi" 1>
  <L L1 1 290 100 -26 -48 1 0 "280.9nH" 1 "" 0>
  <C C1 1 220 150 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <C C2 1 360 150 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 5 220 200 0 0 0 0>
  <GND * 5 360 200 0 0 0 0>
  <Pac P2 1 480 150 18 -26 0 1 "2" 1 "60 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 5 480 200 0 0 0 0>
  <Pac P1 1 140 150 -95 -26 1 1 "1" 1 "60 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 5 140 200 0 0 0 0>
</Components>
<Wires>
  <220 100 260 100 "" 0 0 0 "">
  <320 100 360 100 "" 0 0 0 "">
  <220 100 220 120 "" 0 0 0 "">
  <360 100 360 120 "" 0 0 0 "">
  <220 180 220 200 "" 0 0 0 "">
  <360 180 360 200 "" 0 0 0 "">
  <360 100 480 100 "" 0 0 0 "">
  <480 100 480 120 "" 0 0 0 "">
  <480 180 480 200 "" 0 0 0 "">
  <140 100 220 100 "" 0 0 0 "">
  <140 100 140 120 "" 0 0 0 "">
  <140 180 140 200 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 680 339 421 299 3 #c0c0c0 1 00 1 0 2e+07 2e+08 1 -33.9071 10 2.80973 1 -120 20 10.6582 315 0 225 "" "" "">
	<"ngspice/ac.s11_db" #0000ff 0 3 0 0 1>
	<"ngspice/ac.s21_db" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 680 1119 421 299 3 #c0c0c0 1 00 1 0 2e+07 2e+08 1 -33.9071 10 2.80973 1 -268.006 50 21.162 315 0 225 "" "" "">
	<"ngspice/ac.s21_db" #ff0000 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #0000ff 0 3 0 0 1>
  </Rect>
  <Rect 1260 339 421 299 3 #c0c0c0 1 00 1 0 2e+07 2e+08 1 -4e-07 1e-07 4.15121e-08 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.gd" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_rad" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 680 739 421 299 3 #c0c0c0 1 00 1 0 2e+07 2e+08 1 -33.9071 10 2.80973 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.s21_db" #ff0000 0 3 0 0 0>
	<"ngspice/ac.group_delay" #0000ff 0 3 0 0 1>
  </Rect>
  <Smith 1260 758 338 338 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"ngspice/ac.v(s_1_1)" #0000ff 0 3 0 0 0>
  </Smith>
</Diagrams>
<Paintings>
  <Text 90 270 12 #000000 0 "3rd order\nButterworth low-pass filter\n68MHz cutoff, PI-type,\nimpedance matching 60 Ohm">
  <Text 1330 -40 14 #ff0000 0 "ngspice 37 group delay does not\nunwrap phase causing "notch"\nfuture release to fix">
  <Text 130 0 14 #000000 0 "group delay using S-parameter simulation">
</Paintings>
