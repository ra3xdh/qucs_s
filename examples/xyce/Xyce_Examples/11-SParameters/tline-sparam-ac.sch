<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-190,1808,729,1,550,240>
  <Grid=10,10,1>
  <DataSet=tline-sparam-ac.dat>
  <DataDisplay=tline-sparam-ac.dpl>
  <OpenDisplay=1>
  <Script=AC-sparams.m>
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
  <Vac V1 1 140 220 18 -26 0 1 "2 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 140 250 0 0 0 0>
  <R R1 1 210 140 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vac V2 1 320 140 -26 -56 0 2 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 380 240 0 0 0 0>
  <GND * 1 400 490 0 0 0 0>
  <R R2 1 380 210 -84 -26 0 3 "1MEG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 400 460 -101 -26 0 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 540 180 -101 -26 0 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 540 210 0 0 0 0>
  <Vac V3 1 730 200 -26 -56 0 2 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <R R5 1 760 230 -84 -26 0 3 "1MEG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 760 260 0 0 0 0>
  <R R6 1 650 230 -101 -26 0 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vac V4 1 650 290 18 -26 0 1 "2V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 650 320 0 0 0 0>
  <SPICE_dev DUT1 1 310 390 -26 -97 0 0 "2" 1 "YTRANSLINE " 1 "testline" 0 "len=12 lumps=1440" 1>
  <SPICE_dev DUT2 1 610 140 -26 -97 0 0 "2" 1 "YTRANSLINE " 1 "testline" 0 "len=12 lumps=1440" 1>
  <.XYCESCR XYCESCR1 1 920 -20 0 51 0 0 "\n.AC DEC 100 10K 20MEG\n.PRINT AC format=raw file=ac.txt V(S11) V(S12) V(S21) V(S22)" 1 "" 0 "ac.txt" 0>
  <SpiceModel SpiceModel1 1 130 530 -30 16 0 0 ".MODEL testline transline r=1e-3 l=0.0u c=60p" 1 "" 0 "" 0 "" 0 "Line_5=" 0>
</Components>
<Wires>
  <140 140 140 190 "" 0 0 0 "">
  <140 140 180 140 "" 0 0 0 "">
  <350 140 380 140 "" 0 0 0 "">
  <380 140 380 180 "" 0 0 0 "">
  <240 140 260 140 "" 0 0 0 "">
  <260 140 290 140 "" 0 0 0 "">
  <260 140 260 390 "" 0 0 0 "">
  <260 390 270 390 "" 0 0 0 "">
  <350 390 400 390 "" 0 0 0 "">
  <400 390 400 430 "" 0 0 0 "">
  <650 200 700 200 "" 0 0 0 "">
  <650 140 650 200 "" 0 0 0 "">
  <540 140 540 150 "" 0 0 0 "">
  <540 140 570 140 "" 0 0 0 "">
  <760 200 760 200 "S22" 790 170 0 "">
  <380 140 380 140 "S11" 410 110 0 "">
  <400 390 400 390 "S21" 430 360 0 "">
  <540 140 540 140 "S12" 570 110 0 "">
</Wires>
<Diagrams>
  <Smith 930 320 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 0 0 0 "" "" "">
	<"xyce/tline-sparam-ac:ac.V(S11)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 930 580 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 0 0 0 "" "" "">
	<"xyce/tline-sparam-ac:ac.V(S21)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 1200 320 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 0 0 0 "" "" "">
	<"xyce/tline-sparam-ac:ac.V(S12)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 1200 590 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 0 0 0 "" "" "">
	<"xyce/tline-sparam-ac:ac.V(S22)" #0000ff 0 3 0 0 0>
  </Smith>
</Diagrams>
<Paintings>
  <Text 100 -170 12 #000000 0 "This netlist demonstrates how to generate S parameters for a device under test using only standard SPICE AC analysis in Xyce.\n\nTwo copies of the device under test (in this case, a lumped-element transmission line), wired in two unconnected circuits.  \nTwo S parameters are obtained from each of the circuits, and plotted in Smith charts.\n\nA companion schematic, tline-sparam.sch, computes S parameters using Xyce's S-parameter analysis for the same device.\n\nOne can see that both schematics produce the same Smith charts.\n">
</Paintings>
