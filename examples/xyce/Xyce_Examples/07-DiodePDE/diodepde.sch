<Qucs Schematic 0.0.20>
<Properties>
  <View=0,0,1269,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=diodepde.dat>
  <DataDisplay=diodepde.dpl>
  <OpenDisplay=0>
  <Script=diodepde.m>
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
  <Vdc V1 1 120 180 18 -26 0 1 "1 V" 1>
  <SpiceModel SpiceModel1 1 200 320 -25 15 0 0 ".MODEL DIODE ZOD" 1 "" 0 "" 0 "" 0 "Line_5=" 0>
  <GND * 1 120 210 0 0 0 0>
  <GND * 1 500 160 0 0 0 0>
  <SPICE_dev d1 1 460 150 -26 -69 0 0 "2" 1 "ypde " 1 "DIODE" 0 "na=1.0e15 nd=1.0e15  tecplotlevel=0 gnuplotlevel=0  graded=0 l=5.0e-4 wj=0.1e-3 nx=101  area=1.0  mobmodel=carr useOldNi=true" 0>
  <.XYCESCR XYCESCR1 1 80 520 0 51 0 0 "\n.dc  V1 0.0 -0.21 -0.01\n\n.PRINT dc format=raw file=dc.txt V(n2) v(n1) I(V1)\n" 1 "" 0 "dc.txt" 0>
  <SpiceOptions SpiceOptions1 1 580 350 -32 15 0 0 "NONLIN" 0 "maxstep=50" 1 "abstol=1e-17" 1 "maxsearchstep=3" 1 "searchmethod=1" 1>
  <R R1 1 300 150 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <500 150 500 160 "" 0 0 0 "">
  <120 150 270 150 "n2" 230 120 77 "">
  <330 150 420 150 "n1" 390 120 30 "">
</Wires>
<Diagrams>
  <Rect 770 720 240 160 3 #c0c0c0 1 00 1 -0.21 0.05 0 1 -0.231 0.1 0.021 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/V(N1)" #0000ff 0 3 0 0 0>
	<"xyce/V(N2)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 770 530 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"xyce/I(V1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 560 40 12 #000000 0 "The purpose of this schematic is to demonstrate how to use Xyce\n"Y" devices in Qucs-s.  It exactly reproduces the PDE 1D DIODE/diodepde\ntest case in the Xyce test suite.\n\nThe primary point to recognize is that the "SPICE generic" device\nallows one to chose a "Letter" for the device that is actually just a string.\n\nIt then takes the "Letter" and appends the name of the device to it.\n\nTherefore one can get Qucs-s to generate correct Xyce "Y" lines by \nmaking the "Letter" be the full Y device type specification with a trailing space, \ne.g. "YPDE ".\n\nDo NOT attempt to do this by putting a leading space in the device name.  This \ncan cause qucs-s to throw an error about "Component" line format\nthat explains nothing, and can segfault qucs-s.">
</Paintings>
