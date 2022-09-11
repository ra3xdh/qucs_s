<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=divisortension@transient.dat>
  <DataDisplay=divisortension@transient.dpl>
  <OpenDisplay=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <R R2 1 610 270 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R1 1 530 270 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.TR TR1 1 110 160 0 77 0 0 "lin" 1 "0" 1 "1 ms" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <VProbe div 1 580 190 28 -31 0 0>
  <GND * 1 590 210 0 0 0 0>
  <GND * 1 660 400 0 0 0 0>
  <Eqn Eqn1 1 270 450 -33 19 0 0 "ok=assert(abs(div.Vt-sin(2*pi*f*time))<1e-6)" 1 "yes" 0>
  <Vac V1 1 410 330 18 -26 0 1 "2V" 1 "f" 1 "0" 0 "0" 0>
  <Eqn Eqn2 1 270 370 -33 19 0 0 "f=1e3" 1 "yes" 0>
</Components>
<Wires>
  <660 270 660 390 "" 0 0 0 "">
  <560 270 570 270 "" 0 0 0 "">
  <640 270 660 270 "" 0 0 0 "">
  <570 270 580 270 "" 0 0 0 "">
  <570 210 570 270 "" 0 0 0 "">
  <660 390 660 400 "" 0 0 0 "">
  <410 390 660 390 "" 0 0 0 "">
  <410 360 410 390 "" 0 0 0 "">
  <410 270 500 270 "" 0 0 0 "">
  <410 270 410 300 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
