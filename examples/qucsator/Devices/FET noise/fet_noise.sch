<Qucs Schematic 25.1.2>
<Properties>
  <View=-15,-198,1398,598,1.11402,0,0>
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
  <GND * 1 240 130 0 0 0 0>
  <Sub SUB1 1 220 110 30 -6 0 0 "fet.sch" 1>
  <L L1 1 130 110 -26 10 0 0 "1.1 nH" 1 "" 0>
  <L L2 1 180 180 10 -26 0 1 "4.0 nH" 1 "" 0>
  <GND * 1 180 210 0 0 0 0>
  <GND * 1 40 210 0 0 0 0>
  <Pac P1 1 40 180 18 -25 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 1 400 170 0 0 0 0>
  <Pac P2 1 400 140 18 -25 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <.SP SP1 1 400 250 0 61 0 0 "lin" 1 "1 GHz" 1 "20 GHz" 1 "39" 1 "yes" 1 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <160 110 180 110 "" 0 0 0 "">
  <180 110 200 110 "" 0 0 0 "">
  <180 110 180 150 "" 0 0 0 "">
  <40 110 40 150 "" 0 0 0 "">
  <40 110 100 110 "" 0 0 0 "">
  <400 90 400 110 "" 0 0 0 "">
  <260 90 400 90 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 610 288 395 228 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"F" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 260 50 12 #000000 0 "For a description of the FET noise\nmodel take a look into its subcircuit.">
  <Text 30 260 12 #000000 0 "This amplifier is noise matched at the input\nfor a frequency of 10GHz.\nThus, the noise optimum Sopt of the overall\ncircuit is zero. (Okay, the matching circuit\nis really ugly, but hey, this is a simulation.)">
</Paintings>
