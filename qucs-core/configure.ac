#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.64)

dnl Read version from file
m4_define([QUCS_VERSION], m4_esyscmd([tr -d '\n' < VERSION]))

AC_INIT([qucsator], [QUCS_VERSION], [qucs-bugs@lists.sourceforge.net], [], [https://sf.net/p/qucs])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
LT_PREREQ([2.2.2])
dnl use AM_PROG_AR if present
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([disable-static dlopen win32-dll])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/circuit.cpp])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl AM_INIT_AUTOMAKE does several things, including defining VERSION and PACKAGE
AM_INIT_AUTOMAKE([1.12 no-define])

AC_PREFIX_DEFAULT([/usr/local])
test "x$prefix" = xNONE && prefix="/usr/local"

dnl Checks for programs.
AC_PROG_CXX(clang++ g++)
AX_CXX_COMPILE_STDCXX_11
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_CC_C_O

AX_COMPILER_VENDOR
AC_LANG_PUSH(C++)
AX_COMPILER_VENDOR
AC_LANG_POP(C++)

dnl Check for Win32.
case $host_os in
  *mingw* | *msys* ) WIN32=yes ;;
  *)       WIN32=no  ;;
esac
AM_CONDITIONAL(COND_WIN32, test x$WIN32 = xyes)

dnl check for compiler problem
# see http://llvm.org/bugs/show_bug.cgi?id=13530
m4_define([CHECK_CLANG13530_PROGRAM],dnl
	  [AC_LANG_PROGRAM([
			     #include <iostream>
			   ],
			   [std::cout << "Hello, world!" << std::endl;])])
AC_MSG_CHECKING([whether c++ compiler is affected by clang bug 13530])
AC_LANG_PUSH(C++)
AS_IF([test "x$ax_cv_cxx_compiler_vendor" = "xclang"],
      [dnl
	AC_COMPILE_IFELSE(dnl
		[CHECK_CLANG13530_PROGRAM],
		[dnl
		  clang13530_workarround_needed="no"
		],
		[dnl
		  AX_SAVE_FLAGS
		  CXXFLAGS="$CXXFLAGS -D__STRICT_ANSI__"
		  AC_COMPILE_IFELSE(dnl
			[CHECK_CLANG13530_PROGRAM],
			[dnl
			   clang13530_workarround_needed="yes"
			],
			[AC_MSG_FAILURE([Cannot compile test suite for clang 13530 even with workaround])])
		  AX_RESTORE_FLAGS
		]dnl
	)dnl
      ],
      [dnl
	clang13530_workarround_needed="no"dnl
      ])
AS_IF([test "x$clang13530_workarround_needed" = "xyes"],
      [AX_APPEND_COMPILE_FLAGS([-D__STRICT_ANSI__],CXXFLAGS)],
      [])
AC_LANG_POP(C++)
AC_MSG_RESULT([$clang13530_workarround_needed])

dnl Check for parser and lexer generators.
AC_PROG_YACC
AC_PROG_LEX

lexver_req="2.5.9";

if test "x$LEX" != "x:"; then
  if test yes = yes; then
    AC_MSG_CHECKING([for $LEX >= $lexver_req])
    [lexver=`eval $LEX --version 2>/dev/null | head -1 | sed 's/[^0-9\.]//g'`]
    if test -n "$lexver"; then
      AS_VERSION_COMPARE(${lexver},${lexver_req},
	    [cmp=-1],[cmp=0],[cmp=1])
      AS_IF([test "${cmp}" != "-1"],
	    [AC_MSG_RESULT([yes])],
	    [AC_MSG_RESULT([no])
	     AC_MSG_ERROR([need $LEX >= $lexver_req])])
    else
      AC_MSG_RESULT([not identified])
    fi
    unset lexver
  fi
  AC_MSG_CHECKING([whether $LEX accepts --nounistd])
  cat <<EOF > lextest.l
%%
token     action;
EOF
  lexout="`$LEX --nounistd -olextest.c lextest.l 2>&1`"
  rm -f lextest.l lextest.c
  if test "x$lexout" != "x"; then
    AC_MSG_RESULT([no])
  else
    AC_MSG_RESULT([yes])
    LEX="$LEX --nounistd"
  fi
  unset lexout
fi

