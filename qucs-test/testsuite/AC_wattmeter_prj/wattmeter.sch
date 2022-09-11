<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-180,1524,908,0.909091,0,57>
  <Grid=10,10,1>
  <DataSet=wattmeter.dat>
  <DataDisplay=wattmeter.dpl>
  <OpenDisplay=0>
  <Script=wattmeter.m>
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
  <Vac V1 1 56 277 18 -26 0 1 "150 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 1 56 437 0 0 0 0>
  <R R1 1 306 257 15 -26 0 1 "15 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 406 257 15 -26 0 1 "6 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 516 257 15 -26 0 1 "8 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C1 1 516 377 17 -26 0 1 "177 uF" 1 "" 0 "neutral" 0>
  <WProbe PF 1 216 187 28 -31 0 0>
  <.AC AC1 1 63 -26 0 46 0 0 "lin" 1 "100 Hz" 1 "1000 Hz" 1 "10" 1 "no" 0>
  <L L1 1 406 377 10 -26 0 1 "0.1 H" 1 "" 0>
</Components>
<Wires>
  <56 187 186 187 "" 0 0 0 "">
  <516 187 516 227 "" 0 0 0 "">
  <516 287 516 347 "" 0 0 0 "">
  <516 407 516 437 "" 0 0 0 "">
  <56 307 56 437 "" 0 0 0 "">
  <406 437 516 437 "" 0 0 0 "">
  <406 407 406 437 "" 0 0 0 "">
  <406 287 406 347 "" 0 0 0 "">
  <406 187 406 227 "" 0 0 0 "">
  <246 187 306 187 "" 0 0 0 "">
  <306 187 406 187 "" 0 0 0 "">
  <306 187 306 227 "" 0 0 0 "">
  <306 437 406 437 "" 0 0 0 "">
  <306 287 306 437 "" 0 0 0 "">
  <206 207 206 217 "" 0 0 0 "">
  <56 187 56 217 "" 0 0 0 "">
  <56 217 56 247 "" 0 0 0 "">
  <56 217 206 217 "" 0 0 0 "">
  <56 437 226 437 "" 0 0 0 "">
  <226 437 306 437 "" 0 0 0 "">
  <226 207 226 437 "" 0 0 0 "">
  <406 187 516 187 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 621 449 636 278 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 10 315 0 225 "" "" "" "">
	<"PF.i" #0000ff 0 3 1 0 0>
	<"PF.v" #0000ff 0 3 0 0 0>
	<"PF.P" #0000ff 0 3 0 0 0>
	<"PF.PF" #0000ff 0 3 1 0 0>
	<"PF.Q" #0000ff 0 3 1 0 0>
	<"PF.S" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 232 -21 12 #000000 0 "Wattmeter in AC, the component always\nacts as both voltmeter and ammeter.\nWith the collected values, it then calculates the\npower triangle and corresponding power factor.">
</Paintings>
