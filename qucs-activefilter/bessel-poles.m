#!/usr/bin/octave -qf

printf("Generating bessel.h...\n");

maxorder=20;

fid=fopen("bessel.h","w");
fprintf(fid,"#ifndef BESSEL_H\n");
fprintf(fid,"#define BESSEL_H\n");
fprintf(fid,"//Bessel coeffs table\n //Generated automatically! DO NOT EDIT!!!\n\n\n");
fprintf(fid,"\n\ndouble BesselPoles[%d][%d]={\n\n",maxorder,2*maxorder);

for n=1:maxorder;

fprintf(fid,"/* %d th order */ {\n",n);
b=1;
for k=1:n+1;
b(k)=factorial(2*n-(k-1))/((2^(n-(k-1)))*factorial(k-1)*factorial(n-(k-1)));
endfor;
b=fliplr(b);
#disp(b);
#printf("Bessel polynomial %dth order poles:",n);
poles=roots(b);
Np=length(poles);

for i=1:Np;
fprintf(fid," %f, ",real(poles(i)));
#fprintf(fid,"\t%f,\n",imag(poles(i)));
if (i==maxorder) 
fprintf(fid," %f \n",imag(poles(i)));
else 
fprintf(fid," %f, ",imag(poles(i)));
endif;
endfor;

for i=Np+1:maxorder;
#fprintf(fid," 0.0,\n");
if (i==maxorder)
fprintf(fid," 0.0 \n");
else
fprintf(fid," 0.0, ");
endif;
endfor;

if (n==maxorder)
fprintf(fid,"\t}\n");
else
fprintf(fid,"\t},\n");
endif;

endfor;

fprintf(fid,"\n};\n");
fprintf(fid,"#endif\n");
fclose(fid);

printf("bessel.h successfully generated!\n");
