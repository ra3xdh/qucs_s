<Qucs Schematic 1.0.0>
<Properties>
  <View=-60,-60,1199,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=gain_phase_AC.dat>
  <DataDisplay=gain_phase_AC.dpl>
  <OpenDisplay=0>
  <Script=gain_phase_AC.m>
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
  <R R2 1 260 310 -28 -50 0 2 "50k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R1 1 360 270 13 -26 0 1 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 210 400 0 0 0 0>
  <Sub OPV1 1 200 220 25 -51 0 0 "singleOPV.sch" 1>
  <.DC DC1 1 0 470 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.AC AC1 1 0 550 0 40 0 0 "log" 1 "1" 1 "10 MHz" 1 "200" 1 "no" 0>
  <GND * 1 360 320 0 0 0 0>
  <Eqn Eqn1 1 230 470 -31 17 0 0 "Gain_dB=dB(out.v/in.v)" 1 "Phase=(cph(out.v)-cph(in.v))*180/pi" 1 "yes" 0>
  <GND * 1 160 160 0 0 0 0>
  <GND * 1 40 280 0 0 0 0>
  <Vac V1 1 40 240 -64 -26 1 1 "1 V" 1 "2 kHz" 0 "0" 0 "0" 0>
  <Vdc V2 1 160 120 -64 -26 1 1 "5 V" 1>
  <Vdc V3 1 210 360 18 -26 0 1 "-5 V" 1>
  <GND * 1 70 320 0 0 0 0>
  <R R3 1 110 310 -20 17 0 2 "1.7k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <260 220 300 220 "" 0 0 0 "">
  <360 220 360 240 "out" 380 200 7 "">
  <210 80 210 170 "" 0 0 0 "">
  <360 300 360 320 "" 0 0 0 "">
  <290 310 300 310 "" 0 0 0 "">
  <300 220 360 220 "" 0 0 0 "">
  <300 220 300 310 "" 0 0 0 "">
  <160 80 210 80 "" 0 0 0 "">
  <160 80 160 90 "" 0 0 0 "">
  <160 150 160 160 "" 0 0 0 "">
  <210 390 210 400 "" 0 0 0 "">
  <210 270 210 330 "" 0 0 0 "">
  <40 270 40 280 "" 0 0 0 "">
  <40 200 40 210 "" 0 0 0 "">
  <40 200 160 200 "in" 40 170 26 "">
  <70 310 70 320 "" 0 0 0 "">
  <70 310 80 310 "" 0 0 0 "">
  <140 310 150 310 "" 0 0 0 "">
  <150 240 160 240 "" 0 0 0 "">
  <150 310 230 310 "" 0 0 0 "">
  <150 240 150 310 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 560 418 433 318 3 #c0c0c0 1 10 1 0 1e+06 1e+07 1 -11.5803 10 33.41 1 -120 20 10.6114 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.gain_db" #0000ff 0 3 0 0 0>
	<"ngspice/ac.phase" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
