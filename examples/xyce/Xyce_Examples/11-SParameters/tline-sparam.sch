<Qucs Schematic 0.0.23>
<Properties>
  <View=0,-108,1000,780,1,0,0>
  <Grid=10,10,1>
  <DataSet=tline-sparam.dat>
  <DataDisplay=tline-sparam.dpl>
  <OpenDisplay=1>
  <Script=tline-sparam.m>
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
  <Pac P1 1 150 140 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 350 140 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 150 170 0 0 0 0>
  <GND * 1 350 170 0 0 0 0>
  <SpiceModel SpiceModel1 1 90 290 -30 16 0 0 ".MODEL testline transline r=1e-3 l=0.0u c=60p" 1 "" 0 "" 0 "" 0 "Line_5=" 0>
  <SPICE_dev DUT1 1 250 110 -26 -97 0 0 "2" 1 "YTRANSLINE " 1 "testline" 0 "len=12 lumps=1440" 1>
</Components>
<Wires>
  <150 110 210 110 "" 0 0 0 "">
  <290 110 350 110 "" 0 0 0 "">
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
