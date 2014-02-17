#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(qucs,0.0.18,qucs-bugs@lists.sourceforge.net)
AC_CONFIG_SRCDIR([qucs/qucs.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define])
AM_MAINTAINER_MODE
AC_PREFIX_DEFAULT([/usr/local])
test "x$prefix" = xNONE && prefix="/usr/local"

dnl Checks for programs.
AC_PROG_CXX(clang++ g++)
AC_PROG_CC
AC_PROG_RANLIB
AC_CHECK_TOOL(AR, ar, :)

dnl Check for debugging option.
AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug],
                 [include debug output @<:@default=no@:>@]),
  [case "$enableval" in
   yes) enable_qucs_gui_debug="yes" ;;
   no)  enable_qucs_gui_debug="no"  ;;
   *)   enable_qucs_gui_debug="no"  ;;
   esac],
  [enable_qucs_gui_debug="no"])
if test $USE_MAINTAINER_MODE = yes; then
  enable_qucs_gui_debug="yes"
fi
if test "$enable_qucs_gui_debug" = yes; then
  AC_DEFINE(DEBUG, 1, [Define if debug output should be supported.])
  QT_DEF="-DQT_DEBUG"
else
  AC_DEFINE(NDEBUG, 1, [Define if debug code should be suppressed.])
  QT_DEF="-DQT_NO_DEBUG"
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

dnl Check for Linux
case $host_os in
  *linux*) LINUX=yes ;;
  *)       LINUX=no  ;;
esac
AM_CONDITIONAL(COND_LINUX, test x$LINUX = xyes)

dnl Check for Win32.
case $host_os in
  *mingw*) WIN32=yes ;;
  *)       WIN32=no  ;;
esac
AM_CONDITIONAL(COND_WIN32, test x$WIN32 = xyes)

dnl Check for MacOSX.
case $host_os in
  *darwin*) MACOSX=yes ;;
  *)        MACOSX=no  ;;
esac
AM_CONDITIONAL(COND_MACOSX, test x$MACOSX = xyes)

dnl Check for new platforms.
osversion="`grep \"$host\" $srcdir/PLATFORMS 2>/dev/null`"
if test "x$osversion" = "x"; then
  AC_MSG_RESULT([
  The $PACKAGE package has never been compiled or run on this platform.
  Please send an email to <qucs-devel@lists.sourceforge.net> including your
  compiler and Qt version and whether it worked correctly or not.  The
  '$host' line will then be added to the PLATFORMS file included
  in this package.
])
fi
unset osversion

dnl Release specific. Uncomment these as required.
# AC_CONFIG_SUBDIRS(qucs-core)
# AC_CONFIG_SUBDIRS(qucs-doc)
# AC_CONFIG_SUBDIRS(freehdl)
# AC_CONFIG_SUBDIRS(verilog)
# AC_CONFIG_SUBDIRS(asco)
# AC_CONFIG_SUBDIRS(examples)

# RELEASEDIRS="qucs-core"
# RELEASEDIRS="$RELEASEDIRS qucs-doc"
# RELEASEDIRS="$RELEASEDIRS freehdl"
# RELEASEDIRS="$RELEASEDIRS verilog"
# RELEASEDIRS="$RELEASEDIRS asco"
# RELEASEDIRS="$RELEASEDIRS examples"
AC_SUBST(RELEASEDIRS)

dnl Check for language support.
AC_PATH_PROGS(LRELEASE, lrelease-qt4 lrelease, :, [$QTDIR/bin:$PATH])
AC_PATH_PROGS(LUPDATE, lupdate-qt4 lupdate, :, [$QTDIR/bin:$PATH])

dnl Check for MOC/UIC/RCC support.
AC_PATH_PROGS(MOC, moc-qt4 moc, :, [$QTDIR/bin:$PATH])
AC_PATH_PROGS(UIC, uic-qt4 uic, :, [$QTDIR/bin:$PATH])
AC_PATH_PROGS(RCC, rcc, :, [$QTDIR/bin:$PATH])
if test "$MOC" = ":"; then
  AC_MSG_WARN([
  The $PACKAGE package needs the 'Qt Meta Object Compiler' to compile properly.
  Though Qt itself may be properly installed including headers and libraries
  the 'moc' program is missing.  Possibly you need to install the full
  development package of Qt.])
