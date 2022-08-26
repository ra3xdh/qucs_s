<Qucs Schematic 0.0.24>
<Properties>
  <View=-78,26,1020,1186,1,0,0>
  <Grid=10,10,1>
  <DataSet=BJT_ac_tran.dat>
  <DataDisplay=BJT_ac_tran.dpl>
  <OpenDisplay=0>
  <Script=BJT_ac_tran.m>
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
  <_BJT Q2N2222A_1 1 400 210 5 -10 0 0 "npn" 0 "8.11e-14" 0 "1" 0 "1" 0 "0.5" 0 "0.225" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "205" 0 "4" 0 "0" 0 "0" 0 "0.137" 0 "0.343" 0 "1.37" 0 "2.95e-11" 0 "0.75" 0 "0.33" 0 "1.52e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "3.97e-10" 0 "0" 0 "0" 0 "0" 0 "8.5e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <GND * 1 400 330 0 0 0 0>
  <GND * 1 280 310 0 0 0 0>
  <GND * 1 150 310 0 0 0 0>
  <R R5 1 400 130 18 -17 0 1 "4.7k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V2 1 150 280 -99 -29 0 1 "200 mV" 1 "4 kHz" 1 "0" 0 "0" 0>
  <R R3 1 280 150 17 -21 0 1 "24k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 400 290 17 -20 0 1 "470" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R1 1 280 260 19 -19 0 1 "2k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 210 210 -35 -56 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <C C2 1 520 170 -35 -57 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <R R4 1 580 230 15 -19 0 1 "Rload" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 580 280 0 0 0 0>
  <IProbe Pr1 1 590 80 -10 17 1 2>
  <Vdc V1 1 680 130 18 -26 0 1 "12 V" 1>
  <GND * 1 680 170 0 0 0 0>
  <Eqn Eqn1 1 90 390 -31 17 0 0 "Rload=47k" 1 "K=out.v/in.v" 1 "Pwr=(out.Vt*out.Vt)/Rload" 1 "yes" 0>
  <.DC DC1 1 590 390 0 49 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR1 1 280 390 0 71 0 0 "lin" 1 "0" 1 "1 ms" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.AC AC1 1 420 390 -3 52 0 0 "log" 1 "10 Hz" 1 "100 MHz" 1 "100" 1 "no" 0>
</Components>
<Wires>
  <280 80 280 120 "" 0 0 0 "">
  <280 80 400 80 "" 0 0 0 "">
  <280 180 280 210 "" 0 0 0 "">
  <280 210 370 210 "" 0 0 0 "">
  <400 80 400 100 "" 0 0 0 "">
  <400 240 400 260 "" 0 0 0 "">
  <400 320 400 330 "" 0 0 0 "">
  <400 160 400 170 "" 0 0 0 "">
  <400 170 400 180 "" 0 0 0 "">
  <280 210 280 230 "" 0 0 0 "">
  <280 290 280 310 "" 0 0 0 "">
  <240 210 280 210 "" 0 0 0 "">
  <150 210 150 250 "" 0 0 0 "">
  <150 210 180 210 "in" 120 180 6 "">
  <400 170 490 170 "" 0 0 0 "">
  <550 170 580 170 "out" 600 140 30 "">
  <580 170 580 200 "" 0 0 0 "">
  <580 260 580 280 "" 0 0 0 "">
  <400 80 560 80 "" 0 0 0 "">
  <620 80 680 80 "" 0 0 0 "">
  <680 80 680 100 "" 0 0 0 "">
  <680 160 680 170 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 506 835 387 269 3 #c0c0c0 1 10 1 100 1 1e+09 1 0 2 9.08763 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.k" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 510 1166 378 279 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.i(pr1)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.pwr" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 0 540 12 #000000 0 "This example shows the following \nfeatures of Ngspice support:\n1. Semiconductor devices usage\n2. AC and TRAN analysis\n3. Current probes usage\n4. Parametrization usage. Rload is parameter.\n5. Postprocessing usage in frequency domain.\n Volatge gain K calculation.\n6. Postrprocessing usage in time domain.\n\nNOTE: You can run this example with Qucsator and\n Ngspice  and compare results.">
</Paintings>