dnl Check for debugging.
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug],
		 [include debug output @<:@default=no@:>@]),
  [case "$enableval" in
   yes) enable_debug="yes" ;;
   no)  enable_debug="no"  ;;
   *)   enable_debug="no"  ;;
   esac],
  [enable_debug="no"])
if test "$enable_debug" = yes; then
  AC_DEFINE(DEBUG, 1, [Define if debug output should be supported.])
else
  AC_DEFINE(NDEBUG, 1, [Define if debug code should be suppressed.])
  if test "x$GCC" = xyes; then
    CFLAGS="`echo $CFLAGS | sed -e 's/\-g //g'`"
    CXXFLAGS="`echo $CXXFLAGS | sed -e 's/\-g //g'`"
# disable stripping (debian bug #437876)
#    case $host_os in
#      *darwin*) LDFLAGS="$LDFLAGS" ;;
#      *)        LDFLAGS="$LDFLAGS -s" ;;
#    esac
  fi
fi

dnl append -O0 to CXXFLAGS if in debug mode and using gcc
dnl to assist with debugging by preventing optimisation.
dnl gcc will use the final invocation of -OX and ignore earlier
dnl values
if test "$enable_debug" = yes; then
 if test "x$GCC" = xyes; then
    CXXFLAGS="$CXXFLAGS -O0"
    AC_MSG_NOTICE([Appending gcc optimisation flag -O0 due to --enable-debug setting.])
 fi
fi
unset enable_debug


dnl MacOSX build and runtime environment options
dnl   borrowed from http://cgit.freedesktop.org/libreoffice/core/tree/configure.ac

dnl Default-disabled search for SDK
AC_ARG_ENABLE([sdk],
  AS_HELP_STRING([--enable-sdk],
		 [search for Mac OSX SDK @<:@default=yes@:>@. \
		  Disable this option to set custom compiler flags CXX, CXXFLAGS]),
  [case "$enableval" in
   yes) enable_sdk="yes" ;;
   no)  enable_sdk="no"  ;;
   *)   enable_sdk="yes"  ;;
   esac],
  [enable_sdk="no"])

dnl Specify version of SDK
AC_ARG_WITH(macosx-sdk,
    AS_HELP_STRING([--with-macosx-sdk],
	[Use a specific SDK for building.])
    [
			  Usage:     --with-macosx-sdk=<version>

			  e. g.: --with-macosx-sdk=10.6

			  there are 3 options to control the MacOSX build:
			  --with-macosx-sdk (referred to as 'sdk' below)
			  --with-macosx-version-min-required (referred to as 'min' below)
			  --with-macosx-version-max-allowed (referred to as 'max' below)

			  the connection between these value and the default they take is as follow:
			  ( ? means not specified on the command line, s means the SDK version found,
			  constraint: x <= y <= z)

			  ==========================================
			   command line      || config result
			  ==========================================
			  min  | max  | sdk  || min  | max  | sdk  |
			  ?    | ?    | ?    || 10.6 | 10.s | 10.s |
			  ?    | ?    | 10.x || 10.6 | 10.x | 10.x |
			  ?    | 10.x | ?    || 10.6 | 10.s | 10.s |
			  ?    | 10.x | 10.y || 10.6 | 10.x | 10.y |
			  10.x | ?    | ?    || 10.x | 10.s | 10.s |
			  10.x | ?    | 10.y || 10.x | 10.y | 10.y |
			  10.x | 10.y | ?    || 10.x | 10.y | 10.y |
			  10.x | 10.y | 10.z || 10.x | 10.y | 10.z |


			  see: http://developer.apple.com/library/mac/#technotes/tn2064/_index.html
			  for a detailed technical explanation of these variables

			  Note: MACOSX_DEPLOYMENT_TARGET will be set to the value of 'min'.

			  Note that even if in theory using a --with-macosx-version-max-allowed
			  (i.e. the MAC_OS_X_VERSION_MAX_ALLOWED macro) less than the SDK version
			  should work, in practice Apple does not seem to test that, and at least
			  compiling with -DMAC_OS_X_VERSION_MAX_ALLOWED=1060 against the 10.7 SDK
			  fails in a couple of places. Just because of oversights in ifdefs in the SDK
			  headers, but still.
    ],
,)

