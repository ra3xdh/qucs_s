<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,867,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=ueqri.dat>
  <DataDisplay=ueqri.dpl>
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
  <IProbe current 1 450 130 -26 16 0 0>
  <R R1 1 340 130 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Eqn Eqn1 1 210 360 -32 17 0 0 "crashif=assert(abs(current.i-1) < 1e-6)" 1 "yes" 0>
  <GND * 1 360 270 0 0 0 0>
  <.AC AC1 1 540 360 0 77 0 0 "log" 1 "1 Hz" 1 "1 GHz" 1 "91" 1 "yes" 0>
  <Vac V1 1 180 200 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <370 130 420 130 "" 0 0 0 "">
  <480 130 570 130 "" 0 0 0 "">
  <570 130 570 270 "" 0 0 0 "">
  <180 270 360 270 "" 0 0 0 "">
  <180 230 180 270 "" 0 0 0 "">
  <180 130 180 170 "" 0 0 0 "">
  <180 130 310 130 "" 0 0 0 "">
  <360 270 570 270 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
