<Qucs Schematic 0.0.23>
<Properties>
  <View=0,-108,1000,780,1,0,0>
  <Grid=10,10,1>
  <DataSet=mrf501-sparam.dat>
  <DataDisplay=mrf501-sparam.dpl>
  <OpenDisplay=1>
  <Script=mrf501-sparam.m>
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
  <.SP SP1 1 740 60 0 65 0 0 "log" 1 "10k" 1 "20MEG" 1 "100" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Pac P2 1 350 140 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 350 170 0 0 0 0>
  <Pac P1 1 70 130 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 70 160 0 0 0 0>
  <GND * 1 220 170 0 0 0 0>
  <Sub DUT 1 250 140 -26 48 0 0 "/users/russo/Xyce_qucs-s_Examples/11-SParameters/MRF501.sch" 0>
</Components>
<Wires>
  <70 100 70 110 "" 0 0 0 "">
  <70 110 220 110 "" 0 0 0 "">
  <280 110 350 110 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Smith 540 470 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(1,1)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 540 720 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(2,1)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 780 470 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(1,2)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 780 720 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/S(2,2)" #0000ff 0 3 0 0 0>
  </Smith>
</Diagrams>
<Paintings>
</Paintings>
