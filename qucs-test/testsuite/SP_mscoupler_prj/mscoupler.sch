<Qucs Schematic 0.0.7>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=mscoupler.dat>
  <DataDisplay=mscoupler.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac P1 1 80 90 -74 -26 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 300 90 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P3 1 260 180 18 -26 0 1 "3" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P4 1 120 160 -74 -26 1 1 "4" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 80 120 0 0 0 0>
  <GND * 1 120 190 0 0 0 0>
  <GND * 1 300 120 0 0 0 0>
  <GND * 1 260 210 0 0 0 0>
  <SUBST SubstTC1 1 390 180 -30 24 0 0 "9.8" 1 "0.635 mm" 1 "17.5 um" 1 "0.0001" 1 "2.43902e-08" 1 "1.5e-07" 1>
  <.SP SPTC1 1 80 250 0 51 0 0 "lin" 1 "0.2 GHz" 1 "4.2 GHz" 1 "101" 1 "no" 0 "1" 0 "2" 0>
  <MCOUPLED MSTC1 1 170 70 -26 37 0 0 "SubstTC1" 1 "0.518 mm" 1 "14.94 mm" 1 "0.185 mm" 1 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
  <Eqn Eqn1 1 250 300 -23 14 0 0 "reflect=dB(S[1,1])" 1 "isolated=dB(S[3,1])" 1 "through=dB(S[2,1])" 1 "coupled=dB(S[4,1])" 1 "yes" 0>
</Components>
<Wires>
  <80 40 140 40 "" 0 0 0 "">
  <80 40 80 60 "" 0 0 0 "">
  <120 100 120 130 "" 0 0 0 "">
  <120 100 140 100 "" 0 0 0 "">
  <300 40 300 60 "" 0 0 0 "">
  <200 40 300 40 "" 0 0 0 "">
  <260 100 260 150 "" 0 0 0 "">
  <200 100 260 100 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
