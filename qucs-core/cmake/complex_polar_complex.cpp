/*
 * Check whether you have the complex polar (complex, complex) function.
 */

# include <complex>
using namespace std;

int main()
{
  /* In order to get an error with g++ need to take real */
  complex<double> x,y;
  double d;
  d=real(polar(x,y));
  d=real(polar(x,2.0));
  d=real(polar(2.0,y));
  return 0;
}
