# DESCRIPTION
#
#   Checks for presence of Qt features.
#
# LICENSE
#
#   Copyright (c) 2014 Richard Crozier <richard dot crozier at yahoo dot co dot uk>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

dnl
dnl Check whether the Qt QTabWidget::setMovable() function exists.
dnl This function was added in Qt 4.5.
dnl
AC_DEFUN([QUCS_CHECK_FUNC_QTABWIDGET_SETMOVABLE], [
  AC_CACHE_CHECK([whether Qt has the QTabWidget::setMovable() function],
    [qucs_cv_func_qtabwidget_setmovable],
    [AC_LANG_PUSH(C++)
    ac_qucs_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$QT_INCLUDES $CPPFLAGS"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        #include <QTabWidget>
        class tab_widget : public QTabWidget
        {
        public:
          tab_widget (QWidget *parent = 0) : QTabWidget (parent) { this->setMovable (true); }
          ~tab_widget () {}
        };
        ]], [[
        tab_widget tw;
        ]])],
      qucs_cv_func_qtabwidget_setmovable=yes,
      qucs_cv_func_qtabwidget_setmovable=no)
    CPPFLAGS="$ac_qucs_save_CPPFLAGS"
    AC_LANG_POP(C++)
  ])
  if test $qucs_cv_func_qtabwidget_setmovable = yes; then
    AC_DEFINE(HAVE_QTABWIDGET_SETMOVABLE, 1,
      [Define to 1 if Qt has the QTabWidget::setMovable() function.])
  fi
])
