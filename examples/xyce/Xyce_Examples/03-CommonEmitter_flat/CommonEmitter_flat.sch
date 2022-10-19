<Qucs Schematic 0.0.19>
<Properties>
  <View=-74,-54,1439,787,1,0,44>
  <Grid=10,10,1>
  <DataSet=CommonEmitter_flat.dat>
  <DataDisplay=CommonEmitter_flat.dpl>
  <OpenDisplay=0>
  <Script=CommonEmitter_flat.m>
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
  <Vac V1 1 80 310 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 80 340 0 0 0 0>
  <C C2 1 540 250 3 -41 0 0 "10uF" 1 "" 0 "neutral" 0>
  <C C1 1 190 260 -26 17 0 0 "10uF" 1 "" 0 "neutral" 0>
  <GND * 1 260 370 0 0 0 0>
  <GND * 1 480 420 0 0 0 0>
  <GND * 1 660 320 0 0 0 0>
  <Vdc V2 1 110 590 18 -26 0 1 "1 V" 1>
  <GND * 1 110 620 0 0 0 0>
  <NPN_SPICE Q1 1 480 300 6 -31 0 0 "2N2222" 1 "" 0 "" 0 "" 0 "" 0>
  <SpiceModel 2N2222 1 360 580 -30 16 0 0 ".MODEL 2N2222 NPN" 1 "+ Is=14.34f Xti=3 Eg=1.11 Vaf=74.03 Bf=255.9 Ne=1.307 Ise=14.34f" 0 "+ Ikf=.2847 Xtb=1.5 Br=6.092 Nc=2 Isc=0 Ikr=0 Rc=1 Cjc=7.306p" 0 "+ Mjc=.3416 Vjc=.75 Fc=.5 Cje=22.01p Mje=.377 Vje=.75 Tr=46.91n" 0 "Line_5=+ Tf=411.1p Itf=.6 Vtf=1.7 Xtf=3 Rb=10" 0>
  <R R1 1 260 190 15 -26 0 1 "39K Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 260 340 15 -26 0 1 "6.8k Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 480 390 15 -26 0 1 "270 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 480 170 15 -26 0 1 "1.5k Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R5 1 660 290 14 -25 0 3 "1M Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.XYCESCR XYCESCR1 1 650 560 0 51 0 0 "\n.AC OCT 10 .01 1e5\n.PRINT AC format=raw file=ac.txt Vdb(Ve) Vp(Ve) Vdb(out) Vp(out)" 1 "" 0 "ac.txt" 0>
</Components>
<Wires>
  <80 260 80 280 "" 0 0 0 "">
  <80 260 160 260 "" 0 0 0 "">
  <220 260 260 260 "" 0 0 0 "">
  <260 220 260 260 "" 0 0 0 "">
  <260 260 260 300 "" 0 0 0 "">
  <260 140 480 140 "" 0 0 0 "">
  <260 140 260 160 "" 0 0 0 "">
  <480 200 480 250 "" 0 0 0 "">
  <260 300 260 310 "" 0 0 0 "">
  <260 300 450 300 "Vb" 450 270 161 "">
  <480 330 480 360 "" 0 0 0 "">
  <480 250 480 270 "" 0 0 0 "">
  <480 250 510 250 "" 0 0 0 "">
  <570 250 660 250 "" 0 0 0 "">
  <660 250 660 260 "" 0 0 0 "">
  <110 560 110 560 "Vcc" 140 530 0 "">
  <480 360 480 360 "Ve" 510 330 0 "">
  <480 140 480 140 "Vcc" 510 110 0 "">
  <480 250 480 250 "Vc" 510 220 0 "">
  <660 250 660 250 "Out" 690 220 0 "">
</Wires>
<Diagrams>
  <Rect 800 170 240 160 3 #c0c0c0 1 10 1 0 20000 100000 1 -238.527 100 -31.0363 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/ac.VDB(VE)" #0000ff 0 3 0 0 0>
	<"xyce/ac.VDB(OUT)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 800 420 240 160 3 #c0c0c0 1 10 1 0 20000 100000 1 -5 5 5 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/ac.VP(VE)" #0000ff 0 3 0 0 0>
	<"xyce/ac.VP(OUT)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -60 0 12 #000000 0 "This is a basic BJT common emitter amplifier.  Its primary purpose\nas a demo is to serve as a basis for a subcircuited version in a separate\ndirectory.\n">
</Paintings>
