* Ngspice functions

.func limexp(x) {x<80.0?exp(x):exp(80)*(x-80)}
.func step(x) {u(x)}
.func stp(x)  {u(x)}