AC_ARG_WITH(macosx-version-min-required,
    AS_HELP_STRING([--with-macosx-version-min-required],
	[set the minimum OS version needed to run the built Qucsator])
    [
			  Usage:     --with-macosx-version-min-required=<version>

			  e. g.: --with-macosx-version-min-required=10.6
			  see --with-macosx-sdk for more info
    ],
,)

AC_ARG_WITH(macosx-version-max-allowed,
    AS_HELP_STRING([--with-macosx-version-max-allowed],
	[set the maximum allowed OS version the Qucsator compilation can use APIs from])
    [
			  Usage:     --with-macosx-version-max-allowed=<version>

			  e. g.: --with-macosx-version-max-allowed=10.6
			  see --with-macosx-sdk for more info
    ],
,)




case $host_os in
  *linux* | *cygwin* )

  dnl Set Clang
  AS_IF([test "x$ax_cv_cxx_compiler_vendor" = "xclang"],
	[CFLAGS="$CFLAGS -pipe"
	 CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -D__STRICT_ANSI__"
	 LDFLAGS="$LDFLAGS -ldl"
	 use_CLANG="yes"],[])

  dnl Set GCC
  AS_IF([test "x$use_CLANG" != "xyes" -a "x$GCC" = "xyes"],
	[CFLAGS="$CFLAGS -pipe"
	 CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -fno-check-new -ldl -rdynamic"
	 LDFLAGS="$LDFLAGS -ldl"],[])

  if test "$enable_debug" = yes; then
    CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
    CXXFLAGS="$CXXFLAGS -W -Wall"
  fi
  ;;

  *darwin* )

  dnl ===================================================================
  dnl Test if search for SDK was enabled
  AS_IF([test "x$enable_sdk" = "xyes"], [

  dnl Check OS X SDK and compiler
  dnl borrowed from http://cgit.freedesktop.org/libreoffice/core/tree/configure.ac
  AC_MSG_CHECKING([what Mac OS X SDK to use])

  bitness=-m64

  if test -z "$with_macosx_sdk"; then
      if test -d /Developer/SDKs/MacOSX10.6.sdk; then
	  with_macosx_sdk=10.6
      elif test -d /Developer-old/SDKs/MacOSX10.6.sdk; then
	  with_macosx_sdk=10.6
      elif test -d /Xcode3/SDKs/MacOSX10.6.sdk; then
	  with_macosx_sdk=10.6
      elif test -d /Developer/SDKs/MacOSX10.7.sdk; then
	  with_macosx_sdk=10.7
      elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"; then
	  with_macosx_sdk=10.6
      elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk"; then
	  with_macosx_sdk=10.7
      elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk"; then
	  with_macosx_sdk=10.8
      elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"; then
	  with_macosx_sdk=10.9
      elif test -x /usr/bin/xcode-select; then
	  xcodepath="`xcode-select -print-path`"
	  if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"; then
	      with_macosx_sdk=10.6
	  elif test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk"; then
	      with_macosx_sdk=10.7
	  elif test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk"; then
	      with_macosx_sdk=10.8
	  elif test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"; then
	      with_macosx_sdk=10.9
	  fi
      fi
      if test -z "$with_macosx_sdk"; then
	  AC_MSG_ERROR([Could not figure out the location of a Mac OS X SDK and its version])
      fi
  fi

  case $with_macosx_sdk in
  10.5)
      MACOSX_SDK_VERSION=1050
      ;;
  10.6)
      MACOSX_SDK_VERSION=1060
      ;;
  10.7)
      MACOSX_SDK_VERSION=1070
      ;;
  10.8)
      MACOSX_SDK_VERSION=1080
      ;;
  10.9)
      MACOSX_SDK_VERSION=1090
      ;;
  10.10)
      MACOSX_SDK_VERSION=101000
      ;;
  *)
      AC_MSG_ERROR([with-macosx-sdk $with_macosx_sdk is not a supported value, supported values are 10.5--10])
      ;;
  esac

  # Next find it (again, if we deduced its version above by finding
  # it... but we need to look for it once more in case
  # --with-macosx-sdk was given so that the above search did not
  # happen).
  if test -z "$MACOSX_SDK_PATH"; then
      case $with_macosx_sdk in
      10.5)
	  if test -x /usr/bin/xcode-select; then
	      xcodepath="`xcode-select -print-path`"
	      if test -d "$xcodepath/SDKs/MacOSX$with_macosx_sdk.sdk"; then
		  MACOSX_SDK_PATH="$xcodepath/SDKs/MacOSX$with_macosx_sdk.sdk"
	      fi
	  elif test -d /Developer/SDKs/MacOSX10.5.sdk; then
	      MACOSX_SDK_PATH=/Developer/SDKs/MacOSX10.5.sdk
	  elif test -d /Developer-old/SDKs/MacOSX10.5.sdk; then
	      MACOSX_SDK_PATH=/Developer-old/SDKs/MacOSX10.5.sdk
	  elif test -d /Xcode3/SDKs/MacOSX10.5.sdk; then
	      MACOSX_SDK_PATH=/Xcode3/SDKs/MacOSX10.5.sdk
	  fi
	  ;;
      10.6)
	  if test -d /Developer/SDKs/MacOSX10.6.sdk; then
	      MACOSX_SDK_PATH=/Developer/SDKs/MacOSX10.6.sdk
	  elif test -d /Developer-old/SDKs/MacOSX10.6.sdk; then
	      MACOSX_SDK_PATH=/Developer-old/SDKs/MacOSX10.6.sdk
	  elif test -d /Xcode3/SDKs/MacOSX10.6.sdk; then
	      MACOSX_SDK_PATH=/Xcode3/SDKs/MacOSX10.6.sdk
	  elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
	      MACOSX_SDK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk
	  elif test -x /usr/bin/xcode-select; then
	      xcodepath="`xcode-select -print-path`"
	      if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
		  MACOSX_SDK_PATH="$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"
	      fi
	  fi
	  ;;
      10.7)
	  if test -d /Developer/SDKs/MacOSX$with_macosx_sdk.sdk; then
	      MACOSX_SDK_PATH=/Developer/SDKs/MacOSX$with_macosx_sdk.sdk
	  elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
	      MACOSX_SDK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk
	  elif test -x /usr/bin/xcode-select; then
	      xcodepath="`xcode-select -print-path`"
	      if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
		  MACOSX_SDK_PATH="$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"
	      fi
	  fi
	  ;;
      10.8|10.9|10.10)
	  xcodepath="`xcode-select -print-path`"
	  if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
	      MACOSX_SDK_PATH="$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"
	  fi
	  ;;
      esac
      if test -z "$MACOSX_SDK_PATH"; then
	  AC_MSG_ERROR([Could not figure out the location of Mac OS X $with_macosx_sdk SDK])
      fi
  fi
  AC_MSG_RESULT([SDK $with_macosx_sdk at $MACOSX_SDK_PATH])

  if test "$with_macosx_version_min_required" = ""; then
      case $with_macosx_sdk in
      10.5)
	  with_macosx_version_min_required="10.5";;
      *)
	  with_macosx_version_min_required="10.6";;
      esac
  fi

  if test "$with_macosx_version_max_allowed" = ""; then
      with_macosx_version_max_allowed="$with_macosx_sdk"
  fi

  # export this so that "xcrun" invocations later return matching values
  DEVELOPER_DIR="${MACOSX_SDK_PATH%/SDKs*}"
  DEVELOPER_DIR="${DEVELOPER_DIR%/Platforms*}"
  export DEVELOPER_DIR
  FRAMEWORKSHOME="$MACOSX_SDK_PATH/System/Library/Frameworks"
  MACOSX_DEPLOYMENT_TARGET="$with_macosx_version_min_required"

  case "$with_macosx_version_min_required" in
  10.5)
      MAC_OS_X_VERSION_MIN_REQUIRED="1050"
      ;;
  10.6)
      MAC_OS_X_VERSION_MIN_REQUIRED="1060"
      ;;
  10.7)
      MAC_OS_X_VERSION_MIN_REQUIRED="1070"
      ;;
  10.8)
      MAC_OS_X_VERSION_MIN_REQUIRED="1080"
      ;;
  10.9)
      MAC_OS_X_VERSION_MIN_REQUIRED="1090"
      ;;
  10.10)
      MAC_OS_X_VERSION_MIN_REQUIRED="101000"
      ;;
  *)
      AC_MSG_ERROR([with-macosx-version-min-required $with_macosx_version_min_required is not a supported value, supported values are 10.5--10])
      ;;
  esac

  if test "$BITNESS_OVERRIDE" = ""; then
      case "$with_macosx_version_min_required" in
      10.5)
	  case "$with_macosx_sdk" in
	  10.5)
	      ;;
	  *)
	      AC_MSG_WARN([Building with a SDK > 10.5 possibly breaks 10.5 compatibility.])
	      add_warning "Building with a SDK > 10.5 possibly breaks 10.5 compatibility."
	      ;;
	  esac
	  ;;
      esac
  fi

  # If no CC and CXX environment vars, try to guess where the compiler is
  #LIBTOOL=libtool
  INSTALL_NAME_TOOL=install_name_tool
  if test -z "$save_CC"; then
      AC_MSG_CHECKING([what compiler to use])
      case $with_macosx_sdk in
      10.5)
	  CC="${gccprefix}gcc-4.2 $arch -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	  CXX="${gccprefix}g++-4.2 $arch -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	  INSTALL_NAME_TOOL=`xcrun -find install_name_tool`
	  ;;
      10.6)
	  # did someone copy her 10.6 sdk into xcode 4 (needed on Mountain Lion)?
	  if test "$(echo $MACOSX_SDK_PATH | cut -c1-23)" = "/Applications/Xcode.app"; then
	      CC="`xcrun -find gcc` $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	      CXX="`xcrun -find g++` $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	  else
	      CC="gcc-4.2 $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	      CXX="g++-4.2 $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	  fi
	  INSTALL_NAME_TOOL=`xcrun -find install_name_tool`
	  #LIBTOOL=libtool
	  ;;
      10.7|10.8|10.9|10.10)
	  if test "$with_macosx_version_min_required" != 10.6; then
	      # Use libc++ instead of libstdc++ when possible
	      stdlib=-stdlib=libc++
	  fi
	  #if test "$ENABLE_LTO" = TRUE; then
	  #    lto=-flto
	  #fi
	  CC="`xcrun -find clang` $bitness $lto -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	  CXX="`xcrun -find clang++` $bitness $lto $stdlib -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
	  INSTALL_NAME_TOOL=`xcrun -find install_name_tool`
	  AR=`xcrun -find ar`
	  NM=`xcrun -find nm`
	  STRIP=`xcrun -find strip`
	  #LIBTOOL=`xcrun -find libtool`
	  RANLIB=`xcrun -find ranlib`
	  ;;
      esac
      AC_MSG_RESULT([$CC and $CXX])
  fi

  case "$with_macosx_version_max_allowed" in
  10.5)
      MAC_OS_X_VERSION_MAX_ALLOWED="1050"
      ;;
  10.6)
      MAC_OS_X_VERSION_MAX_ALLOWED="1060"
      ;;
  10.7)
      MAC_OS_X_VERSION_MAX_ALLOWED="1070"
      ;;
  10.8)
      MAC_OS_X_VERSION_MAX_ALLOWED="1080"
      ;;
  10.9)
      MAC_OS_X_VERSION_MAX_ALLOWED="1090"
      ;;
  10.10)
      MAC_OS_X_VERSION_MAX_ALLOWED="101000"
      ;;
  *)
      AC_MSG_ERROR([with-macosx-version-max-allowed $with_macosx_version_max_allowed is not a supported value, supported values are 10.5--10])
      ;;
  esac

  AC_MSG_CHECKING([that macosx-version-min-required is coherent with macosx-version-max-allowed])
  if test $MAC_OS_X_VERSION_MIN_REQUIRED -gt $MAC_OS_X_VERSION_MAX_ALLOWED; then
      AC_MSG_ERROR([the version minimum required must be inferior or equal to the version maximum allowed])
  else
      AC_MSG_RESULT([ok])
  fi

  AC_MSG_CHECKING([that macosx-version-max-allowed is coherent with macos-with-sdk])
  if test $MAC_OS_X_VERSION_MAX_ALLOWED -gt $MACOSX_SDK_VERSION; then
      AC_MSG_ERROR([the version maximum allowed cannot be greater than the sdk level])
  else
      AC_MSG_RESULT([ok])
  fi
  AC_MSG_NOTICE([MAC_OS_X_VERSION_MIN_REQUIRED=$MAC_OS_X_VERSION_MIN_REQUIRED])
  AC_MSG_NOTICE([MAC_OS_X_VERSION_MAX_ALLOWED=$MAC_OS_X_VERSION_MAX_ALLOWED])


  if test "$enable_debug" = yes; then
    CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
    CXXFLAGS="$CXXFLAGS -W -Wall"
  fi

  dnl End test for enable_sdk
  ])

  ;;

  *mingw* )
  if test "x$GCC" = xyes; then
    CFLAGS="$CFLAGS -pipe"
    CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -fno-check-new"
    if test "$enable_debug" = yes; then
      CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
      CXXFLAGS="$CXXFLAGS -W -Wall"
    fi
  fi
  ;;
