<Qucs Schematic 0.0.12>
<Properties>
  <View=0,10,750,594,1,0,0>
  <Grid=10,10,1>
  <DataSet=groupdelay_ac.dat>
  <DataDisplay=groupdelay_ac.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Titel>
  <FrameText1=Gezeichnet von:>
  <FrameText2=Datum:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <C C1 1 240 220 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 1 240 250 0 0 0 0>
  <L L1 1 310 140 -26 10 0 0 "280.9nH" 1 "" 0>
  <C C2 1 380 220 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 1 380 250 0 0 0 0>
  <GND * 1 60 250 0 0 0 0>
  <Vac V1 1 60 200 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <Vac V2 1 550 200 18 -26 0 1 "0 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 550 250 0 0 0 0>
  <R R2 1 480 140 -26 15 0 0 "Z0" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.AC AC1 1 50 330 0 46 0 0 "log" 1 "1 MHz" 1 "200 MHz" 1 "458" 1 "no" 0>
  <Eqn Eqn1 1 270 340 -28 15 0 0 "dBS21_3rd=dB(S21)" 1 "S21=b2/a1" 1 "S11=b1/a1" 1 "a1=(P1.v+Z0*-V1.i)/(2*sqrt(Z0))" 1 "Z0=60" 1 "b1=(P1.v-Z0*-V1.i)/(2*sqrt(Z0))" 1 "b2=(P2.v-Z0*-V2.i)/(2*sqrt(Z0))" 1 "gain_phase=unwrap(angle(S21))" 1 "group_delay=-diff(gain_phase,2*pi*acfrequency)" 1 "yes" 0>
  <R R1 1 150 140 -26 15 0 0 "Z0" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <240 140 240 190 "" 0 0 0 "">
  <380 140 380 190 "" 0 0 0 "">
  <240 140 280 140 "" 0 0 0 "">
  <340 140 380 140 "" 0 0 0 "">
  <60 230 60 250 "" 0 0 0 "">
  <380 140 450 140 "P2" 430 100 31 "">
  <510 140 550 140 "" 0 0 0 "">
  <550 140 550 170 "" 0 0 0 "">
  <550 230 550 250 "" 0 0 0 "">
  <180 140 240 140 "P1" 230 100 28 "">
  <60 140 60 170 "" 0 0 0 "">
  <60 140 120 140 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 470 310 12 #000000 0 "3rd order\nButterworth low-pass filter\n68MHz cutoff, PI-type,\nimpedance matching 60 Ohm">
  <Text 60 50 14 #000000 0 "group delay using AC simulation">
</Paintings>
