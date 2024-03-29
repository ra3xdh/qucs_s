<Qucs Schematic 0.0.24>
<Properties>
  <View=-44,-120,1044,800,1,0,0>
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
  <R R3 1 130 140 -27 -52 0 0 "390" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 230 140 -12 20 0 0 "47n" 1 "" 0 "neutral" 0>
  <C C2 1 180 90 17 -26 0 1 "47n" 1 "" 0 "neutral" 0>
  <R R1 1 300 90 15 -20 0 1 "3.9k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.DC DC1 1 40 330 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.AC AC1 1 40 420 0 40 0 0 "log" 1 "0.1 kHz" 1 "100 kHz" 1 "200" 1 "no" 0>
  <Vac V1 1 60 190 -64 -26 1 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 60 240 0 0 0 0>
  <R R2 1 180 210 -65 -16 0 1 "5.6k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <160 140 180 140 "" 0 0 0 "">
  <180 140 200 140 "" 0 0 0 "">
  <180 120 180 140 "" 0 0 0 "">
  <260 140 300 140 "" 0 0 0 "">
  <300 140 340 140 "" 0 0 0 "">
  <300 120 300 140 "" 0 0 0 "">
  <300 180 340 180 "" 0 0 0 "">
  <300 50 300 60 "" 0 0 0 "">
  <180 50 300 50 "" 0 0 0 "">
  <180 50 180 60 "" 0 0 0 "">
  <60 140 100 140 "" 0 0 0 "">
  <60 140 60 160 "" 0 0 0 "">
  <60 220 60 240 "" 0 0 0 "">
  <180 140 180 180 "" 0 0 0 "">
  <180 240 180 260 "" 0 0 0 "">
  <410 160 420 160 "" 0 0 0 "">
  <300 50 420 50 "" 0 0 0 "">
  <420 50 420 160 "out" 450 120 98 "">
  <60 140 60 140 "in" 30 110 0 "">
</Wires>
<Diagrams>
  <Rect 576 305 387 269 3 #c0c0c0 1 10 1 100 1 100000 1 -1 0.5 1 1 -0.403999 1 5.48661 315 0 225 "" "" "">
	<"ngspice/ac.v(out)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 0 14 #000000 0 "active bandpass filter">
</Paintings>
