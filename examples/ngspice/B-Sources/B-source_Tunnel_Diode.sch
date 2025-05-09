<Qucs Schematic 0.0.21>
<Properties>
  <View=0,-80,1190,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=B-source_Tunnel_Diode.dat>
  <DataDisplay=B-source_Tunnel_Diode.dpl>
  <OpenDisplay=0>
  <Script=B-source_Tunnel_Diode.m>
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
  <.DC DC1 1 80 390 0 39 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 280 300 0 0 0 0>
  <GND * 1 100 300 0 0 0 0>
  <Vdc V1 1 100 250 -66 -20 0 1 "1 V" 1>
  <S4Q_Ieqndef B1 1 280 250 26 -34 0 1 "Is*(exp(V(anode)/phiT)-1.0) +" 1 "+ Iv*exp(K*(V(anode)-Vv)) +" 1 "+ Ip*(V(anode)/Vp)*exp((Vp-V(anode))/Vp)" 1 "" 0 "" 0>
  <IProbe Pr1 1 200 160 -15 -58 0 0>
  <SpicePar SpicePar1 1 430 400 -31 16 0 0 "kB=1.38e-23" 1 "q=1.6e-19" 1 "Vv=0.4" 1 "Iv=1e-6" 1 "Ip=1e-5" 1 "Is=1e-12" 1 "Vp=0.1" 1 "K=5" 1 "Temp0=300" 1 "phiT={(kB*Temp0)/q}" 1>
  <.SW SW1 1 250 390 0 63 0 0 "DC1" 1 "lin" 1 "V1" 1 "-0.05" 1 "0.4" 1 "50" 1 "false" 0>
</Components>
<Wires>
  <100 280 100 300 "" 0 0 0 "">
  <280 280 280 300 "" 0 0 0 "">
  <280 160 280 220 "" 0 0 0 "">
  <230 160 280 160 "" 0 0 0 "">
  <100 160 100 220 "" 0 0 0 "">
  <100 160 170 160 "" 0 0 0 "">
  <280 160 280 160 "anode" 310 130 0 "">
</Wires>
<Diagrams>
  <Rect 760 417 353 337 3 #c0c0c0 1 00 1 -0.05 0.1 0.4 1 -0.095 0.2 0.445 1 -1 1 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 0 16 #000000 0 "Tunnel Diode modeling with a Spice B-source\nNote the placement of "+" signs in the B-source equations!!">
</Paintings>
