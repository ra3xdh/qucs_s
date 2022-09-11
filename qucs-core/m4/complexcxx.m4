# Test C++ complex implementation
#
# Copyright (C) 2008 Bastien ROUCARIES <bastien.roucaries+qucs@gmail.com>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright notice
# and this notice are preserved.
#
# This script use script from http://autoconf-archive.cryp.to/
# under the following licence
#
# Copyright © 2004 Todd Veldhuizen
# Copyright © 2004 Luc Maisonobe <luc@spaceroots.org>
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright notice
# and this notice are preserved.

dnl Tests for the presence of a std::complex class
AC_DEFUN([AC_CXX_HAVE_COMPLEX],
[AC_CACHE_CHECK(whether the compiler has complex<T>,
  ac_cv_cxx_have_complex,
  [AC_LANG_PUSH(C++)
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                        #include <complex>
                        using namespace std;
                        ]],
                        [[
                        complex<float> a;
                        complex<double> b;
                        abs(b);
                        real(b);
                        imag(b);
                        arg(b);
                        conj(b);
                        return 0; ]])],
   ac_cv_cxx_have_complex=yes, ac_cv_cxx_have_complex=no)
   AC_LANG_POP
  ])
  if test "$ac_cv_cxx_have_complex" = yes; then
    AC_DEFINE(HAVE_COMPLEX,[1],[Define if the compiler has complex<T>.])
  fi
])


dnl equivalent of AC_CHECK_FUNC
AC_DEFUN([AC_CHECK_CXX_COMPLEX_FUNC],
[AC_REQUIRE([AC_CXX_HAVE_COMPLEX])
 AS_VAR_PUSHDEF([ac_var], [ac_cv_cpp_complex_func_$1])dnl
 AC_CACHE_CHECK([for complex $1], [ac_var],
[AC_LANG_PUSH(C++)
 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                      #include <complex>
                      using namespace std;
                      #ifdef log2
                      #undef log2
                      #endif
                      ]],
                      [[
                      complex<double> a;
                      $1(a);
                      return 0;  ]])],
                   [AS_VAR_SET([ac_var], [yes])],
                   [AS_VAR_SET([ac_var], [no])])])
 AS_VAR_IF([ac_var], [yes], [$2], [$3])dnl
 AS_VAR_POPDEF([ac_var])dnl
])


