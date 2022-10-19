<Qucs Schematic 0.0.23>
<Properties>
  <View=0,-250,1808,631,1,65,0>
  <Grid=10,10,1>
  <DataSet=mrf501-sparam-ac.dat>
  <DataDisplay=mrf501-sparam-ac.dpl>
  <OpenDisplay=1>
  <Script=mrf501-sparam-ac.m>
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
  <R R2 1 380 210 -84 -26 0 3 "1MEG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 540 180 -101 -26 0 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 540 210 0 0 0 0>
  <.XYCESCR XYCESCR1 1 920 -20 0 51 0 0 "\n.AC DEC 100 10K 20MEG\n.PRINT AC format=raw file=ac.txt V(S11) V(S12) V(S21) V(S22)" 1 "" 0 "ac.txt" 0>
  <GND * 1 260 450 0 0 0 0>
  <GND * 1 390 490 0 0 0 0>
  <R R3 1 390 460 9 -23 0 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Sub DUT1 1 310 420 -26 48 0 0 "MRF501.sch" 0>
  <Sub DUT2 1 650 170 -26 48 0 0 "MRF501.sch" 0>
  <GND * 1 620 200 0 0 0 0>
  <Vac V3 1 810 260 -26 -56 0 2 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <R R5 1 840 290 -84 -26 0 3 "1MEG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 840 320 0 0 0 0>
  <R R6 1 730 290 -101 -26 0 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vac V4 1 730 350 18 -26 0 1 "2V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 730 380 0 0 0 0>
</Components>
<Wires>
  <140 140 140 190 "" 0 0 0 "">
  <140 140 180 140 "" 0 0 0 "">
  <350 140 380 140 "" 0 0 0 "">
  <380 140 380 180 "" 0 0 0 "">
  <240 140 260 140 "" 0 0 0 "">
  <540 140 540 150 "" 0 0 0 "">
  <260 140 290 140 "" 0 0 0 "">
  <260 450 280 450 "" 0 0 0 "">
  <390 390 390 430 "" 0 0 0 "">
  <340 390 390 390 "" 0 0 0 "">
  <260 140 260 390 "" 0 0 0 "">
  <260 390 280 390 "" 0 0 0 "">
  <540 140 620 140 "" 0 0 0 "">
  <730 260 780 260 "" 0 0 0 "">
  <680 140 730 140 "" 0 0 0 "">
  <730 140 730 260 "" 0 0 0 "">
  <840 260 840 260 "S22" 870 230 0 "">
  <380 140 380 140 "S11" 410 110 0 "">
  <540 140 540 140 "S12" 570 110 0 "">
  <390 390 390 390 "S21" 420 360 0 "">
</Wires>
<Diagrams>
  <Smith 930 320 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/ac.V(S11)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 930 580 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/ac.V(S21)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 1200 320 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/ac.V(S12)" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 1200 590 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"xyce/ac.V(S22)" #0000ff 0 3 0 0 0>
  </Smith>
</Diagrams>
<Paintings>
  <Text 100 -170 12 #000000 0 "This netlist demonstrates how to generate S parameters for a device under test using only standard SPICE AC analysis in Xyce.\n\nTwo copies of the device under test (in this case, an MRF501 NPN transitor in a subcircuit), wired in two unconnected circuits.  \nTwo S parameters are obtained from each of the circuits, and plotted in Smith charts.\n\nA companion schematic, mrf501-sparam.sch, computes S parameters using Xyce's S-parameter analysis for the same device.\n\nOne can see that both schematics produce the same Smith charts.\n">
</Paintings>
