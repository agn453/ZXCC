dnl Process this file with autoconf to produce a configure script.
AC_INIT([cpmredir],[1.1.4])
AC_CONFIG_SRCDIR([lib/cpmredir.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
VERSION=1.1.4
UPDATED='December 28, 2013'
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
LT_INIT

AC_ARG_WITH(filetracker, [  --with-filetracker      Use FILETRACKER to keep track of CP/M open files (no)])

dnl Use FILETRACKER to keep track of CP/M open files.
if test x"$with_filetracker" == xyes
then
	CPPFLAGS="$CPPFLAGS -DFILETRACKER=1"
fi

dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS(fcntl.h unistd.h sys/vfs.h windows.h winnt.h utime.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(mkdir rmdir)

AC_CONFIG_FILES([Makefile include/Makefile lib/Makefile])
AC_OUTPUT

