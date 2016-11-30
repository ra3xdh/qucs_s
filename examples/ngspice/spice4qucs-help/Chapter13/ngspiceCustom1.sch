<Qucs Schematic 0.0.19>
<Properties>
  <View=-140,370,4665,1250,1,0,0>
  <Grid=10,10,1>
  <DataSet=ngspiceCustom1.dat>
  <DataDisplay=ngspiceCustom1.dpl>
  <OpenDisplay=1>
  <Script=ngspiceCustom1.m>
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
  <Vac U1 1 880 1040 27 -34 1 1 "1 V" 1 "1 kHz" 1 "0" 0 "0" 0>
  <GND * 1 880 1070 0 0 1 2>
  <R_SPICE R4 1 840 980 -18 18 1 0 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 860 740 0 0 0 0>
  <R_SPICE R3 1 860 710 15 -26 0 1 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <R_SPICE R2 1 450 660 -26 -61 0 2 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <Vac U0 1 420 700 21 -24 0 1 "1 V" 1 "1 kHz" 1 "0" 0 "0" 0>
  <GND * 1 420 730 0 0 0 0>
  <Sub S11_Probe1 1 540 480 -40 34 0 0 "S11_Probe.sch" 0 "50" 1>
  <GND * 1 540 770 0 0 0 0>
  <GND * 1 700 760 0 0 0 0>
  <L_SPICE L10 1 620 660 -26 10 0 0 "1.592m" 1 "" 0 "" 0 "" 0 "" 0>
  <C_SPICE C15 1 700 730 17 -26 0 1 "318.3n" 1 "" 0 "" 0 "" 0 "" 0>
  <C_SPICE C14 1 540 740 17 -26 0 1 "318.3n" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 550 1090 0 0 0 0>
  <L_SPICE L11 1 630 980 -26 10 0 0 "1.592m" 1 "" 0 "" 0 "" 0 "" 0>
  <C_SPICE C17 1 550 1060 17 -26 0 1 "318.3n" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 710 1090 0 0 0 0>
  <C_SPICE C16 1 710 1060 17 -26 0 1 "318.3n" 1 "" 0 "" 0 "" 0 "" 0>
  <Sub S12_Probe1 1 880 490 -40 34 0 0 "S12_Probe.sch" 0 "50" 1>
  <Sub S22_Probe1 1 850 860 -20 44 0 0 "S22_Probe.sch" 0 "50" 1>
  <Sub S21_Probe1 1 420 960 -60 -66 0 0 "S21_Probe.sch" 0 "50" 1>
  <R_SPICE R5 1 330 1020 28 13 1 1 "50" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 330 1050 0 0 1 2>
  <.CUSTOMSIM CUSTOM1 1 -70 430 0 77 0 0 "\n* AC small signal simulation.\n* \nac dec 121 1 1e6\n*\nlet Z0 = 50.0\n*\n* Find two port S parameters from test circuit.\n*\nlet S11 = V(nS11)\nlet S12 = V(nS12)\nlet S21 = V(nS21)\nlet S22 = V(nS22)\n*\n* Extract Y paramerters.\n*\nlet StoYDelS =  ( (1+S11)*(1+S22)-S12*S21 )*Z0\nlet Y11 = ( (1+S22)*(1-S11)+S12*S21 )/StoYDelS\nlet Y12 =  -2*S12/StoYDelS\nlet Y21 =  -2*S21/StoYDelS\nlet Y22 = ( (1+S11)*(1-S22)+S12*S21 )/StoYDelS\n*\n* Extract Z parameters\n*\nlet StoZDelS = ( (1-S11)*(1-S22)-S12*S21 )/Z0\nlet Z11 = ( (1+S11)*(1-S22)+S12*S21 )/StoZDelS\nlet Z12 =   2*S12/StoZDelS\nlet Z21 =   2*S21/StoZDelS\nlet Z22 = ( (1-S11)*(1+S22)+S12*S21 )/StoZDelS\n" 1 "S11; S12; S21; S22; Y11; Y12; Y21; Y22; Z11; Z12; Z21; Z22" 0 "" 0>
</Components>
<Wires>
  <870 980 880 980 "" 0 0 0 "">
  <880 980 880 1010 "" 0 0 0 "">
  <420 660 420 670 "" 0 0 0 "">
  <480 500 480 660 "" 0 0 0 "">
  <860 660 860 680 "" 0 0 0 "">
  <650 660 700 660 "" 0 0 0 "">
  <480 660 540 660 "" 0 0 0 "">
  <540 660 590 660 "" 0 0 0 "">
  <540 660 540 710 "" 0 0 0 "">
  <700 660 820 660 "" 0 0 0 "">
  <700 660 700 700 "" 0 0 0 "">
  <660 980 710 980 "" 0 0 0 "">
  <550 980 600 980 "" 0 0 0 "">
  <550 980 550 1030 "" 0 0 0 "">
  <710 980 800 980 "" 0 0 0 "">
  <710 980 710 1030 "" 0 0 0 "">
  <820 660 860 660 "" 0 0 0 "">
  <820 510 820 660 "" 0 0 0 "">
  <800 880 810 880 "" 0 0 0 "">
  <800 980 810 980 "" 0 0 0 "">
  <800 880 800 980 "" 0 0 0 "">
  <330 980 550 980 "" 0 0 0 "">
  <330 980 330 990 "" 0 0 0 "">
  <330 880 340 880 "" 0 0 0 "">
  <330 880 330 980 "" 0 0 0 "">
  <600 500 600 500 "nS11" 630 470 0 "">
  <940 510 940 510 "nS12" 970 480 0 "">
  <930 880 930 880 "nS22" 960 850 0 "">
  <460 880 460 880 "nS21" 490 850 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Rectangle 480 900 310 270 #ff0000 2 1 #c0c0c0 1 0>
  <Rectangle 490 580 300 270 #ff0000 2 1 #c0c0c0 1 0>
  <Text 750 620 12 #ff0000 0 "OUT">
  <Text 500 610 12 #ff0000 0 "IN">
  <Text 620 580 20 #000000 0 "DUT">
  <Text 590 900 20 #000000 0 "DUT">
  <Text 490 940 12 #ff0000 0 "IN">
  <Text 740 940 12 #ff0000 0 "OUT">
  <Rectangle -100 410 390 800 #ff0000 3 1 #c0c0c0 1 0>
</Paintings>
