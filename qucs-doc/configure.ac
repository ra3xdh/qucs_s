#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

dnl Read version from file
m4_define([QUCS_VERSION], m4_esyscmd([tr -d '\n' < VERSION]))

AC_INIT(qucs-doc, QUCS_VERSION, qucs-bugs@lists.sourceforge.net)
AC_CONFIG_SRCDIR([technical/technical.tex])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define])
AC_PREFIX_DEFAULT([/usr/local])
test "x$prefix" = xNONE && prefix="/usr/local"

# FIXME: don't use colons
AC_PATH_PROG(LATEX, latex, :)
AC_PATH_PROG(PDFLATEX, pdflatex, :)
AC_PATH_PROG(FIG2DEV, fig2dev, :)
AC_PATH_PROG(BIBTEX, bibtex, :)
AC_PATH_PROG(DVIPS, dvips, :)
AC_PATH_PROG(PS2EPS, ps2eps, :)
AC_PATH_PROG(PS2PDF, ps2pdf, :)
AC_PATH_PROG(EPSTOPDF, epstopdf, :)

AC_PATH_PROG(OCTAVE, octave, no)

AS_IF([test "x$OCTAVE" != xno],
	AS_IF([octave --eval eglobpar 2>/dev/null],
		     [OCTAVE_EPSTK=yes], [OCTAVE_EPSTK=no])
	)
AM_CONDITIONAL(USE_OCTAVE, test x$OCTAVE_EPSTK != xno)

# FIXME: don't use colons
AC_PATH_PROG(LATEX2HTML, latex2html, no)
AM_CONDITIONAL(USE_LATEX2HTML, test x$LATEX2HTML != xno)

AC_PATH_PROG(TEX2PDF, tex2pdf, :)
AC_PATH_PROG(MAKEINDEX, makeindex, :)

dnl Define the check for doxygen.
AC_DEFUN([CHECK_DOXYGEN],
[
AC_ARG_ENABLE([doxygen],
  AC_HELP_STRING([--enable-doxygen],
                 [enable documentation generation with doxygen @<:@auto@:>@]))
AC_ARG_ENABLE([dot],
  AC_HELP_STRING([--enable-dot],
                 [use 'dot' to generate graphs in doxygen @<:@auto@:>@]))
AC_ARG_ENABLE([html-docs],
  AC_HELP_STRING([--enable-html-docs],
                 [enable HTML generation with doxygen @<:@yes@:>@]),
  [], [enable_html_docs=yes])
AC_ARG_ENABLE([latex-docs],
  AC_HELP_STRING([--enable-latex-docs],
                 [enable LaTeX generation with doxygen @<:@no@:>@]),
  [], [enable_latex_docs=no])
if test "x$enable_doxygen" = xno; then
  enable_doc=no
else
  AC_PATH_PROG(DOXYGEN, doxygen, , $PATH)
  if test x$DOXYGEN = x; then
    if test "x$enable_doxygen" = xyes; then
      AC_MSG_ERROR([could not find doxygen])
    fi
    enable_doc=no
  else
    enable_doc=yes
    AC_PATH_PROG(DOT, dot, , $PATH)
  fi
fi
AM_CONDITIONAL(DOC, test x$enable_doc = xyes)
if test x$DOT = x; then
  if test "x$enable_dot" = xyes; then
    AC_MSG_ERROR([could not find dot])
  fi
  enable_dot=no
else
  enable_dot=yes
fi
AM_CONDITIONAL(ENABLE_DOXYGEN, test x$enable_doc = xtrue)
AM_CONDITIONAL(ENABLE_DOT, test x$enable_dot = xtrue)
AC_SUBST(enable_dot)
AC_SUBST(enable_html_docs)
AC_SUBST(enable_latex_docs)
])

# sometimes you may need special flags.
# (such as -shell-escape?)
# (not implemented any further)
AC_SUBST([PDFLATEX_FLAGS])

dnl Check for doxygen.
CHECK_DOXYGEN

dnl Add here all your Makefiles. These are created by configure.
AC_CONFIG_FILES([Makefile
		 doxygen/Makefile
		 technical/Makefile
		 report/Makefile
		 report/activefilter/Makefile
		 report/activefilter/pics/Makefile
		 report/verilog/Makefile
		 report/verilog/pics/Makefile
		 report/StoQ_test1/Makefile
		 report/StoQ_test1/pics/Makefile
		 report/StoQ_test2/Makefile
		 report/StoQ_test2/pics/Makefile
		 report/StoQ_test3/Makefile
		 report/StoQ_test3/pics/Makefile
		 report/StoQ_test4/Makefile
		 report/StoQ_test4/pics/Makefile
		 report/StoQ_test5/Makefile
		 report/StoQ_test5/pics/Makefile
		 report/curtice/Makefile
		 report/curtice/pics/Makefile
		 report/mod_amp/Makefile
		 report/mod_amp/pics/Makefile
		 report/log_amp/Makefile
		 report/log_amp/pics/Makefile
		 report/potentiometer/Makefile
		 report/potentiometer/pics/Makefile
		 report/mesfet/Makefile
		 report/mesfet/pics/Makefile
		 report/EKV26/Makefile
		 report/EKV26/pics/Makefile
		 report/photodiode/Makefile
		 report/photodiode/pics/Makefile
		 tutorial/Makefile
		 tutorial/getstarted/Makefile
		 tutorial/getstarted/pics/Makefile
		 tutorial/dcstatic/Makefile
		 tutorial/dcstatic/pics/Makefile
		 tutorial/linblocks/Makefile
		 tutorial/linblocks/pics/Makefile
		 tutorial/coupler/Makefile
		 tutorial/coupler/pics/Makefile
		 tutorial/modelbjt/Makefile
		 tutorial/modelbjt/pics/Makefile
		 tutorial/poweramp/Makefile
		 tutorial/poweramp/pics/Makefile
		 tutorial/lna/Makefile
		 tutorial/digital/Makefile
		 tutorial/digital/pics/Makefile
		 tutorial/ffmodels/Makefile
		 tutorial/ffmodels/pics/Makefile
		 tutorial/functions/Makefile
		 tutorial/functions/pics/Makefile
		 tutorial/opamp/Makefile
		 tutorial/opamp/pics/Makefile
		 tutorial/timer555/Makefile
		 tutorial/timer555/pics/Makefile
		 tutorial/spicetoqucs/Makefile
		 tutorial/spicetoqucs/pics/Makefile
		 tutorial/equations/Makefile
		 tutorial/equations/pics/Makefile
		 tutorial/bjtbias/Makefile
		 tutorial/bjtbias/pics/Makefile
		 tutorial/textmode/Makefile
		 tutorial/textmode/pics/Makefile
		 tutorial/RFResModels/Makefile
		 tutorial/RFResModels/pics/Makefile
		 tutorial/activefilter/Makefile
		 tutorial/activefilter/pics/Makefile
                 tutorial/sparameteramp/Makefile
                 tutorial/sparameteramp/pics/Makefile])

AC_OUTPUT

# Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([
Configure Information:
        OCTAVE    : $OCTAVE
    OCTAVE_EPSTK  : $OCTAVE_EPSTK
	 ])
AC_MSG_RESULT([])