esac

dnl Check for new platforms.
osversion="`grep \"$host\" $srcdir/doc/PLATFORMS 2>/dev/null`"
if test "x$osversion" = "x"; then
  AC_MSG_RESULT([
  The $PACKAGE package has never been compiled or run on this platform.
  Please send an email to <qucs-devel@lists.sourceforge.net> including your
  compiler version and whether it worked correctly or not.  '$host'
  will then be added to the PLATFORMS file included in this package.
])
fi
unset osversion

dnl Check for additional programs in PATH
AC_PATH_PROG([GPERF],[gperf],[])
AS_IF([test -z "$GPERF"],
      [AC_MSG_ERROR([please install gperf])])

dnl check for adms (queued for removal)
dnl The default behaviour is "check for adms, if it's not there,
dnl error out.
dnl override with --with-mkadms=alternative_executable
ADMSXML=admsXml
AC_ARG_WITH([mkadms],
  [AS_HELP_STRING([--with-mkadms], [specify alternative admsXml])],
  [ADMSXML=$withval],)

AC_MSG_CHECKING([Checking if ${ADMSXML} is working])
AS_IF([${ADMSXML} --version > /dev/null 2>&1], , ADMSXML="")
AS_IF([test -z $ADMSXML],
  [ AC_MSG_RESULT([ no ])
    ADMSXML=no],
  [ AC_MSG_RESULT([ yes ])])

