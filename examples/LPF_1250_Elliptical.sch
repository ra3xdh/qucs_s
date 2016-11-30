<Qucs Schematic 0.0.17>
<Properties>
  <View=596,124,1784,896,0.743523,0,0>
  <Grid=10,10,1>
  <DataSet=LPF_1250_Elliptical.dat>
  <DataDisplay=LPF_1250_Elliptical.dpl>
  <OpenDisplay=1>
  <Script=LPF_1250_Elliptical.m>
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
  <MLIN MS3 5 940 510 -26 15 0 0 "Subst1" 0 "25 mil" 1 "210 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MTEE MS4 5 1050 510 -36 -85 0 0 "Subst1" 0 "25 mil" 1 "25 mil" 1 "25 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0 "showNumbers" 0>
  <MLIN MS5 5 1150 510 -26 15 0 0 "Subst1" 0 "25 mil" 1 "520 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS1 5 720 510 -26 15 0 0 "Subst1" 0 "170 mil" 1 "200 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS6 5 1050 600 15 -26 0 1 "Subst1" 0 "25 mil" 1 "105 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MSTEP MS2 5 830 510 -26 17 0 0 "Subst1" 0 "170 mil" 1 "25 mil" 1 "Hammerstad" 0 "Kirschning" 0>
  <MSTEP MS7 5 1050 690 17 -26 0 1 "Subst1" 0 "200 mil" 1 "25 mil" 1 "Hammerstad" 0 "Kirschning" 0>
  <MLIN MS8 5 1050 780 15 -26 0 1 "Subst1" 0 "200 mil" 1 "680 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MOPEN MS9 5 1050 840 -15 -26 0 3 "Subst1" 0 "200 mil" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MLIN MS10 5 1360 510 -26 15 1 2 "Subst1" 0 "25 mil" 1 "210 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS13 5 1580 510 -26 15 1 2 "Subst1" 0 "170 mil" 1 "200 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MSTEP MS15 5 1470 510 -26 17 1 2 "Subst1" 0 "170 mil" 1 "25 mil" 1 "Hammerstad" 0 "Kirschning" 0>
  <Pac P1 5 650 570 23 5 0 1 "1" 0 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 5 1660 560 23 5 0 1 "2" 0 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 1660 610 0 0 0 0>
  <GND * 1 650 620 0 0 0 0>
  <MTEE MS11 5 1250 510 -34 29 0 2 "Subst1" 0 "25 mil" 1 "25 mil" 1 "25 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0 "showNumbers" 0>
  <MSTEP MS16 5 1250 330 29 -22 0 3 "Subst1" 0 "200 mil" 1 "25 mil" 1 "Hammerstad" 0 "Kirschning" 0>
  <MOPEN MS18 5 1250 180 15 -12 0 1 "Subst1" 0 "200 mil" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MLIN MS14 5 1250 420 24 -23 0 3 "Subst1" 0 "25 mil" 1 "510 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS17 5 1250 240 31 -22 0 3 "Subst1" 0 "200 mil" 1 "330 mil" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <Eqn Eqn1 1 1400 620 -30 16 0 0 "dBS11=dB(S[1,1])" 1 "dBS21=dB(S[2,1])" 1 "yes" 0>
  <.SP SP1 1 820 330 0 69 0 0 "lin" 0 "10 MHz" 1 "3 GHz" 1 "100" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <SUBST Subst1 5 860 210 -30 24 0 0 "2.55" 1 "62 mil" 1 "0.7 mil" 1 "2e-4" 0 "0.022e-6" 0 "0.15e-6" 0>
</Components>
<Wires>
  <750 510 800 510 "" 0 0 0 "">
  <860 510 910 510 "" 0 0 0 "">
  <970 510 1020 510 "" 0 0 0 "">
  <1080 510 1120 510 "" 0 0 0 "">
  <1050 540 1050 570 "" 0 0 0 "">
  <1050 630 1050 660 "" 0 0 0 "">
  <1050 720 1050 750 "" 0 0 0 "">
  <1500 510 1550 510 "" 0 0 0 "">
  <1390 510 1440 510 "" 0 0 0 "">
  <1180 510 1220 510 "" 0 0 0 "">
  <1660 590 1660 610 "" 0 0 0 "">
  <650 600 650 620 "" 0 0 0 "">
  <650 510 650 540 "" 0 0 0 "">
  <650 510 690 510 "" 0 0 0 "">
  <1660 510 1660 530 "" 0 0 0 "">
  <1610 510 1660 510 "" 0 0 0 "">
  <1280 510 1330 510 "" 0 0 0 "">
  <1250 450 1250 480 "" 0 0 0 "">
  <1250 360 1250 390 "" 0 0 0 "">
  <1250 270 1250 300 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 1450 449 324 289 3 #c0c0c0 1 00 0 0 1e+09 3e+09 0 -40 20 5.29694 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"dBS11" #0000ff 0 3 0 0 0>
	<"dBS21" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 1180 730 12 #ff0000 0 "1250 MHz microstrip lowpass filter with elliptic function stopband.  Note that\nproper performance of this circuit requires the two shunt elements to be on\nopposite sides, as shown, to minimize unintentional coupling.">
</Paintings>
