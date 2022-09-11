/*
 * Check whether you have the complex atan2 function.
 */

# include <complex>
using namespace std;

int main()
{
  complex<double> x(1,1),y(1,1);
  atan2(x,double(1.0));
  atan2(x, y);
  atan2(double(1.0), x);
  return 0;
}
