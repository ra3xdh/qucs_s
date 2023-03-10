<Qucs Schematic 1.0.1>
<Properties>
  <View=-195,-160,1210,843,1,0,0>
  <Grid=10,10,1>
  <DataSet=rc_tran_ac.dat>
  <DataDisplay=rc_tran_ac.dpl>
  <OpenDisplay=0>
  <Script=RC.m>
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
  <Vac V1 1 110 180 -118 -32 0 1 "1 V" 1 "1 kHz" 1 "0" 0 "0" 0>
  <GND * 1 110 270 0 0 0 0>
  <Vdc V2 1 110 240 18 -26 0 1 "0.5" 1>
  <GND * 1 300 230 0 0 0 0>
  <C C1 1 300 200 17 -26 0 1 "1 uF" 1 "" 0 "neutral" 0>
  <R R1 1 220 150 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.AC AC1 1 440 150 0 48 0 0 "log" 1 "1 Hz" 1 "100kHz" 1 "100" 1 "no" 0>
  <.TR TR1 1 -40 -50 0 81 0 0 "lin" 1 "0" 1 "10ms" 1 "1001" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <110 150 190 150 "Vn1" 120 110 35 "">
  <250 150 300 150 "" 0 0 0 "">
  <300 150 300 170 "" 0 0 0 "">
  <300 150 300 150 "Vn3" 330 120 0 "">
</Wires>
<Diagrams>
  <Rect -90 503 403 173 2 #c0c0c0 1 00 1 0 0.002 0.01 1 0.301747 0.2 0.8 1 -1 1 1 315 0 225 0 0 0 "Time (s)" "Vn3 (V)" "">
	<"ngspice/tran.v(vn3)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 430 500 240 160 3 #c0c0c0 1 10 1 1 1 100000 1 -0.0982473 0.5 1.09982 1 -1 1 1 315 0 225 0 0 0 "frequency (Hz)" "Vn3 (V)" "">
	<"ngspice/ac.v(vn3)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
