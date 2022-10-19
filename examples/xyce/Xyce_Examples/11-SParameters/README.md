## Demo of S parameter extraction

This very embryonic demo shows off Qucs-s support for Xyce S parameter
simulation and plotting using Smith diagrams.

This example will only work in versions of Qucs-s *after* release
0.0.23 (at this time, only available by compiling the "current" branch
from source out of git).

The schematic "sparam.sch" is a simple example taken from a qucs-s
issue report calling for the implementation of Xyce S-parameter
simulation in qucs-s.

The schematic "sparams-ts1-3port.sch" tries to duplicate the circuit
in a Xyce\_Regression test case,
"Xyce\_Regression/Netlists/Output/SPARAMS/sparams-ts1-3port.cir".
As such, the S-parameters it produces are not so interesting, but it
does demonstrate multi-port S parameter generation in a schematic
capture environment that matches the way the Xyce team verifies that
multi-port S-parameter simulation actually works.

The next pair of schematics, tline-sparam-ac.sch and tline-sparam.sch
show two different ways of computing S parameters for the same device
under test.  The first uses only standard SPICE-style AC analysis and
specially wired circuits such as could be built on the workbench to
measure S parameters, and the second uses Xyce S parameter analysis
and port sources.  The Smith plots of S parameters obtained by either
of these methods match the other.

The final pair of schematics, mrf501-sparam-ac.sch and
mrf501-sparam.sch mirror the two transmission line examples, one using
only SPICE primitives the other using S Parameter analysis, this time
on an MRF501 NPN transistor from National Semiconductor.  The Smith
plots of S parameters obtained by either of these methods match the
other.

![Created with Qucs "Export as image" option"](export.png)

![Created with Qucs "Export as image" option"](export2.png)

![Created with Qucs "Export as image" option"](export3.png)

![Created with Qucs "Export as image" option"](export4.png)

![Created with Qucs "Export as image" option"](export5.png)

![Created with Qucs "Export as image" option"](export6.png)
