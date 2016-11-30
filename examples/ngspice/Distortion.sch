<Qucs Schematic 0.0.19>
<Properties>
  <View=50,10,1067,944,1,88,364>
  <Grid=10,10,1>
  <DataSet=Distortion.dat>
  <DataDisplay=Distortion.dpl>
  <OpenDisplay=1>
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
  <.DC DC1 1 720 380 0 61 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR1 1 390 380 0 79 0 0 "lin" 1 "0" 1 "1 ms" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.DISTO DISTO1 1 550 380 0 79 0 0 "lin" 1 "1 Hz" 1 "10 kHz" 1 "1000" 1 "" 0>
  <_BJT Q2N2222A_1 1 480 210 8 -26 0 0 "npn" 0 "8.11e-14" 0 "1" 0 "1" 0 "0.5" 0 "0.225" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "205" 0 "4" 0 "0" 0 "0" 0 "0.137" 0 "0.343" 0 "1.37" 0 "2.95e-11" 0 "0.75" 0 "0.33" 0 "1.52e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "3.97e-10" 0 "0" 0 "0" 0 "0" 0 "8.5e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <R R1 1 360 240 15 -26 0 1 "2 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 360 150 15 -26 0 1 "24 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 680 330 0 0 0 0>
  <Vdc V1 1 900 140 18 -26 0 1 "12 V" 1>
  <GND * 1 900 170 0 0 0 0>
  <R R5 1 480 130 15 -26 0 1 "4.7k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 680 300 15 -26 0 1 "Rload" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 760 80 -26 16 1 2>
  <Eqn Eqn1 1 170 440 -31 17 0 0 "Rload=47k" 1 "K=out.v/in.v" 1 "yes" 0>
  <GND * 1 360 290 0 0 0 0>
  <R R2 1 480 270 15 -26 0 1 "470 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 480 300 0 0 0 0>
  <GND * 1 150 370 0 0 0 0>
  <C C1 1 270 210 -26 17 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <C C2 1 620 170 -26 17 0 0 "0.1 uF" 1 "" 0 "neutral" 0>
  <Vac_SPICE V2 1 150 320 18 -26 0 1 "Vac= DISTOF1 0.2 DC 0" 1 "Vac_Line 2=+  SIN(0 0.2 4k 0 0) AC 0.2" 1 "Vac_Line 3=" 0 "Vac_Line 4=" 0 "Vac_Line 5=" 0>
  <NutmegEq NutmegEq1 1 270 440 -27 17 0 0 "disto" 1 "H2=disto1.v(out)" 1 "H3=disto2.v(out)" 1>
</Components>
<Wires>
  <360 80 360 120 "" 0 0 0 "">
  <360 80 480 80 "" 0 0 0 "">
  <360 180 360 210 "" 0 0 0 "">
  <360 210 450 210 "" 0 0 0 "">
  <480 80 480 100 "" 0 0 0 "">
  <480 160 480 170 "" 0 0 0 "">
  <480 170 480 180 "" 0 0 0 "">
  <480 80 730 80 "" 0 0 0 "">
  <790 80 900 80 "" 0 0 0 "">
  <900 80 900 110 "" 0 0 0 "">
  <360 270 360 290 "" 0 0 0 "">
  <300 210 360 210 "" 0 0 0 "">
  <680 170 680 270 "" 0 0 0 "">
  <650 170 680 170 "out" 690 130 12 "">
  <480 170 590 170 "" 0 0 0 "">
  <150 350 150 370 "" 0 0 0 "">
  <150 210 240 210 "in" 210 170 6 "">
  <150 210 150 290 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 697 825 323 243 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.v(in)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(out)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 230 822 372 242 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/ac.h2" #0000ff 0 3 0 0 0>
	<"ngspice/ac.h3" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 80 30 12 #000000 0 "DISTORTION analysis example.\n 2nd and 3rd harmonic \ncomponents are plotted.\n\nNOTE: This Example could be run \nonly with Ngspice">
</Paintings>
