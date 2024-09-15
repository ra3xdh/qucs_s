<Qucs Schematic 24.3.99>
<Properties>
  <View=-564,25,2087,1477,1.1909,992,71>
  <Grid=10,10,1>
  <DataSet=testCombLogic2.dat>
  <DataDisplay=testCombLogic2.dpl>
  <OpenDisplay=0>
  <Script=testCombLogic2.m>
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
  <.TR TR1 1 1230 470 0 64 0 0 "lin" 1 "0" 1 "55us" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <SpLib X33 1 580 310 -380 -58 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "PATGENX2" 1 "auto" 1 "PulseFreq=25k ScaleFactor=5" 1 "" 0>
  <SpLib X32 1 580 350 -377 -1 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "PATGENX8" 1 "auto" 1 "PulseFreq=250k ScaleFactor=5" 1 "" 0>
  <SpiceModel SpiceModel1 1 130 540 -29 17 0 0 ".model DMOD DIG ( RLOAD=1000 CLOAD=1e-12 DELAY=20e-9 CLO=1e-12 CHI=1e-12" 1 "+  S0RLO=5 S0RHI=5 S0TSW=5e-9 S0VLO=-1 S0VHI=0.16 S1RLO=200 S1RHI=5 S1TSW=5e-9 " 1 "+  S1VLO=0.52 S1VHI=1 )" 1 "" 0 "Line_5=" 0>
  <SpLib X27 1 1010 350 -78 110 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "MUX4TO1" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <SpLib X31 1 830 270 -6 -107 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "LOGIC0" 1 "auto" 1 "" 1 "" 0>
  <SpLib X30 1 840 320 -207 -115 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "A2DBRIDGEX2" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <SpLib X29 1 840 380 -205 52 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "A2DBRIDGEX4" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
  <GND * 1 1260 430 0 0 0 0>
  <R_SPICE R1 1 1260 390 15 -26 0 1 "47k" 1 "" 0 "" 0 "" 0 "" 0 "2" 0 "R" 0>
  <SpLib X28 1 1140 350 -67 -168 0 0 "C:/QUCS-S 24.3.0/share/qucs-s/library/XyceDigital.lib" 0 "D2ABRIDGE" 1 "auto" 1 "ScaleFactor=5" 1 "" 0>
</Components>
<Wires>
  <930 270 930 290 "" 0 0 0 "">
  <860 270 930 270 "" 0 0 0 "">
  <910 330 930 330 "" 0 0 0 "">
  <620 330 790 330 "nAG1" 640 310 28 "">
  <910 310 930 310 "" 0 0 0 "">
  <620 310 790 310 "nAG0" 640 290 28 "">
  <910 410 930 410 "" 0 0 0 "">
  <620 410 790 410 "" 0 0 0 "">
  <910 390 930 390 "" 0 0 0 "">
  <620 390 790 390 "" 0 0 0 "">
  <910 370 930 370 "" 0 0 0 "">
  <620 370 790 370 "" 0 0 0 "">
  <910 350 930 350 "" 0 0 0 "">
  <620 350 790 350 "" 0 0 0 "">
  <1260 420 1260 430 "" 0 0 0 "">
  <1260 350 1260 360 "" 0 0 0 "">
  <1210 350 1260 350 "nAZ" 1260 310 36 "">
  <1070 350 1090 350 "nDZ" 1090 310 12 "">
</Wires>
<Diagrams>
  <Rect 130 740 1262 61 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.5 5 5.5 1 -1 2 1 315 0 225 1 0 0 "" "V(nAG0)" "">
	<"xyce/tran.V(NAG0)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 845 1267 49 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.5 5 5.5 1 -1 2 1 315 0 225 1 0 0 "" "V(nAG1)" "">
	<"xyce/tran.V(NAG1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 1280 1270 161 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.495288 2 6 1 -1 1 1 315 0 225 1 0 0 "" "V(nAZ)" "">
	<"xyce/tran.V(NAZ)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 120 1050 1283 149 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.0990787 0.5 1.08911 1 -1 1 1 315 0 225 1 0 0 "" "V(nDZ)" "">
	<"xyce/tran.V(NDZ)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Line 40 780 0 190 #000000 3 1>
  <Line 1480 790 0 190 #000000 3 1>
  <Line 575 65 190 0 #000000 3 1>
  <Line 665 1435 190 0 #000000 3 1>
</Paintings>
