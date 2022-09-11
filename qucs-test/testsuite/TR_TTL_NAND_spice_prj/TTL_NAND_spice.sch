<Qucs Schematic 0.0.19>
<Properties>
  <View=-41,199,583,740,1.45673,0,0>
  <Grid=10,10,1>
  <DataSet=TTL_NAND_spice.dat>
  <DataDisplay=TTL_NAND_spice.dpl>
  <OpenDisplay=1>
  <Script=TTL_NAND_spice.m>
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
  <SPICE X1 1 280 330 -26 -91 0 0 "NAND_GATE.cir" 1 "_netA,_netVCC,_netB,_netY" 0 "yes" 0 "none" 0>
  <GND * 1 280 490 0 0 0 0>
  <C C1 1 380 410 17 -26 0 1 "5 pF" 1 "" 0 "neutral" 0>
  <.TR TR1 1 20 550 0 71 0 0 "lin" 1 "0" 1 "5 ms" 1 "101" 0 "Gear" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vrect VA 1 80 380 -80 -30 0 1 "5 V" 1 "1 ms" 1 "1 ms" 1 "10 ns" 1 "10 ns" 1 "1 ms" 1>
  <Vrect VA1 1 190 410 -80 -30 0 1 "5 V" 1 "1.3 ms" 1 "1.3 ms" 1 "10 ns" 1 "10 ns" 1 "1 ms" 1>
  <Vdc V1 1 500 340 18 -26 0 1 "5 V" 1>
  <GND * 1 500 380 0 0 0 0>
</Components>
<Wires>
  <280 390 280 460 "" 0 0 0 "">
  <280 460 280 490 "" 0 0 0 "">
  <310 360 380 360 "out" 380 330 45 "">
  <380 360 380 380 "" 0 0 0 "">
  <280 460 380 460 "" 0 0 0 "">
  <380 440 380 460 "" 0 0 0 "">
  <80 460 190 460 "" 0 0 0 "">
  <80 410 80 460 "" 0 0 0 "">
  <80 300 250 300 "A" 60 260 13 "">
  <80 300 80 350 "" 0 0 0 "">
  <190 360 190 380 "" 0 0 0 "">
  <190 360 250 360 "B" 180 320 22 "">
  <190 460 280 460 "" 0 0 0 "">
  <190 440 190 460 "" 0 0 0 "">
  <310 300 500 300 "" 0 0 0 "">
  <500 300 500 310 "" 0 0 0 "">
  <500 370 500 380 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
