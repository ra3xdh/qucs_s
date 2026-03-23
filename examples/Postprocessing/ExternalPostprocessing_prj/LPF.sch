<Qucs Schematic 25.2.0>
<Properties>
  <View=-142,-165,1644,829,0.880639,0,0>
  <Grid=10,10,1>
  <DataSet=LPF.dat>
  <DataDisplay=LPF.dpl>
  <OpenDisplay=0>
  <Script=LPF.m>
  <RunScript=0>
  <showFrame=3>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac T1 1 150 280 -100 -20 0 1 "1" 1 "50.0Ohm" 1 "0 dBm" 0 "1 MHz" 0 "26.85" 0 "true" 0>
  <GND * 1 150 310 0 0 0 0>
  <GND * 1 250 310 0 0 0 0>
  <GND * 1 450 310 0 0 0 0>
  <Pac T2 1 570 280 25 -20 0 1 "2" 1 "50.0Ohm" 1 "0 dBm" 0 "1 MHz" 0 "26.85" 0 "true" 0>
  <GND * 1 570 310 0 0 0 0>
  <.SP SP1 1 730 210 0 56 0 0 "log" 1 "10 MHz" 1 "2000 MHz" 1 "200" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Qucsator 1 920 240 -28 15 0 0 "S21_dB=dB(S[2,1])" 1 "S11_dB=dB(S[1,1])" 1 "yes" 0>
  <C C1 1 250 280 17 -26 0 1 "3.76 pF" 1 "" 0 "neutral" 0>
  <C C2 1 450 280 17 -26 0 1 "3.76 pF" 1 "" 0 "neutral" 0>
  <L L1 1 350 230 -31 -62 0 0 "14.5 nH" 1 "" 0>
  <NutmegEq NGspice 0 1090 240 -28 15 0 0 "SP1" 1 "dBS21=dB(S_2_1)" 1 "dBS11=dB(S_1_1)" 1>
  <CMD PythonWithVENV 1 180 470 -67 18 0 0 "# Path to the virtual environment where \n# scikit-rf and matplotlib are installed\nVENV=~/jupyter-venv\n# Path to the python script\nSCRIPT_DIR=~/QucsWorkspace/ExternalPostprocessing_prj\n# Simulator backend ''qucsator'' or ''ngspice''\nSIMULATOR=qucsator\n# Simulator backend\nsource $VENV/bin/activate\ncd $SCRIPT_DIR\npython3 PythonPlot.py $SIMULATOR" 1 "yes" 1 "yes" 0>
  <CMD Octave 0 780 470 -67 18 0 0 "# Path to the octave script\nSCRIPT_DIR=~/QucsWorkspace/ExternalPostprocessing_prj\n# Simulator backend ''qucsator'' or ''ngspice''\nSIMULATOR=qucsator\ncd $SCRIPT_DIR\noctave OctavePlot.m $SIMULATOR" 1 "yes" 1 "yes" 0>
</Components>
<Wires>
  <150 230 250 230 "" 0 0 0 "">
  <150 230 150 250 "" 0 0 0 "">
  <250 230 250 250 "" 0 0 0 "">
  <250 230 320 230 "" 0 0 0 "">
  <380 230 450 230 "" 0 0 0 "">
  <450 230 450 250 "" 0 0 0 "">
  <570 230 570 250 "" 0 0 0 "">
  <450 230 570 230 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 70 60 20 #000000 0 "Postprocessing with Python and Octave">
  <Text 110 120 12 #000000 0 "S-parameter simulation with Qucsator-RF">
</Paintings>
