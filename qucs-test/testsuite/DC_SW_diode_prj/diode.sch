<Qucs Schematic 0.0.18>
<Properties>
  <View=0,0,977,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=diode.dat>
  <DataDisplay=diode.dpl>
  <OpenDisplay=0>
  <Script=diode.m>
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
  <GND * 1 260 280 0 0 0 0>
  <.DC DC1 1 80 360 0 33 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Idc I1 1 170 170 18 -26 0 1 "Idiode" 1>
  <Diode D1 1 350 170 15 -26 0 1 "1e-15 A" 1 "1" 1 "15 fF" 1 "0.5" 1 "0.7 V" 1 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <.SW SW1 1 260 350 0 51 0 0 "DC1" 1 "lin" 1 "Idiode" 1 "1e-3" 1 "1" 1 "12001" 1>
</Components>
<Wires>
  <170 80 170 140 "" 0 0 0 "">
  <170 80 350 80 "" 0 0 0 "">
  <350 80 350 140 "" 0 0 0 "">
  <350 200 350 260 "" 0 0 0 "">
  <170 200 170 260 "" 0 0 0 "">
  <170 260 260 260 "" 0 0 0 "">
  <260 260 350 260 "" 0 0 0 "">
  <260 260 260 280 "" 0 0 0 "">
  <350 80 350 80 "vdiode" 380 50 0 "">
</Wires>
<Diagrams>
  <Rect 510 447 420 367 3 #c0c0c0 1 00 1 0 0.1 1 1 0.68146 0.05 0.910755 1 -1 0.2 1 315 0 225 "" "" "">
	<"vdiode.V" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 60 10 12 #000000 0 "http://www.youtube.com/watch?v=TzJiYSi6ZRc&feature=related">
</Paintings>
