<Qucs Schematic 0.0.15>
<Properties>
  <View=0,-115,903,800,1,0,32>
  <Grid=10,10,1>
  <DataSet=test.dat>
  <DataDisplay=test.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Titre>
  <FrameText1=Auteur :>
  <FrameText2=Date :>
  <FrameText3=Version :>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 120 200 0 0 0 0>
  <IProbe I 1 150 40 -26 16 0 0>
  <VProbe V 1 280 -40 28 -31 0 0>
  <GND * 1 290 -10 0 0 0 0>
  <Vac V1 1 120 130 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <C C1 1 340 40 -26 17 0 0 "C" 1 "" 0 "neutral" 0>
  <.AC AC1 1 140 250 0 79 0 0 "log" 1 "1 Hz" 1 "10 GHz" 1 "100" 1 "yes" 0>
  <R R1 1 230 40 -26 15 0 0 "R" 1 "TC" 1 "0.0" 0 "0.0" 0 "TC" 0 "european" 0>
  <Eqn Eqn1 1 470 60 -33 19 0 0 "R=1" 1 "C=1e-6" 1 "TC=25" 1 "T=TC+273.15" 1 "noiseR2=4*1.3806488e-23*T*R" 1 "tol=1e-10" 1 "Vc=1/(1+j*2*pi*acfrequency*C*R)" 1 "diffV=V.v-Vc" 1 "assertV=assert(abs(diffV)<tol)" 1 "Vcn=sqrt(((abs(1/(1+j*2*pi*acfrequency*C*R)))^2)*noiseR2)" 1 "diffVn=V.vn-Vcn" 1 "assertVn=assert(abs(diffVn)<tol)" 1 "Ic=((1-1/(1+j*2*pi*acfrequency*C*R))/R)" 1 "diffI=I.i-Ic" 1 "assertI=assert(abs(diffI)<tol)" 1 "yes" 0>
</Components>
<Wires>
  <120 200 400 200 "" 0 0 0 "">
  <120 160 120 200 "" 0 0 0 "">
  <120 40 120 100 "" 0 0 0 "">
  <180 40 200 40 "" 0 0 0 "">
  <270 -20 270 40 "" 0 0 0 "">
  <290 -20 290 -10 "" 0 0 0 "">
  <260 40 270 40 "" 0 0 0 "">
  <400 40 400 200 "" 0 0 0 "">
  <370 40 400 40 "" 0 0 0 "">
  <270 40 310 40 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
