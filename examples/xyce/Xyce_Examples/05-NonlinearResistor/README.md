# Nonlinear Resistor circuit from Xyce Test Suite

This Qucs-s schematic set implements the nonlinear resistor model in
Netlists/NL_RESISTOR from the Xyce test suite.  NLR_test.sch is the
top level driver schematic with analysis and graphing objects,
NLR_subc.sch is the actual nonlinear resistor subcircuit.

This demo makes use of the new ".FUNC" schematic symbol, which was
introduced on the spice4qucs_current (development) branch of
https://github.com/ra3xdh/qucs, and is NOT present in either the
qucs-s-stable branch or the released 0.0.19S version of Qucs-s.  So
this schematic will only work in the development version or in qucs-s
0.0.20.

The purpose of this demo is to show how one may create hierarchical
schematics in Qucs-s where subcircuits take parameters.

The important thing to realize here is that when you create a
schematic for a subcircuit that uses parameters, you MUST edit the
circuit SYMBOL and add the parameters in the dialog box.  There is
absolutely nothing in the Qucs-s documentation that explains that, and
there is nothing to place in the schematic itself to make these
parameters recognized.  You have to attach them to the symbol.

Another thing to note: as long as one uses only parameters and
variables in the Equation blocks (and not solution values), Equations
will be turned in to ".PARAM" lines.

### Exported schematic image
![Created with Qucs "Export as image" option"](export.png)
