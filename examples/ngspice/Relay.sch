<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-60,1466,877,1,0,0>
  <Grid=10,10,1>
  <DataSet=Relay.dat>
  <DataDisplay=Relay.dpl>
  <OpenDisplay=0>
  <Script=Relay.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Название>
  <FrameText1=Чертил:>
  <FrameText2=Дата:>
  <FrameText3=Версия:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <L L1 1 190 70 10 -26 0 1 "0.5" 1 "" 0>
  <R R1 1 190 130 15 -26 0 1 "120" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 310 90 17 -26 0 1 "5000pF" 1 "" 0 "neutral" 0>
  <GND * 1 310 320 0 0 0 0>
  <VProbe Pr1 1 400 270 -16 28 0 3>
  <GND * 1 370 320 0 0 0 0>
  <Vdc V1 1 100 70 -72 -21 0 1 "12V" 1>
  <GND * 1 100 120 0 0 0 0>
  <GND * 1 140 300 0 0 0 0>
  <Vpulse V2 1 140 250 -86 -39 0 1 "0 V" 1 "1 V" 1 "0" 1 "1 ms" 1 "1 ns" 0 "1 ns" 0>
  <GND * 1 250 300 0 0 0 0>
  <.TR TR1 1 110 360 0 71 0 0 "lin" 1 "0" 1 "20 ms" 1 "2001" 1 "AdamsMoulton" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Relais S1 1 280 260 49 -26 0 0 "0.5 V" 0 "0.1 V" 0 "1e-20" 0 "1e12" 0 "26.85" 0>
</Components>
<Wires>
  <190 20 190 40 "" 0 0 0 "">
  <190 160 310 160 "" 0 0 0 "">
  <190 20 310 20 "" 0 0 0 "">
  <310 20 310 60 "" 0 0 0 "">
  <310 120 310 160 "" 0 0 0 "">
  <310 160 310 200 "drain" 340 140 21 "">
  <310 200 370 200 "" 0 0 0 "">
  <370 200 370 260 "" 0 0 0 "">
  <370 260 380 260 "" 0 0 0 "">
  <370 280 370 320 "" 0 0 0 "">
  <370 280 380 280 "" 0 0 0 "">
  <100 20 190 20 "" 0 0 0 "">
  <100 20 100 40 "" 0 0 0 "">
  <100 100 100 120 "" 0 0 0 "">
  <140 280 140 300 "" 0 0 0 "">
  <140 200 140 220 "" 0 0 0 "">
  <250 290 250 300 "" 0 0 0 "">
  <310 290 310 320 "" 0 0 0 "">
  <310 200 310 230 "" 0 0 0 "">
  <140 200 250 200 "" 0 0 0 "">
  <250 200 250 230 "" 0 0 0 "">
  <140 200 140 200 "sw" 110 170 0 "">
</Wires>
<Diagrams>
  <Rect 510 253 417 223 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.v(drain)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(pr1)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 510 531 429 202 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/tran.V(PR1)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 510 801 429 202 3 #c0c0c0 1 00 1 0 0.002 0.02 1 -237.493 200 265.524 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/tran.V(DRAIN)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 80 560 12 #000000 0 "This example illustrates how to use ideal switch\nwith LCR circuit. \n\nNOTE: This Example could be run with Qucs and\n Ngspice/Xyce">
  <Text 70 -30 16 #ff0000 0 "XYCE Relay Ron CANNOT be 0 Ohms">
</Paintings>
