<Qucs Schematic 0.0.19>
<Properties>
  <View=33,92,1091,1000,0.859168,0,0>
  <Grid=10,10,1>
  <DataSet=filter_optimization.dat>
  <DataDisplay=filter_optimization.dpl>
  <OpenDisplay=0>
  <Script=filter_optimizationt.m>
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
  <Pac P1 1 100 260 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 100 290 0 0 0 0>
  <GND * 1 210 290 0 0 0 0>
  <GND * 1 350 290 0 0 0 0>
  <GND * 1 490 290 0 0 0 0>
  <L L1 1 420 180 -18 -48 0 0 "L2" 1 "" 0>
  <L L3 1 280 180 -17 -48 0 0 "L1" 1 "" 0>
  <C C1 1 210 260 17 -26 0 1 "C1" 1 "" 0 "neutral" 0>
  <C C2 1 350 260 17 -26 0 1 "C2" 1 "" 0 "neutral" 0>
  <C C3 1 490 260 17 -26 0 1 "C3" 1 "" 0 "neutral" 0>
  <L L2 1 560 180 -17 -48 0 0 "L3" 1 "" 0>
  <GND * 1 630 290 0 0 0 0>
  <C C4 1 630 260 17 -26 0 1 "C4" 1 "" 0 "neutral" 0>
  <Pac P2 1 750 260 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 750 290 0 0 0 0>
  <.SP SP1 1 100 360 0 74 0 0 "log" 1 "1MHz" 1 "200MHz" 1 "233" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn2 1 310 370 -28 15 0 0 "S21_dB=dB(S[2,1])" 1 "S11_dB=dB(S[1,1])" 1 "S22_dB=dB(S[2,2])" 1 "yes" 0>
  <.Opt Opt1 1 470 360 0 44 0 0 "Sim=SP1" 0 "DE=3|1000|2|50|0.85|0.95|3|1e-6|10|100" 0 "Var=L1|yes|3.900000E-07|100e-9|560e-9|E12" 0 "Var=L2|yes|4.700000E-07|100e-9|560e-9|E12" 0 "Var=L3|yes|3.900000E-07|100e-9|560e-9|E12" 0 "Var=C1|yes|6.200000E-11|56e-12|330e-12|E24" 0 "Var=C2|yes|1.600000E-10|56e-12|470e-12|E24" 0 "Var=C3|yes|1.800000E-10|56e-12|470e-12|E24" 0 "Var=C4|yes|9.100000E-11|56e-12|330e-12|E24" 0 "Goal=Ripple|MIN|0" 0 "Goal=Min_S11|LE|-15" 0 "Goal=Min_Rej|GE|25" 0>
  <Eqn Eqn1 1 690 370 -28 15 0 0 "f0=1e6" 1 "f1=31e6" 1 "fs=45e6" 1 "Max_Gain=max(dB(S[2,1]), f0:f1)" 1 "Min_Gain=min(dB(S[2,1]), f0:f1)" 1 "Min_S11=max(dB(S[1,1]), f0:f1)" 1 "Min_Rej=-max(dB(S[2,1]), fs:200e6)" 1 "Ripple=Max_Gain-Min_Gain" 1 "yes" 0>
</Components>
<Wires>
  <100 180 100 230 "" 0 0 0 "">
  <100 180 210 180 "" 0 0 0 "">
  <210 180 210 230 "" 0 0 0 "">
  <350 180 350 230 "" 0 0 0 "">
  <490 180 490 230 "" 0 0 0 "">
  <450 180 490 180 "" 0 0 0 "">
  <350 180 390 180 "" 0 0 0 "">
  <310 180 350 180 "" 0 0 0 "">
  <210 180 250 180 "" 0 0 0 "">
  <590 180 630 180 "" 0 0 0 "">
  <490 180 530 180 "" 0 0 0 "">
  <630 180 630 230 "" 0 0 0 "">
  <630 180 750 180 "" 0 0 0 "">
  <750 180 750 230 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 150 836 359 281 3 #c0c0c0 1 10 1 1e+06 1 3e+08 0 -50 10 0 0 -2.5 0.5 2.5 315 0 225 "frequency, Hz" "" "">
	<"S21_dB" #0000ff 0 3 0 0 0>
	<"S11_dB" #ff0000 0 3 0 0 0>
	<"S21_dB" #00aa00 0 3 0 0 1>
  </Rect>
  <Tab 186 960 304 49 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 "" "" "">
	<"Ripple" #0000ff 0 3 1 0 0>
	<"Min_S11" #0000ff 0 3 1 0 0>
	<"Min_Rej" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 620 640 12 #ff5500 0 "Filter optimization using ASCO example:\n\nThe filter response is optimized for the minimum ripple\nwith constraints on the minimum in-band reflection coefficient\nand on the attenuation at a frequency in the stop band.\n\nThe components values are forced to be in the E12 series for\nthe inductors and in the E24 series for the capacitors.">
</Paintings>
