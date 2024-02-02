<Qucs Schematic 24.1.0>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=genericopa_sub.dat>
  <DataDisplay=genericopa_sub.dpl>
  <OpenDisplay=0>
  <Script=genericopa_sub.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym -40 -20 1 0>
  <.PortSym -40 20 2 0>
  <.PortSym 10 -40 3 0>
  <.PortSym 10 40 4 0>
  <.PortSym 60 0 5 180>
  <Line -20 40 0 -80 #000080 2 1>
  <Line 40 0 -60 -40 #000080 2 1>
  <Line -20 40 60 -40 #000080 2 1>
  <Line 10 -20 0 -20 #000080 2 1>
  <Line 10 40 0 -20 #000080 2 1>
  <Line -40 -20 20 0 #000080 2 1>
  <Line -40 20 20 0 #000080 2 1>
  <Line -15 20 10 0 #000000 2 1>
  <Line -15 -20 10 0 #ff0000 2 1>
  <Line -10 -15 0 -10 #ff0000 2 1>
  <Text 15 -35 8 #000000 0 "VCC">
  <Text 15 25 8 #000000 0 "VEE">
  <Line 40 0 20 0 #000080 2 1>
  <.ID 50 24 SUB "1=Voffset=5m=Input Offset (Volts)=" "1=Gain=20k=Open-Loop Gain=" "1=Rout=10=Output Resistance (Ohms)=" "1=Rin=10M=Input Resistance (Ohms)=" "1=Pole=20=Dominant Pole Frequency (Hz)=">
</Symbol>
<Components>
  <Port P1 1 100 80 -52 -9 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P3 1 100 140 -50 -9 0 0 "3" 1 "analog" 0 "v" 0 "" 0>
  <Port P5 1 100 200 -50 -11 0 0 "5" 1 "analog" 0 "v" 0 "" 0>
  <Port P4 1 200 140 34 -10 0 2 "4" 1 "analog" 0 "v" 0 "" 0>
  <Port P2 1 200 80 34 -11 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <SpLib X1 1 150 140 -26 92 0 0 "genericopa.cir" 1 "genopa" 1 "auto" 0 "POLE=pole GAIN=gain VOFFSET=voffset ROUT=rout RIN=rin" 1>
</Components>
<Wires>
  <100 200 120 200 "" 0 0 0 "">
  <180 140 200 140 "" 0 0 0 "">
  <100 140 120 140 "" 0 0 0 "">
  <180 80 200 80 "" 0 0 0 "">
  <100 80 120 80 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
