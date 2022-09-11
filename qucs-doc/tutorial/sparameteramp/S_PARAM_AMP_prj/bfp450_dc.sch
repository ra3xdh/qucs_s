<Qucs Schematic 0.0.18>
<Properties>
  <View=0,-60,1013,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=bfp450_dc.dat>
  <DataDisplay=bfp450_dc.dpl>
  <OpenDisplay=1>
  <Script=bfp450_dc.m>
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
  <.DC DC1 1 40 310 0 39 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 200 310 0 63 0 0 "DC1" 1 "lin" 1 "UCE" 1 "0" 1 "5" 1 "200" 1>
  <.SW SW2 1 330 310 0 63 0 0 "SW1" 1 "lin" 1 "UBE" 1 "0.7" 1 "0.9" 1 "21" 1>
  <GND * 1 210 270 0 0 0 0>
  <SPICE X1 1 210 210 -26 -83 1 2 "BFP450_spice.cir" 1 "_net1,_net2,_net3" 0 "yes" 0 "none" 0>
  <GND * 1 240 240 0 0 0 0>
  <IProbe Pr2 1 100 180 -26 16 0 0>
  <IProbe Pr1 1 380 180 -26 16 1 2>
  <GND * 1 440 260 0 0 0 0>
  <Vdc V2 1 440 230 -62 -26 1 1 "UCE" 1>
  <GND * 1 40 270 0 0 0 0>
  <Vdc V1 1 40 240 18 -26 0 1 "UBE" 1>
</Components>
<Wires>
  <130 180 180 180 "" 0 0 0 "">
  <40 180 70 180 "" 0 0 0 "">
  <240 180 350 180 "" 0 0 0 "">
  <410 180 440 180 "" 0 0 0 "">
  <440 180 440 200 "" 0 0 0 "">
  <40 180 40 210 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 540 281 331 231 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"Pr1.I" #0000ff 0 3 0 0 0>
	  <Mkr 2.51256/0.88 143 -216 3 0 0>
  </Rect>
  <Rect 543 524 333 198 3 #c0c0c0 1 00 1 0 0.5 5 0 0 0.0001 0.001 1 -1 0.2 1 315 0 225 "" "" "">
	<"Pr2.I" #0000ff 0 3 0 0 0>
	  <Mkr 2.98995/0.87 170 -163 3 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 150 80 14 #000000 0 "DC Parameter Simulation ">
</Paintings>
