<Qucs Schematic 25.1.2>
<Properties>
  <View=-315,-196,1488,821,1.05546,172,92>
  <Grid=10,10,1>
  <DataSet=bridge.dat>
  <DataDisplay=bridge.dpl>
  <OpenDisplay=0>
  <Script=bridge.m>
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
  <IProbe Pr1 1 340 200 -26 16 0 0>
  <R R2 1 460 130 15 -26 0 1 "500 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 180 200 18 -26 0 1 "1 V" 1>
  <GND * 1 280 300 0 0 0 0>
  <GND * 1 460 300 0 0 0 0>
  <R R5 1 460 270 15 -26 0 1 "Rmeasure" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 180 300 0 0 0 0>
  <R R1 1 280 130 15 -26 0 1 "Rbranch" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 280 270 15 -26 0 1 "Rbranch" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.DC DC1 1 30 380 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R R3 1 400 200 -26 15 0 0 "500 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.SW SW1 1 250 370 0 61 0 0 "DC1" 1 "lin" 1 "Rmeasure" 1 "10 Ohm" 1 "1 kOhm" 1 "100" 1>
  <.SW SW2 1 410 370 0 61 0 0 "SW1" 1 "lin" 1 "Rbranch" 1 "200 Ohm" 1 "1 kOhm" 1 "4" 1>
  <Eqn Eqn1 1 60 480 -23 14 0 0 "Umeasure=500 * abs(Pr1.I)" 1 "yes" 0>
</Components>
<Wires>
  <280 100 460 100 "" 0 0 0 "">
  <280 160 280 200 "" 0 0 0 "">
  <460 160 460 200 "" 0 0 0 "">
  <430 200 460 200 "" 0 0 0 "">
  <280 200 310 200 "" 0 0 0 "">
  <180 100 180 170 "" 0 0 0 "">
  <180 100 280 100 "" 0 0 0 "">
  <180 230 180 300 "" 0 0 0 "">
  <460 200 460 240 "" 0 0 0 "">
  <280 200 280 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 650 328 320 228 3 #c0c0c0 1 00 1 0 20 100 1 -89 500 1099 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"Umeasure" #ff0000 1 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 180 0 16 #000000 0 "Just a simple example:\nNo current flows through the middle branch,\nif the bridge is matched.">
</Paintings>
