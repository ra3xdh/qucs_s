<Qucs Schematic 24.1.0>
<Properties>
  <View=-443,-72,1514,1091,1,0,0>
  <Grid=10,10,1>
  <DataSet=TRAN_FFT_OpAmp.dat>
  <DataDisplay=TRAN_FFT_OpAmp.dpl>
  <OpenDisplay=0>
  <Script=TRAN_FFT_OpAmp.m>
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
  <NutmegEq NutmegEq1 1 240 530 -30 18 0 0 "fft" 1 "S_dB=db(v(out))" 1>
  <.TR TR1 1 40 410 0 73 0 0 "lin" 1 "0" 1 "5ms" 1 "5000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.DC DC1 1 40 330 0 44 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.FFT FFT1 1 210 330 -3 73 0 0 "50 kHz" 1 "10 hz" 1 "hanning" 1 "2" 0 "0" 1>
  <Lib OP1 1 220 80 50 34 0 0 "SpiceOpamp" 0 "ad822" 0>
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
  <Rect 510 290 541 322 3 #c0c0c0 1 00 1 0 0.0005 0.005 1 -1.2 0.5 1.2 1 -1.19967 0.5 1.20007 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(in)" #0000ff 1 3 0 0 0>
	<"ngspice/tran.v(out)" #ff0000 1 3 0 0 1>
  </Rect>
  <Rect 510 680 541 322 3 #c0c0c0 1 00 0 0 1000 10000 1 -275.258 50 25.0184 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.s_db" #ff0000 1 3 0 0 0>
  </Rect>
  <Rect 510 1050 541 322 3 #c0c0c0 1 00 1 0 2500 20000 1 -0.0606327 0.1 0.66696 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.s_db" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 630 12 #000000 0 "Qucs-S sends 2 transient simulations to ngspice\n\n1st used for V/I versus time\n2nd used for S versus frequency\n\nBW, dF, Tstart\nTstop = 1.0/dF + (Tstart)\nTstep = 1.0/(2*BW)\n.tran Tstep Tstop Tstart\n\n#Points = Tstop/2*Tstep">
</Paintings>
