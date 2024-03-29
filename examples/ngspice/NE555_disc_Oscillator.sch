<Qucs Schematic 1.0.1>
<Properties>
  <View=10,-80,1380,978,1,0,0>
  <Grid=10,10,1>
  <DataSet=NE555_disc_Oscillator.dat>
  <DataDisplay=NE555_disc_Oscillator.dpl>
  <OpenDisplay=0>
  <Script=NE555_disc_Oscillator.m>
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
  <GND * 1 160 380 0 0 0 0>
  <GND * 1 280 400 0 0 0 0>
  <GND * 1 420 180 0 0 0 0>
  <GND * 1 380 400 0 0 0 0>
  <R R3 1 500 310 15 -26 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 500 360 0 0 0 0>
  <R R1 1 160 160 -67 -26 1 1 "4.8k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 160 240 -67 -26 1 1 "4.8k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C1 1 160 340 -83 -26 1 1 "0.1 uF" 1 "0V" 1 "neutral" 0>
  <C C2 1 380 360 17 -26 0 1 "0.01 uF" 1 "" 0 "neutral" 0>
  <.DC DC1 1 100 470 -5 56 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 300 40 0 0 0 0>
  <S4Q_V V2 1 300 -10 18 -26 0 1 "pwl(0 4.9 1.7e-3 4.9 1.71e-3 0.1 1.8e-3 0.1 1.81e-3 4.9) " 1 "" 0 "" 0 "" 0 "" 0>
  <.TR TR1 1 270 470 0 75 0 0 "lin" 1 "0" 1 "5 ms" 1 "5001" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "no" 1 "0" 0>
  <Vdc V1 1 420 130 18 -26 0 1 "15 V" 1>
  <Lib SUB1 1 280 260 70 -96 0 0 "555_timer" 1 "NE555_bipolar" 1>
</Components>
<Wires>
  <280 380 280 400 "" 0 0 0 "">
  <420 160 420 180 "" 0 0 0 "">
  <420 80 420 100 "" 0 0 0 "">
  <300 80 420 80 "" 0 0 0 "">
  <300 80 300 140 "" 0 0 0 "">
  <160 80 300 80 "" 0 0 0 "">
  <160 80 160 130 "" 0 0 0 "">
  <160 190 160 200 "" 0 0 0 "">
  <160 200 160 210 "" 0 0 0 "">
  <160 200 200 200 "" 0 0 0 "">
  <160 270 160 280 "" 0 0 0 "">
  <160 280 180 280 "" 0 0 0 "">
  <180 320 200 320 "" 0 0 0 "">
  <180 280 200 280 "" 0 0 0 "">
  <180 280 180 320 "" 0 0 0 "">
  <160 280 160 310 "Vcap" 103 270 15 "">
  <160 370 160 380 "" 0 0 0 "">
  <500 340 500 360 "" 0 0 0 "">
  <360 260 500 260 "" 0 0 0 "">
  <500 260 500 280 "" 0 0 0 "">
  <360 320 380 320 "" 0 0 0 "">
  <380 320 380 330 "" 0 0 0 "">
  <380 390 380 400 "" 0 0 0 "">
  <300 20 300 40 "" 0 0 0 "">
  <300 -60 300 -40 "" 0 0 0 "">
  <260 -60 300 -60 "" 0 0 0 "">
  <260 -60 260 140 "Vpwl" 210 -10 82 "">
  <500 260 500 260 "Out" 510 230 0 "">
</Wires>
<Diagrams>
  <Rect 781 874 540 384 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.2 1 1 -1 0.2 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(out)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vcap)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 781 424 540 384 3 #c0c0c0 1 00 1 0 0.0005 0.005 1 -0.502012 1 5.50084 1 -0.333068 0.5 3.66376 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(out)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vpwl)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
