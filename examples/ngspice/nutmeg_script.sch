<Qucs Schematic 24.1.0>
<Properties>
  <View=146,70,1657,930,1,0,0>
  <Grid=10,10,1>
  <DataSet=nutmeg_script.dat>
  <DataDisplay=nutmeg_script.dpl>
  <OpenDisplay=0>
  <Script=nutmeg_script.m>
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
  <Vac V1 1 200 210 18 -26 0 1 "1 V" 1 "1 kHz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <C C1 1 270 140 -26 17 0 0 "1 nF" 1 "" 0 "neutral" 0>
  <L L1 1 370 140 -26 10 0 0 "1 mH" 1 "" 0>
  <GND * 1 440 240 0 0 0 0>
  <GND * 1 200 240 0 0 0 0>
  <R R1 1 440 210 15 -26 0 1 "1 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.CUSTOMSIM CUSTOM1 1 200 290 0 51 0 0 "\nAC DEC 100 1K 10MEG\nlet K=V(out)/V(in)\n\n* Extra output\n* A custom prefix could be placed between # #\n* It will be prepended to all dataset variables\nwrite custom#ac1#.plot K\n\n* Scalars can be printed\n* They will be available in the dataset\nlet Vout_max=vecmax(V(out))\nlet KdB_max=db(vecmax(K))\nprint Vout_max KdB_max > custom#ac1#.print\n" 1 "V(out);V(in)" 0 "custom#ac1#.plot;custom#ac1#.print" 0>
</Components>
<Wires>
  <200 140 200 180 "" 0 0 0 "">
  <200 140 240 140 "" 0 0 0 "">
  <300 140 340 140 "" 0 0 0 "">
  <400 140 440 140 "" 0 0 0 "">
  <440 140 440 180 "" 0 0 0 "">
  <200 140 200 140 "in" 230 110 0 "">
  <440 140 440 140 "out" 470 110 0 "">
</Wires>
<Diagrams>
  <Rect 620 290 356 167 3 #c0c0c0 1 10 1 0 2e+06 1e+07 1 -0.0930848 0.5 1.09933 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.v(out)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 620 510 363 165 3 #c0c0c0 1 11 1 0 2e+06 1e+07 1 nan 0.5 1.09933 1 -1 1 1 315 0 225 1 1 0 "" "" "">
	<"ngspice/ac1.ac.k" #0000ff 0 3 0 0 0>
  </Rect>
  <Tab 620 640 365 57 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac1.vout_max" #0000ff 0 3 1 0 0>
	<"ngspice/ac1.kdb_max" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
</Paintings>
