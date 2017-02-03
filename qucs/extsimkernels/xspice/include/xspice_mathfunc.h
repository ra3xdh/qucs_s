#ifndef XSPICE_COMPAT_H
#define XSPICE_COMPAT_H

#define D_0_step(x) (0)

#define step(x) ((x)>0.0?1.0:(((x)==0)?0.5:0.0))

#define Xpow(x,p) pow(fabs((x)),(p))

#define limexp(x) ((x)<80.0?exp(x):(exp(80.0)*((x)-80.0)))

#endif
