<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,800,868,1,0,0>
  <Grid=10,10,1>
  <DataSet=boostconverter.dat>
  <DataDisplay=boostconverter.dpl>
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
  <L L1 1 170 150 -26 10 0 0 "47uH" 1 "0" 1>
  <Vdc V2 1 70 200 18 -26 0 1 "12V" 1>
  <GND * 1 70 250 0 0 0 0>
  <Eqn Eqn1 1 400 300 -35 16 0 0 "Tmax=Bperiod*20" 1 "Tstep=Bperiod/1000" 1 "yes" 0>
  <.TR TR1 1 540 290 0 57 0 0 "lin" 1 "0" 1 "Tmax" 1 "201" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "Tstep" 1 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "Tstep" 1>
  <Eqn Eqn2 1 580 520 -35 16 0 0 "Bfreq=40k" 1 "Bperiod=1/Bfreq" 1 "Bduty=50" 1 "Ton=Bperiod*Bduty/100" 1 "Toff=Bperiod-Ton" 1 "yes" 0>
  <Relais S1 1 350 200 49 -26 0 0 "0.5 V" 0 "0.1 V" 0 "1" 0 "1e12" 0 "26.85" 0>
  <Vrect V1 1 220 300 18 -26 0 1 "1V" 1 "Ton" 1 "Toff" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0>
  <GND * 1 220 370 0 0 0 0>
  <Diode D1 1 490 150 -26 -43 0 2 "1e-12 A" 0 "1" 0 "10 fF" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 660 250 0 0 0 0>
  <C C1 1 560 200 17 -26 1 3 "100u" 1 "0" 1 "neutral" 0>
  <R R1 1 660 200 15 -26 0 1 "5" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <70 230 70 250 "" 0 0 0 "">
  <70 150 70 170 "" 0 0 0 "">
  <70 150 140 150 "" 0 0 0 "">
  <380 150 380 170 "" 0 0 0 "">
  <320 230 320 370 "" 0 0 0 "">
  <220 370 320 370 "" 0 0 0 "">
  <220 330 220 370 "" 0 0 0 "">
  <200 150 380 150 "" 0 0 0 "">
  <220 170 320 170 "" 0 0 0 "">
  <220 170 220 270 "ctrl" 170 260 76 "">
  <380 150 460 150 "dio" 410 120 15 "">
  <380 230 380 250 "" 0 0 0 "">
  <660 150 660 170 "" 0 0 0 "">
  <660 230 660 250 "" 0 0 0 "">
  <520 150 560 150 "" 0 0 0 "">
  <560 150 660 150 "out" 640 120 62 "">
  <560 150 560 170 "" 0 0 0 "">
  <380 250 560 250 "" 0 0 0 "">
  <560 250 660 250 "" 0 0 0 "">
  <560 230 560 250 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 90 612 359 192 3 #c0c0c0 1 00 1 0 0.0001 0.0005 1 -0.636605 2 7.00266 1 -1 0.5 1 315 0 225 "time" "" "">
	<"out.Vt" #0000ff 2 3 0 0 0>
	<"ctrl.Vt" #ff0000 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 40 16 #000000 0 "Boost converter circuit.">
  <Text 50 80 12 #000000 0 "Change duty cycle ("Bduty" in the equations) to manipulate output voltage.">
</Paintings>
