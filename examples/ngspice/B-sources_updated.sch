<Qucs Schematic 1.0.3>
<Properties>
  <View=0,-10,1131,775,1.125,0,0>
  <Grid=10,10,1>
  <DataSet=B-sources_updated.dat>
  <DataDisplay=B-sources_updated.dpl>
  <OpenDisplay=0>
  <Script=B-sources_updated.m>
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
  <Vac V1 1 100 190 18 -26 0 1 "1 V" 1 "10 kHz" 0 "0" 0 "0" 0>
  <GND * 1 100 230 0 0 0 0>
  <Vac V2 1 100 350 18 -26 0 1 "1 V" 1 "7 kHz" 0 "0" 0 "0" 0>
  <GND * 1 220 230 0 0 0 0>
  <src_eqndef B1 1 220 190 18 -26 0 1 "V(in1)*V(in2)" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 100 390 0 0 0 0>
  <.TR TR1 1 50 470 0 71 0 0 "lin" 1 "0" 1 "1 ms" 1 "200" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R_SPICE R1 1 360 390 17 -20 0 1 "1" 1 "" 0 "" 0 "" 0 "" 0>
  <S4Q_Ieqndef B2 1 280 390 -123 -19 0 3 "V(in1)*V(in2)" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 280 440 0 0 0 0>
  <GND * 1 360 440 0 0 0 0>
</Components>
<Wires>
  <100 220 100 230 "" 0 0 0 "">
  <100 300 120 300 "" 0 0 0 "">
  <100 300 100 320 "" 0 0 0 "">
  <220 220 220 230 "" 0 0 0 "">
  <220 140 250 140 "" 0 0 0 "">
  <220 140 220 160 "" 0 0 0 "">
  <100 130 100 160 "" 0 0 0 "">
  <100 130 110 130 "" 0 0 0 "">
  <100 380 100 390 "" 0 0 0 "">
  <360 340 360 360 "" 0 0 0 "">
  <280 340 360 340 "" 0 0 0 "">
  <280 340 280 360 "" 0 0 0 "">
  <280 420 280 440 "" 0 0 0 "">
  <360 420 360 440 "" 0 0 0 "">
  <120 300 120 300 "in2" 150 270 0 "">
  <250 140 250 140 "Vmul" 280 110 0 "">
  <110 130 110 130 "in1" 140 100 0 "">
  <360 340 360 340 "Imul" 390 310 0 "">
</Wires>
<Diagrams>
  <Rect 500 290 256 161 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(vmul)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 500 510 256 161 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(imul)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 830 294 254 164 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 0 0 0 "" "" "">
	<"xyce/tran.V(IMUL)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 80 10 12 #000000 0 "This is voltage multipliers that use B-type spice sources.\nNOTE: This Example could be run only with Ngspice/Xyce">
</Paintings>
