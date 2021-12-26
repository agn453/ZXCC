/* config-win.h. Hand modified version for windows */
#define WIN32_LEAN_AND_MEAN			/* remove bloat */
#define _CRT_SECURE_NO_WARNINGS		/* VC is picky about some potentially unsafe functions */
#define _CRT_NONSTDC_NO_WARNINGS	/* VC renames some POSIX functions */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `PDcurses' library pdcurses.lib. */
#define HAVE_LIBCURSES 1
#define HAVE_CURSES_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
//#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
// #define HAVE_UNISTD_H 1


/* Define to 1 if  you are using the tracker functions to track open files*/
/* needed for Windows */
#define FILETRACKER 1

/* windows include files */
#define HAVE_WINDOWS_H 1
#define HAVE_DIRENT_H 1
#define HAVE_DIRECT_H 1
#define HAVE_IO_H	1

/* Name of package */
#define PACKAGE "zxcc"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.5.7"

