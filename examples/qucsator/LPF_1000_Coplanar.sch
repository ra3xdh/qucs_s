<Qucs Schematic 0.0.17>
<Properties>
  <View=314,129,1719,727,0.76201,0,0>
  <Grid=10,10,1>
  <DataSet=LPF_1000_Coplanar.dat>
  <DataDisplay=LPF_1000_Coplanar.dpl>
  <OpenDisplay=1>
  <Script=LPF_1000_Coplanar.m>
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
  <CLIN CL1 5 700 370 -26 28 0 0 "Subst1" 0 "10 mil" 1 "S1" 1 "L3" 1 "Air" 1 "yes" 0>
  <MCROSS MS23 5 550 370 -173 84 0 0 "Subst1" 0 "W1" 0 "10 mil" 0 "10 mil" 0 "10 mil" 0 "Hammerstad" 0 "Kirschning" 0 "showNumbers" 0>
  <MMBEND MS28 5 550 250 -26 -81 1 1 "Subst1" 0 "10 mil" 0>
  <MCROSS MS36 5 860 370 -173 84 1 2 "Subst1" 0 "W1" 0 "10 mil" 0 "10 mil" 0 "10 mil" 0 "Hammerstad" 0 "Kirschning" 0 "showNumbers" 0>
  <MMBEND MS41 5 860 250 -7 -81 0 1 "Subst1" 0 "10 mil" 0>
  <MLIN MS3 5 430 250 -20 -61 0 2 "Subst1" 0 "150 mil" 1 "L2" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MSTEP MS17 5 490 250 -26 17 0 0 "Subst1" 0 "150 mil" 0 "10 mil" 0 "Hammerstad" 0 "Kirschning" 0>
  <MOPEN MS4 5 370 250 -12 -15 0 2 "Subst1" 0 "150 mil" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS39 5 1040 250 -26 -15 1 0 "Subst1" 0 "150 mil" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MLIN MS40 5 980 250 -20 -61 1 0 "Subst1" 0 "150 mil" 0 "L2" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MSTEP MS38 5 920 250 -26 17 1 2 "Subst1" 0 "150 mil" 0 "10 mil" 0 "Hammerstad" 0 "Kirschning" 0>
  <MLIN MS19 5 550 430 15 -26 0 1 "Subst1" 0 "10 mil" 0 "L4" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS37 5 860 310 -15 -26 1 1 "Subst1" 0 "10 mil" 0 "L4" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS34 5 860 430 -15 -26 1 1 "Subst1" 0 "10 mil" 0 "L4" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <Eqn Eqn1 1 930 280 -30 16 0 0 "RL_in=dB(S[1,1])" 1 "Loss=dB(S[2,1])" 1 "yes" 0>
  <Eqn Eqn3 1 1120 430 -30 16 0 0 "S1=3.847e-3" 1 "L4=1.614e-3" 1 "W1=2.562e-4" 1 "L1=7e-3" 1 "L2=4.584e-3" 1 "L3=9.75e-3" 1 "yes" 0>
  <.SP SP1 1 1090 210 0 69 0 0 "lin" 0 "100 MHz" 1 "6 GHz" 1 "60" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.Opt Opt1 0 1090 350 0 42 0 0 "Sim=SP1" 0 "DE=3|50|2|30|0.85|1|3|1e-6|10|100" 0 "Var=L2|yes|4.583992E-003|3E-3|6E-3|LIN_DOUBLE" 0 "Var=W1|yes|2.562316E-004|2.5E-4|5E-4|LIN_DOUBLE" 0 "Var=L1|yes|7.006797E-003|5E-3|10E-3|LIN_DOUBLE" 0 "Var=L3|yes|9.750342E-003|7E-3|1.6E-2|LIN_DOUBLE" 0 "Var=S1|yes|3.847314E-003|2E-3|5E-3|LIN_DOUBLE" 0 "Var=L4|yes|1.614098E-003|8e-4|2e-3|LIN_DOUBLE" 0 "Goal=Pass_band|GE|30" 0 "Goal=Stop_band|GE|50" 0>
  <MMBEND MS44 5 860 520 -7 81 1 3 "Subst1" 0 "10 mil" 0>
  <MSTEP MS42 5 920 520 -26 17 1 2 "Subst1" 0 "150 mil" 0 "10 mil" 0 "Hammerstad" 0 "Kirschning" 0>
  <MLIN MS45 5 980 520 -20 -61 1 0 "Subst1" 0 "150 mil" 0 "L2" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MOPEN MS43 5 1040 520 -26 -15 1 0 "Subst1" 0 "150 mil" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MMBEND MS33 5 550 520 -26 81 0 3 "Subst1" 0 "10 mil" 0>
  <MOPEN MS31 5 370 520 -12 -15 0 2 "Subst1" 0 "150 mil" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MLIN MS32 5 430 520 -20 -61 0 2 "Subst1" 0 "150 mil" 0 "L2" 0 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MSTEP MS30 5 490 520 -26 17 0 0 "Subst1" 0 "150 mil" 0 "10 mil" 0 "Hammerstad" 0 "Kirschning" 0>
  <MLIN MS15 5 550 310 15 -26 0 1 "Subst1" 0 "10 mil" 1 "L4" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <Eqn Eqn2 1 610 520 -30 16 0 0 "Pass_band=abs(max(dB(S[1,1]),100e6:1.2e9))" 1 "Stop_band=abs(max(dB(S[2,1]),3e9:6e9))" 1 "yes" 0>
  <Pac P1 5 400 420 18 -9 0 1 "1" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 400 450 0 0 0 0>
  <Pac P2 5 1010 420 -18 -9 1 1 "2" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 1010 450 0 0 1 2>
  <SUBST Subst1 5 700 260 -30 24 0 0 "4.4" 1 "31 mil" 1 "0.7 mil" 1 "2e-4" 0 "0.022e-6" 0 "0.15e-6" 0>
  <MLIN MS35 5 950 370 -36 15 1 2 "Subst1" 0 "W1" 1 "L1" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS1 5 460 370 -36 15 0 0 "Subst1" 0 "W1" 1 "L1" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
</Components>
<Wires>
  <730 370 830 370 "" 0 0 0 "">
  <580 370 670 370 "" 0 0 0 "">
  <860 460 860 490 "" 0 0 0 "">
  <550 460 550 490 "" 0 0 0 "">
  <890 370 920 370 "" 0 0 0 "">
  <1010 370 1010 390 "" 0 0 0 "">
  <980 370 1010 370 "" 0 0 0 "">
  <490 370 520 370 "" 0 0 0 "">
  <400 370 400 390 "" 0 0 0 "">
  <400 370 430 370 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 1310 555 305 355 3 #c0c0c0 1 00 0 0 2e+09 6e+09 0 -50 10 0.1 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"RL_in" #0000ff 0 3 0 0 0>
	<"Loss" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 550 180 12 #000000 0 "To optimize, DISABLE Eqn3 and ENABLE Opt1.">
  <Text 370 610 12 #ff0000 0 "Microstrip low pass filter with coplanar transmission line connecting the input and output shunt sections of the filter.  The coplanar structure is obtained by etching away\nthe ground plane underneath the center transmission line, leaving a cavity above and below that area.  The reason is to achieve much higher impedance than possible\nwith reasonable dimensions in microstrip.  Layout of this circuit should be folded as shown to minimize coupling between the two sets of shunt arms.  The circuit is sym-\nmetrical so identifiers are shown for only one-half of the circuit, the rest are hidden.  This circuit has been validated by "Sonnet" as performing close to the QUCS prediction.">
</Paintings>
