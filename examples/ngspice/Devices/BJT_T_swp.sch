<Qucs Schematic 24.3.99>
<Properties>
  <View=-257,-171,1108,578,1.02807,0,0>
  <Grid=10,10,1>
  <DataSet=BJT_T_swp.dat>
  <DataDisplay=BJT_T_swp.dpl>
  <OpenDisplay=0>
  <Script=BJT_T_swp.m>
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
  <R R2 1 170 130 16 -21 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 240 80 -12 -57 1 2>
  <R R1 1 100 130 -76 -20 0 1 "300k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 310 120 18 -26 0 1 "12 V" 1>
  <GND * 1 310 160 0 0 0 0>
  <GND * 1 170 250 0 0 0 0>
  <.DC DC1 1 70 340 0 38 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 240 340 0 64 0 0 "DC1" 1 "lin" 1 "TEMP" 1 "-40" 1 "85" 1 "200" 1>
  <_BJT X2N2222A_1 1 170 210 15 -20 0 0 "npn" 0 "14.34f" 0 "1" 0 "1" 0 "0.2847" 0 "0" 0 "74.03" 0 "0" 0 "14.34f" 0 "1.307" 0 "0" 0 "2" 0 "255.9" 1 "6.092" 0 "0" 0 "0" 0 "1" 0 "0" 0 "10" 0 "22.01p" 0 "0.75" 0 "0.377" 0 "7.306p" 0 "0.75" 0 "0.3416" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "411.1p" 0 "3" 0 "0" 0 "0.6V" 0 "46.91n" 0 "26.85" 1 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 1 "1" 0 "yes" 1>
</Components>
<Wires>
  <170 80 170 100 "" 0 0 0 "">
  <170 80 210 80 "" 0 0 0 "">
  <100 80 170 80 "" 0 0 0 "">
  <100 80 100 100 "" 0 0 0 "">
  <270 80 310 80 "" 0 0 0 "">
  <310 80 310 90 "" 0 0 0 "">
  <310 150 310 160 "" 0 0 0 "">
  <170 240 170 250 "" 0 0 0 "">
  <170 160 170 180 "" 0 0 0 "">
  <100 160 100 210 "" 0 0 0 "">
  <100 210 140 210 "" 0 0 0 "">
  <170 180 170 180 "Vce" 190 150 0 "">
</Wires>
<Diagrams>
  <Rect 440 357 387 297 3 #c0c0c0 1 00 1 -40 20 84.3719 0 0 2 10 0 0 0.002 0.01 315 0 225 1 0 0 "" "" "">
	<"ngspice/sw1.v(vce)" #0000ff 0 3 0 0 0>
	<"ngspice/sw1.i(pr1)" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 -100 12 #082dff 0 "ra3xdh:\nI have added the property UseGlobTemp=yes/no \nThe default is yes. If set to yes it uses global temperature. \nOtherwise the device uses temperature defined in Temp. \nThe temperature sweep will be possible for device using default parameters.">
  <Text 50 -130 12 #000000 0 "https://github.com/ra3xdh/qucs_s/pull/925">
</Paintings>
