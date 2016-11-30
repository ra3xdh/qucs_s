<Qucs Schematic 0.0.19>
<Properties>
  <View=-336,-430,1160,1500,1,0,900>
  <Grid=10,10,1>
  <DataSet=testCombLogic1.dat>
  <DataDisplay=testCombLogic1.dpl>
  <OpenDisplay=1>
  <Script=testCombLogic1.m>
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
  <SpLib X1 1 440 180 -64 -212 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "AND4" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X2 1 440 310 -52 66 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "AND4" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X5 1 260 200 -59 -213 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "D2ABRIDGEX4" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X6 1 260 330 -74 35 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "D2ABRIDGEX4" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X3 1 590 260 -29 -164 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "OR2" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X4 1 730 290 -72 49 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "D2ABRIDGE" 1 "auto" 1 "ScaleFactor=5" 1>
  <GND * 1 790 290 0 0 0 0>
  <R_SPICE R1 1 790 260 15 -26 0 1 "47k" 1 "" 0 "" 0 "" 0 "" 0>
  <SpLib X7 1 0 180 -186 -35 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "LOGIC1" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X9 1 100 140 -286 -113 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "PATGENX4" 1 "auto" 1 "PulseFreq=1000 ScaleFactor=5" 1>
  <SpLib X8 1 0 310 -187 -35 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "LOGIC1" 1 "auto" 1 "ScaleFactor=5" 1>
  <SpLib X10 1 100 270 -289 105 0 0 "/home/mike/S4QC30SEPT2016/work/share/qucs/library/XyceDigital.lib" 0 "PATGENX4" 1 "auto" 1 "PulseFreq=5000 ScaleFactor=5" 1>
  <SpiceModel SpiceModel1 1 -130 -160 -29 17 0 0 ".model DMOD DIG ( RLOAD=1000 CLOAD=1e-12 DELAY=20e-9 CLO=1e-12 CHI=1e-12" 1 "+  S0RLO=5 S0RHI=5 S0TSW=5e-9 S0VLO=-1 S0VHI=0.16 S1RLO=200 S1RHI=5 S1TSW=5e-9 " 1 "+  S1VLO=0.52 S1VHI=1 )" 1 "" 0 "Line_5=" 0>
  <.TR TR1 1 700 -70 0 84 0 0 "lin" 1 "0" 1 "1 ms" 1 "101" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <320 200 380 200 "nD3" 320 180 25 "">
  <320 180 380 180 "nD2" 320 160 21 "">
  <320 160 380 160 "nD1" 320 140 20 "">
  <320 140 380 140 "nD0" 320 120 21 "">
  <320 330 380 330 "nD7" 330 310 38 "">
  <320 310 380 310 "nD6" 330 290 35 "">
  <320 290 380 290 "nD5" 330 270 32 "">
  <320 270 380 270 "nD4" 330 250 33 "">
  <630 230 670 230 "" 0 0 0 "">
  <500 240 500 300 "" 0 0 0 "">
  <500 240 550 240 "" 0 0 0 "">
  <500 170 500 220 "" 0 0 0 "">
  <500 220 550 220 "" 0 0 0 "">
  <40 200 200 200 "" 0 0 0 "">
  <140 180 200 180 "" 0 0 0 "">
  <140 160 200 160 "" 0 0 0 "">
  <140 140 200 140 "" 0 0 0 "">
  <40 330 200 330 "nA7" 170 310 133 "">
  <140 310 200 310 "nA6" 170 290 34 "">
  <140 290 200 290 "nA5" 170 270 33 "">
  <140 270 200 270 "nA4" 170 250 35 "">
  <500 170 500 170 "nDI1" 490 120 0 "">
  <500 300 500 300 "nDI2" 510 310 0 "">
  <200 140 200 140 "nA0" 160 120 0 "">
  <200 160 200 160 "nA1" 160 140 0 "">
  <200 180 200 180 "nA2" 160 160 0 "">
  <200 200 200 200 "nA3" 160 180 0 "">
  <630 230 630 230 "nDout" 630 190 0 "">
  <790 230 790 230 "nAout" 780 190 0 "">
</Wires>
<Diagrams>
  <Time -190 839 1248 350 3 #c0c0c0 1 00 1 1678 1 11 1 0 1 1 1 0 1 2006 315 0 225 "" "" "">
	<"xyce/tran.V(NA0)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(NA1)" #ff0000 0 3 0 0 0>
	<"xyce/tran.V(NA2)" #ff00ff 0 3 0 0 0>
	<"xyce/tran.V(NA3)" #00ff00 0 3 0 0 0>
	<"xyce/tran.V(NA4)" #00ffff 0 3 0 0 0>
	<"xyce/tran.V(NA5)" #ffff00 0 3 0 0 0>
	<"xyce/tran.V(NA6)" #777777 0 3 0 0 0>
	<"xyce/tran.V(NA7)" #000000 0 3 0 0 0>
	<"xyce/tran.V(NDI1)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(NDI2)" #ff0000 0 3 0 0 0>
	<"xyce/tran.V(NDOUT)" #ff00ff 0 3 0 0 0>
	<"xyce/tran.V(NAOUT)" #00ff00 0 3 0 0 0>
  </Time>
  <Rect -200 982 1248 82 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/tran.V(NDOUT)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect -200 1146 1249 86 3 #c0c0c0 1 00 1 0 0.0002 0.001 1 -0.522068 2 5.23788 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/tran.V(NAOUT)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Line 1120 590 0 250 #000000 3 1>
  <Line 215 -285 250 0 #000000 3 1>
  <Line 285 1355 250 0 #000000 3 1>
  <Line -330 650 0 250 #000000 3 1>
</Paintings>
