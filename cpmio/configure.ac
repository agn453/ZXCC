dnl Process this file with autoconf to produce a configure script.
AC_INIT([cpmio],[1.1.1])
AC_CONFIG_SRCDIR([lib/cpmio.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
LT_INIT
VERSION=1.1.1
UPDATED='March 28, 2001'
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_ARG_WITH(readline,
[  --with-readline         Use GNU readline for line input (no)])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Checks for libraries.
AC_CHECK_LIB(ncurses, printw, LIBS="-lncurses $LIBS")
AC_CHECK_LIB(curses,  printw, LIBS="-lcurses  $LIBS")
if test x"$with_readline" = x"yes"; then
  CFLAGS="$CFLAGS -DUSE_READLINE"
  AC_CHECK_LIB(readline, readline)
fi

dnl Checks for header files.
AC_CHECK_HEADERS(sys/time.h unistd.h)
AC_CHECK_HEADERS([ncurses.h curses.h], break, AC_MSG_ERROR([either ncurses.h or curses.h is required]))

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(select)

AC_CONFIG_FILES([Makefile lib/Makefile include/Makefile check/Makefile])
AC_OUTPUT
