<Qucs Schematic 0.0.19>
<Properties>
  <View=15,-50,1520,1663,1,0,840>
  <Grid=10,10,1>
  <DataSet=testCombLogic2.dat>
  <DataDisplay=testCombLogic2.dpl>
  <OpenDisplay=1>
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
  <SpLib X28 1 1130 410 -109 57 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "D2ABRIDGE" 1 "auto" 1 "ScaleFactor=5" 1>
  <GND * 1 1190 410 0 0 0 0>
  <R_SPICE R1 1 1190 380 15 -26 0 1 "47k" 1 "" 0 "" 0 "" 0 "" 0>
  <SpLib X27 1 990 330 -162 135 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "MUX4TO1" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X31 1 820 270 -383 -81 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "LOGIC0" 1 "auto" 1 "" 1>
  <SpLib X33 1 580 310 -356 -58 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "PATGENX2" 1 "auto" 1 "PulseFreq=25k ScaleFactor=5" 1>
  <SpLib X29 1 850 360 -231 108 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "A2DBRIDGEX4" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X30 1 850 320 -227 -128 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "A2DBRIDGEX2" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X32 1 580 350 -353 -1 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "PATGENX8" 1 "auto" 1 "PulseFreq=250k ScaleFactor=5" 1>
  <SpiceModel SpiceModel1 1 270 540 -29 17 0 0 ".model DMOD DIG ( RLOAD=1000 CLOAD=1e-12 DELAY=20e-9 CLO=1e-12 CHI=1e-12" 1 "+  S0RLO=5 S0RHI=5 S0TSW=5e-9 S0VLO=-1 S0VHI=0.16 S1RLO=200 S1RHI=5 S1TSW=5e-9 " 1 "+  S1VLO=0.52 S1VHI=1 )" 1 "" 0 "Line_5=" 0>
  <.TR TR1 1 1230 470 0 77 0 0 "lin" 1 "0" 1 "55us" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <860 290 910 290 "" 0 0 0 "">
  <620 410 790 410 "" 0 0 0 "">
  <620 390 790 390 "" 0 0 0 "">
  <620 370 790 370 "" 0 0 0 "">
  <620 350 790 350 "" 0 0 0 "">
  <620 330 790 330 "nAG1" 640 310 28 "">
  <620 310 790 310 "nAG0" 640 290 28 "">
  <1070 350 1070 350 "nDZ" 1080 290 0 "">
  <1190 350 1190 350 "nAZ" 1190 300 0 "">
</Wires>
<Diagrams>
  <Rect 130 740 1262 61 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.5 5 5.5 1 -1 2 1 315 0 225 "" "V(nAG0)" "">
	<"xyce/tran.V(NAG0)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 845 1267 49 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.5 5 5.5 1 -1 2 1 315 0 225 "" "V(nAG1)" "">
	<"xyce/tran.V(NAG1)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 130 1280 1270 161 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.495288 2 6 1 -1 1 1 315 0 225 "" "V(nAZ)" "">
	<"xyce/tran.V(NAZ)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 120 1050 1283 149 3 #c0c0c0 1 00 1 0 0.0001 0.0006 1 -0.0990787 0.5 1.08911 1 -1 1 1 315 0 225 "" "V(nDZ)" "">
	<"xyce/tran.V(NDZ)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Line 40 780 0 190 #000000 3 1>
  <Line 1480 790 0 190 #000000 3 1>
  <Line 575 65 190 0 #000000 3 1>
  <Line 665 1435 190 0 #000000 3 1>
</Paintings>
