<Qucs Schematic 1.0.0>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=XTAL.dat>
  <DataDisplay=XTAL.dpl>
  <OpenDisplay=1>
  <Script=XTAL.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Titel>
  <FrameText1=Gezeichnet von:>
  <FrameText2=Datum:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -20 -10 0 20 #000000 2 1>
  <.PortSym -40 0 1 0>
  <Line -40 0 20 0 #000000 2 1>
  <Rectangle -10 -20 10 40 #000000 2 1 #c0c0c0 1 0>
  <Line 10 -10 0 20 #000000 2 1>
  <Line 10 0 20 0 #000000 2 1>
  <.PortSym 30 0 2 180>
  <.ID -40 24 Q "0=Quarz=Quarz==" "1=fs=10e6=Quarzfrequenz=">
</Symbol>
<Components>
  <L Ls 1 440 290 20 -23 0 3 "Lser" 1 "" 0>
  <Port P1 1 540 180 -62 -23 0 3 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P2 1 560 560 12 4 0 1 "2" 1 "analog" 0 "v" 0 "" 0>
  <R Rs 1 440 370 21 -23 0 1 "Rser" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C Cs 1 440 450 21 -23 0 1 "Cser" 1 "" 0 "neutral" 0>
  <C Cp 1 660 370 -73 -22 0 1 "Cpar" 1 "" 0 "neutral" 0>
  <Eqn Eqn1 1 130 270 -33 16 0 0 "Q=29000" 1 "Rser=23" 1 "Cpar=6" 1 "Lser=(Q*Rser)/(2*pi*fs)" 1 "Cser=1/(2*pi*Q*Rser)" 1 "yes" 0>
</Components>
<Wires>
  <440 320 440 340 "" 0 0 0 "">
  <440 400 440 420 "" 0 0 0 "">
  <440 220 440 260 "" 0 0 0 "">
  <440 220 540 220 "" 0 0 0 "">
  <660 220 660 340 "" 0 0 0 "">
  <660 400 660 520 "" 0 0 0 "">
  <440 520 560 520 "" 0 0 0 "">
  <440 480 440 520 "" 0 0 0 "">
  <540 220 660 220 "" 0 0 0 "">
  <540 180 540 220 "" 0 0 0 "">
  <560 520 660 520 "" 0 0 0 "">
  <560 520 560 560 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 100 140 12 #000000 0 "Ls = (Q * Rs) / (2 * pi * fs)\nCs = 1 / (2 * pi * fs * Q * Rs)">
  <Rectangle 420 200 260 340 #000000 0 1 #c0c0c0 1 0>
</Paintings>
