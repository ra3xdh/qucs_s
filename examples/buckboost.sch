<Qucs Schematic 0.0.15>
<Properties>
  <View=-3,10,1366,676,1,18,0>
  <Grid=10,10,1>
  <DataSet=buckboost.dat>
  <DataDisplay=buckboost.dpl>
  <OpenDisplay=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Vrect V2 1 130 290 18 -26 0 1 "1V" 1 "Ton" 1 "Toff" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0>
  <GND * 1 130 360 0 0 0 0>
  <Vdc V1 1 50 190 18 -26 0 1 "12V" 1>
  <GND * 1 50 240 0 0 0 0>
  <Relais S1 1 200 170 -26 -69 0 1 "0.5 V" 0 "0.1 V" 0 "1" 0 "1e12" 0 "26.85" 0>
  <GND * 1 640 240 0 0 0 0>
  <C C1 1 540 190 17 -26 1 3 "100u" 1 "0" 1 "neutral" 0>
  <.TR TR1 1 490 280 0 61 0 0 "lin" 1 "0" 1 "Tmax" 1 "2001" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "Tstep" 1 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "Tstep" 1>
  <L L1 1 330 190 10 -26 0 1 "47uH" 1 "0" 1>
  <Diode D1 1 410 140 -26 23 0 0 "1e-12 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <R R1 1 640 190 15 -26 0 1 "10" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Eqn Eqn2 1 360 290 -35 16 0 0 "Tmax=Bperiod*20" 1 "Tstep=Bperiod/1000" 1 "yes" 0>
  <Eqn Eqn1 1 520 510 -35 16 0 0 "Bfreq=40k" 1 "Bperiod=1/Bfreq" 1 "Bduty=30" 1 "Ton=Bperiod*Bduty/100" 1 "Toff=Bperiod-Ton" 1 "yes" 0>
</Components>
<Wires>
  <330 140 380 140 "dio" 360 110 15 "">
  <330 140 330 160 "" 0 0 0 "">
  <230 140 330 140 "" 0 0 0 "">
  <230 200 270 200 "" 0 0 0 "">
  <130 200 170 200 "" 0 0 0 "">
  <270 200 270 360 "" 0 0 0 "">
  <130 360 270 360 "" 0 0 0 "">
  <130 200 130 260 "ctrl" 80 250 36 "">
  <130 320 130 360 "" 0 0 0 "">
  <50 140 170 140 "" 0 0 0 "">
  <50 140 50 160 "" 0 0 0 "">
  <50 220 50 240 "" 0 0 0 "">
  <330 220 330 240 "" 0 0 0 "">
  <640 140 640 160 "" 0 0 0 "">
  <640 220 640 240 "" 0 0 0 "">
  <540 140 640 140 "out" 620 110 62 "">
  <540 140 540 160 "" 0 0 0 "">
  <540 240 640 240 "" 0 0 0 "">
  <540 220 540 240 "" 0 0 0 "">
  <440 140 540 140 "" 0 0 0 "">
  <330 240 540 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 90 602 359 192 3 #c0c0c0 1 00 1 0 0.0001 0.0005 1 -0.636605 2 7.00266 1 -1 0.5 1 315 0 225 "time" "" "">
	<"out.Vt" #0000ff 2 3 0 0 0>
	<"ctrl.Vt" #ff0000 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 70 12 #000000 0 "Change duty cycle ("Bduty" in the equations) to manipulate output voltage.">
  <Text 50 30 16 #000000 0 "Inverting buck-boost converter circuit.">
</Paintings>
