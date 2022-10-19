<Qucs Schematic 0.0.17>
<Properties>
  <View=186,-31,1221,738,0.746424,0,0>
  <Grid=10,10,1>
  <DataSet=Amp_Two_Tone.dat>
  <DataDisplay=Amp_Two_Tone.dpl>
  <OpenDisplay=1>
  <Script=Amp_Two_Tone.m>
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
  <GND * 1 540 190 0 0 0 0>
  <_BJT BFP193W_1 5 540 160 8 -26 0 0 "npn" 0 "0.2738f" 0 "0.95341" 0 "1.4289" 0 "0.26949" 0 "0.037925" 0 "24" 0 "3.8742" 0 "10.627f" 0 "1.935" 0 "0.037409f" 0 "0.94371" 0 "125" 0 "14.267" 0 "1" 0 "0.91763m" 0 "0.11938" 0 "0.76534" 0 "1.8368" 0 "1.1824f" 0 "0.70276" 0 "0.48654" 0 "935.03f" 0 "1.1828" 0 "0.30002" 0 "0.053563" 0 "0" 0 "0.75" 0 "0" 0 "0.72063" 0 "18.828p" 0 "0.69477" 0 "0.8" 0 "0.96893m" 0 "1.0037n" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "0" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <GND * 1 630 370 0 0 0 0>
  <_BJT T_2N2907_1 5 530 230 -3 -83 0 1 "pnp" 0 "1e-14" 0 "1" 0 "1" 0 "0.3" 0 "0" 0 "120" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "250" 0 "3" 0 "0" 0 "0" 0 "3" 0 "1" 0 "10" 0 "30e-12" 0 "0.75" 0 "0.33" 0 "8e-12" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "400e-12" 0 "3" 0 "2" 0 "1" 0 "100e-9" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <R R1 5 470 230 -13 12 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 430 330 0 0 0 0>
  <C C1 5 660 130 -23 -33 0 0 "1000 pF" 1 "" 0 "neutral" 0>
  <C C2 5 670 230 -33 -32 0 2 "100 pF" 1 "" 0 "neutral" 0>
  <L L1 5 630 180 13 -20 0 3 "120 nH" 1 "" 0>
  <R R3 5 480 310 -19 12 0 2 "6.8k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 730 190 0 0 0 0>
  <GND * 1 790 190 0 0 0 0>
  <GND * 1 840 190 0 0 0 0>
  <GND * 1 700 230 0 0 0 0>
  <.DC DC1 5 690 60 0 40 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Tr Tr2 5 760 160 16 -7 0 0 "0.6" 1>
  <R R6 5 570 60 -23 -28 0 0 "1.8k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Pac P3 1 840 160 18 -11 0 1 "3" 0 "50 Ohm" 0 "-100 dBm" 0 "500 MHz" 0 "26.85" 0>
  <C C3 5 400 160 -23 -33 0 0 "1000 pF" 1 "" 0 "neutral" 0>
  <R R2 5 340 160 -23 -28 0 0 "25" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.TR TR1 1 730 230 0 65 0 0 "lin" 0 "0" 1 "5e-6" 1 "5001" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Eqn Eqn1 1 860 340 -30 16 0 0 "Spectrum_dBm=16+dB(Time2Freq(Vout.Vt,time))" 1 "yes" 0>
  <GND * 1 240 220 0 0 0 0>
  <GND * 1 240 330 0 0 0 0>
  <Pac P1 1 240 190 15 -11 0 1 "1" 0 "50 Ohm" 0 "0 dBm" 1 "520 MHz" 1 "26.85" 0>
  <Pac P2 1 240 300 14 -10 0 1 "2" 0 "50 Ohm" 0 "0 dBm" 1 "480 MHz" 1 "26.85" 0>
  <R R5 5 580 310 -25 11 0 0 "330" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vdc V1 5 630 340 -57 10 0 1 "3 V" 1>
  <R R4 5 630 270 12 -7 0 1 "1.8" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <630 130 630 150 "" 0 0 0 "">
  <630 210 630 230 "" 0 0 0 "">
  <560 230 630 230 "" 0 0 0 "">
  <530 260 530 310 "" 0 0 0 "">
  <430 230 440 230 "" 0 0 0 "">
  <430 160 430 230 "" 0 0 0 "">
  <630 230 630 240 "" 0 0 0 "">
  <630 300 630 310 "" 0 0 0 "">
  <510 310 530 310 "" 0 0 0 "">
  <430 310 450 310 "" 0 0 0 "">
  <430 310 430 330 "" 0 0 0 "">
  <630 230 640 230 "" 0 0 0 "">
  <690 130 730 130 "" 0 0 0 "">
  <790 130 840 130 "" 0 0 0 "">
  <610 310 630 310 "" 0 0 0 "">
  <530 310 550 310 "" 0 0 0 "">
  <630 60 630 130 "" 0 0 0 "">
  <600 60 630 60 "" 0 0 0 "">
  <510 60 510 160 "" 0 0 0 "">
  <510 60 540 60 "" 0 0 0 "">
  <430 160 510 160 "" 0 0 0 "">
  <540 130 630 130 "" 0 0 0 "">
  <310 160 310 270 "" 0 0 0 "">
  <240 270 310 270 "" 0 0 0 "">
  <240 160 310 160 "" 0 0 0 "">
  <540 130 540 130 "Vc" 580 170 0 "">
  <840 130 840 130 "Vout" 870 90 0 "">
</Wires>
<Diagrams>
  <Rect 940 325 234 265 3 #c0c0c0 1 00 0 0 1e+09 1e+09 0 -40 10 20 1 -1 1 1 315 0 225 "Frequency, Hz" "" "">
	<"Spectrum_dBm" #0000ff 0 3 0 0 0>
	  <Mkr 5.202e+08 -77 -316 3 0 0>
	  <Mkr 5.602e+08 33 -197 3 0 0>
  </Rect>
  <Tab 250 105 182 49 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 "" "" "">
	<"Vc.V" #0000ff 0 3 1 0 0>
	<"V1.I" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 230 410 12 #000000 0 "This simulation displays two-tone distortion characteristics.  Note that computation time is lengthy to obtain accurate results.  The\nlonger the time duration and the shorter the time interval, the more accurate results.  The time duration should be approximately\n100 / lowest frequency of interest and the interval should be an integer devisor of 1 / highest frequency of interest.  (This usually\nresults in more than 1000 intervals.)  Unless the DUT  (device under test) is meant to operate in a non-linear region, the two signal\nsources should be adjusted so that the power levels are within the approximate linear region of the amplifier.\n\nThis amplifier includes an active bias circuit since the collector voltage must operate very near the supply voltage, in this case less.\nthan 0.2 volts below the supply voltage of 3.0 volts  This type of circuit often exhibits low-frequency oscillations, modulating the carrier.\nThese can be prevented by modeling closed-loop stability of the complete circuit (phase margin at unity gain must be > 135 degrees).\n\nThe DUT output intercept can be determined by noting the ratio of the sidebands to the carrier and performing a simple calculation.\nExamining the above spectrum, the carriers are at +13.4 dBm and the sideband level is -11.5 dBm.  The ratio is then sideband level\nminus  carrier level or:  (-11.5) - (+13.4) = -24.9 dB.\n\nThe ouput intercept point is calculated by:  IP3out = sideband/carrier ratio + (2 times output level) or:  (-24.9) + 2(+13.4) = +1.9 dBm.  The\ninput intercept point is simply output intercept minus gain or:  IP3in = IP3out - gain in dB.  ">
</Paintings>
