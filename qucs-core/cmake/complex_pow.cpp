/*
 * Check whether you have the complex pow function.
 */

# include <complex>
using namespace std;

int main()
{
  complex<double> x,y;
  std::pow(x,double(2.0));
  std::pow(x, y);
  std::pow(double(2.0), x);
  return 0;
}
