The schematics in this directory are just straightforward
implementations of the Diode Clipper examples from the Xyce Users'
Guide.  We also demonstrate use of native qucs-s graphing of results
and proper use of parameter sweeps.

### DC example trips a qucs-s bug in versions older than 0.0.21

As noted in the SuperSimpleDC example, versions of qucs-s prior to
version 0.0.21 have a bug in netlist generation for DC sweeps, which
requires that one set the "Number" parameter in the Parameter Sweep
object one lower than the actual number of points desired.

This bug was fixed in release 0.0.21, and this demonstration circuit
is set up to work in that version.  If you are running an earlier
version, the parameter sweep object must be modified to cause the
buggy netlist generator to do the right thing.

Here, we want 26 DC sweep points with a step size of 1 and in version
0.0.21 that's what we enter into the parameter sweep object.  But in
order to accomplish that in version 0.0.20 or earlier, one must enter
25 into the "steps" entry in the sweep parameter dialog.


### Exported schematic image
![Created with Qucs "Export as image" option"](export.png)
