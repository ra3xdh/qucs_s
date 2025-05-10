<Qucs Schematic 1.0.0>
<Properties>
  <View=-78,-81,1387,1001,1,28,0>
  <Grid=10,10,1>
  <DataSet=BJT.dat>
  <DataDisplay=BJT.dpl>
  <OpenDisplay=0>
  <Script=BJT.m>
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
  <_BJT Q2N2222A_1 1 400 210 3 -12 0 0 "npn" 0 "8.11e-14" 0 "1" 0 "1" 0 "0.5" 0 "0.225" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "205" 0 "4" 0 "0" 0 "0" 0 "0.137" 0 "0.343" 0 "1.37" 0 "2.95e-11" 0 "0.75" 0 "0.33" 0 "1.52e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "3.97e-10" 0 "0" 0 "0" 0 "0" 0 "8.5e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <GND * 1 400 330 0 0 0 0>
  <GND * 1 280 310 0 0 0 0>
  <R R2 1 400 290 19 -20 0 1 "470 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C2 1 550 170 -26 17 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <R R5 1 400 130 15 -26 0 1 "4.7k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.DC DC1 1 660 380 0 61 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Eqn Eqn1 1 150 390 -31 17 0 0 "Rload=47k" 1 "K=out.v/in.v" 1 "Pwr=(out.Vt*out.Vt)/Rload" 1 "yes" 0>
  <.AC AC1 1 490 380 0 61 0 0 "log" 1 "100 Hz" 1 "10 MHz" 1 "101" 1 "no" 0>
  <Vdc V1 1 700 130 18 -26 0 1 "12 V" 1>
  <GND * 1 700 180 0 0 0 0>
  <R R4 1 620 250 16 -21 0 1 "Rload" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 620 300 0 0 0 0>
  <Vac V2 1 120 250 -101 -33 0 1 "200 mV" 1 "4 kHz" 1 "0" 0 "0" 0>
  <GND * 1 120 300 0 0 0 0>
  <C C1 1 220 210 -43 -58 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <R R1 1 280 260 15 -17 0 1 "2 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 280 150 15 -20 0 1 "24 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 510 60 -10 -61 1 2>
  <.TR TR1 1 350 380 0 71 0 0 "lin" 1 "0" 1 "1 ms" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <280 210 370 210 "" 0 0 0 "">
  <400 240 400 260 "" 0 0 0 "">
  <400 320 400 330 "" 0 0 0 "">
  <400 160 400 170 "" 0 0 0 "">
  <400 170 400 180 "" 0 0 0 "">
  <400 170 520 170 "" 0 0 0 "">
  <700 160 700 180 "" 0 0 0 "">
  <580 170 620 170 "out" 610 130 2 "">
  <620 170 620 220 "" 0 0 0 "">
  <620 280 620 300 "" 0 0 0 "">
  <120 280 120 300 "" 0 0 0 "">
  <250 210 280 210 "" 0 0 0 "">
  <120 210 120 220 "" 0 0 0 "">
  <120 210 190 210 "in" 130 170 36 "">
  <280 210 280 230 "" 0 0 0 "">
  <280 290 280 310 "" 0 0 0 "">
  <280 180 280 210 "" 0 0 0 "">
  <700 60 700 100 "" 0 0 0 "">
  <540 60 700 60 "" 0 0 0 "">
  <400 60 400 100 "" 0 0 0 "">
  <280 60 280 120 "" 0 0 0 "">
  <280 60 400 60 "" 0 0 0 "">
  <400 60 480 60 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 896 365 387 269 3 #c0c0c0 1 10 1 0 1 0 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.k" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 900 706 383 272 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.i(pr1)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.pwr" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 110 580 12 #000000 0 "This example shows the following \nfeatures of Ngspice support:\n1. Semiconductor devices usage\n2. AC and TRAN analysis\n3. Current probes usage\n4. Parametrization usage. Rload is parameter.\n5. Postprocessing usage in frequency domain.\n Voltage gain K calculation.\n6. Postrprocessing usage in time domain.\n\nNOTE: You can run this example with Qucsator and\n Ngspice and compare results.">
</Paintings>
