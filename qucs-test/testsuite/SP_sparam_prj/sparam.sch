<Qucs Schematic 0.0.17>
<Properties>
  <View=65,72,1212,786,0.7,0,0>
  <Grid=10,10,1>
  <DataSet=sparam.dat>
  <DataDisplay=sparam.dpl>
  <OpenDisplay=0>
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
  <Pac P1 1 150 230 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 150 260 0 0 0 0>
  <C C1 1 260 230 17 -26 0 1 "6.796pF" 1 "" 0 "neutral" 0>
  <GND * 1 260 260 0 0 0 0>
  <L L1 1 330 150 -26 10 0 0 "8.683nH" 1 "" 0>
  <C C2 1 400 230 17 -26 0 1 "9.552pF" 1 "" 0 "neutral" 0>
  <GND * 1 400 260 0 0 0 0>
  <L L2 1 470 150 -26 10 0 0 "8.683nH" 1 "" 0>
  <GND * 1 540 260 0 0 0 0>
  <Pac P2 1 650 230 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 650 260 0 0 0 0>
  <Eqn Eqn1 1 350 340 -28 15 0 0 "dBS21=dB(S[2,1])" 1 "dBS11=dB(S[1,1])" 1 "yes" 0>
  <.SP SP1 1 160 330 0 50 0 0 "log" 1 "100MHz" 1 "2GHz" 1 "601" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <C C3 1 540 230 17 -26 0 1 "6.796pF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <150 150 150 200 "" 0 0 0 "">
  <150 150 260 150 "" 0 0 0 "">
  <260 150 260 200 "" 0 0 0 "">
  <400 150 400 200 "" 0 0 0 "">
  <540 150 540 200 "" 0 0 0 "">
  <260 150 300 150 "" 0 0 0 "">
  <360 150 400 150 "" 0 0 0 "">
  <400 150 440 150 "" 0 0 0 "">
  <500 150 540 150 "" 0 0 0 "">
  <650 150 650 200 "" 0 0 0 "">
  <540 150 650 150 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Smith 770 397 283 283 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"S[1,1]" #0000ff 0 3 0 0 0>
	  <Mkr 5.91486e+08 270 -22 3 0 0>
	  <Mkr 3.13733e+08 -75 -285 3 0 0>
	  <Mkr 8.10126e+08 -79 -19 3 0 0>
	  <Mkr 9.41031e+08 268 -274 3 0 0>
  </Smith>
  <Rect 170 712 376 222 3 #c0c0c0 1 00 1 1e+08 2e+08 2e+09 0 -20 5 0 1 -1 0.5 1 315 0 225 "" "" "">
	<"dBS21" #0000ff 0 3 0 0 0>
	  <Mkr 9.41031e+08 227 -282 3 0 0>
	  <Mkr 3.13733e+08 12 -181 3 0 0>
	  <Mkr 8.10126e+08 151 -181 3 0 0>
	  <Mkr 5.91486e+08 87 -282 3 0 0>
	<"dBS11" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 490 340 12 #000000 0 "Chebyshev low-pass filter\n1GHz cutoff, PI-type,\nimpedance matching 50 Ohm">
  <Text 590 530 16 #0000ff 0 "Qucs S-parameter simulation.\n\nMore details on:\nhttp://www.youtube.com/watch?v=99qwF1zVg5k&feature=endscreen&NR=1">
</Paintings>
