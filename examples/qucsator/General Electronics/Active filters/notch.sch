<Qucs Schematic 25.1.2>
<Properties>
  <View=-346,-133,1544,873,1.00697,207,46>
  <Grid=10,10,1>
  <DataSet=notch.dat>
  <DataDisplay=notch.dpl>
  <OpenDisplay=0>
  <Script=notch.m>
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
  <GND * 1 130 370 0 0 0 0>
  <C C1 1 210 170 -26 17 0 0 "1.01u" 1 "" 0 "neutral" 0>
  <R R5 1 310 120 -26 15 0 0 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 390 200 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 470 280 -26 -47 0 2 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 300 260 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C2 1 500 170 17 -26 0 1 "1u" 1 "" 0 "neutral" 0>
  <.DC DC1 1 0 540 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <OpAmp OP1 1 210 260 -26 42 0 0 "1e6" 0 "15 V" 0>
  <OpAmp OP2 1 380 280 -26 42 0 0 "1e6" 0 "15 V" 0>
  <R R1 1 130 340 -50 -26 1 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 40 250 0 0 0 0>
  <.AC AC1 1 200 540 0 36 0 0 "log" 1 "5" 1 "500" 1 "100" 1 "no" 0>
  <Vac V1 1 40 220 18 -26 0 1 "1V" 0 "1 GHz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <Eqn Eqn1 1 400 550 -23 12 0 0 "Gain=dB(Output.v/Input.v)" 1 "Phase=phase(Output.v/Input.v)" 1 "yes" 0>
</Components>
<Wires>
  <420 280 430 280 "" 0 0 0 "">
  <420 200 430 200 "" 0 0 0 "">
  <430 280 440 280 "" 0 0 0 "">
  <430 200 430 280 "" 0 0 0 "">
  <330 260 340 260 "" 0 0 0 "">
  <250 260 260 260 "" 0 0 0 "">
  <240 170 260 170 "" 0 0 0 "">
  <260 260 270 260 "" 0 0 0 "">
  <260 170 260 260 "" 0 0 0 "">
  <130 240 130 310 "" 0 0 0 "">
  <130 240 180 240 "" 0 0 0 "">
  <130 170 130 240 "" 0 0 0 "">
  <130 170 180 170 "" 0 0 0 "">
  <500 200 500 280 "" 0 0 0 "">
  <340 260 350 260 "" 0 0 0 "">
  <340 200 340 260 "" 0 0 0 "">
  <340 200 360 200 "" 0 0 0 "">
  <340 120 500 120 "Output" 450 90 75 "">
  <500 120 500 140 "" 0 0 0 "">
  <160 280 180 280 "" 0 0 0 "">
  <160 280 160 320 "" 0 0 0 "">
  <160 320 340 320 "" 0 0 0 "">
  <340 300 350 300 "" 0 0 0 "">
  <340 320 500 320 "" 0 0 0 "">
  <340 300 340 320 "" 0 0 0 "">
  <500 280 500 320 "" 0 0 0 "">
  <40 120 40 190 "" 0 0 0 "">
  <40 120 280 120 "Input" 190 90 119 "">
</Wires>
<Diagrams>
  <Rect 610 335 320 228 3 #c0c0c0 1 10 1 1 1 1000 0 -40 5 5 0 -180 45 180 315 0 225 1 0 0 "" "" "">
	<"Gain" #0000ff 1 3 0 0 0>
	<"Phase" #ff0000 1 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 0 420 12 #000000 0 "50Hz notch filter:\nbuilt by use of extremely high Q gyrator,\nOpAmps fake a 10.1Henry+0.5mOhm coil\nthat forms resonance circuit with C2">
  <Text 320 430 12 #000000 0 "OpAmp gyrator fakes:\ninductance = C1*R1*R2*R4/R3\nresistance = 0.5*R2*R4/R3/OpAmpGain">
</Paintings>
