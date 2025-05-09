<Qucs Schematic 0.0.12>
<Properties>
  <View=56,-10,840,509,1,0,0>
  <Grid=10,10,1>
  <DataSet=groupdelay_sp.dat>
  <DataDisplay=groupdelay_sp.dpl>
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
  <GND * 1 110 210 0 0 0 0>
  <C C1 1 220 180 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 1 220 210 0 0 0 0>
  <L L1 1 290 100 -26 10 0 0 "280.9nH" 1 "" 0>
  <C C2 1 360 180 17 -26 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 1 360 210 0 0 0 0>
  <Pac P1 1 110 180 18 -26 0 1 "1" 1 "60 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <.SP SP1 1 110 290 0 80 0 0 "log" 1 "1MHz" 1 "200MHz" 1 "458" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Pac P2 1 500 190 18 -26 0 1 "2" 1 "60 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 500 220 0 0 0 0>
  <Eqn Eqn1 1 310 300 -28 15 0 0 "dBS21_3rd=dB(S[2,1])" 1 "gain_phase=unwrap(angle(S[2,1]))" 1 "group_delay=-diff(gain_phase,2*pi*frequency)" 1 "yes" 0>
</Components>
<Wires>
  <220 100 220 150 "" 0 0 0 "">
  <360 100 360 150 "" 0 0 0 "">
  <220 100 260 100 "" 0 0 0 "">
  <320 100 360 100 "" 0 0 0 "">
  <110 100 220 100 "" 0 0 0 "">
  <110 100 110 150 "" 0 0 0 "">
  <360 100 500 100 "" 0 0 0 "">
  <500 100 500 160 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 560 270 12 #000000 0 "3rd order\nButterworth low-pass filter\n68MHz cutoff, PI-type,\nimpedance matching 60 Ohm">
  <Text 110 30 14 #000000 0 "group delay using S-parameter simulation">
</Paintings>
