<Qucs Schematic 0.0.4>
<Properties>
  <View=0,16,790,531,1,0,0>
  <Grid=10,10,1>
  <DataSet=multiplier.dat>
  <DataDisplay=multiplier.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Diode D1 1 320 190 13 -26 1 3 "1e-15 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0>
  <Diode D2 1 380 190 13 -26 0 1 "1e-15 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0>
  <C C1 1 290 140 -26 -45 0 2 "1 uF" 1>
  <C C2 1 350 240 -26 17 1 2 "1 uF" 1>
  <Diode D3 1 500 190 13 -26 1 3 "1e-15 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0>
  <Diode D4 1 560 190 13 -26 0 1 "1e-15 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0>
  <C C3 1 470 140 -26 -45 0 2 "1 uF" 1>
  <C C4 1 530 240 -26 17 1 2 "1 uF" 1>
  <GND * 1 640 300 0 0 0 0>
  <R R_load 1 640 270 15 -26 0 1 "100 kOhm" 1 "26.85" 0 "european" 0>
  <GND * 1 100 240 0 0 0 0>
  <R R1 1 130 140 -26 -43 1 0 "1 Ohm" 1 "26.85" 0 "european" 0>
  <Vac V1 1 100 210 18 -26 0 1 "10 V" 1 "1 kHz" 1 "0" 0>
  <.TR TR1 1 90 350 0 51 0 0 "lin" 1 "0" 1 "30 ms" 1 "300" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0>
</Components>
<Wires>
  <320 140 320 160 "" 0 0 0>
  <320 220 320 240 "" 0 0 0>
  <320 140 380 140 "" 0 0 0>
  <380 140 380 160 "" 0 0 0>
  <380 220 380 240 "" 0 0 0>
  <500 220 500 240 "" 0 0 0>
  <500 140 500 160 "" 0 0 0>
  <560 220 560 240 "" 0 0 0>
  <560 140 560 160 "" 0 0 0>
  <500 140 560 140 "" 0 0 0>
  <380 240 500 240 "" 0 0 0>
  <380 140 440 140 "" 0 0 0>
  <560 240 640 240 "Output" 640 190 61>
  <100 140 100 180 "" 0 0 0>
  <100 240 320 240 "" 0 0 0>
  <160 140 260 140 "Input" 200 90 16>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Rectangle 250 80 170 220 #000000 0 1 #c0c0c0 1 0>
  <Rectangle 430 80 170 220 #000000 0 1 #c0c0c0 1 0>
  <Rectangle 70 80 100 210 #000000 0 1 #c0c0c0 1 0>
  <Text 90 310 12 #ff0000 0 "Generator">
  <Text 310 310 12 #ff0000 0 "1. stage">
  <Text 500 310 12 #ff0000 0 "2. stage">
  <Text 260 360 12 #000000 0 "Two-stage Villard circuit:\nEvery stage adds two-times the input voltage.\n(Decreased by the diode voltage drop)\nMany stages can be put in cascade.\nThis circuit can only drive a high-impedance load.">
</Paintings>
