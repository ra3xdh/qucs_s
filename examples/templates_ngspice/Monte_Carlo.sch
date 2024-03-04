<Qucs Schematic 24.1.0>
<Properties>
  <View=44,-66,1209,633,1.41367,75,0>
  <Grid=10,10,1>
  <DataSet=Monte_Carlo.dat>
  <DataDisplay=Monte_Carlo.dpl>
  <OpenDisplay=0>
  <Script=Monte_Carlo.m>
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
  <Pac P1 1 150 140 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 1 150 170 0 0 0 0>
  <L L1 1 300 140 8 -26 0 1 "mc(16.08nH,10)" 1 "" 0>
  <GND * 1 300 170 0 0 0 0>
  <L L2 1 420 60 -26 -44 0 0 "mc(795.8nH,10)" 1 "" 0>
  <L L3 1 480 140 8 -26 0 1 "mc(16.08nH,10)" 1 "" 0>
  <GND * 1 480 170 0 0 0 0>
  <C C2 1 360 60 -26 10 0 0 "mc(3.215pF,5)" 1 "" 0 "neutral" 0>
  <C C1 1 270 140 -8 46 0 1 "mc(159.2pF,5)" 1 "" 0 "neutral" 0>
  <C C3 1 450 140 -8 46 0 1 "mc(159.2pF,5)" 1 "" 0 "neutral" 0>
  <SpiceFunc SpiceFunc1 5 490 -40 -22 18 0 0 "mc(v,t)={(i=0)?v:unif(v,t/100)}" 1>
  <.SP SP1 1 840 -40 0 74 0 0 "log" 0 "10 MHz" 1 "1 GHz" 1 "200" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.SP SP2 1 1010 -40 0 74 0 0 "log" 0 "10 MHz" 1 "1 GHz" 1 "200" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <SpicePar SpicePar1 5 750 -40 -28 18 0 0 "i=0" 1>
  <Pac P2 1 630 140 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 1 630 170 0 0 0 0>
  <.SW SW1 5 840 100 0 74 0 0 "SP1" 1 "lin" 0 "i" 1 "1" 0 "100" 0 "100" 1 "false" 0>
  <.SW SW2 5 1010 100 0 74 0 0 "SP2" 1 "const" 0 "i" 1 "1" 0 "100" 0 "[0]" 1 "false" 0>
</Components>
<Wires>
  <150 60 150 110 "" 0 0 0 "">
  <150 60 300 60 "" 0 0 0 "">
  <300 60 300 110 "" 0 0 0 "">
  <270 110 300 110 "" 0 0 0 "">
  <270 170 300 170 "" 0 0 0 "">
  <450 60 480 60 "" 0 0 0 "">
  <300 60 330 60 "" 0 0 0 "">
  <480 60 480 110 "" 0 0 0 "">
  <450 110 480 110 "" 0 0 0 "">
  <450 170 480 170 "" 0 0 0 "">
  <480 60 630 60 "" 0 0 0 "">
  <630 60 630 110 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 226 512 934 283 3 #c0c0c0 1 11 1 1e+07 1 1e+09 1 2.99985e-06 1 1 1 -1 0.5 1 315 0 225 1 1 0 "" "" "">
	<"ngspice/sp1.ac.v(s_2_1)" #00aaff 0 3 0 0 0>
	<"ngspice/sp2.ac.v(s_2_1)" #ff0000 3 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 150 -60 10 #000000 0 "MONTE-CARLO SIMULATION\n\nButterworth band-pass filter\n90-110MHz pi-type 50 Ohm\n3-th order">
</Paintings>
