<Qucs Schematic 0.0.24>
<Properties>
  <View=0,44,1573,972,1,8,0>
  <Grid=10,10,1>
  <DataSet=s_param_lc_filter.dat>
  <DataDisplay=s_param_lc_filter.dpl>
  <OpenDisplay=0>
  <Script=s_param_lc_filter.m>
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
  <GND * 1 230 320 0 0 0 0>
  <C C1 1 230 290 17 -26 0 1 "68p" 1 "" 0 "neutral" 0>
  <GND * 1 320 320 0 0 0 0>
  <C C2 1 320 290 17 -26 0 1 "68p" 1 "" 0 "neutral" 0>
  <L L1 1 270 180 -26 10 0 0 "0.68u" 1 "" 0>
  <L L2 1 360 180 -26 10 0 0 "0.68u" 1 "" 0>
  <GND * 1 410 320 0 0 0 0>
  <C C3 1 410 290 17 -26 0 1 "68p" 1 "" 0 "neutral" 0>
  <Pac P2 1 500 260 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "8 MHz" 0 "26.85" 0>
  <Pac P1 1 170 260 -95 -26 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "8 MHz" 0 "26.85" 0>
  <GND * 1 170 320 0 0 0 0>
  <GND * 1 500 320 0 0 0 0>
  <.SP SP1 1 40 390 0 87 0 0 "log" 1 "1MHz" 1 "100MHz" 1 "200" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <230 180 230 260 "" 0 0 0 "">
  <320 180 320 260 "" 0 0 0 "">
  <300 180 320 180 "" 0 0 0 "">
  <230 180 240 180 "" 0 0 0 "">
  <320 180 330 180 "" 0 0 0 "">
  <390 180 410 180 "" 0 0 0 "">
  <410 180 410 260 "" 0 0 0 "">
  <410 180 500 180 "out" 440 120 90 "">
  <500 180 500 230 "" 0 0 0 "">
  <170 180 230 180 "" 0 0 0 "">
  <170 180 170 230 "" 0 0 0 "">
  <170 290 170 320 "" 0 0 0 "">
  <500 290 500 320 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 217 740 384 306 3 #c0c0c0 1 00 1 0 2e+07 1e+08 1 -55.9848 10 5.07603 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/SDB(2,1)" #0000ff 0 3 0 0 0>
	  <Mkr 3.46737e+07 153 -380 3 0 0>
  </Rect>
  <Rect 680 762 409 332 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/S(2,1)" #0000ff 0 3 0 0 0>
  </Rect>
  <Smith 664 396 332 332 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(1,1)" #0000ff 0 3 0 0 0>
	  <Mkr 4.0738e+07 164 -308 3 0 0>
  </Smith>
</Diagrams>
<Paintings>
  <Text 80 90 12 #000000 0 "This example illustrates S-parameter\nsimulation on the passive circuit using Xyce">
</Paintings>