AS_IF([test "$ADMSXML" = no],
  [AC_MSG_ERROR([need admsXml, but not found. consider --with-mkadms=<alternative admsXml>])])
AC_SUBST([ADMSXML])

dnl Checks for libraries.
AC_CHECK_LIB(m, sin)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h ieeefp.h])

dnl gtest.h, Google Test support
AC_LANG_PUSH(C++)
GTEST_CPPFLAGS=

dnl use if available, don't use otherwise.
AC_ARG_WITH([gtest], AS_HELP_STRING(
    [--with-gtest=DIR], [Run tests that are implemented for gtest.]
   ),
 [
 AS_IF([test "x$with_gtest" != "xno"], [
  GTEST_CPPFLAGS="-I$withval/include -I$withval"
  with_gtest=manual
  ])
 ],
[
  AC_CHECK_HEADER([gtest/gtest.h],
		  [ with_gtest=yes ],
		  [ with_gtest=no ])
 ]
)

# don't know how to check. fix later.
# GTEST_VERSION="1.7.0"

AM_CONDITIONAL(USE_GTEST, test "x$with_gtest" != "xno")

AS_IF([test "x$with_gtest" == "xmanual"], [
  OLD_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS+=" $GTEST_CPPFLAGS"
  AC_CHECK_HEADER([gtest/gtest.h],,[AC_MSG_ERROR([cannot find gtest headers])])
  CPPFLAGS="$OLD_CPPFLAGS"
])
AC_LANG_POP()