fi

dnl Check for X11
if test x$LINUX == "xyes"; then
  AC_PATH_XTRA
  X11_INCLUDES="$X_CFLAGS"
  X11_LDFLAGS=""
  X11_LIBS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
  AC_SUBST(X11_INCLUDES)
  AC_SUBST(X11_LDFLAGS)
  AC_SUBST(X11_LIBS)
fi

case $host_os in
  *linux* | *cygwin* | *mingw* )
    dnl Yet another compiler check.
    if test "x$GCC" = xyes; then
      CFLAGS="$CFLAGS -pipe"
      CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -fno-check-new"
      if test x$WIN32 = xyes; then
        CXXFLAGS="$CXXFLAGS"
      fi
      if test $USE_MAINTAINER_MODE = yes; then
        CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
        CXXFLAGS="$CXXFLAGS -W -Wall"
      fi
    fi

    dnl Check include path to Qt.
    QT_INCLUDES=""
    QT_VER=2
    AC_MSG_CHECKING([for Qt headers])
    paths="$QTDIR/include /usr/local/qt4/include /usr/include/qt4 /usr/include/qt4 \
      /usr/include /usr/X11R6/include/X11/qt4 /usr/X11R6/include/X11/qt4 \
      /usr/X11R6/include/qt4 /usr/X11R6/include /sw/include/qt4 \
      /usr/X11R6/include/qt4 $prefix/include/qt4"
    for path in $paths; do
      if test -f "$path/Qt/qapplication.h"; then
        QT_INCLUDES="$path -I$path/Qt -I$path/QtGui -I$path/QtCore -I$path/QtSvg -I$path/QtXml"
        QT_INCLUDES="$QT_INCLUDES -I$path/Qt3Support"
        QT_VER=4
        break
      fi
    done
    if test "x$QT_INCLUDES" != "x"; then
      AC_MSG_RESULT([found in $QT_INCLUDES])
      QT_INCLUDES="-I$QT_INCLUDES"
    else
      AC_MSG_ERROR([not found])
    fi
    AC_SUBST(QT_INCLUDES)

    dnl Check for multi-threaded option.
    AC_ARG_ENABLE([mt],
      AC_HELP_STRING([--disable-mt],
                     [link to non-threaded Qt (deprecated)]),
      enable_mt="$enableval",
      [if test $QT_VER = 4; then
         enable_mt="yes"
       else
         enable_mt="no"
       fi])
    if test "$enable_mt" = yes; then
      QT_LDF=""
      QT_LIB="-lQtCore -lQtGui -lQtXml -lQt3Support -lQtSvg"
      QT_INC="$QT_DEF -DQT3_SUPPORT -DQT_THREAD_SUPPORT -D_REENTRANT"
      [case $host_os in
        *freebsd4*) QT_LIB="$QT_LIB -pthread" ;;
        *freebsd[567]*) QT_LIB="$QT_LIB -lpthread" ;;
        *mingw*) QT_INC="$QT_INC -mthreads"; QT_LDF="$QT_LDF -mthreads" ;;
      esac]
      QT_MTS="multi-threaded"
    else
      QT_LDF=""
      QT_LIB="-lQtCore -lQtGui -lQtXml -lQt3Support -lQtSvg"
      QT_INC="$QT_DEF"
      QT_MTS="non-threaded"
    fi
    case $host_os in
      *mingw*)
      QT_LIB="-lQtCore4 -lQtGui4 -lQtXml4 -lQt3Support4 -lQtSvg4"
      QT_INC="$QT_INC -DQT_DLL -DUNICODE"
      QT_LDF="$QT_LDF -mwindows"
      ;;
    esac
    unset enable_mt

    if test "x$QTDIR" == "x"; then
      QTDIR="nonexistent"
    fi

    echo "QTDIR: .$QTDIR."

    dnl Check library path to Qt.
    QT_LDFLAGS=""
    QT_LIBS=""
    AC_MSG_RESULT([checking for Qt... $QT_VER ($QT_MTS)])
    AC_MSG_CHECKING([for Qt library])
    paths="$QTDIR/lib $QTDIR/lib64 /usr/local/qt4/lib /usr/lib/qt4 /usr/lib/qt4/Qtconf /usr/lib \
      /usr/X11R6/lib/X11/qt4 /usr/X11R6/lib/X11/qt4 /usr/X11R6/lib/qt4 \
      /usr/X11R6/lib /sw/lib /usr/lib64/qt4 /usr/X11R6/lib/qt4"
    AC_LANG(C++)
    for path in $paths; do
      save_LIBS="$LIBS"
      save_LDFLAGS="$LDFLAGS"
      save_CXXFLAGS="$CXXFLAGS"
      LIBS="$LIBS $X11_LIBS $QT_LIB"
      LDFLAGS="$LDFLAGS $X11_LDFLAGS -L$path $QT_LDF"
      CXXFLAGS="$CXXFLAGS $X11_INCLUDES $QT_INCLUDES $QT_INC"
