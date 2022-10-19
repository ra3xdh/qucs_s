<Qucs Schematic 0.0.13>
<Properties>
  <View=0,10,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=time_resistor.dat>
  <DataDisplay=time_resistor.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 14 TRES "1=R_amp=5.0=resistance amplitude" "1=freq=1k=resistance frequency">
  <.PortSym -30 0 1 0>
  <Line -20 -10 40 0 #000080 2 1>
  <Line 20 -10 0 20 #000080 2 1>
  <Line -20 10 40 0 #000080 2 1>
  <Line -20 -10 0 20 #000080 2 1>
  <Line -30 0 10 0 #000080 2 1>
  <Line 20 0 10 0 #000080 2 1>
  <.PortSym 30 0 2 180>
</Symbol>
<Components>
  <Idc I1 1 140 260 18 -26 0 1 "1" 1>
  <C C1 1 210 260 17 -26 0 1 "1" 1 "0" 0 "neutral" 0>
  <R R1 1 60 260 15 -26 0 1 "1e9" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <VCVS VCVS1 1 330 260 -21 56 0 0 "1" 1 "0" 0>
  <GND * 1 460 310 0 0 0 0>
  <GND * 1 60 310 0 0 0 0>
  <EDD D1 1 420 180 -26 -110 0 0 "explicit" 0 "2" 0 "V1/(R_amp*cos(2*pi*freq*V2))" 1 "0" 0 "0" 1 "0" 0>
  <Port P1 1 360 150 -23 12 0 0 "1" 1 "analog" 0>
  <Port P2 1 480 150 4 -44 0 2 "2" 1 "analog" 0>
</Components>
<Wires>
  <210 290 210 310 "" 0 0 0 "">
  <140 290 140 310 "" 0 0 0 "">
  <140 310 210 310 "" 0 0 0 "">
  <210 210 210 230 "" 0 0 0 "">
  <140 210 140 230 "" 0 0 0 "">
  <140 210 210 210 "" 0 0 0 "">
  <60 210 140 210 "" 0 0 0 "">
  <60 210 60 230 "" 0 0 0 "">
  <60 310 140 310 "" 0 0 0 "">
  <60 290 60 310 "" 0 0 0 "">
  <210 210 300 210 "V_time" 270 170 45 "">
  <300 210 300 230 "" 0 0 0 "">
  <300 290 300 310 "" 0 0 0 "">
  <210 310 300 310 "" 0 0 0 "">
  <360 210 360 230 "" 0 0 0 "">
  <360 210 390 210 "" 0 0 0 "">
  <360 290 360 310 "" 0 0 0 "">
  <460 210 460 310 "" 0 0 0 "">
  <450 210 460 210 "" 0 0 0 "">
  <360 310 460 310 "" 0 0 0 "">
  <300 310 360 310 "" 0 0 0 "">
  <360 150 390 150 "" 0 0 0 "">
  <450 150 480 150 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 50 30 16 #000000 0 "Time-dependent variable Resistor Schematic">
  <Text 60 360 12 #000000 0 "Parameters:\n  R_{amp} - amplitude of resistor value\n  freq - frequency of resistor value">
</Paintings>
