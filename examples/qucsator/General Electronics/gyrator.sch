<Qucs Schematic 0.0.4>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=gyrator.dat>
  <DataDisplay=gyrator.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 120 320 0 0 0 0>
  <Vdc V1 1 120 290 18 -26 0 1 "10 V" 1>
  <Vac V2 1 120 230 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0>
  <GND * 1 570 320 0 0 0 0>
  <R Load 1 570 290 15 -26 0 1 "1000 Ohm" 1 "26.85" 0 "european" 0>
  <.DC DC1 1 190 350 0 33 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0>
  <.AC AC1 1 320 350 0 33 0 0 "log" 1 "1 Hz" 1 "100 kHz" 1 "100" 1>
  <_BJT T1 1 360 130 -26 -36 0 1 "npn" 0 "1e-16" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "250" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0>
  <R R1 1 310 160 -26 15 0 0 "20 kOhm" 1 "26.85" 0 "european" 0>
  <C C1 1 410 160 -26 17 0 0 "200 uF" 1>
</Components>
<Wires>
  <280 130 330 130 "" 0 0 0>
  <280 130 280 160 "" 0 0 0>
  <340 160 360 160 "" 0 0 0>
  <440 130 440 160 "" 0 0 0>
  <390 130 440 130 "" 0 0 0>
  <440 160 570 160 "" 0 0 0>
  <570 160 570 260 "" 0 0 0>
  <120 160 280 160 "" 0 0 0>
  <120 160 120 200 "" 0 0 0>
  <360 160 380 160 "" 0 0 0>
  <570 160 570 160 "Output" 600 130 0>
  <120 160 120 160 "Input" 150 130 0>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 210 240 12 #000000 0 "This circuit is a primitive gyrator. It simulates an inductor\nby taking a capacitor and an transistor inverter. At the\noutput there should be only little ac voltage.">
</Paintings>
