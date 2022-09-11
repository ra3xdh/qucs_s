<Qucs Schematic 0.0.17>
<Properties>
  <View=188,80,540,401,1.83178,0,0>
  <Grid=5,5,0>
  <DataSet=timer_Discharge.dat>
  <DataDisplay=timer_Discharge.dpl>
  <OpenDisplay=1>
  <Script=timer_Discharge.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -40 -60 0 120 #000080 2 1>
  <Line -40 60 80 0 #000080 2 1>
  <Line -40 -60 80 0 #000080 2 1>
  <Line -50 20 10 0 #000080 2 1>
  <.PortSym -50 20 1 0>
  <Line 40 -60 0 120 #000080 2 1>
  <Line -10 -20 0 0 #000000 0 1>
  <Line -10 0 0 40 #00007f 4 1>
  <Line -40 20 30 0 #00007f 2 1>
  <Line -10 5 20 -15 #00007f 2 1>
  <Line -10 35 20 15 #00007f 2 1>
  <Line 10 -10 0 -20 #00007f 2 1>
  <.PortSym 10 80 3 0>
  <Line 5 45 0 -15 #00007f 2 1>
  <Line 5 45 -10 0 #00007f 2 1>
  <Line -10 45 5 0 #00007f 2 1>
  <.ID -50 104 SUB>
  <Line 10 50 0 10 #00007f 2 1>
  <Line 10 60 0 20 #000080 2 1>
  <Line 10 -30 40 0 #000080 2 1>
  <.PortSym 50 -30 2 180>
  <Text -30 -50 12 #000000 0 "Discharge\nSwitch">
</Symbol>
<Components>
  <R R1 1 350 150 -26 15 0 0 "10K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port P_control_in 1 255 150 -23 12 0 0 "1" 0 "analog" 0>
  <Port P_GND 1 260 260 -23 12 0 0 "3" 0 "analog" 0>
  <Port P_Discharge 1 260 320 -23 12 0 0 "2" 0 "analog" 0>
  <_BJT T1 1 410 150 8 -26 0 0 "npn" 1 "1e-16" 1 "1" 1 "1" 0 "0" 0 "0" 0 "0" 1 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "100" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "1e-9" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <R R2 1 370 320 -26 15 0 0 "200" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <255 150 320 150 "" 0 0 0 "">
  <260 260 410 260 "" 0 0 0 "">
  <410 180 410 260 "" 0 0 0 "">
  <260 320 340 320 "" 0 0 0 "">
  <410 120 500 120 "" 0 0 0 "">
  <500 120 500 320 "" 0 0 0 "">
  <400 320 500 320 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
