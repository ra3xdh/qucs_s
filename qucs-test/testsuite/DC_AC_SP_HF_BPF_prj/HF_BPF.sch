<Qucs Schematic 0.0.19>
<Properties>
  <View=-34,-74,1000,729,0.977584,0,0>
  <Grid=10,10,1>
  <DataSet=HF_BPF.dat>
  <DataDisplay=HF_BPF.dpl>
  <OpenDisplay=1>
  <Script=HF_BPF.m>
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
  <INDQ INDQ1 1 550 90 -30 -122 0 0 "1.82uH" 1 "Ql" 1 "f0" 1 "SquareRoot" 1 "26.85" 0>
  <INDQ INDQ3 1 630 170 16 -52 0 1 "221.8nH" 1 "Ql" 1 "f0" 1 "SquareRoot" 1 "26.85" 0>
  <CAPQ CAPQ3 1 590 170 -108 -57 0 1 "560 pF" 1 "Qc" 1 "f0" 1 "Linear" 1 "26.85" 0>
  <GND * 1 800 220 0 0 0 0>
  <Pac P1 1 800 170 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 W" 0 "1 GHz" 0 "-273.15" 1>
  <INDQ INDQ2 1 310 170 15 -53 0 1 "221.8nH" 1 "Ql" 1 "f0" 1 "SquareRoot" 1 "26.85" 0>
  <CAPQ CAPQ1 1 410 90 -41 -124 0 0 "68 pF" 1 "Qc" 1 "f0" 1 "Linear" 1 "26.85" 0>
  <CAPQ CAPQ2 1 270 170 -108 -57 0 1 "560 pF" 1 "Qc" 1 "f0" 1 "Linear" 1 "26.85" 0>
  <GND * 1 310 220 0 0 0 0>
  <.DC DC1 1 20 310 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Eqn Eqn2 1 450 320 -28 15 0 0 "S21_dB=dB(S[2,1])" 1 "S11_dB=dB(S[1,1])" 1 "yes" 0>
  <Eqn Eqn1 1 450 440 -31 17 0 0 "Ql=200" 1 "Qc=5000" 1 "f0=14e6" 1 "yes" 0>
  <.SP SP1 1 230 310 0 72 0 0 "log" 1 "1.2 MHz" 1 "170 MHz" 1 "433" 1 "yes" 1 "1" 0 "2" 0 "no" 0 "no" 0>
  <.AC AC1 1 20 410 0 43 0 0 "log" 1 "1.2 MHz" 1 "170 MHz" 1 "218" 1 "yes" 1>
  <GND * 1 100 220 0 0 0 0>
  <Pac P2 1 100 150 -122 -26 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "-273.15" 1>
  <GND * 1 630 290 0 0 0 0>
  <Vdc V1 1 630 250 -75 -22 1 1 "1.1 V" 1>
</Components>
<Wires>
  <630 90 630 140 "" 0 0 0 "">
  <590 140 630 140 "" 0 0 0 "">
  <590 200 630 200 "" 0 0 0 "">
  <630 200 630 220 "" 0 0 0 "">
  <580 90 630 90 "" 0 0 0 "">
  <630 90 800 90 "" 0 0 0 "">
  <800 90 800 140 "" 0 0 0 "">
  <800 200 800 220 "" 0 0 0 "">
  <440 90 520 90 "" 0 0 0 "">
  <310 90 310 140 "" 0 0 0 "">
  <270 140 310 140 "" 0 0 0 "">
  <270 200 310 200 "" 0 0 0 "">
  <310 90 380 90 "" 0 0 0 "">
  <310 200 310 220 "" 0 0 0 "">
  <100 90 310 90 "" 0 0 0 "">
  <100 90 100 120 "" 0 0 0 "">
  <100 180 100 220 "" 0 0 0 "">
  <630 280 630 290 "" 0 0 0 "">
  <800 90 800 90 "out" 830 60 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 40 -40 12 #ff5500 0 "Chebyshev band-pass filter \n 12MHz...17MHz, pi-type, \n impedance matching 50 Ohm">
</Paintings>
