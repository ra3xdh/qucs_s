<Qucs Schematic 1.0.0>
<Properties>
  <View=-59,-29,1035,747,1.15104,0,60>
  <Grid=10,10,1>
  <DataSet=Sawtooth-discreet.dat>
  <DataDisplay=Sawtooth-discreet.dpl>
  <OpenDisplay=0>
  <Script=Sawtooth-discreet.m>
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
  <R R2 1 320 110 -68 -20 0 1 "6.8k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R1 1 140 190 17 -20 0 1 "220" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 80 90 -59 -20 0 1 "15" 1>
  <GND * 1 80 140 0 0 0 0>
  <Diode D2 1 380 170 13 -10 0 1 "1e-15 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D1 1 380 90 15 -12 0 1 "1e-15 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 180 420 0 0 0 0>
  <Lib D3 1 140 370 -32 -9 0 3 "Z-Diodes" 0 "1N4728A" 0>
  <GND * 1 140 420 0 0 0 0>
  <R R3 1 380 270 15 -19 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 380 320 0 0 0 0>
  <C C1 1 320 350 20 -18 0 1 "680 pF" 1 "" 0 "neutral" 0>
  <GND * 1 320 400 0 0 0 0>
  <_BJT Q2N2222A_1 1 180 370 12 21 1 2 "npn" 0 "8.11e-14" 0 "1" 0 "1" 0 "0.5" 0 "0.225" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "205" 0 "4" 0 "0" 0 "0" 0 "0.137" 0 "0.343" 0 "1.37" 0 "2.95e-11" 0 "0.75" 0 "0.33" 0 "1.52e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "3.97e-10" 0 "0" 0 "0" 0 "0" 0 "8.5e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <_BJT Q2N2907A_2 1 320 210 -102 -7 0 2 "pnp" 0 "3.81e-13" 0 "1" 0 "1" 0 "0.14" 0 "0.21" 0 "139" 0 "20" 0 "1.53e-11" 0 "2" 0 "0" 0 "2" 0 "154" 0 "4" 0 "0" 0 "0" 0 "0.221" 0 "0.552" 0 "2.21" 0 "1.56e-11" 0 "0.75" 0 "0.33" 0 "2.08e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "6.36e-10" 0 "0" 0 "0" 0 "0" 0 "6.37e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <_BJT Q2N2907A_1 1 220 310 3 -8 1 0 "pnp" 0 "3.81e-13" 0 "1" 0 "1" 0 "0.14" 0 "0.21" 0 "139" 0 "20" 0 "1.53e-11" 0 "2" 0 "0" 0 "2" 0 "154" 0 "4" 0 "0" 0 "0" 0 "0.221" 0 "0.552" 0 "2.21" 0 "1.56e-11" 0 "0.75" 0 "0.33" 0 "2.08e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "6.36e-10" 0 "0" 0 "0" 0 "0" 0 "6.37e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <.TR TR1 1 100 490 -1 67 0 0 "lin" 1 "0" 1 "30 us" 1 "4000" 1 "Trapezoidal" 1 "2" 0 "0" 0 "1e-10" 1 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "no" 0 "0" 0>
</Components>
<Wires>
  <320 140 320 180 "" 0 0 0 "">
  <350 210 380 210 "" 0 0 0 "">
  <320 240 320 260 "" 0 0 0 "">
  <140 220 140 310 "" 0 0 0 "">
  <80 120 80 140 "" 0 0 0 "">
  <380 200 380 210 "" 0 0 0 "">
  <380 120 380 140 "" 0 0 0 "">
  <140 40 140 160 "" 0 0 0 "">
  <320 40 320 80 "" 0 0 0 "">
  <140 40 320 40 "" 0 0 0 "">
  <380 40 380 60 "" 0 0 0 "">
  <320 40 380 40 "" 0 0 0 "">
  <80 40 80 60 "" 0 0 0 "">
  <80 40 140 40 "" 0 0 0 "">
  <180 310 180 340 "" 0 0 0 "">
  <140 310 180 310 "" 0 0 0 "">
  <180 400 180 420 "" 0 0 0 "">
  <140 310 140 340 "" 0 0 0 "">
  <140 400 140 420 "" 0 0 0 "">
  <380 210 380 240 "" 0 0 0 "">
  <380 300 380 320 "" 0 0 0 "">
  <320 260 320 320 "" 0 0 0 "">
  <320 380 320 400 "" 0 0 0 "">
  <220 260 320 260 "" 0 0 0 "">
  <220 260 220 280 "" 0 0 0 "">
  <210 370 220 370 "" 0 0 0 "">
  <220 340 220 370 "" 0 0 0 "">
  <180 310 190 310 "" 0 0 0 "">
  <380 210 380 210 "UCON" 400 190 0 "">
  <320 260 320 260 "OUT" 330 230 0 "">
  <140 310 140 310 "UTOP" 90 280 0 "">
</Wires>
<Diagrams>
  <Rect 570 232 256 152 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 2 1 1 -1 2 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(utop)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 570 443 258 153 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(out)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
