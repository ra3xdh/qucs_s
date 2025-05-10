<Qucs Schematic 25.1.2>
<Properties>
  <View=-299,-221,1260,658,1.22089,208,104>
  <Grid=10,10,1>
  <DataSet=active_bp.dat>
  <DataDisplay=active_bp.dpl>
  <OpenDisplay=0>
  <Script=active_bp.m>
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
  <OpAmp OP1 1 370 160 -26 42 0 0 "1e6" 1 "15 V" 0>
  <GND * 1 300 180 0 0 0 0>
  <GND * 1 180 260 0 0 0 0>
  <GND * 1 60 260 0 0 0 0>
  <Vac V1 1 60 230 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <R R3 1 130 140 -26 15 0 0 "390" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 180 230 15 -26 0 1 "5.6k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 230 140 -26 17 0 0 "47n" 1 "" 0 "neutral" 0>
  <C C2 1 180 90 17 -26 0 1 "47n" 1 "" 0 "neutral" 0>
  <R R1 1 300 90 15 -26 0 1 "3.9k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.DC DC1 1 240 340 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.AC AC1 1 50 340 0 36 0 0 "log" 1 "0.1 kHz" 1 "100 kHz" 1 "151" 1 "no" 0>
  <Eqn Eqn1 1 450 350 -23 12 0 0 "Gain=dB(Output.v/Input.v)" 1 "Phase=phase(Output.v/Input.v)" 1 "yes" 0>
</Components>
<Wires>
  <160 140 180 140 "" 0 0 0 "">
  <180 140 200 140 "" 0 0 0 "">
  <180 120 180 140 "" 0 0 0 "">
  <180 140 180 200 "" 0 0 0 "">
  <260 140 300 140 "" 0 0 0 "">
  <300 140 340 140 "" 0 0 0 "">
  <300 120 300 140 "" 0 0 0 "">
  <300 180 340 180 "" 0 0 0 "">
  <60 140 100 140 "Input" 90 90 24 "">
  <60 140 60 200 "" 0 0 0 "">
  <300 50 410 50 "" 0 0 0 "">
  <300 50 300 60 "" 0 0 0 "">
  <410 50 410 160 "Output" 450 50 30 "">
  <180 50 300 50 "" 0 0 0 "">
  <180 50 180 60 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 560 255 320 228 3 #c0c0c0 1 10 1 100 1 100000 0 -25 5 20 0 -180 45 180 315 0 225 1 0 0 "" "" "">
	<"Gain" #0000ff 1 3 0 0 0>
	<"Phase" #ff0000 1 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 250 250 14 #000000 0 "active bandpass filter">
</Paintings>
