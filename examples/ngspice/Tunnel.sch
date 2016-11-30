<Qucs Schematic 0.0.19>
<Properties>
  <View=0,56,800,680,1,0,0>
  <Grid=10,10,1>
  <DataSet=Tunnel.dat>
  <DataDisplay=Tunnel.dpl>
  <OpenDisplay=1>
  <Script=Tunnel.m>
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
  <IProbe Pr1 1 150 110 -26 16 0 0>
  <GND * 1 40 250 0 0 0 0>
  <.DC DC1 1 90 290 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Vdc V1 1 40 220 18 -26 0 1 "1" 1>
  <.SW SW1 1 290 290 0 71 0 0 "DC1" 1 "lin" 1 "V1" 1 "-0.05" 1 "0.4" 1 "50" 1>
  <GND * 1 250 240 0 0 0 0>
  <EDD D1 1 250 190 96 -26 0 3 "explicit" 0 "3" 0 "Is*(exp(V1/VT)-1.0)" 1 "C*V1" 1 "Iv*exp(K*(V1-Vv))" 1 "0" 0 "Ip*(V1/Vp)*exp((Vp-V1)/Vp)" 1 "0" 0>
  <Eqn Eqn1 1 600 110 -23 15 0 0 "Temp0=300" 1 "VT=(kB*Temp0)/q" 1 "Is=1e-12" 1 "Ip=1e-5" 1 "Iv=1e-6" 1 "Vp=0.1" 1 "Vv=0.4" 1 "C=0.01p" 1 "K=5" 1 "yes" 0>
</Components>
<Wires>
  <40 110 40 190 "" 0 0 0 "">
  <40 110 120 110 "" 0 0 0 "">
  <180 110 250 110 "" 0 0 0 "">
  <250 110 250 130 "" 0 0 0 "">
  <190 240 250 240 "" 0 0 0 "">
  <190 220 190 240 "" 0 0 0 "">
  <250 220 250 240 "" 0 0 0 "">
  <250 240 310 240 "" 0 0 0 "">
  <310 220 310 240 "" 0 0 0 "">
  <310 130 310 160 "" 0 0 0 "">
  <190 130 190 160 "" 0 0 0 "">
  <250 130 250 160 "" 0 0 0 "">
  <190 130 250 130 "" 0 0 0 "">
  <250 130 310 130 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 510 577 275 217 3 #c0c0c0 1 00 1 -0.05 0.1 0.4 1 -2.55556e-05 1e-05 1.34747e-05 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 20 390 12 #000000 0 "This examples shows how to use\n current-defined EDDwith Ngspice. \nYou can simulate it and obtain\n IV-chart of the Tunnel diode. \n\nNOTE: This Example could be run \nwith Qucs and Ngspice/Xyce">
</Paintings>
