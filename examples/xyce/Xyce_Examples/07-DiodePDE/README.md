This example duplicates the PDE_1D_DIODE/diodepde test case from
the Xyce regression suite.

The DiodePDE circuit demonstrates how to insert a Xyce-specific "Y"
device using the "SPICE generic" symbol of Qucs-s.  The trick is to
set the "Spice Letter" to "YPDE ", i.e. have the space in the
"letter."  Qucs-s does the right thing and outputs the "letter" with
the device name concatenated, and gets the space between them right
because you told it to.  The *wrong* trick is to try to put the space
at the beginning of the name, which won't work, and may cause qucs-s
to crash during netlist generation instead.

### Exported schematic image
![Created with Qucs "Export as image" option"](export.png)
