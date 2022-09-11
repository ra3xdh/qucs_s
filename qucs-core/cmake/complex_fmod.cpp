/*
 * Check whether you have the complex fmod function.
 */

# include <complex>
using namespace std;

int main()
{
  complex<double> x,y;
  fmod(x,double(2.0));
  fmod(x, y);
  fmod(double(2.0), x);
  return 0;
}
