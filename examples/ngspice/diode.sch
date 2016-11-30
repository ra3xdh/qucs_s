<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,924,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=diode.dat>
  <DataDisplay=diode.dpl>
  <OpenDisplay=1>
  <Script=diode.m>
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
  <Diode D_1N4148_1 1 200 140 -26 -73 0 2 "222p" 1 "1.65" 1 "4p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "68.6m" 0 "5.76n" 0 "0" 0 "0" 0 "1" 0 "1" 0 "75" 0 "1u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Vdc V1 1 100 200 18 -26 0 1 "1 V" 1>
  <IProbe Pr1 1 140 140 -26 16 0 0>
  <GND * 1 250 180 0 0 0 0>
  <GND * 1 100 230 0 0 0 0>
  <.SW SW1 1 290 110 0 71 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "2" 1 "200" 1>
  <.DC DC1 1 110 270 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
</Components>
<Wires>
  <100 140 100 170 "" 0 0 0 "">
  <100 140 110 140 "" 0 0 0 "">
  <230 140 250 140 "" 0 0 0 "">
  <250 140 250 180 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 490 330 351 240 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 120 370 12 #000000 0 "This is DC-sweep usage example. Refer to Chapter 2 of\nspice4qucs project documentation. Run simulation\nand obtain IV-chart of diode.\n\nNOTE: This Example could be run only with Ngspice/Xyce">
</Paintings>
