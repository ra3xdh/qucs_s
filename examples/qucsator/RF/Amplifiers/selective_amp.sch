<Qucs Schematic 0.0.5>
<Properties>
  <View=0,0,883,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=selective_amp.dat>
  <DataDisplay=selective_amp.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 120 330 0 0 0 0>
  <GND * 1 190 330 0 0 0 0>
  <GND * 1 280 330 0 0 0 0>
  <R R1 1 120 150 15 -26 0 1 "100k" 1 "26.85" 0 "european" 0>
  <R R2 1 120 250 15 -26 0 1 "10k" 1 "26.85" 0 "european" 0>
  <R R3 1 190 300 15 -26 0 1 "500" 1 "26.85" 0 "european" 0>
  <.DC DC1 1 40 20 0 33 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0>
  <GND * 1 20 330 0 0 0 0>
  <DCBlock C3 1 70 200 -26 21 0 0>
  <_BJT T1 1 190 200 8 -26 0 0 "npn" 0 "1e-16" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "100" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0>
  <C C1 1 190 110 19 -26 0 1 "1.5n" 1>
  <R R4 1 270 110 15 -26 0 1 "100k" 1 "26.85" 0 "european" 0>
  <L L1 1 350 110 10 -26 0 1 "80u" 1>
  <Vac V2 1 20 270 18 -26 0 1 "1 uV" 1 "1 GHz" 0 "0" 0>
  <C C2 1 280 300 17 -26 0 1 "100n" 1>
  <GND * 1 420 230 0 0 0 0>
  <Vdc V1 1 420 200 -60 -26 1 1 "12 V" 1>
  <.AC AC1 1 370 280 0 33 0 0 "log" 1 "300 kHz" 1 "800 kHz" 1 "100" 1>
  <Eqn Eqn1 1 500 200 -23 12 0 0 "Gain=dB(Output.v/Input.v)" 1 "yes" 0>
</Components>
<Wires>
  <120 180 120 200 "" 0 0 0 "">
  <120 200 120 220 "" 0 0 0 "">
  <120 200 160 200 "" 0 0 0 "">
  <190 230 190 250 "" 0 0 0 "">
  <280 250 280 270 "" 0 0 0 "">
  <190 250 190 270 "" 0 0 0 "">
  <190 250 280 250 "" 0 0 0 "">
  <120 280 120 330 "" 0 0 0 "">
  <100 200 120 200 "" 0 0 0 "">
  <20 200 20 240 "" 0 0 0 "">
  <20 200 40 200 "" 0 0 0 "">
  <20 300 20 330 "" 0 0 0 "">
  <190 140 270 140 "" 0 0 0 "">
  <190 140 190 170 "Output" 260 180 18 "">
  <120 80 190 80 "" 0 0 0 "">
  <120 80 120 120 "" 0 0 0 "">
  <190 80 270 80 "" 0 0 0 "">
  <270 80 350 80 "" 0 0 0 "">
  <270 140 350 140 "" 0 0 0 "">
  <420 80 420 170 "" 0 0 0 "">
  <350 80 420 80 "" 0 0 0 "">
  <120 200 120 200 "Input" 53 150 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 440 60 12 #000000 0 "Classical design of selective voltage amplifier:\n- common-emitter circuit\n- bias control with voltage divider R1, R2\n- bias stabilisation with negative current\n  feedback R3\n- use C2 to prevent R3 from decreasing gain\n- filtering with high Q resonance C1/L1/R4 load">
</Paintings>
