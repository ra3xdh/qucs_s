<Qucs Schematic 0.0.24>
<Properties>
  <View=-426,-90,1063,816,0.925532,0,0>
  <Grid=10,10,1>
  <DataSet=SmithChartTest.dat>
  <DataDisplay=SmithChartTest.sch>
  <OpenDisplay=1>
  <Script=SmithChartTest.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By: SunnySan>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <L L1 1 10 230 8 -26 0 1 "196.6nH" 1 "" 0>
  <C C1 1 -20 230 -8 46 0 1 "644.1pF" 1 "" 0 "neutral" 0>
  <GND * 1 10 260 0 0 0 0>
  <L L2 1 120 150 -34 -54 0 0 "791.1nH" 1 "" 0>
  <C C2 1 60 150 -26 10 0 0 "160.1pF" 1 "" 0 "neutral" 0>
  <L L3 1 150 230 8 -26 0 1 "196.6nH" 1 "" 0>
  <C C3 1 120 230 -8 46 0 1 "644.1pF" 1 "" 0 "neutral" 0>
  <GND * 1 150 260 0 0 0 0>
  <GND * 1 260 260 0 0 0 0>
  <NutmegEq NutmegEq1 1 230 370 -27 16 0 0 "sp" 1 "S11_dB=dB(s_1_1)" 1 "S12_dB=dB(s_1_2)" 1 "S21_dB=dB(s_2_1)" 1 "S22_dB=dB(s_2_2)" 1>
  <Pac P2 1 260 210 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P1 1 -80 210 -98 -30 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 -80 260 0 0 0 0>
  <.SP SP2 1 -130 440 0 79 0 0 "log" 1 "1MHz" 1 "40MHz" 1 "200" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <10 150 10 200 "" 0 0 0 "">
  <150 150 150 200 "" 0 0 0 "">
  <10 150 30 150 "" 0 0 0 "">
  <-20 200 10 200 "" 0 0 0 "">
  <-20 260 10 260 "" 0 0 0 "">
  <120 200 150 200 "" 0 0 0 "">
  <120 260 150 260 "" 0 0 0 "">
  <260 240 260 260 "" 0 0 0 "">
  <150 150 260 150 "" 0 0 0 "">
  <260 150 260 180 "" 0 0 0 "">
  <-80 150 10 150 "" 0 0 0 "">
  <-80 150 -80 180 "" 0 0 0 "">
  <-80 240 -80 260 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Smith 400 790 330 330 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 "" "" "">
	<"ngspice/ac.v(s_1_1)" #0000ff 0 3 0 0 0>
  </Smith>
  <Rect 420 403 467 303 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"ngspice/ac.s21_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.s11_db" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text -100 350 12 #000000 0 "Chebyshev band-pass filter\n10MHz...20MHz, PI-type,\nimpedance matching 50 Ohm">
  <Text 20 20 14 #5500ff 0 "Smith Chart example test with a band pass filter">
</Paintings>
