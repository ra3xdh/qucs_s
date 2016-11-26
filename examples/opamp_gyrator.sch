<Qucs Schematic 0.0.5>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=opamp_gyrator.dat>
  <DataDisplay=opamp_gyrator.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 260 280 0 0 0 0>
  <C C1 1 210 150 -26 17 0 0 "1 uF" 1>
  <Pac P1 1 70 160 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0>
  <GND * 1 70 190 0 0 0 0>
  <OpAmp OP1 1 290 130 -26 42 0 0 "1e6" 1 "15 V" 0>
  <R R1 1 210 110 -26 -43 1 0 "10 Ohm" 1 "26.85" 0 "european" 0>
  <R R2 1 260 250 -57 -26 1 1 "500k" 1 "26.85" 0 "european" 0>
  <GND * 1 530 160 0 0 0 0>
  <GND * 1 420 160 0 0 0 0>
  <Pac P2 1 420 130 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0>
  <R R4 1 530 130 15 -26 0 1 "500k" 1 "26.85" 0 "european" 0>
  <R R3 1 640 130 15 -26 0 1 "10 Ohm" 1 "26.85" 0 "european" 0>
  <GND * 1 640 250 0 0 0 0>
  <L L1 1 640 220 10 -26 0 1 "5 H" 1>
  <.SP SP1 1 80 230 0 51 0 0 "log" 1 "1 Hz" 1 "10 kHz" 1 "101" 1 "no" 0 "1" 0 "2" 0>
  <.DC DC1 1 340 270 0 33 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0
"150" 0 "no" 0 "none" 0>
</Components>
<Wires>
  <240 110 260 110 "" 0 0 0 "">
  <240 150 260 150 "" 0 0 0 "">
  <260 70 260 110 "" 0 0 0 "">
  <180 110 180 150 "" 0 0 0 "">
  <260 150 260 220 "" 0 0 0 "">
  <70 110 70 130 "" 0 0 0 "">
  <70 110 180 110 "" 0 0 0 "">
  <330 70 330 130 "" 0 0 0 "">
  <260 70 330 70 "" 0 0 0 "">
  <420 80 420 100 "" 0 0 0 "">
  <640 80 640 100 "" 0 0 0 "">
  <420 80 530 80 "" 0 0 0 "">
  <530 80 640 80 "" 0 0 0 "">
  <530 80 530 100 "" 0 0 0 "">
  <640 160 640 190 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 480 50 14 #000000 0 "equivalent circuit">
  <Text 90 50 14 #000000 0 "Gyrator">
  <Text 520 220 14 #000000 0 "L = R1 * R2 * C">
  <Text 330 200 12 #000000 0 "As can be seen: With the use of\nan OpAmps, a capacitor can\nrebuild a very large inductor.">
</Paintings>
