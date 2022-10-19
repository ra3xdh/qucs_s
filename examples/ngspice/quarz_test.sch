<Qucs Schematic 1.0.0>
<Properties>
  <View=0,-220,1265,907,1,0,0>
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
  <R R1 1 360 160 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Sub SUB1 1 250 160 -26 55 0 0 "quarz.sch" 1 "8863k" 1 "0.01406" 1 "6.5p" 1>
  <.AC AC1 1 110 380 0 43 0 0 "lin" 1 "8800 kHz" 1 "9000 kHz" 1 "400" 1 "no" 0>
  <Eqn Eqn1 1 330 390 -30 15 0 0 "K=dB(out.v/in.v)" 1 "yes" 0>
  <GND * 1 120 300 0 0 0 0>
  <R R2 1 120 200 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 120 260 -64 -26 1 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <390 160 450 160 "" 0 0 0 "">
  <450 160 450 190 "" 0 0 0 "">
  <290 160 330 160 "out" 320 120 21 "">
  <120 290 120 300 "" 0 0 0 "">
  <120 160 120 170 "" 0 0 0 "">
  <120 160 210 160 "in" 180 120 49 "">
</Wires>
<Diagrams>
  <Rect 620 330 301 191 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/ac.k" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 625 590 293 196 3 #c0c0c0 1 01 1 -1 0.5 1 1 0 1 0 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"xyce/ac.V(OUT)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 190 -20 12 #000000 0 "This example illustrates how to use subcircuits with Ngspice and Xyce.\nYou need to specify absolute location of subcircuit "quarz.sch".\nThis subcircuit is in the same directory where is this example.\n\nNOTE: This Example could be run with Qucs and Ngspice/Xyce">
</Paintings>
