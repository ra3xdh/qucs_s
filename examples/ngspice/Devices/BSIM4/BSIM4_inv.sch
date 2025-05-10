<Qucs Schematic 0.0.24>
<Properties>
  <View=125,-42,737,552,1,0,0>
  <Grid=10,10,1>
  <DataSet=BSIM4_inv.dat>
  <DataDisplay=BSIM4_inv.dpl>
  <OpenDisplay=0>
  <Script=BSIM4_inv.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID 40 44 SUB>
  <Line 20 -40 0 -20 #000080 2 1>
  <Line 20 60 0 -20 #000080 2 1>
  <.PortSym 20 60 4 0>
  <.PortSym 20 -60 2 0>
  <Line -40 0 20 0 #000080 2 1>
  <.PortSym -40 0 1 0>
  <Line -20 -40 0 80 #000080 2 1>
  <Line 60 -40 0 80 #000080 2 1>
  <Line -20 -40 80 0 #000080 2 1>
  <Line -20 40 80 0 #000080 2 1>
  <Line 60 0 20 0 #000080 2 1>
  <.PortSym 80 0 3 180>
  <Line 45 0 -50 30 #000000 2 1>
  <Line 45 0 -50 -30 #000000 2 1>
  <Line -5 -30 0 60 #000000 2 1>
  <Ellipse 45 -5 10 10 #000000 1 1 #c0c0c0 1 1>
</Symbol>
<Components>
  <Port P3 1 430 190 30 -12 1 2 "3" 1 "analog" 0 "v" 0 "" 0>
  <Port P1 1 220 190 -55 -9 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P4 1 350 320 -9 28 1 1 "4" 1 "analog" 0 "v" 0 "" 0>
  <Port P2 1 350 70 -8 -72 0 3 "2" 1 "analog" 0 "v" 0 "" 0>
  <PMOS_SPICE M2 1 350 130 55 -19 1 0 "P1 w=4.3u l=0.35u m=1 pdbcp=0.5u" 1 "" 0 "" 0 "" 0 "" 0>
  <SpiceInclude SpiceInclude1 1 220 290 -34 16 0 0 "nmos4p0.mod" 1 "pmos4p0.mod" 1 "" 0 "" 0 "" 0>
  <NMOS_SPICE M3 1 350 250 57 0 0 0 "N1 w=4.3u l=0.35u m=1 pdbcp=0.5u" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <350 190 430 190 "" 0 0 0 "">
  <220 190 280 190 "" 0 0 0 "">
  <350 280 350 310 "" 0 0 0 "">
  <280 190 280 250 "" 0 0 0 "">
  <280 250 320 250 "" 0 0 0 "">
  <350 190 350 220 "" 0 0 0 "">
  <280 130 280 190 "" 0 0 0 "">
  <280 130 320 130 "" 0 0 0 "">
  <350 160 350 190 "" 0 0 0 "">
  <350 70 350 100 "" 0 0 0 "">
  <370 130 400 130 "" 0 0 0 "">
  <400 70 400 130 "" 0 0 0 "">
  <350 70 400 70 "" 0 0 0 "">
  <350 310 350 320 "" 0 0 0 "">
  <350 310 400 310 "" 0 0 0 "">
  <370 250 400 250 "" 0 0 0 "">
  <400 250 400 310 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
