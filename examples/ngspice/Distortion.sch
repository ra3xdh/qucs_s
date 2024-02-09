<Qucs Schematic 24.1.0>
<Properties>
  <View=40,6,1652,922,0.974891,0,0>
  <Grid=10,10,1>
  <DataSet=Distortion.dat>
  <DataDisplay=Distortion.dpl>
  <OpenDisplay=0>
  <Script=Distortion.m>
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
  <_BJT Q2N2222A_1 1 480 210 8 -26 0 0 "npn" 0 "8.11e-14" 0 "1" 0 "1" 0 "0.5" 0 "0.225" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "205" 0 "4" 0 "0" 0 "0" 0 "0.137" 0 "0.343" 0 "1.37" 0 "2.95e-11" 0 "0.75" 0 "0.33" 0 "1.52e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "3.97e-10" 0 "0" 0 "0" 0 "0" 0 "8.5e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <R R5 1 480 130 15 -26 0 1 "4.7k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 480 270 15 -26 0 1 "470 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 480 300 0 0 0 0>
  <C C2 1 620 170 -26 17 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <R R4 1 680 250 15 -26 0 1 "47k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 680 300 0 0 0 0>
  <IProbe Pr1 1 650 80 -26 16 1 2>
  <GND * 1 360 300 0 0 0 0>
  <R R3 1 360 130 15 -26 0 1 "24 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 780 300 0 0 0 0>
  <Vdc V1 1 780 180 18 -26 0 1 "12 V" 1>
  <C C1 1 310 180 -26 17 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <S4Q_V V3 1 100 270 18 -26 0 1 "DISTOF2 0.2" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 100 300 0 0 0 0>
  <NutmegEq NutmegEq1 5 130 530 -27 17 0 0 "DISTO1" 1 "h2.out=disto1.v(out)" 1 "h3.out=disto2.v(out)" 1>
  <NutmegEq NutmegEq2 5 280 550 -27 17 0 0 "DISTO2" 1 "s1.out=disto1.v(out)" 1 "s2.out=disto2.v(out)" 1 "s3.out=disto3.v(out)" 1>
  <.DC DC1 5 280 670 0 44 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.DISTO DISTO3 1 400 350 0 79 0 0 "lin" 1 "10 kHz" 1 "1 MHz" 1 "100" 0 "" 0>
  <NutmegEq NutmegEq3 5 430 530 -27 17 0 0 "DISTO3" 1 "h2.out=disto1.v(out)" 1 "h3.out=disto2.v(out)" 1>
  <.DISTO DISTO1 1 100 350 0 79 0 0 "log" 1 "1 Hz" 1 "1 GHz" 1 "900" 0 "" 0>
  <.DISTO DISTO2 1 250 350 0 79 0 0 "log" 1 "1 Hz" 1 "1 GHz" 1 "900" 0 "0.9" 1>
  <.AC AC1 5 100 670 0 44 0 0 "log" 1 "1 Hz" 1 "1 GHz" 1 "900" 0 "no" 0>
  <R R1 1 360 270 15 -26 0 1 "2kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <S4Q_V V2 1 100 210 18 -26 0 1 "DISTOF1 0.2" 1 "+  AC 0.2" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <360 210 450 210 "" 0 0 0 "">
  <480 80 480 100 "" 0 0 0 "">
  <480 160 480 170 "" 0 0 0 "">
  <480 170 480 180 "" 0 0 0 "">
  <480 170 590 170 "" 0 0 0 "">
  <650 170 680 170 "out" 670 140 12 "">
  <680 170 680 220 "" 0 0 0 "">
  <680 280 680 300 "" 0 0 0 "">
  <480 80 620 80 "" 0 0 0 "">
  <360 80 480 80 "" 0 0 0 "">
  <360 80 360 100 "" 0 0 0 "">
  <360 160 360 180 "" 0 0 0 "">
  <780 210 780 300 "" 0 0 0 "">
  <680 80 780 80 "" 0 0 0 "">
  <780 80 780 150 "" 0 0 0 "">
  <360 180 360 210 "" 0 0 0 "">
  <340 180 360 180 "" 0 0 0 "">
  <100 180 280 180 "" 0 0 0 "">
  <360 210 360 240 "" 0 0 0 "">
  <450 210 450 210 "b" 430 180 0 "">
  <480 240 480 240 "e" 510 210 0 "">
  <480 170 480 170 "c" 510 140 0 "">
</Wires>
<Diagrams>
  <Rect 1030 418 566 366 3 #c0c0c0 1 10 1 1 1 1e+09 1 -0.002 0.002 0.0191222 1 -0.2 0.2 1.83555 315 0 225 1 0 0 "" "" "">
	<"ngspice/disto1.ac.v(h2.out)" #0000ff 2 3 0 0 0>
	<"ngspice/disto1.ac.v(h3.out)" #ff0000 2 3 0 0 0>
	<"ngspice/ac1.ac.v(out)" #550000 2 3 0 1 1>
  </Rect>
  <Rect 1030 881 574 381 3 #c0c0c0 1 10 1 1 1 1e+09 1 -5e-06 5e-06 4e-05 1 -0.2 0.2 1.83555 315 0 225 1 0 0 "" "" "">
	<"ngspice/disto2.ac.v(s1.out)" #0000ff 2 3 0 0 0>
	<"ngspice/disto2.ac.v(s2.out)" #ff0000 2 3 0 0 0>
	<"ngspice/disto2.ac.v(s3.out)" #ff00ff 2 3 0 0 0>
	<"ngspice/ac1.ac.v(out)" #550000 2 3 0 1 1>
  </Rect>
  <Tab 100 855 548 54 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/dc1.b" #0000ff 0 3 1 0 0>
	<"ngspice/dc1.e" #0000ff 0 3 1 0 0>
	<"ngspice/dc1.c" #0000ff 0 3 1 0 0>
	<"ngspice/dc1.vpr1#branch" #0000ff 0 3 0 0 0>
  </Tab>
  <Rect 640 500 240 160 3 #c0c0c0 1 00 1 0 200000 1e+06 1 0.0132354 0.002 0.018 1 0.00284697 0.0005 0.00364608 315 0 225 1 0 0 "" "" "">
	<"ngspice/disto3.ac.v(h2.out)" #0000ff 2 3 0 0 0>
	<"ngspice/disto3.ac.v(h3.out)" #ff0000 2 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 110 80 10 #000000 0 "DISTORTION analysis example.\n\nNOTE: This Example could be run \nonly with Ngspice">
</Paintings>
