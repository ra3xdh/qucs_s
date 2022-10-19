<Qucs Schematic 0.0.24>
<Properties>
  <View=-75,-194,920,559,1,0,0>
  <Grid=10,10,1>
  <DataSet=fet_noise.dat>
  <DataDisplay=fet_noise.dpl>
  <OpenDisplay=0>
  <Script=fet_noise.m>
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
  <GND * 5 180 210 0 0 0 0>
  <Sub SUB1 1 220 110 30 -6 0 0 "fet.sch" 1>
  <Pac P1 1 60 150 -95 -25 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 5 60 200 0 0 0 0>
  <Pac P2 1 360 150 18 -25 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 5 360 200 0 0 0 0>
  <L L1 1 130 110 -26 -48 1 0 "1.1 nH" 1 "" 0>
  <L L2 1 180 160 -77 -26 1 1 "4.0 nH" 1 "" 0>
  <GND * 5 240 140 0 0 0 0>
  <.SP SP1 1 40 290 0 65 0 0 "lin" 1 "1 GHz" 1 "20 GHz" 1 "39" 1 "yes" 1 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <160 110 180 110 "" 0 0 0 "">
  <180 110 200 110 "" 0 0 0 "">
  <60 110 100 110 "" 0 0 0 "">
  <60 110 60 120 "" 0 0 0 "">
  <60 180 60 200 "" 0 0 0 "">
  <260 90 360 90 "" 0 0 0 "">
  <360 90 360 120 "" 0 0 0 "">
  <360 180 360 200 "" 0 0 0 "">
  <180 110 180 130 "" 0 0 0 "">
  <180 190 180 210 "" 0 0 0 "">
  <240 130 240 140 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Smith 680 90 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"ngspice/ac.v(s_1_1)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(s_2_2)" #ff0000 0 3 0 0 0>
  </Smith>
  <Smith 690 370 200 200 3 #c0c0c0 1 00 1 0 1 1 1 0 4 6.36785 1 0 1 1 315 0 225 "" "" "">
	<"ngspice/ac.v(s_2_1)" #0000ff 0 3 0 0 0>
	<"ngspice/ac.v(s_1_2)" #ff0000 2 3 0 0 0>
  </Smith>
</Diagrams>
<Paintings>
  <Text 250 300 12 #000000 0 "This amplifier is noise matched at the input\nfor a frequency of 10GHz. Thus, the noise \noptimum Sopt of the overall circuit is zero.\n(Okay, the matching circuit is really ugly, \nbut hey, this is a simulation.)">
  <Text 130 -40 16 #ff0000 0 "AC, SP analysis only. No DC">
  <Text 290 30 12 #000000 0 "For a description of the FET noise\nmodel take a look into its subcircuit.">
</Paintings>
