<Qucs Schematic 0.0.17>
<Properties>
  <View=410,140,1700,898,0.758575,0,0>
  <Grid=10,10,1>
  <DataSet=BPF_1550_edge_cpld.dat>
  <DataDisplay=BPF_1550_edge_cpld.dpl>
  <OpenDisplay=1>
  <Script=BPF_1550_edge_cpld.m>
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
  <GND * 1 870 790 0 0 0 0>
  <MOPEN MS6 5 690 460 15 -12 0 1 "Subst1" 0 "W1" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MCOUPLED MS1 5 660 520 -30 -117 0 0 "Subst1" 0 "W1" 1 "L1" 1 "S1" 1 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
  <MOPEN MS13 5 630 580 -15 -26 0 3 "Subst1" 0 "W1" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS14 5 870 640 15 -12 0 1 "Subst1" 0 "W1" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS7 5 810 760 -15 -26 0 3 "Subst1" 0 "W1" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MCOUPLED MS4 5 840 690 -26 37 0 0 "Subst1" 0 "W1" 0 "L1" 0 "S1" 0 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
  <MOPEN MS9 5 750 520 15 -12 0 1 "Subst1" 0 "W2" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS11 5 690 640 -15 -26 0 3 "Subst1" 0 "W2" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS10 5 810 580 15 -12 0 1 "Subst1" 0 "W2" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MOPEN MS12 5 750 700 -15 -26 0 3 "Subst1" 0 "W2" 0 "Hammerstad" 0 "Kirschning" 0 "Kirschning" 0>
  <MCOUPLED MS2 5 720 580 -63 79 0 0 "Subst1" 0 "W2" 1 "L2" 1 "S2" 1 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
  <MCOUPLED MS3 5 780 640 -26 37 0 0 "Subst1" 0 "W2" 0 "L2" 0 "S2" 0 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
  <Pac P1 5 870 760 19 -14 0 1 "1" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 5 470 520 25 20 0 1 "2" 0 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 470 550 0 0 0 0>
  <.SP SP1 1 470 740 0 69 0 0 "lin" 0 "1 GHz" 1 "3 GHz" 1 "201" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.SP SP2 0 640 740 0 69 0 0 "lin" 0 "1.5 GHz" 1 "1.6 GHz" 1 "101" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 930 480 -30 16 0 0 "dB_S11=dB(S[1,1])" 1 "Phase=phase(S[2,1])" 1 "Phase_Delta=max(phase(S[2,1]))-min(phase(S[2,1]))" 1 "Ret_Loss_dB=dB(S[1,1])" 1 "Pass_Band=abs(max(dB(S[1,1]),1.5e9:1.6e9))" 1 "Insertion_Loss_dB=dB(S[2,1])" 1 "yes" 0>
  <SUBST Subst1 5 800 470 -30 24 0 0 "3.22" 1 "1.6e-3" 1 "1.78e-5" 1 "2e-4" 0 "0.022e-6" 0 "0.15e-6" 0>
  <.Opt Opt2 0 470 670 0 42 0 0 "Sim=SP1" 0 "DE=3|50|2|20|0.85|1|3|1e-6|10|100" 0 "Var=L3|yes|-1.317074E+000|-2|-1e-1|LIN_DOUBLE" 0 "Goal=Phase_Delta|MIN|5" 0>
  <TLIN Line1 1 550 470 -23 -64 0 0 "50 Ohm" 1 "-1.317" 1 "0 dB" 0 "26.85" 0>
  <.Opt Opt1 0 470 600 0 42 0 0 "Sim=SP1" 0 "DE=3|50|2|20|0.85|1|3|1e-6|10|100" 0 "Var=W2|yes|2.479421E-003|1E-3|4E-3|LIN_DOUBLE" 0 "Var=L2|yes|3.021602E-002|2E-2|4E-2|LIN_DOUBLE" 0 "Var=S2|yes|1.309740E-003|8E-4|3E-3|LIN_DOUBLE" 0 "Var=S1|yes|3.823353E-004|2.54e-4|9e-4|LIN_DOUBLE" 0 "Var=W1|yes|1.705437E-003|1E-3|3E-3|LIN_DOUBLE" 0 "Var=L1|yes|3.063855E-002|2E-2|4E-2|LIN_DOUBLE" 0 "Goal=Pass_Band|GE|30" 0>
  <Eqn Eqn2 1 930 660 -30 16 0 0 "W1=1.705e-3" 1 "W2=2.479e-3" 1 "L1=3.064e-2" 1 "L2=3.022e-2" 1 "S1=3.823e-4" 1 "S2=1.31e-3" 1 "yes" 0>
</Components>
<Wires>
  <810 720 810 730 "" 0 0 0 "">
  <870 720 870 730 "" 0 0 0 "">
  <870 660 870 670 "" 0 0 0 "">
  <810 660 810 670 "" 0 0 0 "">
  <630 470 630 490 "" 0 0 0 "">
  <580 470 630 470 "" 0 0 0 "">
  <470 470 470 490 "" 0 0 0 "">
  <470 470 520 470 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 1370 407 320 237 3 #c0c0c0 1 00 0 1e+09 5e+08 2.5e+09 0 -50 10 0.1 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"Ret_Loss_dB" #0000ff 0 3 0 0 0>
	<"Insertion_Loss_dB" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 1370 710 323 220 3 #c0c0c0 1 00 0 1.5e+09 5e+07 1.6e+09 1 -1 1 1 1 -1 1 1 315 0 225 "" "" "">
	<"Phase" #0000ff 0 3 0 0 0>
  </Rect>
  <Tab 1450 830 177 49 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 "" "" "">
	<"Phase_Delta" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 450 180 12 #000000 0 "1550 MHz microstrip bandpass filter.  The circuit is symmetrical so identifiers are shown for only half of the circuit,\nremainder are hidden.  Two sets of simulations/optimizations are included, one for fillter dimensions and the second\nfor determining the deviation from linear phase.  (Line 1 is the variable for linear phase, it is a negative lgth of 50\nohm line for normalizing phase deviation.\n\nTo optimize filter dimensions, disable the following:  Opt2, SP2 and Eqn2.  Set the length, L, of Line1 to any positive\ntemporarily until dimensional optimization is complete, ".01" for example.\n\nIf optimization is satisfactory, revise Eqn2 by replacing the variable values with the optimized values.  To evaluate\nphase linearity, disable Opt1, SP1 and enable Opt2, SP2 and Eqn2.  Replace the placeholder value for "L" in Line1 with\nthe variable "L3".  Optimization will normalize the phase shift through the filter and display deviation from linearity.">
</Paintings>
