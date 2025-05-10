<Qucs Schematic 25.1.2>
<Properties>
  <View=-818,-708,3133,1358,0.920372,658,469>
  <Grid=10,10,1>
  <DataSet=wilkinson.dat>
  <DataDisplay=wilkinson.dpl>
  <OpenDisplay=0>
  <Script=wilkinson.m>
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
  <Pac P1 1 60 110 -74 -26 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 1 60 140 0 0 0 0>
  <TLIN Line1 1 130 80 -26 14 0 0 "50 Ohm" 1 "100 mm" 1 "0 dB" 0 "26.85" 0>
  <TLIN Line2 1 230 40 -26 14 0 0 "70.7 Ohm" 1 "75 mm" 1 "0 dB" 0 "26.85" 0>
  <TLIN Line3 1 230 140 -26 14 0 0 "70.7 Ohm" 1 "75 mm" 1 "0 dB" 0 "26.85" 0>
  <Pac P2 1 410 70 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <Pac P3 1 410 170 18 -26 0 1 "3" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 1 410 200 0 0 0 0>
  <GND * 1 410 100 0 0 0 0>
  <R R1 1 300 90 15 -26 0 1 "100 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.SP SP1 1 -80 260 0 61 0 0 "lin" 1 "0.1 MHz" 1 "2 GHz" 1 "100" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 120 270 -23 12 0 0 "Attenuation2=dB(S[2,1])" 1 "Reflect=dB(S[1,1])" 1 "Attenuation3=dB(S[3,1])" 1 "Decoupling=dB(S[3,2])" 1 "yes" 0>
</Components>
<Wires>
  <180 40 200 40 "" 0 0 0 "">
  <180 40 180 80 "" 0 0 0 "">
  <180 140 200 140 "" 0 0 0 "">
  <260 40 300 40 "" 0 0 0 "">
  <260 140 300 140 "" 0 0 0 "">
  <60 80 100 80 "" 0 0 0 "">
  <180 80 180 140 "" 0 0 0 "">
  <160 80 180 80 "" 0 0 0 "">
  <300 140 410 140 "" 0 0 0 "">
  <300 120 300 140 "" 0 0 0 "">
  <300 40 410 40 "" 0 0 0 "">
  <300 40 300 60 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 730 322 501 312 3 #c0c0c0 1 00 1 0 2e+08 2e+09 0 -12 3 0 1 -50 10 3.69359 315 0 225 1 0 0 "" "" "">
	<"Attenuation2" #0000ff 1 3 0 0 0>
	<"Attenuation3" #ff0000 1 3 0 0 0>
	<"Decoupling" #ff00ff 1 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 300 270 12 #000000 0 "ideal Wilkinson divider:\n- power into port 1 is split equally to port 2 and 3\n- port 2 and 3 are decoupled\n- if port 2 and 3 are terminated equally, no power\n  is consumed by R1\n- a wider bandwidth can be achieved by using\n  more than one Line2/Line3/R1 section">
</Paintings>
