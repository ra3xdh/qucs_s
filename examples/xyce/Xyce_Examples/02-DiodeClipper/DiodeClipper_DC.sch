<Qucs Schematic 0.0.21>
<Properties>
  <View=-51,-140,1537,680,1,0,0>
  <Grid=10,10,1>
  <DataSet=DiodeClipper_DC.dat>
  <DataDisplay=DiodeClipper_DC.dpl>
  <OpenDisplay=0>
  <Script=DiodeClipper_DC.m>
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
  <Vdc VCC 1 220 310 18 -26 0 1 "5 V" 1>
  <Vdc Vin 1 390 310 18 -26 0 1 "0 V" 1>
  <Vac V3 1 390 370 18 -26 0 1 "10 V" 1 "1 KHz" 0 "0" 0 "0" 0>
  <GND * 1 220 340 0 0 0 0>
  <GND * 1 390 400 0 0 0 0>
  <R R2 1 560 -30 15 -26 0 1 "3.3K Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 1040 120 0 0 0 0>
  <DIODE_SPICE D1 1 760 -30 15 -26 0 1 "D1N3940" 1 "" 0 "" 0 "" 0 "" 0>
  <SpiceModel D1N3940 1 550 510 -30 16 0 0 ".MODEL D1N3940 D(IS = 4E-10 N = 1.48 CJO = 1.95E-11 M = .38 PHI=.4 FC=.9 TT=8e-7 BV=600 RS=.105)" 1 "" 0 "" 0 "" 0 "Line_5=" 0>
  <DIODE_SPICE D2 1 760 90 15 -26 0 1 "D1N3940" 1 "" 0 "" 0 "" 0 "" 0>
  <C C1 1 930 30 -26 17 0 0 ".47uF" 1 "" 0 "neutral" 0>
  <R R1 1 470 30 -26 15 0 0 "1K Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 560 90 15 -26 0 1 "3.3K Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 1040 90 15 -26 0 1 "5.6K Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.DC DC1 1 580 230 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 580 310 0 57 0 0 "DC1" 1 "lin" 1 "VIN" 1 "-10V" 1 "15V" 1 "26" 1 "false" 0>
</Components>
<Wires>
  <500 30 560 30 "" 0 0 0 "">
  <560 30 760 30 "" 0 0 0 "">
  <560 0 560 30 "" 0 0 0 "">
  <560 30 560 60 "" 0 0 0 "">
  <760 30 900 30 "" 0 0 0 "">
  <760 0 760 30 "" 0 0 0 "">
  <760 30 760 60 "" 0 0 0 "">
  <560 -60 760 -60 "" 0 0 0 "">
  <960 30 1040 30 "" 0 0 0 "">
  <1040 30 1040 60 "" 0 0 0 "">
  <560 120 760 120 "" 0 0 0 "">
  <760 120 1040 120 "" 0 0 0 "">
  <220 280 220 280 "VCC" 250 250 0 "">
  <390 280 390 280 "IN" 420 250 0 "">
  <760 -60 760 -60 "VCC" 790 -90 0 "">
  <960 30 960 30 "OUT" 990 0 0 "">
  <440 30 440 30 "IN" 400 -10 0 "">
  <760 30 760 30 "N2" 790 0 0 "">
</Wires>
<Diagrams>
  <Rect 50 130 240 160 3 #c0c0c0 1 00 1 -10 5 15 1 -12.5 20 20 1 -1 1 1 315 0 225 "VIN (Volts)" "" "">
	<"xyce/V(N2)" #0000ff 0 3 0 0 0>
	<"xyce/V(OUT)" #ff0000 0 3 0 0 0>
	<"xyce/V(IN)" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 250 -120 12 #000000 0 "This is the Diode Clipper circuit from the Xyce Users' Guide">
</Paintings>
