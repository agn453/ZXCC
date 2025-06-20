dnl Process this file with autoconf to produce a configure script.
AC_INIT([zxcc],[0.5.7])
AC_CONFIG_SRCDIR([bin/zxcc.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
UPDATED='December 28, 2013'
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
LT_INIT

dnl Get an absolute version of $srcdir and .

LSUBDIRS=""
DSTROOT=`pwd`
if test x"$srcdir" = x
then
	SRCROOT=`pwd`
else
	cd $srcdir
	SRCROOT=`pwd`
	cd $DSTROOT
fi


AC_ARG_WITH(readline,  [  --with-readline         Use GNU Readline for line input (no)])
AC_ARG_WITH(cpmio,     [  --with-cpmio            Use CPMIO library for output (yes)])
AC_ARG_WITH(filetracker, [  --with-filetracker      Use FILETRACKER to keep track of CP/M open files (no)])

dnl Checks for programs.
AC_PROG_CC

if test x"$with_readline" = xyes
then
	CPPFLAGS="$CPPFLAGS -DUSE_READLINE"
	AC_CHECK_LIB(readline, readline)
fi

dnl If using CPMIO, add it to libs. Can't use AC_CHECK_LIB because
dnl CPMIO hasn't been built yet.
if test x"$with_cpmio" != xno
then
	AC_CHECK_LIB(ncurses, printw)
	AC_CHECK_LIB(curses,  printw)
	CPPFLAGS="$CPPFLAGS -I$SRCROOT/cpmio/include -DUSE_CPMIO"
	LDFLAGS="$LDFLAGS -L$DSTROOT/cpmio/lib"
	LSUBDIRS="$LSUBDIRS cpmio"
fi

dnl Use FILETRACKER to keep track of CP/M open files.
if test x"$with_filetracker" == xyes
then
	CPPFLAGS="$CPPFLAGS -DFILETRACKER=1"
fi

dnl CPMREDIR isn't conditional, so just add it
CPPFLAGS="$CPPFLAGS -I$SRCROOT/cpmredir/include"
LDFLAGS="$LDFLAGS -L$DSTROOT/cpmredir/lib"
LSUBDIRS="$LSUBDIRS cpmredir"

dnl General preprocessor symbols
CPPFLAGS="$CPPFLAGS -DZXCC"

dnl Checks for libraries.
dnl
dnl Actually, we can't, because the libraries haven't been built yet.
dnl
dnl AC_CHECK_LIB(cpmio, cpm_const)
dnl AC_CHECK_LIB(cpmredir, fcb_drive)

dnl Checks for header files.
AC_CHECK_HEADERS(fcntl.h unistd.h sys/param.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strstr)


AC_SUBST(LSUBDIRS)
AC_CONFIG_SUBDIRS([cpmio cpmredir])

dnl Location of the ZXCC library directories
dnl CPPFLAGS="$CPPFLAGS -DLIBROOT=\"$libdir/cpm/\""
CPPFLAGS="$CPPFLAGS -DBINDIR80=\\\"$libdir/cpm/bin80/\\\""
CPPFLAGS="$CPPFLAGS -DLIBDIR80=\\\"$libdir/cpm/lib80/\\\""
CPPFLAGS="$CPPFLAGS -DINCDIR80=\\\"$libdir/cpm/include80/\\\""

dnl Do this last, because adding "-lcpmio" to libraries when there isn't
dnl in fact a CPMIO library built yet will make other tests go funny.
if test x"$with_cpmio" != xno
then
	LIBS="-lcpmio $LIBS"
fi
LIBS="-lcpmredir $LIBS"

AC_CONFIG_FILES([Makefile bin/Makefile Z80/Makefile ])
AC_OUTPUT

