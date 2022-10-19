<Qucs Schematic 0.0.23>
<Properties>
  <View=0,-180,2139,639,1,0,0>
  <Grid=10,10,1>
  <DataSet=quarz_test.dat>
  <DataDisplay=quarz_test.dpl>
  <OpenDisplay=0>
  <Script=quarz_test.m>
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
  <GND * 1 450 190 0 0 0 0>
  <GND * 1 120 280 0 0 0 0>
  <Vac V1 1 120 250 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <R R2 1 120 190 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R1 1 360 160 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Sub SUB1 1 250 160 -26 55 0 0 "quarz.sch" 1 "8863k" 1 "0.01406" 1 "6.5p" 1>
  <.XYCESCR XYCESCR1 1 120 530 0 51 0 0 ".ac lin 400 8800k 9000k\n.PRINT AC format=raw file=ac.txt V(OUT) {db(v(out)/v(in))}" 1 "" 0 "ac.txt" 0>
</Components>
<Wires>
  <390 160 450 160 "" 0 0 0 "">
  <450 160 450 190 "" 0 0 0 "">
  <290 160 330 160 "out" 330 100 21 "">
  <120 160 210 160 "in" 190 100 49 "">
</Wires>
<Diagrams>
  <Rect 508 475 264 215 3 #c0c0c0 1 01 1 8.8e+06 50000 9e+06 1 1e-06 1 0.03 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/ac.V(OUT)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 884 461 301 191 3 #c0c0c0 1 00 1 -1 0.5 1 0 -120 25 -20 1 -1 0.5 1 315 0 225 "" "K" "">
	<"xyce/ac.{DB(V(OUT)/V(IN))}" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 250 -110 12 #000000 0 "This example illustrates how to use subcircuits with Ngspice and Xyce.\nYou need to specify absolute location of subcircuit "quarz.sch".\nThis subcircuit is in the same directory where is this example.\n\nThe primary difference between this version and the qucs-s standard \nngspice example is in the presence of the "XYCE script" block.">
</Paintings>
