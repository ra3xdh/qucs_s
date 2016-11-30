<Qucs Schematic 0.0.10>
<Properties>
  <View=0,0,658,570,1,0,0>
  <Grid=10,10,1>
  <DataSet=fgummel.dat>
  <DataDisplay=fgummel.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Titel>
  <FrameText1=Gezeichnet von:>
  <FrameText2=Datum:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Vdc VBE 1 40 160 18 -26 0 1 "Vbe" 1>
  <GND * 1 40 190 0 0 0 0>
  <GND * 1 180 190 0 0 0 0>
  <Vdc VCE 1 360 160 18 -26 0 1 "Vbe" 1>
  <GND * 1 360 190 0 0 0 0>
  <Vdc VSUB 1 270 160 18 -26 0 1 "0 V" 1>
  <GND * 1 270 190 0 0 0 0>
  <BJT T1 1 180 130 34 -26 0 0 "npn" 0 "1e-16" 0 "1" 0 "1.3" 0 "40mA" 0 "1mA" 0 "40" 0 "30" 0 "1e-16" 0 "1.5" 0 "1e-15" 0 "2" 0 "100" 0 "3.5" 0 "1.1" 0 "1.5" 0 "4" 0 "1" 0 "5" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "2.5p" 0 "0.0" 0 "0.0" 0 "0.0" 0 "12p" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <.DC DC1 1 50 250 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Eqn Eqn1 1 80 340 -39 16 0 0 "Ib=-VBE.I" 1 "Beta=Ic/Ib" 1 "Ic=-VCE.I" 1 "Isub=-VSUB.I" 1 "yes" 0>
  <.SW SW1 1 450 50 0 53 0 0 "DC1" 1 "lin" 1 "Vbe" 1 "0.1" 1 "1.1" 1 "201" 1>
</Components>
<Wires>
  <180 60 180 100 "" 0 0 0 "">
  <40 130 150 130 "" 0 0 0 "">
  <180 160 180 190 "" 0 0 0 "">
  <360 60 360 130 "" 0 0 0 "">
  <180 60 360 60 "" 0 0 0 "">
  <210 130 270 130 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 234 477 330 245 3 #c0c0c0 1 01 1 nan nan -1.79769e+308 1 nan 1 -1e+308 1 nan nan -inf 315 0 225 "Vbe" "" "">
	<"Ib" #0000ff 2 3 0 0 0>
	<"Ic" #ff0000 2 3 0 0 0>
	<"Beta" #00aa00 2 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 20 12 #000000 0 "forward gummel characteristic of a BJT">
</Paintings>