#  AC_LINK_IFELSE(
#    [#include <Qt/qapplication.h>
#     int main (int argc, char ** argv) {
#     QApplication a (argc, argv); a.exec (); return 0; }],
#    [
      QT_LDFLAGS="$path";
        QT_INCLUDES="$QT_INCLUDES $QT_INC";
        break;#])
      LIBS="$save_LIBS"
      LDFLAGS="$save_LDFLAGS"
      CXXFLAGS="$save_CXXFLAGS"
    done
    LIBS="$save_LIBS"
    LDFLAGS="$save_LDFLAGS"
    CXXFLAGS="$save_CXXFLAGS"
    if test "x$QT_LDFLAGS" != "x"; then
      AC_MSG_RESULT([found in $QT_LDFLAGS])
      QT_LDFLAGS="-L$QT_LDFLAGS $QT_LDF"
      QT_LIBS="$QT_LIB"
    else
      AC_MSG_ERROR([not found])
    fi
    ;;

   *darwin* )
    dnl Check for MacOSX, require clang++
    if test "$CXX" = "clang++" ; then
      AC_MSG_CHECKING([whether clang works])

      CFLAGS="$CFLAGS -pipe"
      CXXFLAGS="$CXXFLAGS -pipe"
      if test $USE_MAINTAINER_MODE = yes; then
        CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
        CXXFLAGS="$CXXFLAGS -W -Wall"
      fi
    else
      AC_MSG_ERROR([clang++ required but not found])
    fi

    dnl Check for Qt in:
    dnl  * Digia default path: /Library/Frameworks
    dnl  * User provided path: QTDIR
    dnl  * Homebrew defaul:    /usr/local/lib
    dnl  * MacpPorts default   /opt/local/lib
    with_qt=no
    AC_MSG_CHECKING([for Qt OS X framework])
    paths="$QTDIR/lib /Library/Frameworks /opt/local/lib /usr/local/lib"
    for path in $paths; do
      if test -f "$path/QtGui.framework/Headers/QApplication"; then
        with_qt=yes
        break
      fi
    done

    if test "x${with_qt}" = xyes ; then
      AC_MSG_RESULT([found in $path])
      QT_LIBS="-framework QtGui -framework QtCore -framework QtXml -framework Qt3Support -framework QtSvg"
      CPPFLAGS="$CPPFLAGS -I$path/QtCore.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtGui.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtXml.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/Qt3Support.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtSvg.framework/Headers"
      QT_CFLAGS="$CPPFLAGS"
    else
      AC_MSG_ERROR([not found])
    fi

    dnl Define Qt3Support
    QT_DEF="-DQT_SHARED -DQT3_SUPPORT"
    QT_LDFLAGS="-headerpad_max_install_names"
    CXXFLAGS="$CXXFLAGS $QT_CFLAGS $QT_DEF"
    ;;
