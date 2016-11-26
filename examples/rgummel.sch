<Qucs Schematic 0.0.10>
<Properties>
  <View=-41,0,634,560,1,60,0>
  <Grid=10,10,1>
  <DataSet=rgummel.dat>
  <DataDisplay=rgummel.dpl>
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
  <GND * 1 200 180 0 0 0 0>
  <Vdc VCE 1 380 150 18 -26 0 1 "Vce" 1>
  <GND * 1 380 180 0 0 0 0>
  <Vdc VSUB 1 290 150 18 -26 0 1 "0 V" 1>
  <GND * 1 290 180 0 0 0 0>
  <.DC DC1 1 60 250 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 60 180 0 0 0 0>
  <BJT T1 1 200 120 34 -26 0 0 "npn" 0 "1e-16" 0 "1" 0 "1.3" 0 "40mA" 0 "1mA" 0 "40" 0 "30" 0 "1e-16" 0 "1.5" 0 "1e-15" 0 "2" 0 "100" 0 "3.5" 0 "1.1" 0 "1.5" 0 "4" 0 "1" 0 "5" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "2.5p" 0 "0.0" 0 "0.0" 0 "0.0" 0 "12p" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <.SW SW1 1 480 40 0 53 0 0 "DC1" 1 "lin" 1 "Vce" 1 "-0.1" 1 "-1.1" 1 "201" 1>
  <Eqn Eqn1 1 90 340 -39 16 0 0 "Ib=PlotVs(VBE.I,Vbc)" 1 "Vbc=-Vce" 1 "Beta=Ie/Ib" 1 "Ic=PlotVs(VCE.I,Vbc)" 1 "Isub=-VSUB.I" 1 "Ie=Ic+Ib" 1 "yes" 0>
  <Vdc VBE 1 60 150 18 -26 1 3 "0 V" 1>
</Components>
<Wires>
  <200 50 200 90 "" 0 0 0 "">
  <60 120 170 120 "" 0 0 0 "">
  <200 150 200 180 "" 0 0 0 "">
  <380 50 380 120 "" 0 0 0 "">
  <200 50 380 50 "" 0 0 0 "">
  <230 120 290 120 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 242 472 332 254 3 #c0c0c0 1 01 1 0.1 0.1 1.1 1 1e-15 1 0.03 1 3.32227 0.5 5.60858 315 0 225 "Vbc" "" "">
	<"Ib" #0000ff 2 3 0 0 0>
	<"Ie" #ff0000 2 3 0 0 0>
	<"Beta" #00aa00 2 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 20 12 #000000 0 "reverse gummel characteristic of a BJT">
</Paintings>
