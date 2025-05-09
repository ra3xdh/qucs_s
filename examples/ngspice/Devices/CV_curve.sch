<Qucs Schematic 1.0.2>
<Properties>
  <View=0,60,3553,850,1,0,0>
  <Grid=10,10,1>
  <DataSet=CV_curve.dat>
  <DataDisplay=CV_curve.dpl>
  <OpenDisplay=1>
  <Script=CV_curve.m>
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
  <.CUSTOMSIM CUSTOM1 1 70 300 0 51 0 0 "\nlet vecCj = vector(6)\nlet vecV1 = vector(6)\nlet number_v1 = 0\nforeach  v1_act 0 1 2 3 4 5 \nalter V1 = $v1_act\nac lin 1 1meg 1meg \nlet Cj = -imag(V1#branch)/(2*pi*1e6)\nlet vecCj[number_v1] = Cj\nlet vecV1[number_v1] = $v1_act\nlet number_v1 = number_v1 + 1\ndestroy ac1\nend\n\n" 1 "vecCj;vecV1;" 0 "" 0>
  <Vac V2 1 310 360 18 -26 0 1 "1 V" 1 "1 kHz" 0 "0" 0 "0" 0>
  <GND * 1 310 450 0 0 0 0>
  <GND * 1 490 400 0 0 0 0>
  <Diode D_1N4148_1 1 490 370 -96 -26 0 3 "222p" 1 "1.65" 1 "4p" 1 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "68.6m" 0 "5.76n" 0 "0" 0 "0" 0 "1" 0 "1" 0 "75" 0 "1u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Vdc V1 1 310 420 18 -26 0 1 "1 V" 1>
</Components>
<Wires>
  <310 300 310 330 "" 0 0 0 "">
  <310 300 490 300 "" 0 0 0 "">
  <490 300 490 340 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 590 342 477 252 3 #c0c0c0 1 00 1 0 0.5 5 1 1.78858e-12 5e-13 4.20107e-12 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/veccj@vecv1" #0000ff 0 3 0 0 0>
  </Rect>
  <Tab 590 695 473 285 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/veccj" #0000ff 0 3 1 0 0>
	<"ngspice/vecv1" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 60 80 12 #000000 0 "This example illustrates the simulation \nof the p-n jucnction CV-curve using\nNutmeg scripting mode. \n\nThe vector vecCj represents jucntion capacitance\nThe vector vecV1 represents DC bias voltage\n\nPlot vecCj vs. vecV1 to see the CV-curve.">
  <Arrow 370 230 40 60 20 8 #000000 2 1 0>
</Paintings>
