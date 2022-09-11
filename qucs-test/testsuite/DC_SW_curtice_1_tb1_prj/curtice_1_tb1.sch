<Qucs Schematic 0.0.18>
<Properties>
  <View=-10,-14,1075,849,0.9631,0,0>
  <Grid=10,10,1>
  <DataSet=curtice_1_tb1.dat>
  <DataDisplay=curtice_1_tb1.dpl>
  <OpenDisplay=0>
  <Script=curtice_1_tb1.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 -16 SUB>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
</Symbol>
<Components>
  <Vdc V2 1 80 160 18 -26 0 1 "Vgs" 1>
  <GND * 1 80 190 0 0 0 0>
  <.DC DC1 1 40 250 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Eqn Eqn1 1 70 390 -32 16 0 0 "Id=-V1.I" 1 "yes" 0>
  <.SW SW1 1 380 220 0 77 0 0 "DC1" 1 "lin" 1 "Vds" 1 "-10" 1 "10" 1 "41" 1>
  <.SW SW2 1 540 220 0 77 0 0 "SW1" 1 "lin" 1 "Vgs" 1 "-5" 1 "0" 1 "6" 1>
  <GND * 1 380 160 0 0 0 0>
  <Vdc V1 1 380 120 18 -26 0 1 "Vds" 1>
  <GND * 1 210 160 0 0 0 0>
  <IProbe Pr1 1 320 60 -26 16 1 2>
  <Sub Curtice1 1 210 120 20 -46 0 0 "curtice_1.sch" 0 "1m" 1 "1m" 1 "1m" 1 "10e10" 1 "1e-15" 1 "1e-15" 1 "1e-15" 1 "10f" 1 "1" 1 "0" 1 "1.11" 1 "3m" 1 "40m" 1 "-6" 1 "27" 1 "0.8" 1 "300f" 1 "10p" 1 "1m" 1 "300f" 1 "300f" 1 "27" 1>
</Components>
<Wires>
  <380 150 380 160 "" 0 0 0 "">
  <380 60 380 90 "" 0 0 0 "">
  <210 60 290 60 "" 0 0 0 "">
  <80 120 80 130 "" 0 0 0 "">
  <80 120 180 120 "" 0 0 0 "">
  <210 60 210 80 "" 0 0 0 "">
  <350 60 380 60 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 140 724 386 294 3 #c0c0c0 1 00 1 0 2 10 1 -0.0151191 0.05 0.16631 1 -1 0.5 1 315 0 225 "" "" "">
	<"Id" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect3D 610 759 425 357 3 #c0c0c0 1 00 1 -10 2 10 1 -5.5 1 0.5 0 -0.2 0.05 0.2 315 0 148 "" "" "">
	<"Id" #0000ff 2 3 0 0 1>
  </Rect3D>
</Diagrams>
<Paintings>
  <Text 590 80 13 #000000 0 "Author: Mike Brinson">
</Paintings>
