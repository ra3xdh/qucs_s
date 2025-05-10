<Qucs Schematic 0.0.24>
<Properties>
  <View=-44,10,1301,938,1,0,60>
  <Grid=10,10,1>
  <DataSet=groupdelay_ac.dat>
  <DataDisplay=groupdelay_ac.dpl>
  <OpenDisplay=0>
  <Script=groupdelay_ac.m>
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
  <Vac V1 1 60 190 -64 -26 1 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 60 240 0 0 0 0>
  <L L1 1 310 140 -33 -51 1 0 "280.9nH" 1 "" 0>
  <C C1 1 240 190 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 1 240 240 0 0 0 0>
  <C C2 1 380 190 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 1 380 240 0 0 0 0>
  <.DC DC1 1 30 400 -1 47 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.AC AC1 1 30 490 0 46 0 0 "log" 1 "1 MHz" 1 "200 MHz" 1 "458" 1 "no" 0>
  <R R2 1 520 190 20 -19 0 1 "Z0" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 520 240 0 0 0 0>
  <R R1 1 130 140 -22 -53 1 0 "Z0" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Eqn Eqn1 1 250 410 -31 17 0 0 "Z0=60" 1 "yes" 0>
  <NutmegEq NutmegEq1 1 250 490 -27 19 0 0 "ac" 1 "Gain=(ac.v(out)/ac.v(in))" 1 "Gain_dB=dB(ac.v(out)/ac.v(in))" 1 "Phase_rad=cph(ac.v(out)/ac.v(in))" 1 "Phase_deg=180/pi*Phase_rad" 1 "Group_Delay=-1*deriv(cph((ac.v(out)/ac.v(in))))/2/pi" 1>
</Components>
<Wires>
  <240 140 280 140 "" 0 0 0 "">
  <340 140 380 140 "" 0 0 0 "">
  <60 220 60 240 "" 0 0 0 "">
  <240 140 240 160 "" 0 0 0 "">
  <240 220 240 240 "" 0 0 0 "">
  <380 140 380 160 "" 0 0 0 "">
  <380 220 380 240 "" 0 0 0 "">
  <380 140 520 140 "out" 390 110 3 "">
  <520 140 520 160 "" 0 0 0 "">
  <520 220 520 240 "" 0 0 0 "">
  <160 140 240 140 "" 0 0 0 "">
  <60 140 60 160 "" 0 0 0 "">
  <60 140 100 140 "" 0 0 0 "">
  <240 140 240 140 "in" 220 110 0 "">
</Wires>
<Diagrams>
  <Rect 700 843 441 323 3 #c0c0c0 1 00 1 0 2e+07 2e+08 1 -36.9277 5 -3.21087 1 -268.006 50 21.162 315 0 225 "" "" "">
	<"ngspice/ac.gain_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.group_delay" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 700 443 441 323 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.gain_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 60 30 14 #000000 0 "Group Delay using AC simulation">
  <Text 200 290 12 #000000 0 "3rd order Butterworth low-pass filter\n68MHz cutoff, PI-type,\nImpedance 60 Ohms">
</Paintings>
