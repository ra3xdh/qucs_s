<Qucs Schematic 0.0.14>
<Properties>
  <View=0,0,1207,800,0.7,0,0>
  <Grid=10,10,1>
  <DataSet=test1.dat>
  <DataDisplay=test1.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Tytu\x0142>
  <FrameText1=Stworzona przez:>
  <FrameText2=Data:>
  <FrameText3=Rewizja:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <R R1 1 240 190 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 150 220 18 -26 0 1 "{A1}" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 150 250 0 0 0 0>
  <GND * 1 350 250 0 0 0 0>
  <VProbe Pr1 1 490 130 28 -31 0 0>
  <GND * 1 500 150 0 0 0 0>
  <.SW SW1 1 810 70 0 73 0 0 "AC1" 1 "lin" 1 "Cctr" 1 "0" 1 "100n" 1 "3" 1>
  <.SW SW2 1 1020 70 0 72 0 0 "SW1" 1 "lin" 1 "A1" 1 "1V" 1 "2V" 1 "2" 1>
  <.AC AC1 1 620 70 0 44 0 0 "log" 1 "10" 1 "10 MHz" 1 "10" 1 "no" 0>
  <C C1 1 350 220 17 -26 0 1 "{Cctr}" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <150 190 210 190 "" 0 0 0 "">
  <270 190 350 190 "" 0 0 0 "">
  <470 150 470 190 "" 0 0 0 "">
  <470 150 480 150 "" 0 0 0 "">
  <350 190 470 190 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
