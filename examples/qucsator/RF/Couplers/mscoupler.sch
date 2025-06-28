<Qucs Schematic 25.1.2>
<Properties>
  <View=-647,-330,2008,1058,0.775194,174,57>
  <Grid=10,10,1>
  <DataSet=mscoupler.dat>
  <DataDisplay=mscoupler.dpl>
  <OpenDisplay=0>
  <Script=mscoupler.m>
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
  <Pac P1 1 -80 90 -89 -29 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <Pac P2 1 400 100 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <Pac P3 1 330 200 18 -26 0 1 "3" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <Pac P4 1 30 190 -88 -24 1 1 "4" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0 "true" 0>
  <GND * 1 -80 130 0 0 0 0>
  <GND * 1 30 220 0 0 0 0>
  <GND * 1 400 140 0 0 0 0>
  <GND * 1 330 230 0 0 0 0>
  <SUBST SubstTC1 1 320 340 -30 24 0 0 "9.8" 1 "0.635 mm" 1 "17.5 um" 1 "0.0001" 1 "2.43902e-08" 1 "1.5e-07" 1>
  <.SP SPTC1 1 -50 300 0 61 0 0 "lin" 1 "0.2 GHz" 1 "4.2 GHz" 1 "101" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <MCOUPLED MSTC1 1 170 70 -26 37 0 0 "SubstTC1" 1 "0.518 mm" 1 "14.94 mm" 1 "0.185 mm" 1 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
  <Eqn Eqn1 1 140 310 -23 14 0 0 "reflect=dB(S[1,1])" 1 "isolated=dB(S[3,1])" 1 "through=dB(S[2,1])" 1 "coupled=dB(S[4,1])" 1 "yes" 0>
</Components>
<Wires>
  <-80 20 -80 60 "" 0 0 0 "">
  <30 100 140 100 "" 0 0 0 "">
  <400 20 400 70 "" 0 0 0 "">
  <200 20 400 20 "" 0 0 0 "">
  <200 100 330 100 "" 0 0 0 "">
  <-80 120 -80 130 "" 0 0 0 "">
  <-80 20 140 20 "" 0 0 0 "">
  <140 20 140 40 "" 0 0 0 "">
  <200 20 200 40 "" 0 0 0 "">
  <400 140 400 130 "" 0 0 0 "">
  <330 100 330 170 "" 0 0 0 "">
  <30 100 30 160 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 590 355 421 285 3 #c0c0c0 1 00 1 2e+08 5e+08 4.2e+09 0 -50 5 5 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"coupled" #0000ff 1 3 0 0 0>
	<"isolated" #ff0000 1 3 0 0 0>
	<"reflect" #005500 1 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
