# Quartz crystal filter

The QuartzCrystal circuit is a minor variant of what's in the qucs-s
examples suite (examples/ngspice/quarz.sch).  It explicitly outputs
information the Xyce way.

In older variants of this example (still available in git history), we
computed the "K" expression using real equivalents, but as of Xyce 7.2
this is no longer necessary --- in frequency domain, .print statements
can now support expressions that perform complex arithmetic.

As such, the current version of this circuit in this repo *requires*
use of a version of Xyce after release 7.2.

Note also that the quarz.sch schematic (the subcircuit) is another
example of a subcircuit schematic where the subcircuit parameters are
defined and given defaults in the subcircuit symbol.  to see this,
open quarz_test.sch, click the crystal symbol, choose the "Go into
subcircuit" option from the right-click menu, and then choose the
"Edit circuit symbol" option from the right-click menu.

### Exported schematic image
![Created with Qucs "Export as image" option"](export.png)
