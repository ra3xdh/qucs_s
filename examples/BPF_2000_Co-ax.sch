<Qucs Schematic 0.0.17>
<Properties>
  <View=300,-13,1576,757,0.746753,0,0>
  <Grid=10,10,1>
  <DataSet=BPF_2000_Co-ax.dat>
  <DataDisplay=BPF_2000_Co-ax.dpl>
  <OpenDisplay=1>
  <Script=BPF_2000_Co-ax.m>
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
  <GND * 1 480 150 0 0 0 2>
  <C C1 5 480 180 65 -9 0 1 "C1" 1 "" 0 "neutral" 0>
  <GND * 1 480 390 0 0 0 0>
  <GND * 1 600 150 0 0 0 2>
  <C C4 5 600 180 17 -26 0 1 "C1" 0 "" 0 "neutral" 0>
  <GND * 1 600 390 0 0 0 0>
  <C C2 5 420 270 -26 17 0 0 "C3" 1 "" 0 "neutral" 0>
  <GND * 1 360 350 0 0 0 0>
  <Pac P1 5 360 320 18 -26 0 1 "1" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <COAX Line1 5 480 240 72 -8 0 1 "1" 0 "Q1" 0 "1" 0 "1.9e-2" 0 "6e-3" 0 "L1" 1 "Q1" 0 "26.85" 0>
  <COAX Line2 5 480 300 71 -9 0 1 "1" 0 "Q1" 0 "1" 0 "1.9e-2" 0 "6e-3" 0 "L2" 1 "Q1" 0 "26.85" 0>
  <COAX Line5 5 480 360 134 -3 0 1 "1" 0 "Q1" 0 "1" 0 "1.9e-2" 0 "6e-3" 0 "L3" 1 "Q1" 0 "26.85" 0>
  <COAX Line4 5 600 240 16 -26 0 1 "1" 0 "Q1" 0 "1" 0 "1.9e-2" 0 "6e-3" 0 "L1" 0 "Q1" 0 "26.85" 0>
  <COAX Line3 5 600 300 -22 61 0 1 "1" 0 "Q1" 0 "1" 0 "1.9e-2" 0 "6e-3" 0 "L2" 0 "Q1" 0 "26.85" 0>
  <COAX Line6 5 600 360 22 -4 0 1 "1" 0 "Q1" 0 "1" 0 "1.9e-2" 0 "6e-3" 0 "L3" 0 "Q1" 0 "26.85" 0>
  <TLIN Line7 5 540 330 -21 26 0 0 "Z1" 1 "2e-2" 1 "0 dB" 0 "26.85" 0>
  <C C3 5 670 270 -26 17 0 0 "C3" 1 "" 0 "neutral" 0>
  <GND * 1 850 350 0 0 0 0>
  <Pac P2 5 850 320 18 -26 0 1 "2" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <.SP SP2 0 1010 200 0 66 0 0 "lin" 0 "1.995e9" 1 "2.005e9" 1 "101" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn2 1 380 80 -24 15 0 0 "L1=2.5e-2" 1 "Z1=162" 1 "L3=2.611e-3" 1 "L2=3.531e-3" 1 "C1=2.892e-13" 1 "C3=4.211e-13" 1 "yes" 0>
  <.Opt Opt1 0 1020 340 0 40 0 0 "Sim=SP1" 0 "DE=3|50|2|100|0.85|1|3|1e-6|10|100" 0 "Var=C3|yes|4.210918E-013|1e-13|1.2e-12|LIN_DOUBLE" 0 "Var=L2|yes|3.530568E-003|5e-4|5e-3|LIN_DOUBLE" 0 "Var=C1|yes|2.892070E-013|2e-13|8e-13|LIN_DOUBLE" 0 "Var=L3|yes|2.611280E-003|1e-3|5e-3|LIN_DOUBLE" 0 "Var=Z1|yes|1.622360E+002|150|210|LIN_DOUBLE" 0 "Goal=Loss_Hi|GE|35" 0 "Goal=Loss_Lo|GE|35" 0 "Goal=Input_Opt|GE|20" 0>
  <.Opt Opt2 0 1020 410 0 40 0 0 "Sim=SP1" 0 "DE=3|50|2|20|0.85|1|3|1e-6|10|100" 0 "Var=L4|yes|-5.241999E+000|-10|-1e-1|LIN_DOUBLE" 0 "Goal=Phase_Deviation|MIN|1" 0>
  <Eqn Eqn1 1 660 40 -24 15 0 0 "Ret_Loss=dB(S[1,1])" 1 "Phase_Deviation=max(Phase)-min(Phase)" 1 "Phase=phase(S[2,1])" 1 "Q1=1e-6" 1 "Loss_Lo=abs(max(Loss,1.9e9:1.91e9))" 1 "Loss_Hi=abs(max(Loss,2.09e9:2.1e9))" 1 "Input_Opt=abs(max(Ret_Loss, 1.995e9:2.005e9))" 1 "Loss=dB(S[2,1])" 1 "yes" 0>
  <TLIN Line8 1 770 270 -26 20 0 0 "50 Ohm" 1 "-5.242" 1 "0 dB" 0 "26.85" 0>
  <.SP SP1 1 1010 60 0 66 0 0 "lin" 0 "1.95 GHz" 1 "2.05 GHz" 1 "201" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <480 330 510 330 "" 0 0 0 "">
  <570 330 600 330 "" 0 0 0 "">
  <450 270 480 270 "" 0 0 0 "">
  <360 270 390 270 "" 0 0 0 "">
  <360 270 360 290 "" 0 0 0 "">
  <600 270 640 270 "" 0 0 0 "">
  <850 270 850 290 "" 0 0 0 "">
  <800 270 850 270 "" 0 0 0 "">
  <700 270 740 270 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 1220 210 240 160 3 #c0c0c0 1 00 0 1.95e+09 5e+07 2.05e+09 1 -28.0274 20 2.40692 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"Loss" #ff0000 0 3 0 0 0>
	<"Ret_Loss" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 1230 430 240 160 3 #c0c0c0 1 00 0 1.995e+09 1e+07 2.005e+09 1 -271.254 200 273.927 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"Phase" #0000ff 0 3 0 0 0>
  </Rect>
  <Tab 770 451 206 52 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 "" "" "">
	<"Phase_Deviation" #0000ff 0 3 0 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 340 480 12 #ff0000 0 "Two sets of simulation functions are used - SP1 and Opt1 is to optimize the cavity and coupling dimensions and SP2 and Opt2 is to normalize the phase over the\n10 MHz information passband (using a negative length of 50 ohm transmission line)  to determine the maximum deviation from linear phase (which is+/- 0.3 deg).\n\nTransmission line Z1 represents a coupling wire between tapped co-axial resonators at L=2e-2 from ground and approximately .800 inches long (distance between\nresonators).  C3 = input and output voltage probes at location defined by the intersection of L1 and L2.  C1 are tuning screws at the ends of resonators.\n\nTo optimize physical dimensions, deactivate Eqn2, Opt2, SP2 and replace the length of Line8 with any number, "0.1" will do.  SP1 and Opt1 should be activated.  If\nthe optimized dimensions are satisfactory, replace the variables in Eqn2 with them and activate Eqn2.  To evaluate phase linearity, deactivate Opt1 and SP1.  Activate\nOpt2 and SP2 then replace the length of Line8 with variable "L4".  The optimizer (Opt2) will optimize the negative length of the 50 ohm line required to normalize phase\ndeviation so that the actual value is displayed.\n\nNOTE:  optimization and phase normalization can be accomplished in a simpler manner but most applications do not require phase linearity hence separate processes.">
</Paintings>