AC_SUBST(GTEST_CPPFLAGS)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Check for type sizes.
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])

dnl Check for double type.
AC_ARG_ENABLE([double],
  AS_HELP_STRING([--enable-double=TYPE],
		 [type of double representation @<:@default=double@:>@]),
  [case "$enableval" in
   no | yes | double)
     double_type="double"
     double_size=8
     ;;
   ldouble)
     double_type="long double"
     AC_CHECK_SIZEOF([long double],12)
     double_size=$ac_cv_sizeof_long_double
     ;;
   float)
     double_type="float"
     double_size=4
     ;;
   *)
     AC_MSG_ERROR([valid types are double, float and ldouble.])
     ;;
   esac],
  [double_type="double"; double_size=8])
AC_DEFINE_UNQUOTED(nr_double_t, $double_type,
  [The global type of double representation.])
AC_SUBST(QUCS_DOUBLE_TYPE, $double_type)
AC_DEFINE_UNQUOTED(NR_DOUBLE_SIZE, $double_size,
  [The size of the double representation.])
AC_SUBST(QUCS_DOUBLE_SIZE, $double_size)
unset double_type
unset double_size

AC_MSG_NOTICE([Check functions in the global namespace])
dnl http://www.cplusplus.com/reference/cmath/

# Trigonometric functions
AC_CHECK_FUNCS([ \
cos \
sin \
tan \
acos \
asin \
atan \
atan2 \
])

