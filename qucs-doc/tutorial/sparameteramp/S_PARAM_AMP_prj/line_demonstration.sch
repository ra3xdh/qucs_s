<Qucs Schematic 0.0.18>
<Properties>
  <View=0,0,1054,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=line_demonstration.dat>
  <DataDisplay=line_demonstration.dpl>
  <OpenDisplay=1>
  <Script=line_demonstration.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac P1 1 130 240 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 430 240 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 430 270 0 0 0 0>
  <GND * 1 130 270 0 0 0 0>
  <.SP SP1 1 130 370 0 63 0 0 "lin" 1 "0.1 GHz" 1 "30 GHz" 1 "2000" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <MLIN MSTC1 1 270 210 -26 15 0 0 "SubstTC1" 1 "0.912387 mm" 1 "30.099 mm" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <SUBST SubstTC1 1 970 200 -30 24 0 0 "4" 1 "0.5 mm" 1 "317.5 um" 1 "0.1" 1 "2.43902e-08" 1 "2.54e-05" 1>
</Components>
<Wires>
  <300 210 430 210 "" 0 0 0 "">
  <130 210 240 210 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