esac

dnl check for Qt moveable tabwidgets (introduced in Qt 4.5)
QUCS_CHECK_FUNC_QTABWIDGET_SETMOVABLE

dnl append -O0 to CXXFLAGS if in debug mode and using gcc
dnl to assist with debugging by preventing optimisation.
dnl gcc will use the final invocation of -OX and ignore earlier
dnl values
if test "$enable_qucs_gui_debug" = yes; then
 if test "x$GCC" = xyes; then
    CXXFLAGS="$CXXFLAGS -O0"
    AC_MSG_NOTICE([Appending gcc optimisation flag -O0 due to --enable-debug setting.])
 fi
fi

AC_SUBST(QT_LDFLAGS)
AC_SUBST(QT_LIBS)
AC_SUBST(QT_MOC)
AC_LANG(C)

dnl Check for additional header files.
AC_CHECK_HEADERS([ieeefp.h])

dnl Check for LibBoard files.
dnl AC_LANG(C++)
dnl AC_CHECK_HEADERS([Board.h])
dnl AC_CHECK_LIB(board, fprintf)
dnl AC_LANG(C)

dnl Check for path transformation.
case $build_os in
mingw* | cygwin*)
  PATHXFORM="cygpath -w"
  ;;
*)
  PATHXFORM="echo"
  ;;
esac
AC_SUBST(PATHXFORM)

dnl Create path info.
dnl BitmapDir still used by qucs-edit
BIN_PATH="$prefix/bin/"
AC_DEFINE_UNQUOTED([BINARYDIR], ["$BIN_PATH"],
  [Where the binary files go.])
unset BIN_PATH
LANG_PATH="$prefix/share/qucs/lang/"
AC_DEFINE_UNQUOTED([LANGUAGEDIR], ["$LANG_PATH"],
  [Where the language files go.])
unset LANG_PATH
LIBRARY_PATH="$prefix/share/qucs/library/"
AC_DEFINE_UNQUOTED([LIBRARYDIR], ["$LIBRARY_PATH"],
  [Where the component library files go.])
unset LIBRARY_PATH
DOC_PATH="$prefix/share/qucs/docs/"
AC_DEFINE_UNQUOTED([DOCDIR], ["$DOC_PATH"],
  [Where the documentation files go.])
unset DOC_PATH
OCTAVE_PATH="$prefix/share/qucs/octave/"
AC_DEFINE_UNQUOTED([OCTAVEDIR], ["$OCTAVE_PATH"],
  [Where the octave script files go.])
unset OCTAVE_PATH

dnl Definitions at top of <config.h>.
AH_TOP([
/* __BEGIN_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use __END_DECLS at
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
		 contrib/Makefile
		 qucs-edit/Makefile
		 qucs-help/Makefile
                 qucs-help/docs/Makefile
                 qucs-help/docs/en/Makefile
		 qucs-help/docs/de/Makefile
                 qucs-help/docs/es/Makefile
                 qucs-help/docs/fr/Makefile
                 qucs-help/docs/ru/Makefile
                 qucs-help/docs/uk/Makefile
                 qucs-help/docs/cs/Makefile
                 qucs-help/docs/pt/Makefile
		 qucs-filter/Makefile
		 qucs-filter-v2/Makefile
		 qucs-transcalc/Makefile
		 qucs-transcalc/examples/Makefile
		 qucs-lib/Makefile
		 qucs-lib/library/Makefile
		 qucs-attenuator/Makefile
		 qucs/Makefile
		 qucs/octave/Makefile
		 qucs/components/Makefile
		 qucs/diagrams/Makefile
		 qucs/paintings/Makefile
		 qucs/dialogs/Makefile])
AC_OUTPUT

dnl delete the unset enable_qucs_gui_debug variable
unset enable_qucs_gui_debug

dnl Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])