#Hyperbolic functions
AC_CHECK_FUNCS([ \
cosh \
sinh \
tanh \
acosh \
asinh \
atanh \
])

#Exponential and logarithmic functions
AC_CHECK_FUNCS([ \
exp \
log \
log10 \
modf \
])

#Power functions
AC_CHECK_FUNCS([ \
pow \
sqrt \
hypot \
])

#Error and gamma functions
AC_CHECK_FUNCS([ \
erf \
erfc \
])

# Rounding and remainder functions
AC_CHECK_FUNCS([ \
ceil \
floor \
fmod \
trunc \
round \
])

# Other functions
AC_CHECK_FUNCS([ \
fabs \
])

# Bessel functions (libc ?)
AC_CHECK_FUNCS([ jn yn ])

# C strings
# \bug strdup not in C++ STL
AC_CHECK_FUNCS([ strdup strerror strchr])

dnl Checks for complex classes and functions.
AX_CXX_NAMESPACES
AS_VAR_IF([ax_cv_cxx_namespaces],[yes],
	 [],
	 [AC_MSG_ERROR([require a c++ namespace])])
AX_CXX_HAVE_NUMERIC_LIMITS
AS_VAR_IF([ax_cv_cxx_have_numeric_limits],[yes],
	 [],
	 [AC_MSG_ERROR([require a c++ std::numeric_limits<> class])])

AC_CXX_HAVE_COMPLEX
AS_IF([test "X$ac_cv_cxx_have_complex" = "X"],[AC_MSG_ERROR([Qucs needs a templated complex class])])

AX_CXX_ERASE_ITERATOR_TYPE


AC_MSG_NOTICE([Check complex transcendental overload in namespace std])
dnl http://www.cplusplus.com/reference/complex/

AC_CHECK_CXX_COMPLEX_FUNCS([ \
  cos \
  cosh \
  exp \
  log \
  log2 \
  log10 \
dnl  pow \ skip, see below
  sin \
  sinh \
  sqrt \
  tan \
  tanh \
dnl C++11
  acos \
  acosh \
  asin \
  asinh \
  atan \
  atanh ])

dnl Check complex values in namespace std
AC_CHECK_CXX_COMPLEX_FUNCS([ \
  real \
  imag \
  abs \
  arg \
  norm \
  conj \
dnl  polar \   skip, see below
dnl C++11
  proj ])

dnl functions with two arguments
AC_CHECK_CXX_COMPLEX_POW
AC_CHECK_CXX_COMPLEX_ATAN2
AC_CHECK_CXX_COMPLEX_FMOD
AC_CHECK_CXX_COMPLEX_POLAR
AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX


