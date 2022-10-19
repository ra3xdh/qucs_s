<Qucs Schematic 0.0.24>
<Properties>
  <View=0,46,1222,775,1,0,0>
  <Grid=10,10,1>
  <DataSet=Tunnel_Diode_EDD.dat>
  <DataDisplay=Tunnel_Diode_EDD.dpl>
  <OpenDisplay=0>
  <Script=Tunnel_Diode_EDD.m>
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
  <EDD D1 1 250 190 104 -46 0 3 "explicit" 0 "3" 0 "Is*(exp(V1/VT)-1.0)" 1 "C*V1" 1 "Iv*exp(K*(V1-Vv))" 1 "0" 0 "Ip*(V1/Vp)*exp((Vp-V1)/Vp)" 1 "0" 0>
  <.DC DC1 1 40 360 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 250 260 0 0 0 0>
  <IProbe Pr1 1 130 100 -15 17 0 0>
  <Vdc V1 1 60 170 -52 -22 1 1 "1" 1>
  <GND * 1 60 220 0 0 0 0>
  <Eqn Eqn1 1 410 370 -23 15 0 0 "Temp0=300" 1 "VT=(kB*Temp0)/q" 1 "Is=1e-12" 1 "Ip=1e-5" 1 "Iv=1e-6" 1 "Vp=0.1" 1 "Vv=0.4" 1 "C=0.01p" 1 "K=5" 1 "yes" 0>
  <.SW SW1 1 220 360 0 75 0 0 "DC1" 1 "lin" 1 "V1" 1 "-0.05" 1 "0.4" 1 "51" 1 "false" 0>
</Components>
<Wires>
  <190 240 250 240 "" 0 0 0 "">
  <190 220 190 240 "" 0 0 0 "">
  <250 220 250 240 "" 0 0 0 "">
  <250 240 310 240 "" 0 0 0 "">
  <310 220 310 240 "" 0 0 0 "">
  <250 240 250 260 "" 0 0 0 "">
  <250 100 250 140 "" 0 0 0 "">
  <160 100 250 100 "" 0 0 0 "">
  <60 100 100 100 "" 0 0 0 "">
  <60 100 60 140 "" 0 0 0 "">
  <60 200 60 220 "" 0 0 0 "">
  <310 140 310 160 "" 0 0 0 "">
  <250 140 250 160 "" 0 0 0 "">
  <250 140 310 140 "" 0 0 0 "">
  <190 140 190 160 "" 0 0 0 "">
  <190 140 250 140 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 720 534 468 394 3 #c0c0c0 1 00 1 -0.05 0.05 0.4 1 -2.55557e-5 5e-6 1.34758e-5 1 -1 0.2 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 600 12 #000000 0 "This examples shows how to use\n current-defined EDDwith Ngspice. \nYou can simulate it and obtain\n IV-chart of the Tunnel diode. \n\nNOTE: This Example could be run \nwith Qucs and Ngspice/Xyce">
</Paintings>
