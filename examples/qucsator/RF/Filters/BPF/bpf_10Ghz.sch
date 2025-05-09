<Qucs Schematic 0.0.4>
<Properties>
  <View=216,678,1288,1281,0.722015,0,0>
  <Grid=10,10,1>
  <DataSet=bpf_10GHz.dat>
  <DataDisplay=bpf_10GHz.dpl>
  <OpenDisplay=1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <MCOUPLED MS1 1 410 840 -26 37 0 0 "Subst1" 1 "384um" 1 "5.24mm" 1 "482um" 1 "Kirschning" 0 "Kirschning" 0>
  <MCOUPLED MS2 1 590 900 -26 37 0 0 "Subst1" 1 "1.08mm" 1 "4.72mm" 1 "712um" 1 "Kirschning" 0 "Kirschning" 0>
  <MSTEP MS17 1 690 930 -30 -84 1 0 "Subst1" 1 "1.08mm" 1 "1.3mm" 1 "Hammerstad" 0 "Kirschning" 0>
  <MSTEP MS16 1 490 870 -15 -86 0 2 "Subst1" 1 "1.08mm" 1 "384um" 1 "Hammerstad" 0 "Kirschning" 0>
  <MCOUPLED MS3 1 750 960 -26 37 0 0 "Subst1" 1 "1.3m" 1 "5.03mm" 1 "913um" 1 "Kirschning" 0 "Kirschning" 0>
  <MSTEP MS18 1 830 990 -24 -84 0 2 "Subst1" 1 "1.08mm" 1 "1.3mm" 1 "Hammerstad" 0 "Kirschning" 0>
  <MOPEN MS13 1 660 990 -21 15 1 2 "Subst1" 1 "1.3mm" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS14 1 820 1050 -21 15 1 2 "Subst1" 1 "1.08 mm" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MCOUPLED MS4 1 920 1020 -26 37 0 0 "Subst1" 1 "1.08mm" 1 "4.72mm" 1 "712um" 1 "Kirschning" 0 "Kirschning" 0>
  <MSTEP MS19 1 1000 1050 -23 -85 1 0 "Subst1" 1 "1.08mm" 1 "384um" 1 "Hammerstad" 0 "Kirschning" 0>
  <MCOUPLED MS5 1 1090 1080 -26 37 0 0 "Subst1" 1 "384um" 1 "5.24mm" 1 "482um" 1 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS9 1 1150 1050 -27 -64 1 0 "Subst1" 1 "384 um" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <Pac P2 1 1170 1140 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "10 GHz" 0>
  <GND * 1 1170 1170 0 0 0 0>
  <Pac P1 1 270 820 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "10 GHz" 0>
  <GND * 1 270 850 0 0 0 0>
  <SUBST Subst1 1 340 1030 -30 24 0 0 "2.56" 1 "0.76 mm" 1 "18 um" 1 "3e-3" 1 "0.022e-6" 1 "100" 1>
  <Eqn Eqn1 1 470 1130 -31 15 0 0 "dB_S21=dB(S[2,1])" 1 "dB_S11=dB(S[1,1])" 1 "yes" 0>
  <MOPEN MS11 1 310 870 -21 15 1 2 "Subst1" 1 "384 um" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS12 1 490 930 -21 15 1 2 "Subst1" 1 "1.08 mm" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS15 1 990 1110 -21 15 1 2 "Subst1" 1 "384 um" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS10 1 470 770 -26 -62 1 0 "Subst1" 1 "384 um" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS6 1 650 830 -26 -66 1 0 "Subst1" 1 "1.08 mm" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS7 1 810 890 -27 -64 1 0 "Subst1" 1 "1.3mm" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS8 1 980 950 -28 -65 1 0 "Subst1" 1 "1.08 mm" 1 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <.SP SP1 1 590 1120 0 48 0 0 "lin" 1 "5 GHz" 1 "15 GHz" 1 "150" 1 "no" 0 "1" 0 "2" 0>
</Components>
<Wires>
  <520 870 560 870 "" 0 0 0>
  <620 930 660 930 "" 0 0 0>
  <780 990 800 990 "" 0 0 0>
  <690 990 720 990 "" 0 0 0>
  <440 870 460 870 "" 0 0 0>
  <950 1050 970 1050 "" 0 0 0>
  <1030 1050 1060 1050 "" 0 0 0>
  <1120 1110 1170 1110 "" 0 0 0>
  <860 990 890 990 "" 0 0 0>
  <850 1050 890 1050 "" 0 0 0>
  <270 790 380 790 "" 0 0 0>
  <380 790 380 810 "" 0 0 0>
  <340 870 380 870 "" 0 0 0>
  <520 930 560 930 "" 0 0 0>
  <1020 1110 1060 1110 "" 0 0 0>
  <440 770 440 810 "" 0 0 0>
  <620 830 620 870 "" 0 0 0>
  <780 890 780 930 "" 0 0 0>
  <950 950 950 990 "" 0 0 0>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 720 1170 16 #0000ff 0 "10GHz band pass filter\nCreated by Toyoyuki ISHIKAWA">
</Paintings>
