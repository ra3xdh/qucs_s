<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=test.dat>
  <DataDisplay=test.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Titre>
  <FrameText1=Auteur :>
  <FrameText2=Date :>
  <FrameText3=Version :>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac P1 1 130 200 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 130 310 0 0 0 0>
  <R R1 1 270 100 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 360 310 0 0 0 0>
  <.SP SP1 1 420 100 0 77 0 0 "log" 1 "1 Hz" 1 "10 GHz" 1 "101" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 160 380 -33 19 0 0 "assert=assert(abs(S[1,1]))<1e-6" 1 "yes" 0>
</Components>
<Wires>
  <130 230 130 310 "" 0 0 0 "">
  <130 100 130 170 "" 0 0 0 "">
  <130 100 240 100 "" 0 0 0 "">
  <300 100 360 100 "" 0 0 0 "">
  <360 100 360 310 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
