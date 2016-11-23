<Qucs Schematic 0.0.3>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=bridge.dat>
  <DataDisplay=bridge.dpl>
  <OpenDisplay=1>
</Properties>
<Components>
  <IProbe Pr1 1 340 200 -26 16 0 0>
  <R R2 1 460 130 15 -26 0 1 "500 Ohm" 1 "26.85" 0 "european" 0>
  <Vdc V1 1 180 200 18 -26 0 1 "1 V" 1>
  <GND * 1 280 300 0 0 0 0>
  <GND * 1 460 300 0 0 0 0>
  <R R5 1 460 270 15 -26 0 1 "Rmeasure" 1 "26.85" 0 "european" 0>
  <GND * 1 180 300 0 0 0 0>
  <R R1 1 280 130 15 -26 0 1 "Rbranch" 1 "26.85" 0 "european" 0>
  <R R4 1 280 270 15 -26 0 1 "Rbranch" 1 "26.85" 0 "european" 0>
  <.DC DC1 1 180 360 -54 26 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0>
  <R R3 1 400 200 -26 15 0 0 "500 Ohm" 1 "26.85" 0 "european" 0>
  <.SW SW1 1 300 370 -42 35 0 0 "DC1" 1 "lin" 1 "Rmeasure" 1 "10 Ohm" 1 "1 kOhm" 1 "100" 1>
  <.SW SW2 1 410 370 -42 35 0 0 "SW1" 1 "lin" 1 "Rbranch" 1 "200 Ohm" 1 "1 kOhm" 1 "4" 1>
  <Eqn Eqn1 1 500 360 -23 14 0 0 "Umeasure=500 * abs(Pr1.I)" 1 "yes" 0>
</Components>
<Wires>
  <280 100 460 100 "" 0 0 0>
  <280 160 280 200 "" 0 0 0>
  <460 160 460 200 "" 0 0 0>
  <430 200 460 200 "" 0 0 0>
  <280 200 310 200 "" 0 0 0>
  <180 100 180 170 "" 0 0 0>
  <180 100 280 100 "" 0 0 0>
  <180 230 180 300 "" 0 0 0>
  <460 200 460 240 "" 0 0 0>
  <280 200 280 240 "" 0 0 0>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 180 30 16 #000000 0 "Just a simple example:\nNo current flows through the middle branch,\nif the bridge is matched.">
</Paintings>
