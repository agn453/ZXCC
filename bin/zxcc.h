/*
 *  Change the directories in these #defines if necessary. Note trailing slash.
 */
#ifndef _WIN32
#include "config.h"
#define ISDIRSEP(c) ((c) == '/')
#define DIRSEPCH	'/'
#else
#include "config-win.h"
#define ISDIRSEP(c) ((c) == '/' || (c) == '\\')
#define DIRSEPCH	'\\'
#endif

#ifndef CPMDIR80
#ifdef _MSC_VER
#define CPMDIR80    "d:/local/lib/cpm/"
#else
#define CPMDIR80    "/usr/local/lib/cpm/"
#endif
#endif

/* the default sub directories trailing / is required */
#define BIN80   "bin80/"
#define LIB80   "lib80/"
#define INC80   "include80/"

#ifndef BINDIR80
#define BINDIR80 CPMDIR80 BIN80
#endif
#ifndef LIBDIR80
#define LIBDIR80 CPMDIR80 LIB80
#endif
#ifndef INCDIR80
#define INCDIR80 CPMDIR80 INC80
#endif


extern char bindir80[];
extern char libdir80[];
extern char incdir80[];

#define SERIAL "ZXCC05"

/* System include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <time.h>
#ifdef __MSDOS
#include <dos.h>
#endif

/* Library includes */

#ifdef USE_CPMIO
#include "cpmio.h"
#endif

#ifdef USE_CPMGSX
#include "cpmgsx.h"
#endif

#include <cpmredir.h>	/* BDOS disc simulation */

typedef unsigned char byte;	/* Must be exactly 8 bits */
typedef unsigned short word;	/* Must be exactly 16 bits */

/* Prototypes */

void ed_fe  (byte *a, byte *b, byte *c, byte *d, byte *e, byte *f,
             byte *h, byte *l, word *pc, word *ix, word *iy);
void cpmbdos(byte *a, byte *b, byte *c, byte *d, byte *e, byte *f, 
             byte *h, byte *l, word *pc, word *ix, word *iy);
void cpmbios(byte *a, byte *b, byte *c, byte *d, byte *e, byte *f, 
             byte *h, byte *l, word *pc, word *ix, word *iy);
void dump_regs(FILE *fp, byte a, byte b, byte c, byte d, byte e, byte f, 
             byte h, byte l, word pc, word ix, word iy);
void Msg(char *s, ...);
int zxcc_term(void);
void zxcc_exit(int code);

/* Global variables */

extern char *progname;
extern char **argv;
extern int argc;
extern byte RAM[65536]; /* The Z80's address space */
extern int file_conin;  /* non zero if stdin not a terminal */
extern int eof_conin;   /* non zero if eof of stdin */
/* Z80 CPU emulation */

#include "z80.h"

