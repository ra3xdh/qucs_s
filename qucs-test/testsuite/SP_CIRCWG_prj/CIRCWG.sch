<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-99,2220,1258,0.605001,28,136>
  <Grid=10,10,1>
  <DataSet=SP_CIRWG.dat>
  <DataDisplay=CW.dpl>
  <OpenDisplay=0>
  <Script=CW.m>
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
  <Pac P1 1 220 210 -74 -26 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 400 210 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 220 240 0 0 0 0>
  <GND * 1 400 240 0 0 0 0>
  <Eqn EqnTC1 1 370 320 -23 12 0 0 "A=twoport(S,'S','A')" 1 "S11m=abs(S[1,1])" 1 "S21m=abs(S[2,1])" 1 "S11a=(180/pi)*angle(S[1,1])" 1 "S21a=(180/pi)*angle(S[2,1])" 1 "ZL=real(sqrt(A[1,2]/A[2,1]))" 1 "yes" 0>
  <CIRCLINE Line1 1 320 160 -26 18 0 0 "Radius" 1 "Length" 1 "Er" 1 "1" 0 "0" 0 "RsCopper" 0 "26.85" 0 "unspecified" 0>
  <Eqn Eqn1 1 610 290 -31 19 0 0 "CondCopper=5.96e7" 1 "RsCopper=1/CondCopper" 1 "yes" 0>
  <Eqn Eqn3 1 610 110 -31 19 0 0 "Radius=5e-3" 1 "Length=50e-3" 1 "Er=3" 1 "p11=1.841" 1 "yes" 0>
  <Eqn Eqn4 1 190 540 -31 19 0 0 "freq_cf=p11/(2*pi*Radius*sqrt(mu*e0*Er))" 1 "mu=4*pi*1e-7" 1 "e0=8.854187817e-12" 1 "yes" 0>
  <Eqn Eqn2 1 630 420 -31 19 0 0 "S21_dB=dB(S[2,1])" 1 "S11_dB=dB(S[1,1])" 1 "yes" 0>
  <.SP SPTC1 1 160 310 0 84 0 0 "lin" 1 "9 GHz" 1 "13GHz" 1 "1701" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
</Components>
<Wires>
  <400 160 400 180 "" 0 0 0 "">
  <350 160 400 160 "" 0 0 0 "">
  <220 160 220 180 "" 0 0 0 "">
  <220 160 290 160 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 960 244 512 294 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "" "">
	<"S21m" #0000ff 0 3 0 0 0>
	  <Mkr 1.01318e+10 196 104 3 0 0>
	  <Mkr 1.13835e+10 365 -323 3 0 0>
  </Rect>
  <Tab 880 410 179 70 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 "" "" "" "">
	<"freq_cf" #0000ff 0 3 1 0 0>
  </Tab>
  <Rect 874 921 688 413 3 #c0c0c0 1 00 1 1.22e+10 2e+08 1.59e+10 0 -16 2 0 1 -1 0.2 1 315 0 225 "" "" "" "">
	<"S21_dB" #ff0000 0 3 0 0 0>
	<"S11_dB" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 130 945 571 205 3 #c0c0c0 1 00 1 9e+09 5e+08 1.3e+10 0 0 500 5000 1 -1 0.5 1 315 0 225 "" "" "" "">
	<"ZL" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Arrow 1070 370 80 0 20 8 #000000 0 1 0>
  <Text 340 690 12 #000000 0 "WAVE IMPEDANCE">
</Paintings>
