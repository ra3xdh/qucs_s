<Qucs Schematic 0.0.24>
<Properties>
  <View=0,0,1752,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=SuperSimpleDC.dat>
  <DataDisplay=SuperSimpleDC.dpl>
  <OpenDisplay=0>
  <Script=dum.m>
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
  <Vdc V1 1 300 210 18 -26 0 1 "1 V" 1>
  <GND * 1 300 240 0 0 0 0>
  <.DC DC1 1 120 410 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R R1 1 490 180 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <IProbe Pr1 1 430 180 -26 16 0 0>
  <GND * 1 520 180 0 0 0 0>
  <.SW SW1 1 390 500 0 71 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "5" 1 "11" 1 "false" 0>
</Components>
<Wires>
  <300 180 400 180 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 650 400 240 160 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 0 0 0 "" "" "">
	<"xyce/SuperSimpleDC:I(PR1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Arrow 510 610 -40 -10 20 8 #000000 0 1 0>
  <Arrow 510 620 -40 10 20 8 #000000 0 1 0>
  <Text 510 600 12 #000000 0 "Note that the important parameters to set here are Sim, Param, Start, Stop, and Points.  \n\nThe dialog for this sweep will also have a "Step" parameter, but it is not used except to compute "Points," and qucs has an off-by-one error here somehow.\n">
  <Rectangle 510 590 1120 80 #000000 0 1 #c0c0c0 1 0>
  <Arrow 510 640 -40 10 20 8 #000000 0 1 0>
  <Arrow 510 660 -40 10 20 8 #000000 0 1 0>
  <Arrow 510 680 -40 10 20 8 #000000 0 1 0>
  <Text 0 360 12 #000000 0 "This is required, but doesn't have any real parameters other than its name.\nThe name here is used to link to a parameter sweep object.">
</Paintings>
