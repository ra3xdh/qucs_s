<Qucs Schematic 24.3.99>
<Properties>
  <View=-226,-121,1120,618,1.04201,0,0>
  <Grid=10,10,1>
  <DataSet=BJT_TEMPER_swp.dat>
  <DataDisplay=BJT_TEMPER_swp.dpl>
  <OpenDisplay=0>
  <Script=BJT_TEMPER_swp.m>
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
  <R R2 1 160 150 15 -26 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 240 100 -14 -56 1 2>
  <.DC DC1 1 70 380 0 38 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R R1 1 90 150 -74 -20 0 1 "300k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.SW SW1 1 250 380 0 64 0 0 "DC1" 1 "lin" 1 "TEMP" 1 "-40" 1 "85" 1 "200" 1>
  <GND * 1 160 270 0 0 0 0>
  <Vdc V1 1 300 140 18 -26 0 1 "12 V" 1>
  <GND * 1 300 180 0 0 0 0>
  <_BJT Q2N2222A_1 1 160 230 15 -30 0 0 "npn" 0 "8.11e-14" 0 "1" 0 "1" 0 "0.5" 0 "0.225" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "205" 1 "4" 0 "0" 0 "0" 0 "0.137" 0 "0.343" 0 "1.37" 0 "2.95e-11" 0 "0.75" 0 "0.33" 0 "1.52e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "3.97e-10" 0 "0" 0 "0" 0 "0" 0 "8.5e-08" 0 "TEMPER" 1 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 1 "1" 0 "no" 1>
</Components>
<Wires>
  <160 100 160 120 "" 0 0 0 "">
  <160 180 160 200 "" 0 0 0 "">
  <90 100 160 100 "" 0 0 0 "">
  <90 100 90 120 "" 0 0 0 "">
  <90 230 130 230 "" 0 0 0 "">
  <90 180 90 230 "" 0 0 0 "">
  <160 100 210 100 "" 0 0 0 "">
  <160 260 160 270 "" 0 0 0 "">
  <270 100 300 100 "" 0 0 0 "">
  <300 100 300 110 "" 0 0 0 "">
  <300 170 300 180 "" 0 0 0 "">
  <160 200 160 200 "Vce" 180 170 0 "">
</Wires>
<Diagrams>
  <Rect 430 387 387 297 3 #c0c0c0 1 00 1 -40 20 84.3719 0 0 2 10 0 0 0.002 0.01 315 0 225 1 0 0 "" "" "">
	<"ngspice/sw1.v(vce)" #0000ff 0 3 0 0 0>
	<"ngspice/sw1.i(pr1)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 -80 12 #082dff 0 "ra3xdh:\nThe TEMPER represents a global temperature in Ngspice. \nBut the DC sweep requires TEMP as the parameter to sweep. \nThese variable names are a bit ambiguous. \nHere is an example of the temperature sweep.\n\n">
</Paintings>
