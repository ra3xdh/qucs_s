<Qucs Schematic 24.1.0>
<Properties>
  <View=-40,-57,1011,581,1.26019,0,0>
  <Grid=10,10,1>
  <DataSet=NE3509_sub_Id_Vg.dat>
  <DataDisplay=NE3509_sub_Id_Vg.dpl>
  <OpenDisplay=0>
  <Script=NE3509_sub_Id_Vg.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 -16 SUB>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
</Symbol>
<Components>
  <GND * 1 60 200 0 0 0 0>
  <GND * 1 330 140 0 0 0 0>
  <Vdc Vds 1 330 90 18 -26 0 1 "10" 1>
  <GND * 1 150 150 0 0 0 0>
  <Sub Z1 1 150 100 13 -19 0 0 "NE3509_sub.sch" 1>
  <IProbe Ids 1 240 40 -14 -57 1 2>
  <.DC DC1 1 20 260 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 180 260 0 77 0 0 "DC1" 1 "lin" 1 "Vds" 1 "0" 1 "10" 1 "41" 1 "false" 0>
  <.SW SW2 1 320 270 0 77 0 0 "SW1" 1 "lin" 1 "Vgs" 1 "-2" 1 "0" 1 "6" 1 "false" 0>
  <Vdc Vgs 1 60 150 -69 -22 0 1 "-0.5" 1>
</Components>
<Wires>
  <270 40 330 40 "" 0 0 0 "">
  <330 120 330 140 "" 0 0 0 "">
  <330 40 330 60 "" 0 0 0 "">
  <150 40 210 40 "" 0 0 0 "">
  <150 40 150 70 "" 0 0 0 "">
  <150 130 150 150 "" 0 0 0 "">
  <60 180 60 200 "" 0 0 0 "">
  <60 100 120 100 "" 0 0 0 "">
  <60 100 60 120 "" 0 0 0 "">
  <330 40 330 40 "Vds" 350 10 0 "">
  <60 100 60 100 "Vgs" 20 70 0 "">
</Wires>
<Diagrams>
  <Rect 490 304 386 294 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/i(vids)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
