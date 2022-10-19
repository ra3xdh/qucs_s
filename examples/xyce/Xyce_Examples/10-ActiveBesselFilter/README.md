# Fifth-order Bessel filter

This demo implements several orders of active bessel-approximation
filter.

The circuit was generated using the Qucs utility "qucsactivefilter,"
with all the default parameters used except for the approximation
type, which was set to "Bessel," and the order, which is set to 5, 7,
9, and 10 for the four examples here.  Filter type is low-pass, and
filter topology is Multifeedback.  After parameters are chosen, one
clicks "Calculate and copy to clipboard," then pastes the result into
qucs.


## This example contains workarounds for older versions of Xyce

The circuit that is put into the clipboard by qucsactivefilter has a
DC and AC simulation object, and also an "Equation" block.  It will
run exactly as is using Xyce, but the equation block will be ignored.
Since the equation block is intended to compute the transfer function,
it is useful to modify the schematic slightly so that Xyce can compute
the same function.

Since Xyce's original expression library could not do complex
arithmetic, it was necessary to express the transfer function
K=db(V(out)/V(in)) in terms of real and imaginary parts, and allow the
expression library to evaluate it in real arithmetic.

The real-equivalent expression is:

    K = dB(V(out)/V(in))
       = 20*log10(abs(vout/vin))
       = 20*log10(sqrt( (vout/vin)*(complex conjugate)))
       = 10*log10( [(Re(out)*Re(in)+Im(out)*Im(in))**2
                    - (Im(out)*Re(in)-Re(out)*Im(in))**2]
                    /(Re(out)**2+Re(in)**2)**2)

Recognizing this, we can replace the "AC Simulation" object with a
"Xyce Script" object that combines the ac analysis, .print line, and
expression evaluation in a single Qucs object.

The good news here is that since every schematic produced by
qucsactivefilter uses the same names for input and output nodes (IN
and OUT), this exact "Xyce Script" object can simply be cut and pasted
into every such schematic as a drop-in replacement for the "AC
Simulation" and "Equation" objects.  One will have to adjust the
parameters of the AC analysis by editing the Xyce Script object as
needed.

While this real-equivalent computation still works, newer versions of
Xyce (since around Xyce 7.4) can do complex arithmetic in expressions,
and one could simply print "{db(V(out)/V(in)}" on a print line
instead.  I have updated all the examples here to use this formulation
instead of the more complicated real-equivalent expansion.
