<Qucs Schematic 24.1.0>
<Properties>
  <View=-17,-20,1733,1115,1,0,240>
  <Grid=10,10,1>
  <DataSet=S-parameter_passive_analysis.dat>
  <DataDisplay=S-parameter_passive_analysis.dpl>
  <OpenDisplay=0>
  <Script=S-parameter_passive_analysis.m>
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
  <GND * 5 240 220 0 0 0 0>
  <GND * 5 280 220 0 0 0 0>
  <GND * 5 440 220 0 0 0 0>
  <GND * 5 480 220 0 0 0 0>
  <Pac P2 1 620 170 20 -25 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 5 620 220 0 0 0 0>
  <GND * 5 120 220 0 0 0 0>
  <Pac P1 1 120 170 -97 -31 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <C C1 1 180 100 -26 -55 0 2 "4.7pF" 1 "" 0 "neutral" 0>
  <C C5 1 560 100 -32 -54 0 0 "4.7pF" 1 "" 0 "neutral" 0>
  <C C3 1 370 100 -26 -55 0 2 "1pF" 1 "" 0 "neutral" 0>
  <.SP SP1 1 40 310 0 80 0 0 "lin" 1 "107MHz" 1 "167MHz" 1 "101" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <C C4 1 480 170 16 -21 0 3 "15pF" 1 "" 0 "neutral" 0>
  <C C2 1 240 170 -73 -16 0 1 "15pF" 1 "" 0 "neutral" 0>
  <L L2 1 440 170 -71 -18 0 1 "66nH" 1 "" 0>
  <L L1 1 280 170 10 -18 0 1 "66nH" 1 "" 0>
  <NutmegEq NutmegEq1 1 250 330 -27 16 0 0 "sp" 1 "S11_dB=dB(s_1_1)" 1 "S12_dB=dB(s_1_2)" 1 "S21_dB=dB(s_2_1)" 1 "S22_dB=dB(s_2_2)" 1>
  <NutmegEq NutmegEq2 1 430 330 -27 16 0 0 "sp" 1 "Phase_rad=cph(s_2_1)" 1 "Phase_deg=180/pi*Phase_rad" 1 "Group_Delay=-1*deriv(cph(S_2_1))/2/pi" 1>
  <NutmegEq NutmegEq3 1 430 470 -27 16 0 0 "sp" 1 "GD=group_delay(s_2_1)" 1>
</Components>
<Wires>
  <280 200 280 220 "" 0 0 0 "">
  <280 100 340 100 "" 0 0 0 "">
  <280 100 280 140 "" 0 0 0 "">
  <240 100 280 100 "" 0 0 0 "">
  <400 100 440 100 "" 0 0 0 "">
  <440 100 480 100 "" 0 0 0 "">
  <440 100 440 140 "" 0 0 0 "">
  <480 100 530 100 "" 0 0 0 "">
  <480 100 480 140 "" 0 0 0 "">
  <480 200 480 220 "" 0 0 0 "">
  <440 200 440 220 "" 0 0 0 "">
  <590 100 620 100 "" 0 0 0 "">
  <620 100 620 140 "" 0 0 0 "">
  <620 200 620 220 "" 0 0 0 "">
  <120 200 120 220 "" 0 0 0 "">
  <120 100 120 140 "" 0 0 0 "">
  <120 100 150 100 "" 0 0 0 "">
  <210 100 240 100 "" 0 0 0 "">
  <240 100 240 140 "" 0 0 0 "">
  <240 200 240 220 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 780 290 346 273 3 #c0c0c0 1 00 0 1.07e+08 1e+07 1.67e+08 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.s21_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.s11_db" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 790 640 348 277 3 #c0c0c0 1 00 0 1.07e+08 1e+07 1.67e+08 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.group_delay" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 800 1000 348 277 3 #c0c0c0 1 00 0 1.07e+08 1e+07 1.67e+08 1 -5.01367e-09 2e-08 7.29107e-08 1 -313.117 100 100.023 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.gd" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 90 0 14 #000000 0 "Group Delay using S-parameter simulation 132-142 MHz BPF">
</Paintings>
