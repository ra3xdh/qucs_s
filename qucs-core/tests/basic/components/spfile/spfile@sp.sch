<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,893,800,1,0,0>
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
  <.SP SP1 1 660 90 0 77 0 0 "log" 1 "1 Hz" 1 "10 GHz" 1 "101" 1 "yes" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <R R1 1 250 100 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 370 100 -26 17 0 0 "1 pF" 1 "" 0 "neutral" 0>
  <Pac P1 1 140 170 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 140 330 0 0 0 0>
  <Pac P2 1 140 440 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 300 470 0 0 0 0>
  <SPfile X1 1 300 400 -26 -71 0 0 "./R50C1pF.s1p" 1 "rectangular" 0 "linear" 0 "open" 0 "1" 0>
  <Eqn Eqn1 1 470 410 -33 19 0 0 "assert=assert(abs(diff)<1e-6)" 1 "diff=S[1,1]-S[2,2]" 1 "yes" 0>
</Components>
<Wires>
  <280 100 340 100 "" 0 0 0 "">
  <400 100 400 330 "" 0 0 0 "">
  <140 330 400 330 "" 0 0 0 "">
  <140 200 140 330 "" 0 0 0 "">
  <140 100 220 100 "" 0 0 0 "">
  <140 100 140 140 "" 0 0 0 "">
  <300 430 300 470 "" 0 0 0 "">
  <140 400 140 410 "" 0 0 0 "">
  <140 400 270 400 "" 0 0 0 "">
  <140 470 300 470 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
