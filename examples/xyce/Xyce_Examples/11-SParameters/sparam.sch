<Qucs Schematic 0.0.23>
<Properties>
  <View=0,0,1050,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=sparam.dat>
  <DataDisplay=sparam.dpl>
  <OpenDisplay=1>
  <Script=sparam.m>
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
  <Pac P1 1 140 180 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 600 180 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 140 210 0 0 0 0>
  <GND * 1 290 220 0 0 0 0>
  <GND * 1 390 220 0 0 0 0>
  <GND * 1 510 220 0 0 0 0>
  <GND * 1 600 210 0 0 0 0>
  <C C1 1 290 190 17 -26 0 1 "68pF" 1 "" 0 "neutral" 0>
  <C C2 1 390 190 17 -26 0 1 "68pF" 1 "" 0 "neutral" 0>
  <C C3 1 510 190 17 -26 0 1 "68pF" 1 "" 0 "neutral" 0>
  <L L1 1 330 110 -26 10 0 0 "0.68u" 1 "" 0>
  <L L2 1 450 110 -26 10 0 0 "0.68u" 1 "" 0>
  <.SP SP1 1 130 360 0 65 0 0 "log" 1 "1MHz" 1 "100MHz" 1 "151" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <140 110 140 150 "" 0 0 0 "">
  <140 110 290 110 "" 0 0 0 "">
  <360 110 390 110 "" 0 0 0 "">
  <480 110 510 110 "" 0 0 0 "">
  <600 110 600 150 "" 0 0 0 "">
  <290 110 300 110 "" 0 0 0 "">
  <290 110 290 160 "" 0 0 0 "">
  <390 110 420 110 "" 0 0 0 "">
  <390 110 390 160 "" 0 0 0 "">
  <510 110 600 110 "" 0 0 0 "">
  <510 110 510 160 "" 0 0 0 "">
  <600 110 600 110 "out" 550 70 0 "">
</Wires>
<Diagrams>
  <Rect 770 230 240 160 3 #c0c0c0 1 00 1 0 2e+07 1e+08 1 -60 20 6.18051 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/SDB(2,1)" #ff0000 0 3 0 0 0>
  </Rect>
  <Smith 720 530 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(1,1)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 400 540 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(2,1)" #0000ff 0 3 0 0 0>
  </Smith>
</Diagrams>
<Paintings>
</Paintings>
