<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-180,1436,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=diode.dat>
  <DataDisplay=diode.dpl>
  <OpenDisplay=0>
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
  <.DC DC1 1 90 400 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 270 400 0 71 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "2" 1 "200" 1 "false" 0>
  <Vdc V1 1 100 190 -64 -26 1 1 "1 V" 1>
  <GND * 1 100 240 0 0 0 0>
  <IProbe Pr1 1 170 140 -11 -58 0 0>
  <GND * 1 240 240 0 0 0 0>
  <SpiceModel SpiceModel1 1 130 310 -27 16 0 0 ".model 1N4007 D(IS=7.02767n RS=0.0341512 N=1.80803 EG=1.05743 XTI=5 BV=1000 IBV=5e-08 CJO=1e-11 VJ=0.7 M=0.5 FC=0.5 TT=1e-07)" 1 "" 0 "" 0 "" 0 "Line_5=" 0>
  <DIODE_SPICE D1 1 240 190 20 -18 1 3 "1N4007" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <100 220 100 240 "" 0 0 0 "">
  <100 140 100 160 "" 0 0 0 "">
  <100 140 140 140 "" 0 0 0 "">
  <240 220 240 240 "" 0 0 0 "">
  <200 140 240 140 "" 0 0 0 "">
  <240 140 240 160 "" 0 0 0 "">
  <240 140 240 140 "Vd" 260 110 0 "">
</Wires>
<Diagrams>
  <Rect 560 280 351 240 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 60 -70 12 #000000 0 "This is DC-sweep usage example. Refer to Chapter 2 of\nspice4qucs project documentation. Run simulation\nand obtain IV-chart of diode.\n\nNOTE: This Example could be run only with Ngspice/Xyce">
</Paintings>
