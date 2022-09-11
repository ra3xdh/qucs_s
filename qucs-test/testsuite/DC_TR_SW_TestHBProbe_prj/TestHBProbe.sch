<Qucs Schematic 0.0.18>
<Properties>
  <View=0,-27,1627,820,1,0,0>
  <Grid=10,10,1>
  <DataSet=TestHBProbe.dat>
  <DataDisplay=TestHBProbe.dpl>
  <OpenDisplay=1>
  <Script=TestHBProbe.m>
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
  <GND * 1 680 280 0 0 0 0>
  <Diode D1 1 680 250 15 -26 0 1 "7e-9 A" 1 "1" 1 "0" 0 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.83" 1 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <R R1 1 590 220 -26 15 0 0 "RProbe" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 480 290 0 0 0 0>
  <Vac V2 1 480 260 18 -26 0 1 "VAC" 1 "10kHz" 1 "0" 0 "0" 0>
  <.DC DC1 1 290 210 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR1 1 310 320 0 77 0 0 "lin" 1 "0" 1 "1 ms" 1 "1001" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.HB HB1 1 460 330 0 77 0 0 "10kHz" 1 "16" 1 "100 pA" 1 "1 uV" 1 "0.01" 1 "150" 0>
  <.SW SW1 1 680 330 0 77 0 0 "HB1" 1 "lin" 1 "VAC" 1 "0" 1 "1.0" 1 "11" 1>
  <Eqn Eqn1 1 360 70 -31 19 0 0 "RProbe=1e-7" 1 "Id_HB=(P1.Vb-P2.Vb)/RProbe" 1 "Id_Tran=(P1.Vt-P2.Vt)/RProbe" 1 "yes" 0>
</Components>
<Wires>
  <620 220 680 220 "" 0 0 0 "">
  <480 220 560 220 "P1" 470 190 0 "">
  <480 220 480 230 "" 0 0 0 "">
  <680 220 680 220 "P2" 660 180 0 "">
</Wires>
<Diagrams>
  <Rect 850 240 240 160 3 #c0c0c0 1 00 1 0 10000 40000 1 -0.00143417 0.01 0.0157762 1 -1 1 1 315 0 225 "" "" "">
	<"Id_HB" #0000ff 2 3 0 6 0>
	  <Mkr 10000/1 58 -177 8 1 0>
  </Rect>
  <Rect 860 480 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"Id_Tran" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 1250 240 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"P1.Vb" #0000ff 2 3 0 6 0>
  </Rect>
  <Rect 1250 480 240 160 3 #c0c0c0 1 00 1 0 50000 160000 1 -0.1 0.5 1.1 1 -1 1 1 315 0 225 "" "" "">
	<"P1.Vb" #0000ff 2 3 0 6 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
