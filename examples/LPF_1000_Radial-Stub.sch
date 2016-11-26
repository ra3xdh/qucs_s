<Qucs Schematic 0.0.17>
<Properties>
  <View=120,40,1226,580,0.932188,0,0>
  <Grid=10,10,1>
  <DataSet=LPF_1000_Radial-Stub.dat>
  <DataDisplay=LPF_1000_Radial-Stub.dpl>
  <OpenDisplay=1>
  <Script=LPF_1000_Radial-Stub.m>
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
  <GND * 1 190 410 0 0 0 0>
  <GND * 1 680 410 0 0 0 0>
  <MCROSS MS1 5 350 300 -26 34 0 0 "Subst1" 0 "W1" 0 "W1" 0 "W1" 0 "W1" 0 "Hammerstad" 0 "Kirschning" 0 "showNumbers" 0>
  <MLIN MS2 5 270 300 -17 -54 0 0 "Subst1" 0 "W1" 1 "L2" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS3 5 350 360 15 -26 0 1 "Subst1" 0 "W1" 0 "L3" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MRSTUB MS4 5 350 400 -18 -14 1 0 "Subst1" 0 "W1" 0 "L4" 0 "120" 0>
  <MCROSS MS5 5 510 300 -26 34 0 0 "Subst1" 0 "W1" 0 "W1" 0 "W1" 0 "W1" 0 "Hammerstad" 0 "Kirschning" 0 "showNumbers" 0>
  <MLIN MS6 5 510 360 15 -26 0 1 "Subst1" 0 "W1" 0 "L3" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS7 5 510 240 15 -26 0 1 "Subst1" 0 "W1" 0 "L3" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MRSTUB MS8 5 510 200 -18 14 0 0 "Subst1" 0 "W1" 0 "L4" 0 "120" 0>
  <MRSTUB MS9 5 510 400 -18 -14 1 0 "Subst1" 0 "W1" 0 "L4" 0 "120" 0>
  <MRSTUB MS10 5 350 200 -25 -89 0 0 "Subst1" 0 "W1" 1 "L4" 1 "120" 1>
  <MLIN MS11 5 350 250 15 -26 0 1 "Subst1" 0 "W1" 0 "L3" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS12 5 590 300 -27 -58 0 0 "Subst1" 0 "W1" 0 "L2" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS13 5 430 300 -23 18 0 0 "Subst1" 0 "W1" 1 "L1" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <Pac P1 5 190 360 18 -13 0 1 "1" 0 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 5 680 360 17 -10 0 1 "2" 0 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <.SP SP1 1 170 120 0 69 0 0 "log" 0 "100MHz" 1 "3GHz" 1 "200" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn2 1 570 130 -28 15 0 0 "dBS21=max(dB(S[2,1]),1.5e9:3e9)" 1 "dBin=abs(dBS11)" 1 "dBloss=abs(dBS21)" 1 "Input_RL_dB=dB(S[1,1])" 1 "Loss_dB=dB(S[2,1])" 1 "dBS11=max(dB(S[1,1]),100e6:1e9)" 1 "yes" 0>
  <Eqn Eqn3 1 410 380 -24 15 0 0 "W1=2.5e-4" 1 "yes" 0>
  <SUBST Subst1 5 410 260 10 -102 0 0 "4.50" 1 "63 mil" 1 "1.4 mil" 1 "0.002" 0 "0.022e-6" 0 "1.4e-6" 0>
  <Eqn Eqn1 1 570 330 -30 16 0 0 "L4=7.69e-3" 1 "L2=6.23e-3" 1 "L1=1.474e-2" 1 "L3=5.259e-3" 1 "yes" 0>
  <.Opt Opt1 0 850 480 0 42 0 0 "Sim=SP1" 0 "DE=3|50|2|20|0.85|1|3|1e-6|10|100" 0 "Var=L3|yes|5.259246E-003|1e-3|1e-2|LIN_DOUBLE" 0 "Var=L2|yes|6.229818E-003|5e-3|5e-2|LIN_DOUBLE" 0 "Var=L1|yes|1.474364E-002|1e-3|3e-2|LIN_DOUBLE" 0 "Var=L4|yes|7.690430E-003|2e-3|1e-2|LIN_DOUBLE" 0 "Goal=dBloss|MAX|20" 0 "Goal=dBin|MAX|20" 0>
</Components>
<Wires>
  <190 390 190 410 "" 0 0 0 "">
  <680 390 680 410 "" 0 0 0 "">
  <680 300 680 330 "" 0 0 0 "">
  <620 300 680 300 "" 0 0 0 "">
  <540 300 560 300 "" 0 0 0 "">
  <300 300 320 300 "" 0 0 0 "">
  <190 300 190 330 "" 0 0 0 "">
  <190 300 240 300 "" 0 0 0 "">
  <350 210 350 220 "" 0 0 0 "">
  <350 270 350 280 "" 0 0 0 "">
  <460 300 480 300 "" 0 0 0 "">
  <380 300 400 300 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 850 372 280 282 3 #c0c0c0 1 00 0 0 1e+09 3e+09 0 -40 10 0.1 1 -60 20 5.257 315 0 225 "Frequency, Hz" "" "">
	<"Input_RL_dB" #0000ff 0 3 0 0 1>
	<"Loss_dB" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 210 80 12 #ff0000 0 "To optimize, DISABLE equation block 2 and ENABLE Optimization block">
  <Text 170 480 12 #ff0000 0 "1000 MHz microstrip lowpass filter employing radial stubs that reduce physical lengths\n(to increase frequency of the next highers purious passband).  Circuit is symmetrical so\nvariables for only one-half the filter are shown and remaining variables are hidden.">
</Paintings>
