<Qucs Schematic 0.0.19>
<Properties>
  <View=-74,-77,1545,787,1,0,43>
  <Grid=10,10,1>
  <DataSet=CommonEmitter_Driver.dat>
  <DataDisplay=CommonEmitter_Driver.dpl>
  <OpenDisplay=0>
  <Script=CommonEmitter_Driver.m>
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
  <GND * 1 660 320 0 0 0 0>
  <Vdc V2 1 110 590 18 -26 0 1 "1 V" 1>
  <GND * 1 110 620 0 0 0 0>
  <SpiceModel 2N2222 1 360 580 -30 16 0 0 ".MODEL 2N2222 NPN" 1 "+ Is=14.34f Xti=3 Eg=1.11 Vaf=74.03 Bf=255.9 Ne=1.307 Ise=14.34f" 0 "+ Ikf=.2847 Xtb=1.5 Br=6.092 Nc=2 Isc=0 Ikr=0 Rc=1 Cjc=7.306p" 0 "+ Mjc=.3416 Vjc=.75 Fc=.5 Cje=22.01p Mje=.377 Vje=.75 Tr=46.91n" 0 "Line_5=+ Tf=411.1p Itf=.6 Vtf=1.7 Xtf=3 Rb=10" 0>
  <GND * 1 470 420 0 0 0 0>
  <R R5 1 660 250 14 -25 0 3 "1M Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Sub SUB1 1 440 250 -28 50 0 0 "CommonEmitter_subcircuited.sch" 0>
  <.XYCESCR XYCESCR1 1 640 560 0 51 0 0 "\n.AC OCT 10 .01 1e5\n.PRINT AC format=raw file=ac.txt Vdb(XSUB1:Ve) Vp(XSUB1:Ve) Vdb(out) Vp(out)" 1 "" 0 "ac.txt" 0>
</Components>
<Wires>
  <80 260 80 280 "" 0 0 0 "">
  <80 260 170 260 "" 0 0 0 "">
  <660 280 660 320 "" 0 0 0 "">
  <470 280 470 420 "" 0 0 0 "">
  <350 280 410 280 "" 0 0 0 "">
  <470 220 660 220 "" 0 0 0 "">
  <170 220 170 260 "" 0 0 0 "">
  <170 220 410 220 "" 0 0 0 "">
  <110 560 110 560 "Vcc" 140 530 0 "">
  <660 220 660 220 "Out" 690 190 0 "">
  <350 280 350 280 "Vcc" 380 250 0 "">
</Wires>
<Diagrams>
  <Rect 800 170 240 160 3 #c0c0c0 1 10 1 0.01 1 100000 1 -238.527 100 -31.0363 1 -1 1 1 315 0 225 "Frequency (Hz)" "" "">
	<"xyce/ac.VDB(OUT)" #ff0000 0 3 0 0 0>
	<"xyce/ac.VDB(XSUB1_VE)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 800 420 240 160 3 #c0c0c0 1 10 1 0.01 1 100000 1 -5 5 5 1 -1 1 1 315 0 225 "Frequency (Hz)" "" "">
	<"xyce/ac.VP(OUT)" #aa0000 0 3 0 0 0>
	<"xyce/ac.VP(XSUB1_VE)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -30 -10 12 #000000 0 "This circuit demonstrates the use of subcircuits.\nThe "SUB1" symbol references the "CommonEmitter_subcircuited" \nschematic.  The number of pins appearing on the symbol matches\nthe number of "subcircuit pins" present in the referenced schematic.\n\nYou can view the subcircuit schematic just by clicking on the SUB1 symbol, \nthen chosing "Go into subcircuit" either from the right-click menu\nor the tool bar (it looks like a blue arrow pointing downward).  You \ncan also do this by typing Control-I while the SUB1 symbol is selected.">
</Paintings>
