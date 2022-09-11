<Qucs Schematic 0.0.4>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=resonance.dat>
  <DataDisplay=resonance.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Vac V1 1 120 150 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0>
  <GND * 1 320 240 0 0 0 0>
  <GND * 1 120 240 0 0 0 0>
  <R R1 1 320 90 15 -26 0 1 "R_par" 1 "26.85" 0 "european" 0>
  <C C1 1 320 210 17 -26 0 1 "2 pF" 1>
  <L L1 1 320 150 10 -26 0 1 "13 nH" 1>
  <.SW SW1 1 440 200 0 51 0 0 "AC1" 1 "log" 1 "R_par" 1 "1 Ohm" 1 "20 Ohm" 1 "6" 1>
  <.AC AC1 1 440 70 0 33 0 0 "lin" 1 "0.9 GHz" 1 "1.1 GHz" 1 "150" 1>
</Components>
<Wires>
  <120 180 120 240 "" 0 0 0>
  <120 60 320 60 "" 0 0 0>
  <120 60 120 120 "" 0 0 0>
  <320 180 320 180 "Voltage" 240 140 0>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 100 290 12 #000000 0 "The voltage overshot strongly depends on the quality\nof the resonance circuit.">
</Paintings>