dnl Depending on the implementation, functions may or may not be included on the std namespace
AC_MSG_NOTICE([Check for functions in std:: namespace])
 for f in std::acosh std::asinh std::atanh std::hypot std::trunc std::round std::erf std::erfc; do
    AC_MSG_CHECKING([for $f])
    AC_LINK_IFELSE([AC_LANG_SOURCE(
				  [[#include <cmath>
				  int main() { return $f(0.7) ? 1 : 0; }
				  ]])],
      [AC_MSG_RESULT([yes])
       if test $f = "std::acosh"; then
	 AC_DEFINE(HAVE_STD_ACOSH, 1,
		     [Define to 1 if you have the `std::acosh` function.])

       elif test $f = "std::asinh"; then
	 AC_DEFINE(HAVE_STD_ASINH, 1,
	       [Define to 1 if you have the `std::asinh` function.])

       elif test $f = "std::atanh"; then
	 AC_DEFINE(HAVE_STD_ATANH, 1,
	       [Define to 1 if you have the `std::atanh` function.])

       elif test $f = "std::hypot"; then
	 AC_DEFINE(HAVE_STD_HYPOT, 1,
	       [Define to 1 if you have the `std::hypot` function.])

       elif test $f = "std::fmod"; then
	 AC_DEFINE(HAVE_STD_FMOD, 1,
	 [Define to 1 if you have the `std::fmod` function.])

       elif test $f = "std::trund"; then
	 AC_DEFINE(HAVE_STD_TRUNC, 1,
	 [Define to 1 if you have the `std::trunc` function.])

       elif test $f = "std::round"; then
	 AC_DEFINE(HAVE_STD_ROUND, 1,
		     [Define to 1 if you have the `std::round` function.])

       elif test $f = "std::erf"; then
	 AC_DEFINE(HAVE_STD_ERF, 1,
	       [Define to 1 if you have the `std::erf` function.])

       elif test $f = "std::erfc"; then
	 AC_DEFINE(HAVE_STD_ERFC, 1,
	       [Define to 1 if you have the `std::erfc` function.])
       fi],
      [AC_MSG_RESULT([no])])
  done


dnl BUG. remove option. always run tests
dnl FIXME: skip tests that are not applicable (and be verbose about that).
AC_ARG_ENABLE([qucs-test],
  AS_HELP_STRING([--enable-qucs-test], [Enable running qucs test project]))
AM_CONDITIONAL([USE_QUCS_TEST], [test "$enable_qucs_test" = yes])

# enable gcov
AX_CODE_COVERAGE
AS_IF([ test "$enable_code_coverage" = "yes" ],
      [AX_APPEND_COMPILE_FLAGS([--coverage],CFLAGS)
      AX_APPEND_COMPILE_FLAGS([-fno-inline],CFLAGS)
      AX_APPEND_COMPILE_FLAGS([--coverage],CXXFLAGS)
      AX_APPEND_COMPILE_FLAGS([-fno-inline],CXXFLAGS)
      AX_APPEND_LINK_FLAGS([--coverage],LDFLAGS)
      AX_APPEND_LINK_FLAGS([-lgcov],LDFLAGS)
      ],[])

dnl Definitions at top of <config.h>.
AH_TOP([
/* __BEGIN_DECLS should be used at the beginning of your declarations,
   so that C++ compilers do not mangle their names.  Use __END_DECLS at
   the end of C declarations. */
#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
])



dnl Add here all your Makefiles. These are created by configure.
AC_CONFIG_FILES([Makefile
		 qucs_typedefs.h
		 doc/Makefile
		 src/Makefile
		 src/math/Makefile
		 src/interface/Makefile
		 src/converter/Makefile
		 src/components/Makefile
		 src/components/microstrip/Makefile
		 src/components/devices/Makefile
		 src/components/digital/Makefile
		 src/components/verilog/Makefile
		 tests/Makefile
                 doc/qucsator.1
                 doc/qucsconv.1
		 ])


dnl Check for Git short SHA to tag version
dnl The release package should also keep it on the config.h
AC_PATH_PROG([GIT],[git],[])
AS_IF([test -d "../.git"],
      [AS_IF([test X"$GIT" != "X"],
             [AC_MSG_CHECKING([for git tree revision])
	      GIT_REVISION=`git log --pretty=format:'%h' -n 1u`
	      AC_DEFINE_UNQUOTED([GIT_REVISION], ["$GIT_REVISION"], [Git short revision hash.])
	      AC_MSG_RESULT($GIT_REVISION)])])

AC_OUTPUT

dnl Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION ($GIT_REVISION) configured successfully.])
AC_MSG_RESULT([
Configure Information:
  AdmsXml           : $ADMSXML

  C Compiler        : $CC
    DEFS            :   $DEFS
    CPPFLAGS        :   $CPPFLAGS
    CFLAGS          :   $CFLAGS

  C++ Compiler      : $CXX
    DEFS            :   $DEFS
    CPPFLAGS        :   $CPPFLAGS
    CXXFLAGS        :   $CXXFLAGS

  Linker            : $LD
    LDFLAGS         :   $LDFLAGS
    LIBS            :   $LIBS

  gtest             : $with_gtest
    CPPFLAGS        :   $GTEST_CPPFLAGS

    Prefix          : $prefix
])

# vim:ts=8:sw=2:noet
