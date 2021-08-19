/*

    CPMIO: CP/M console emulation library
    Copyright (C) 1998 - 1999, John Elliott <jce@seasip.demon.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/* CPMIO: Internal declarations  */

#include "config.h"

#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#endif

#ifdef HAVE_CURSES_H
#include <curses.h>
#endif

#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "cpmio.h"	/* The public declarations */

RETSIGTYPE on_sigint(int a);

void cpm_console_bdos_init(void);

/* Terminal emulations */

int  raw_term   (int func, int param);
int  vt52_term  (int func, int param);
int  ansi_term  (int func, int param);
int  generic_term(int func, int param);
int  termios_term(int func, int param);

#define CPM_HI(x) ((x >> 8) & 0xFF)
#define CPM_LO(x) (x & 0xFF)

#define AT_BRIGHT     1
#define AT_UNDERSCORE 2
#define AT_BLINK      4
#define AT_REVERSE    8

#define CTL_C ('C' - '@')
#define CTL_P ('P' - '@')
#define CTL_Q ('Q' - '@')
#define CTL_S ('S' - '@')

