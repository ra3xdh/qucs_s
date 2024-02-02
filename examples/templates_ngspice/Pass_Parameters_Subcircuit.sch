<Qucs Schematic 24.1.0>
<Properties>
  <View=-87,-70,1320,772,0.802616,0,1>
  <Grid=10,10,1>
  <DataSet=Pass_Parameters_Subcircuit.dat>
  <DataDisplay=Pass_Parameters_Subcircuit.dpl>
  <OpenDisplay=0>
  <Script=Pass_Parameters_Subcircuit.m>
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
  <.DC DC1 1 -10 450 0 44 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <NutmegEq NutmegEq1 1 350 460 -27 19 0 0 "ac" 1 "Gain_dB=dB(ac.v(out)/ac.v(in))" 1 "Phase_rad=cph(ac.v(out))" 1 "Phase_deg=180/pi*Phase_rad" 1 "Group_Delay=-1*deriv(cph(ac.v(out)))/2/pi" 1>
  <.AC AC1 1 -10 530 -2 45 0 0 "log" 1 "100 Hz" 1 "10 MHz" 1 "601" 1 "no" 0>
  <.TR TR1 1 170 450 0 74 0 0 "lin" 1 "0" 1 "5ms" 1 "1000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R1 1 190 260 -19 -50 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 90 260 -19 -49 0 2 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 170 150 0 0 0 0>
  <GND * 5 40 360 0 0 0 0>
  <S4Q_V V1 1 40 310 27 -16 0 1 "DC 0 AC 1 sin(0 1 1k 0 0)" 1 "" 0 "" 0 "" 0 "" 0>
  <Vdc V3 1 350 220 -16 22 1 0 "15" 1>
  <Vdc V2 1 350 100 -16 -50 1 2 "15" 1>
  <GND * 5 380 100 0 0 0 0>
  <GND * 5 380 220 0 0 0 0>
  <Sub SUB1 1 220 160 -150 -118 0 0 "genericopa_sub.sch" 1 "5m" 1 "20k" 1 "10" 1 "10M" 1 "20" 1>
</Components>
<Wires>
  <120 260 140 260 "" 0 0 0 "">
  <220 260 300 260 "" 0 0 0 "">
  <40 260 60 260 "" 0 0 0 "">
  <40 260 40 280 "" 0 0 0 "">
  <40 340 40 360 "" 0 0 0 "">
  <300 160 300 260 "" 0 0 0 "">
  <280 160 300 160 "" 0 0 0 "">
  <230 220 320 220 "" 0 0 0 "">
  <230 200 230 220 "" 0 0 0 "">
  <230 100 320 100 "" 0 0 0 "">
  <230 100 230 120 "" 0 0 0 "">
  <140 260 160 260 "" 0 0 0 "">
  <140 180 140 260 "" 0 0 0 "">
  <140 180 180 180 "" 0 0 0 "">
  <170 140 170 150 "" 0 0 0 "">
  <170 140 180 140 "" 0 0 0 "">
  <40 260 40 260 "in" 20 230 0 "">
  <300 160 300 160 "out" 310 130 0 "">
</Wires>
<Diagrams>
  <Rect 710 730 541 322 3 #c0c0c0 1 10 1 0 1 30000 1 -0.00025753 2e-8 -0.000257377 1 179.94 0.01 180.003 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.gain_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 710 340 541 322 3 #c0c0c0 1 00 1 0 5e-5 0.0004 1 -1.2 0.5 1.2 1 -1.20016 0.5 1.19977 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(in)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(out)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 -30 16 #ff0000 0 "Passing Parameters to an OpAmp subcircuit\nFix path to subcircuit file">
</Paintings>
