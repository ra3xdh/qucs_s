<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-310,1093,770,1,0,0>
  <Grid=10,10,1>
  <DataSet=hb-test.dat>
  <DataDisplay=hb-test.dpl>
  <OpenDisplay=1>
  <Script=hb-test.m>
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
  <GND * 1 80 390 0 0 0 0>
  <Diode D1 1 450 340 13 -26 0 1 "1e-15 A" 1 "1" 1 "0 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 1 "0.0" 0 "2.0" 0 "0.0 Ohm" 1 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.7" 1 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <R R1 1 140 300 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 0 350 350 15 -26 0 1 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 0 190 350 17 -26 0 1 "1 pF" 1 "" 0 "neutral" 0>
  <.HB HB1 1 40 10 0 71 0 0 "1 MHz" 1 "17" 1 "1 pA" 1 "1 uV" 1 "0.001" 1 "150" 0>
  <R R2 1 320 300 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr2 1 240 300 -26 16 0 0>
  <VProbe Pr1 1 490 250 28 -31 0 0>
  <GND * 1 500 270 0 0 0 0>
  <Vac V1 1 80 350 -70 -26 1 1 "4V" 1 "1 MHz" 1 "0" 0 "0" 0>
</Components>
<Wires>
  <80 380 80 390 "" 0 0 0 "">
  <80 300 110 300 "Node1" 110 250 7 "">
  <80 300 80 320 "" 0 0 0 "">
  <170 300 190 300 "" 0 0 0 "">
  <450 300 450 310 "" 0 0 0 "">
  <450 370 450 390 "" 0 0 0 "">
  <80 390 190 390 "" 0 0 0 "">
  <350 380 350 390 "" 0 0 0 "">
  <350 300 350 320 "" 0 0 0 "">
  <350 300 450 300 "Node3" 400 260 25 "">
  <350 390 450 390 "" 0 0 0 "">
  <190 300 190 320 "" 0 0 0 "">
  <190 390 350 390 "" 0 0 0 "">
  <190 380 190 390 "" 0 0 0 "">
  <190 300 210 300 "" 0 0 0 "">
  <270 300 290 300 "" 0 0 0 "">
  <450 270 480 270 "" 0 0 0 "">
  <450 270 450 300 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 343 133 452 173 3 #c0c0c0 1 00 1 -8e+06 2e+06 8e+06 1 -0.237875 1 2.20344 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/NODE1.Vb" #0000ff 0 3 0 6 0>
	<"xyce/NODE3.Vb" #ff0000 0 3 0 6 0>
  </Rect>
  <Rect 99 692 581 232 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/VPR2.Vb" #0000ff 0 3 0 6 0>
  </Rect>
  <Rect 630 410 240 160 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/PR1.Vb" #0000ff 0 3 0 6 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 60 -160 12 #000000 0 "This example shows how to use Harmonic balance \n(HB) simulation with Qucs and Xyce. You can compare\nHB simulation results from Qucs and Xyce. \n\nNOTE: This example could be run only with Xyce and Qucs">
</Paintings>
