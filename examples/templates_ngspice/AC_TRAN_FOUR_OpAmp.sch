<Qucs Schematic 24.1.0>
<Properties>
  <View=-446,-72,1550,1205,0.5658,0,1>
  <Grid=10,10,1>
  <DataSet=AC_TRAN_FOUR_OpAmp.dat>
  <DataDisplay=AC_TRAN_FOUR_OpAmp.dpl>
  <OpenDisplay=1>
  <Script=AC_TRAN_FOUR_OpAmp.m>
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
  <R R1 1 190 180 -19 -50 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 90 180 -19 -49 0 2 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 170 70 0 0 0 0>
  <GND * 5 40 280 0 0 0 0>
  <S4Q_V V1 1 40 230 27 -16 0 1 "DC 0 AC 1 sin(0 1 1k 0 0)" 1 "" 0 "" 0 "" 0 "" 0>
  <Vdc V3 1 350 140 -16 22 1 0 "15" 1>
  <Vdc V2 1 350 20 -16 -50 1 2 "15" 1>
  <GND * 5 380 20 0 0 0 0>
  <GND * 5 380 140 0 0 0 0>
  <.DC DC1 1 10 320 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR1 1 10 400 0 75 0 0 "lin" 1 "0" 1 "5ms" 1 "500" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.AC AC1 1 10 580 0 51 0 0 "log" 1 "10 Hz" 1 "10 MHz" 1 "500" 1 "no" 0>
  <NutmegEq NutmegEq1 1 210 510 -27 19 0 0 "ac" 1 "Gain_dB=dB(ac.v(out)/ac.v(in))" 1 "Phase_rad=cph(ac.v(out))" 1 "Phase_deg=180/pi*Phase_rad" 1 "Group_Delay=-1*deriv(cph(ac.v(out)))/2/pi" 1>
  <.FOURIER FOUR1 1 180 320 0 75 0 0 "TR1" 1 "10" 1 "1kHz" 1 "V(out)" 1>
  <Lib OP2 1 220 80 50 34 0 0 "SpiceOpamp" 0 "ad822" 0>
</Components>
<Wires>
  <120 180 140 180 "" 0 0 0 "">
  <140 180 160 180 "" 0 0 0 "">
  <140 100 140 180 "" 0 0 0 "">
  <140 100 180 100 "" 0 0 0 "">
  <170 60 170 70 "" 0 0 0 "">
  <170 60 180 60 "" 0 0 0 "">
  <220 180 300 180 "" 0 0 0 "">
  <280 80 300 80 "" 0 0 0 "">
  <300 80 300 180 "" 0 0 0 "">
  <40 180 60 180 "" 0 0 0 "">
  <40 180 40 200 "" 0 0 0 "">
  <40 260 40 280 "" 0 0 0 "">
  <230 20 320 20 "" 0 0 0 "">
  <230 20 230 40 "" 0 0 0 "">
  <230 120 230 140 "" 0 0 0 "">
  <230 140 320 140 "" 0 0 0 "">
  <300 80 300 80 "out" 310 50 0 "">
  <40 180 40 180 "in" 20 150 0 "">
</Wires>
<Diagrams>
  <Rect 560 290 541 322 3 #c0c0c0 1 00 1 0 5e-5 0.0004 1 -1.2 0.5 1.2 1 -1.20016 0.5 1.19977 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(in)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(out)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 560 680 541 322 3 #c0c0c0 1 10 1 0 1e+6 1e+7 1 -30 5 2.67963 1 -14.3531 50 200 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.gain_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase_deg" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 560 1080 541 322 3 #c0c0c0 1 10 1 10 1 1e+7 1 -30 5 2.67963 1 -14.3531 50 200 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.phase_deg" #0000ff 0 3 0 0 0>
	<"ngspice/ac.group_delay" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
