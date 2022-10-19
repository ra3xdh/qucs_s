<Qucs Schematic 0.0.19>
<Properties>
  <View=-74,-36,1239,796,1,0,0>
  <Grid=10,10,1>
  <DataSet=CommonEmitter_subcircuited.dat>
  <DataDisplay=CommonEmitter_subcircuited.dpl>
  <OpenDisplay=1>
  <Script=CommonEmitter_subcircuited.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym 40 20 2 0>
  <.PortSym 40 60 1 0>
  <.PortSym 40 100 3 0>
  <.PortSym 40 140 4 0>
</Symbol>
<Components>
  <C C2 1 540 250 3 -41 0 0 "10uF" 1 "" 0 "neutral" 0>
  <C C1 1 190 260 -26 17 0 0 "10uF" 1 "" 0 "neutral" 0>
  <NPN_SPICE Q1 1 480 300 6 -31 0 0 "2N2222" 1 "" 0 "" 0 "" 0 "" 0>
  <SpiceModel 2N2222 1 360 580 -30 16 0 0 ".MODEL 2N2222 NPN" 1 "+ Is=14.34f Xti=3 Eg=1.11 Vaf=74.03 Bf=255.9 Ne=1.307 Ise=14.34f" 0 "+ Ikf=.2847 Xtb=1.5 Br=6.092 Nc=2 Isc=0 Ikr=0 Rc=1 Cjc=7.306p" 0 "+ Mjc=.3416 Vjc=.75 Fc=.5 Cje=22.01p Mje=.377 Vje=.75 Tr=46.91n" 0 "Line_5=+ Tf=411.1p Itf=.6 Vtf=1.7 Xtf=3 Rb=10" 0>
  <Port P2 1 650 250 4 -48 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <Port P1 1 120 260 -23 12 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P3 1 480 120 -66 -23 0 3 "3" 1 "analog" 0 "v" 0 "" 0>
  <Port P4 1 480 440 12 4 0 1 "4" 1 "analog" 0 "v" 0 "" 0>
  <R R1 1 260 190 15 -26 0 1 "39K Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 260 340 15 -26 0 1 "6.8k Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 480 390 15 -26 0 1 "270 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 480 170 15 -26 0 1 "1.5k Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
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
  <260 370 260 420 "" 0 0 0 "">
  <260 420 480 420 "" 0 0 0 "">
  <570 250 650 250 "" 0 0 0 "">
  <120 260 160 260 "" 0 0 0 "">
  <480 120 480 140 "" 0 0 0 "">
  <480 420 480 440 "" 0 0 0 "">
  <480 360 480 360 "Ve" 510 330 0 "">
  <480 250 480 250 "Vc" 510 220 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
