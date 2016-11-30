<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-69,5151,924,1,0,0>
  <Grid=10,10,1>
  <DataSet=testDiodeSM.dat>
  <DataDisplay=testDiodeSM.dpl>
  <OpenDisplay=1>
  <Script=testDiodeSM.m>
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
  <GND * 1 140 190 0 0 0 0>
  <S4Q_V V1 1 140 160 -88 29 0 1 "dc 0 ac 0 sin(0.4 0.2 1e3 0 0)" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 350 250 0 0 0 0>
  <R R1 1 350 220 15 -26 0 1 "0.1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <DIODE_SPICE D1 1 350 160 -297 81 0 3 "dmod" 1 ".model dmod d(is=1e-15 n=1 cj0=1p bv=20 rs=0)" 1 "" 0 "" 0 "" 0>
  <.CUSTOMSIM CUSTOM1 1 500 -30 0 51 0 0 "\n\ntran 1e-7 10m\nlet Id = v(ncur)/0.1\nwrite testDiodeSMtran1.txt  v(ns) Id\ndisplay\n\nset nfrequs = 10\nfourier 1k v(ns) v(ncur) \nspec 0 10e3 1e3  v(ns) v(ncur) \nlet Idf = v(ncur)/0.1\nwrite testDiodeSMfourier2.txt v(ns) Idf\ndisplay\n\n\nssse v(ncur) 0 1e-6 1e-3 20\ndisplay\n\nspec 0 20e3 1e3 v(ncur) \nlet k = abs(v(ncur)/0.1)\nwrite testDiodeSMsp1.txt k \ndisplay" 1 "V(ns);V(ncur);Id;Idf;k" 0 "testDiodeSMtran1.txt;testDiodeSMfourier2.txt;testDiodeSMsp1.txt" 0>
</Components>
<Wires>
  <140 130 350 130 "" 0 0 0 "">
  <140 130 140 130 "ns" 160 90 0 "">
  <350 190 350 190 "ncur" 380 160 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Rectangle 480 -50 300 610 #ff0000 3 1 #c0c0c0 1 0>
</Paintings>
