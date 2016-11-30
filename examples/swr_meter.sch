<Qucs Schematic 0.0.4>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=swr_meter.dat>
  <DataDisplay=swr_meter.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 380 180 0 0 0 0>
  <GND * 1 320 240 0 0 0 2>
  <R R1 1 260 350 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "european" 0>
  <R R2 1 440 350 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "european" 0>
  <GND * 1 440 380 0 0 0 0>
  <GND * 1 260 380 0 0 0 0>
  <R R4 1 170 120 -26 -43 1 0 "50 Ohm" 1 "26.85" 0 "european" 0>
  <GND * 1 140 220 0 0 0 0>
  <GND * 1 620 220 0 0 0 0>
  <Vac V1 1 140 190 18 -26 0 1 "2 V" 1 "1 GHz" 0 "0" 0>
  <Tr Tr2 1 350 270 37 -29 1 3 "25" 1>
  <R Load 1 620 190 15 -26 0 1 "R_load" 1 "26.85" 0 "european" 0>
  <Tr Tr1 1 350 150 37 -29 0 1 "25" 1>
  <.AC AC1 1 70 280 0 33 0 0 "lin" 1 "1 GHz" 1 "2 GHz" 1 "2" 1>
  <Eqn Eqn1 1 570 320 -19 13 0 0 "r_calc=(R_load-50)/(R_load+50)" 1 "r_measure=reflected.v / forward.v" 1 "yes" 0>
  <.SW SW1 1 70 410 0 51 0 0 "AC1" 1 "lin" 1 "R_load" 1 "2 Ohm" 1 "200 Ohm" 1 "100" 1>
</Components>
<Wires>
  <300 180 300 300 "" 0 0 0>
  <300 180 320 180 "" 0 0 0>
  <300 300 320 300 "" 0 0 0>
  <260 300 300 300 "reflected" 208 270 9>
  <260 300 260 320 "" 0 0 0>
  <440 300 440 320 "" 0 0 0>
  <380 300 440 300 "forward" 440 270 41>
  <200 120 320 120 "" 0 0 0>
  <620 120 620 160 "Output" 650 110 23>
  <380 120 440 120 "" 0 0 0>
  <140 120 140 160 "" 0 0 0>
  <440 120 620 120 "" 0 0 0>
  <440 120 440 240 "" 0 0 0>
  <380 240 440 240 "" 0 0 0>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Rectangle 100 60 130 200 #000000 0 1 #c0c0c0 1 0>
  <Rectangle 580 90 130 190 #000000 0 1 #c0c0c0 1 0>
  <Text 590 260 12 #000000 0 "load (e.g. antenna)">
  <Text 160 240 12 #000000 0 "generator">
  <Text 220 420 12 #000000 0 "This is a VSWR meter often used to measure if the antenna\nis matched. It is connected between generator and load.\nIt creates a voltage proportional to the forward voltage and\none proportional to the reflected voltage. The equations\non this page calculates the reflection coefficient using\nthe definition and using the voltages. As can be seen they\nequal each other. The mismatch created by the VSWR meter\nis very low.">
</Paintings>
