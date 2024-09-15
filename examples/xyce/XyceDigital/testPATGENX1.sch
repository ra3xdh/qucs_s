<Qucs Schematic 24.3.99>
<Properties>
  <View=-541,-130,1632,1061,1.45277,613,243>
  <Grid=10,10,1>
  <DataSet=testPATGENX1.dat>
  <DataDisplay=testPATGENX1.dpl>
  <OpenDisplay=0>
  <Script=testPATGENX1.m>
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
  <SpLib X4 1 80 200 -191 16 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "PATGENX1" 1 "auto" 1 "PulseFreq=1k ScaleFactor=5" 1 "" 0>
  <SpLib X5 1 230 200 -59 19 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "A2DBRIDGE" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <SpLib X6 1 410 200 -20 -116 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "D2ABRIDGE" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <GND * 1 530 290 0 0 0 0>
  <R_SPICE R1 1 530 250 15 -26 0 1 "4.7k" 1 "" 0 "" 0 "" 0 "" 0 "2" 0 "R" 0>
  <.TR TR1 1 690 110 0 64 0 0 "lin" 1 "0" 1 "5 ms" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <300 200 360 200 "" 0 0 0 "">
  <110 200 180 200 "nApulseIN" 110 150 24 "">
  <480 200 530 200 "" 0 0 0 "">
  <530 200 530 220 "" 0 0 0 "">
  <530 280 530 290 "" 0 0 0 "">
  <300 200 300 200 "nDpulse" 310 170 0 "">
  <530 200 530 200 "nApulseOUT" 540 170 0 "">
</Wires>
<Diagrams>
  <Rect 160 449 766 79 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NAPULSEIN)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 160 619 764 89 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NDPULSE)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 150 821 768 88 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"xyce/tran.V(NAPULSEOUT)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Line -20 330 0 340 #000000 3 1>
  <Line 1110 290 0 340 #000000 3 1>
  <Line 380 1020 340 0 #000000 3 1>
  <Line 360 -90 340 0 #000000 3 1>
</Paintings>
