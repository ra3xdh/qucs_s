<Qucs Schematic 24.1.0>
<Properties>
  <View=10,-55,1058,581,1.26415,1,0>
  <Grid=10,10,1>
  <DataSet=Statz_sub_Id_Vg.dat>
  <DataDisplay=Statz_sub_Id_Vg.dpl>
  <OpenDisplay=0>
  <Script=Statz_sub_Id_Vg.m>
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
  <GND * 1 400 140 0 0 0 0>
  <Vdc Vds 1 400 90 18 -26 0 1 "10" 1>
  <GND * 1 140 200 0 0 0 0>
  <GND * 1 220 150 0 0 0 0>
  <Sub Z1 1 220 100 20 -16 0 0 "Statz_sub.sch" 1>
  <IProbe Ids 1 310 40 -9 -55 1 2>
  <.DC DC1 1 60 290 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 230 290 0 77 0 0 "DC1" 1 "lin" 1 "Vds" 1 "0" 1 "10" 1 "41" 1 "false" 0>
  <.SW SW2 1 370 290 0 77 0 0 "SW1" 1 "lin" 1 "Vgs" 1 "-2" 1 "0" 1 "6" 1 "false" 0>
  <Vdc Vgs 1 140 150 -78 -22 0 1 "-0.25" 1>
</Components>
<Wires>
  <400 120 400 140 "" 0 0 0 "">
  <140 180 140 200 "" 0 0 0 "">
  <140 100 140 120 "" 0 0 0 "">
  <140 100 190 100 "" 0 0 0 "">
  <220 130 220 150 "" 0 0 0 "">
  <220 40 220 70 "" 0 0 0 "">
  <220 40 280 40 "" 0 0 0 "">
  <400 40 400 60 "" 0 0 0 "">
  <340 40 400 40 "" 0 0 0 "">
  <140 100 140 100 "Vgs" 100 70 0 "">
  <400 40 400 40 "Vds" 420 10 0 "">
</Wires>
<Diagrams>
  <Rect 570 324 386 294 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/i(vids)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
