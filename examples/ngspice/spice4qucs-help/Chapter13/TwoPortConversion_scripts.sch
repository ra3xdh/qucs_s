<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,1401,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=TwoPortConversion_scripys.dat>
  <DataDisplay=TwoPortConversion_scripys.dpl>
  <OpenDisplay=1>
  <Script=TwoPortConversion_scripys.m>
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
  <NutmegEq StoY_Conversion_Template1 1 160 170 -27 19 0 0 "ac" 1 "StoY_Z0=50.0" 1 "StoY_DelSxZ0=(  (1+V(nS11))*(1+V(nS22))-V(nS12)*V(nS21)  )*StoY_Z0" 1 "Y11=(  (1+V(nS22))*(1-V(nS11))+V(nS12)*V(nS21) )/StoY_DelSxZ0" 1 "Y12=-2*V(nS12)/StoY_DelSxZ0" 1 "Y21=-2*V(nS21)/StoY_DelSxZ0" 1 "Y22=((1+V(nS11))*(1-V(nS22))+V(nS12)*V(nS21))/StoY_DelSxZ0" 1>
  <NutmegEq StoZ_Conversion_Template1 1 150 450 -27 19 0 0 "ac" 1 "StoZ_Z0=50.0" 1 "StoZ_DelSxZ0=(  (1-V(nS11))*(1-V(nS22))-V(nS12)*V(nS21)  )/StoZ_Z0" 1 "Z11=(  (1+V(nS11))*(1-V(nS22))+V(nS12)*V(nS21) )/StoZ_DelSxZ0" 1 "Z12=2*V(nS12)/StoZ_DelSxZ0" 1 "Z21=2*V(nS21)/StoZ_DelSxZ0" 1 "Z22=((1-V(nS11))*(1+V(nS22))+V(nS12)*V(nS21))/StoZ_DelSxZ0" 1>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Rectangle 110 140 560 250 #aa5500 4 1 #c0c0c0 1 0>
  <Rectangle 110 430 550 240 #aa5500 4 1 #c0c0c0 1 0>
</Paintings>
