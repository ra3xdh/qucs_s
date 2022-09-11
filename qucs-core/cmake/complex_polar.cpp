/*
 * Check whether you have the complex polar (double, double) function.
 */

# include <complex>
using namespace std;

int main()
{
  /* In order to get an error with g++ need to take real */
  double d;
  d=real(polar(double(1.0),double(2.0)));
  return 0;
}
