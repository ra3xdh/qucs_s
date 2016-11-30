<Qucs Schematic 0.0.19>
<Properties>
  <View=-333,441,1660,1360,1,0,0>
  <Grid=10,10,1>
  <DataSet=Test_StoY_SPICEOPUS.dat>
  <DataDisplay=Test_StoY_SPICEOPUS.dpl>
  <OpenDisplay=1>
  <Script=Test_StoY_SPICEOPUS.m>
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
  <Sub S11_Probe1 1 270 530 -20 43 0 0 "S11_Probe.sch" 0 "50" 1>
  <Sub S21_Probe1 1 790 630 -60 -66 0 0 "S21_Probe.sch" 0 "50" 1>
  <GND * 1 760 740 0 0 0 0>
  <S4Q_V V1 1 140 690 18 -26 0 1 "dc 0 ac 1" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 140 720 0 0 0 0>
  <R_SPICE R4 1 760 1100 -34 18 1 0 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <Vac U1 1 790 1160 31 -40 1 1 "1 V" 1 "1 kHz" 1 "0" 0 "0" 0>
  <GND * 1 790 1190 0 0 1 2>
  <Sub S22_Probe1 1 770 980 -20 44 0 0 "S22_Probe.sch" 0 "50" 1>
  <.DC DC1 1 -140 470 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.AC AC1 1 -140 570 0 46 0 0 "log" 1 "1" 1 "1e6" 1 "121" 1 "no" 0>
  <Sub S12_Probe1 1 330 930 -40 34 0 0 "S12_Probe.sch" 0 "50" 1>
  <GND * 1 270 1170 0 0 1 2>
  <R_SPICE R1 1 180 650 -24 -66 0 2 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <R_SPICE R2 1 760 700 15 -26 0 1 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <R_SPICE R3 1 270 1140 26 -26 1 1 "50" 1 "" 0 "" 0 "" 0 "" 0>
</Components>
<Wires>
  <210 550 210 650 "" 0 0 0 "">
  <710 550 710 650 "" 0 0 0 "">
  <710 650 760 650 "" 0 0 0 "">
  <760 650 760 670 "" 0 0 0 "">
  <760 730 760 740 "" 0 0 0 "">
  <140 650 150 650 "" 0 0 0 "">
  <140 650 140 660 "" 0 0 0 "">
  <210 650 380 650 "" 0 0 0 "">
  <670 650 710 650 "" 0 0 0 "">
  <790 1100 790 1130 "" 0 0 0 "">
  <720 1000 730 1000 "" 0 0 0 "">
  <720 1100 730 1100 "" 0 0 0 "">
  <720 1000 720 1100 "" 0 0 0 "">
  <660 1100 720 1100 "" 0 0 0 "">
  <270 1100 400 1100 "" 0 0 0 "">
  <270 1100 270 1110 "" 0 0 0 "">
  <270 950 270 1100 "" 0 0 0 "">
  <330 550 330 550 "nS11" 340 520 0 "">
  <830 550 830 550 "nS21" 830 510 0 "">
  <850 1000 850 1000 "nS22" 850 960 0 "">
  <390 950 390 950 "nS12" 400 910 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Rectangle 370 570 310 270 #ff0000 2 1 #c0c0c0 1 0>
  <Rectangle 380 1000 310 270 #ff0000 2 1 #c0c0c0 1 0>
  <Text 490 570 20 #000000 0 "DUT">
  <Text 380 580 12 #ff0000 0 "IN">
  <Text 640 580 12 #ff0000 0 "OUT">
  <Text 640 1010 12 #ff0000 0 "OUT">
  <Text 400 1010 12 #ff0000 0 "IN">
  <Text 440 850 12 #000000 0 "S11 and S21 Test bench">
  <Text 470 1290 12 #000000 0 "S12 and S22 Test bench">
  <Text 490 1000 20 #000000 0 "DUT">
  <Rectangle -290 750 540 590 #000000 4 1 #c0c0c0 1 0>
  <Text -280 760 12 #000000 0 "    A test bench for simulating circuit two port small signal parameters\n                                   as a function of frequency.\n###########################################################\nCopy the four S parameter probes to your current project directory.\nCopy the test bench circuit onto an empty Qucs-S schematic sheet in \nyour current project directory.\n\nPlace copies of the circuit under test in the box marked DUT.\nMake sure that the test circuit input and output terminals are\nconnected to the test bench IN and OUT terminals. Note both\ncopies of the circuit under test must be in the same orientation. \n\nSelect one of the Ngspice, Xyce or SPICE OPUS circuit simulators.\nSet the value of Z0 and resistors R1, R2, R3 and R4 to the required\ncharacteristic impedance (default 50 Ohm).\nSet the AC simulation frequency range to the required value (default log,\nfrom 1Hz to 1Meg Hz with 20 points per decade).  \nSimulate  the DUT by pressing key F2.  \n\nThe S parameters of the circuit under test are the voltages \noutput at circuit nodes nS11, nS12, nS21 and nS22, respectively.\n\nPLEASE NOTE this test bench works with SPICE circuit simulators,\nand is designed for AC simulation ONLY.">
</Paintings>
