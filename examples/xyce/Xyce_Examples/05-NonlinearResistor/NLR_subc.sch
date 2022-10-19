<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,1207,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=NLR_subc.dat>
  <DataDisplay=NLR_subc.dpl>
  <OpenDisplay=1>
  <Script=NLR_subc.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 14 SUB "1=R0=.15==" "1=R1=6==" "1=E1=4==">
  <.PortSym -30 0 1 0>
  <Line -20 -10 40 0 #000080 2 1>
  <Line 20 -10 0 20 #000080 2 1>
  <Line -20 -10 0 20 #000080 2 1>
  <Line -30 0 10 0 #000080 2 1>
  <Line 20 0 10 0 #000080 2 1>
  <Line -20 10 40 0 #000080 2 1>
  <.PortSym 30 0 2 0>
</Symbol>
<Components>
  <Eqn Eqn1 1 130 500 -31 16 0 0 "E2=2*E1" 1 "yes" 0>
  <Eqn Eqn3 1 130 580 -31 16 0 0 "k1=1/E1**2" 1 "yes" 0>
  <Eqn Eqn4 1 240 580 -31 16 0 0 "r2=R0+sqrt(2)*delr" 1 "yes" 0>
  <Eqn Eqn2 1 220 500 -31 16 0 0 "delr=R1-R0" 1 "yes" 0>
  <SpiceFunc SpiceFunc1 1 420 500 -24 16 0 0 "Rreg1(a,b,c,d)={a+(b-a)*c/d}" 1 "Rreg2(a,b,c,d,f)={a+sqrt(2-b*(2*c-d)**2)*f}" 1>
  <Port P1 1 180 240 -23 12 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P2 1 320 240 14 12 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <C C1 1 780 370 17 -26 0 1 "1 F" 1 "" 0 "neutral" 0>
  <GND * 1 880 400 0 0 0 0>
  <S4Q_Ieqndef B2 1 880 370 33 -19 0 3 "{IF(TIME<.1p,0,100*abs(I(Vmon)))}" 1 "" 0 "" 0 "" 0 "" 0>
  <src_eqndef B1 1 290 240 -5 -55 0 2 "{I(Vmon)*IF(V(N101)<E1,Rreg1(R0,R1,V(N101),E1),IF(V(N101)<E2,Rreg2(R0,k1,E1,V(N101),delr),R2))}" 1 "" 0 "" 0 "" 0 "" 0>
  <S4Q_V VMON 1 210 240 -26 -54 0 2 "DC 0" 1 "" 0 "" 0 "" 0 "" 0>
  <R R1 1 650 370 15 -26 0 1 "1e12 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <240 240 260 240 "" 0 0 0 "">
  <650 340 780 340 "" 0 0 0 "">
  <780 340 880 340 "" 0 0 0 "">
  <650 400 780 400 "" 0 0 0 "">
  <780 400 880 400 "" 0 0 0 "">
  <780 340 780 340 "N101" 810 310 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 340 60 12 #000000 0 "This schematic implements the nonlinear resistor subcircuit used in the nlrcs10 test case.\n\nNote that the subcircuit has parameters associated with it, and these parameters are used in the equations,\nfunctions, and B sources.  These parameters are defined and given defaults in the SYMBOL associated with this\nschematic.  To view these, right-click on the schematic and choose "Edit circuit symbol."">
</Paintings>
