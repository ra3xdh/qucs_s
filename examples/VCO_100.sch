<Qucs Schematic 0.0.17>
<Properties>
  <View=276,-80,1752,670,0.69851,0,0>
  <Grid=10,10,1>
  <DataSet=VCO_100.dat>
  <DataDisplay=VCO_100.dpl>
  <OpenDisplay=1>
  <Script=VCO_100.m>
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
  <GND * 1 420 340 0 0 0 0>
  <R R1 5 420 210 18 -11 0 1 "5.6 k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R5 5 420 310 16 -2 0 1 "2.7 k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <_BJT Q2N3904_2 1 520 160 -32 -29 0 1 "npn" 0 "1.4e-14" 0 "1" 0 "1" 0 "0.025" 0 "0" 0 "100" 0 "0" 0 "3e-13" 0 "1.5" 0 "0" 0 "2" 0 "300" 0 "7.5" 0 "0" 0 "0" 0 "2.4" 0 "0" 0 "0" 0 "4.5e-12" 0 "0.75" 0 "0.33" 0 "3.5e-12" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "4e-10" 0 "0" 0 "0" 0 "0" 0 "2.1e-08" 0 "26.85" 0 "9e-16" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <C C5 5 520 290 21 -11 0 1 "1000 pF" 1 "" 0 "neutral" 0>
  <GND * 1 520 440 0 0 0 0>
  <GND * 1 330 240 0 0 0 0>
  <Vdc V1 5 330 210 22 -9 0 1 "12 V" 1>
  <C C2 5 570 260 -24 -37 0 0 "4 pF" 1 "" 0 "neutral" 0>
  <L L6 5 470 260 -31 -28 0 0 "100 uH" 1 "" 0>
  <C C3 5 670 260 -24 -37 0 0 "8 pF" 1 "" 0 "neutral" 0>
  <L L8 5 670 160 -26 1 0 0 "100 uH" 1 "" 0>
  <GND * 1 730 300 0 0 0 0>
  <R R6 5 730 210 16 -2 0 1 "56" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L7 5 520 350 10 -8 0 1 "490 nH" 1 "" 0>
  <GND * 1 790 180 0 0 0 0>
  <C C4 5 670 90 -27 19 0 0 "47 pF" 1 "" 0 "neutral" 0>
  <C C6 5 520 410 22 -9 0 1 "C1" 1 "" 0 "neutral" 0>
  <Pac P1 1 790 150 18 -26 0 1 "1" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 910 180 0 0 0 0>
  <Pac P2 1 910 150 -49 -10 0 1 "2" 0 "50 Ohm" 0 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Circulator X2 5 790 90 -26 -16 0 0 "50 Ohm" 0 "50 Ohm" 0 "50 Ohm" 0>
  <.SP SP1 1 950 -20 0 68 0 0 "lin" 0 "75 MHz" 1 "250 MHz" 1 "101" 0 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <.TR TR1 0 1100 -20 0 68 0 0 "lin" 0 "5e-7" 1 "1e-6" 1 "1001" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.DC DC1 1 1100 130 0 41 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Eqn Eqn2 0 970 240 -24 15 0 0 "Output_dBm=16+dB(Time2Freq(Vrf.Vt, time))" 1 "C1=7.5e-12" 1 "yes" 0>
  <Eqn Eqn1 1 970 140 -24 15 0 0 "Mag=dB(S[2,1])" 1 "Phase=phase(S[2,1])" 1 "yes" 0>
  <.SW SW1 1 340 410 0 68 0 0 "SP1" 1 "lin" 0 "C1" 1 "2.5e-12" 1 "12.5e-12" 1 "5" 0>
</Components>
<Wires>
  <520 190 520 260 "" 0 0 0 "">
  <420 160 490 160 "" 0 0 0 "">
  <420 160 420 180 "" 0 0 0 "">
  <420 240 420 260 "" 0 0 0 "">
  <420 260 420 280 "" 0 0 0 "">
  <330 160 420 160 "" 0 0 0 "">
  <330 160 330 180 "" 0 0 0 "">
  <550 160 620 160 "" 0 0 0 "">
  <620 160 620 260 "" 0 0 0 "">
  <600 260 620 260 "" 0 0 0 "">
  <520 260 540 260 "" 0 0 0 "">
  <500 260 520 260 "" 0 0 0 "">
  <420 260 440 260 "" 0 0 0 "">
  <620 260 640 260 "" 0 0 0 "">
  <620 160 640 160 "" 0 0 0 "">
  <700 160 730 160 "" 0 0 0 "">
  <700 260 730 260 "" 0 0 0 "">
  <730 260 730 300 "" 0 0 0 "">
  <730 240 730 260 "" 0 0 0 "">
  <730 160 730 180 "" 0 0 0 "">
  <820 90 910 90 "" 0 0 0 "">
  <700 90 760 90 "" 0 0 0 "">
  <620 90 620 160 "" 0 0 0 "">
  <620 90 640 90 "" 0 0 0 "">
  <910 90 910 120 "" 0 0 0 "">
  <910 90 910 90 "Vrf" 860 50 0 "">
</Wires>
<Diagrams>
  <Rect 1370 574 318 254 3 #c0c0c0 1 00 0 7.5e+07 5e+07 2.5e+08 0 -30 10 20 1 -1 0.5 1 315 0 225 "Frequency, Hz" "" "">
	<"Output_dBm" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 1350 234 336 244 3 #c0c0c0 1 00 1 9.9e+07 500000 1.01e+08 1 -1.08598 10 16.1964 1 -275.117 200 272.718 315 0 225 "Frequency, Hz" "" "">
	<"Mag" #0000ff 0 3 0 0 0>
	<"Phase" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Text 340 -30 14 #ff0000 0 "100 - 175 MHz VCO.  Replace fixed resonator capacitor with a pair\nof back-back varactors, 25 to 5 pF over desired voltage tuning range\nand add a simple tuning voltage injection and de-coupling circuit.">
  <Text 610 340 14 #000000 0 "Fastest way to obtain approximate component values is to use S parameters with\na circulator connected so that the VCO becomes a reflection amplifier.  Oscillation\nwill occur where the phase crosses "0" with gain.  The "Q" of the circuit can be\nqualitatively evaluated by the phase slope at and near the zero crossing point.  For\nS parameter analysis, enable SP1, Eqn1 and SW1 then disable TR1 and Eqn2.\n\nOnce the swept S parameter analysis is satisfactory, the circuit is configured as\nan oscillator by removing (or bypassing) the circulator and the source, P1.  The\noutput capacitor is then coupled to the load, P2.  Enable TR1 and Eqn2\nthen disable SP1 and Eqn1.  (This assumes that a SPICE model is available,\notherwise use the S parameter values for the overall design.)">
  <Text 1410 290 12 #000000 0 "Spectral Display">
  <Text 1380 -40 12 #000000 0 "S parameter Display">
</Paintings>