m4_define([_AH_CHECK_CXX_COMPLEX_FUNCS],
[m4_foreach_w([AC_Func], [$1],
   [AH_TEMPLATE(AS_TR_CPP([HAVE_CXX_COMPLEX_]m4_defn([AC_Func])),
      [Define to 1 if you have the complex `]m4_defn([AC_Func])[' function.])])])

dnl equivalent of AC_CHECK_FUNCS
AC_DEFUN([AC_CHECK_CXX_COMPLEX_FUNCS],
[_AH_CHECK_CXX_COMPLEX_FUNCS([$1])dnl
for ac_func in $1; do
AC_CHECK_CXX_COMPLEX_FUNC($ac_func,
              [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_CXX_COMPLEX_$ac_func])) $2],
              [$3]);dnl
done
])

dnl equivalent of AC_CHECK_FUNC, but for checking for
dnl complex functions in the
AC_DEFUN([AC_CHECK_CXX_COMPLEX_POW],
  [AC_REQUIRE([AC_CXX_HAVE_COMPLEX])
   AS_VAR_PUSHDEF([ac_var], [ac_cv_cpp_complex_func_pow])dnl
   AC_CACHE_CHECK([for complex pow], [ac_var],
   [AC_LANG_PUSH(C++)
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                        # include <complex>
                        using namespace std;
                        ]],
                        [[
                        complex<double> x,y;
                        std::pow(x,double(2.0));
                        std::pow(x, y);
                        std::pow(double(2.0), x);
                        return 0;  ]])],
                      [AS_VAR_SET([ac_var], [yes])],
                      [AS_VAR_SET([ac_var], [no])])])
  AS_VAR_IF([ac_var], [yes],
     [AC_DEFINE(HAVE_CXX_COMPLEX_POW, 1,
                [Define to 1 if you have the complex pow function.])],
          [])dnl
  AS_VAR_POPDEF([ac_var])dnl
])

dnl equivalent of AC_CHECK_FUNC
AC_DEFUN([AC_CHECK_CXX_COMPLEX_ATAN2],
[AC_REQUIRE([AC_CXX_HAVE_COMPLEX])
 AS_VAR_PUSHDEF([ac_var], [ac_cv_cpp_complex_func_atan2])dnl
 AC_CACHE_CHECK([for complex atan2], [ac_var],
 [AC_LANG_PUSH(C++)
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                       # include <complex>
                       using namespace std;
                       ]],
                       [[
                       complex<double> x(1,1),y(1,1);
                       atan2(x,double(1.0));
                       atan2(x, y);
                       atan2(double(1.0), x);
                       return 0;  ]])],
                    [AS_VAR_SET([ac_var], [yes])],
                    [AS_VAR_SET([ac_var], [no])])])
AS_VAR_IF([ac_var], [yes],
     [AC_DEFINE(HAVE_CXX_COMPLEX_ATAN2, 1,
                [Define to 1 if you have the complex atan2 function.])],
     [])dnl
AS_VAR_POPDEF([ac_var])dnl
])

dnl equivalent of AC_CHECK_FUNC
AC_DEFUN([AC_CHECK_CXX_COMPLEX_FMOD],
[AC_REQUIRE([AC_CXX_HAVE_COMPLEX])
 AS_VAR_PUSHDEF([ac_var], [ac_cv_cpp_complex_func_fmod])dnl
 AC_CACHE_CHECK([for complex fmod], [ac_var],
 [AC_LANG_PUSH(C++)
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                       # include <complex>
                       using namespace std;
                       ]],
                       [[
                       complex<double> x,y;
                       fmod(x,double(2.0));
                       fmod(x, y);
                       fmod(double(2.0), x);
                       return 0;  ]])],
                    [AS_VAR_SET([ac_var], [yes])],
                    [AS_VAR_SET([ac_var], [no])])])
AS_VAR_IF([ac_var], [yes],
     [AC_DEFINE(HAVE_CXX_COMPLEX_FMOD, 1,
                [Define to 1 if you have the complex fmod function.])],
     [])dnl
AS_VAR_POPDEF([ac_var])dnl
])


dnl equivalent of AC_CHECK_FUNC
AC_DEFUN([AC_CHECK_CXX_COMPLEX_POLAR],
[AC_REQUIRE([AC_CXX_HAVE_COMPLEX])
 AS_VAR_PUSHDEF([ac_var], [ac_cv_cpp_complex_func_polar])dnl
 AC_CACHE_CHECK([for complex polar (double, double)], [ac_var],
 [AC_LANG_PUSH(C++)
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                       # include <complex>
                       using namespace std;
                       ]],
                       [[
                       /* In order to get an error with g++ need to take real */
                       double d;
                       d=real(polar(double(1.0),double(2.0)));
                       return 0; ]])],
                    [AS_VAR_SET([ac_var], [yes])],
                    [AS_VAR_SET([ac_var], [no])])])
AS_VAR_IF([ac_var], [yes],
     [AC_DEFINE(HAVE_CXX_COMPLEX_POLAR, 1,
                [Define to 1 if you have the complex polar (double, double) function.])],
     [])dnl
AS_VAR_POPDEF([ac_var])dnl
])

dnl equivalent of AC_CHECK_FUNC
AC_DEFUN([AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX],
[AC_REQUIRE([AC_CXX_HAVE_COMPLEX])
 AS_VAR_PUSHDEF([ac_var], [ac_cv_cpp_complex_func_polar_complex])dnl
 AC_CACHE_CHECK([for complex polar (complex, complex)], [ac_var],
 [AC_LANG_PUSH(C++)
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                       # include <complex>
                       using namespace std;
                       ]],
                       [[
                       /* In order to get an error with g++ need to take real */
                       complex<double> x,y;
                       double d;
                       d=real(polar(x,y));
                       d=real(polar(x,2.0));
                       d=real(polar(2.0,y));
                       return 0; ]])],
                    [AS_VAR_SET([ac_var], [yes])],
                    [AS_VAR_SET([ac_var], [no])])])
AS_VAR_IF([ac_var], [yes],
     [AC_DEFINE(HAVE_CXX_COMPLEX_POLAR_COMPLEX, 1,
                [Define to 1 if you have the complex polar (complex, complex) function.])],
     [])dnl
AS_VAR_POPDEF([ac_var])dnl
])
