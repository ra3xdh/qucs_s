/*
 * Check whether the compiler implements namespaces.
 */

namespace Outer {
  namespace Inner {
    int i = 0;
  }
}

int main()
{
  using namespace Outer::Inner;
  return i;
}
