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

#include "cpmio_i.h"
#include "termcore.h"

/* VT52 Terminal emulation library.

  This interprets the escape codes and implements them as operations for the 
  terminal core library.

  */


static int esc  = 0;
static int escm = 0;

static char escode[10];
static char *mptr;

static void vt52_win(char *s)
{
	core_win(s[0] - 32, s[1] - 32, s[2] - 31, s[3] - 31);
}



static int vt52_colour(char c)
{
	int nc = (c - 32);
	int r, g, b;

	static int colours[] = {COLOR_BLACK,   COLOR_BLUE,  COLOR_RED,
                                COLOR_MAGENTA, COLOR_GREEN, COLOR_CYAN,
                                COLOR_YELLOW,  COLOR_WHITE };

	/* nc is a 6-bit "Amstrad VT52" colour. */

	b = (nc & 3 ) ? 1 : 0;
	g = (nc & 12) ? 2 : 0;
	r = (nc & 48) ? 4 : 0;

	return colours[r|g|b];	
}


static void multi(char ch, int n)
{
	mptr = escode;
	escm = n;
	esc  = ch;
}

static int do_esc(char c)
{
	int esc1 = esc;
	esc = 0;

	if (esc1 == 27) switch(c)	/* 1st char of escape code */
	{
		case '(': core_setattr(AT_BRIGHT);      return 0;
		case ')': core_clearattr(AT_BRIGHT);	return 0;

		case 'A': core_up(0);		return 1;
		case 'B': core_down(0);		return 1;
		case 'C': core_right(0);	return 1;
		case 'D': core_left(0);		return 1;
		case '*': case '+': case ',': case ':': case ';':
		case 'E': clear();              return 1;
		case 'H': core_move (0,0);      return 1;
		case 'I': core_up(1);		return 1;
		case 'J': core_clrtoeos();	return 1;
		case 'K': core_clrtoeol();	return 1;
		case 'L': insertln();		return 1;
		case 'M': deleteln();		return 1;
		case 'N': delch();		return 1;
		case 'R': deleteln();		return 1;
		case 'X': multi('X', 4);	return 0;
		case 'Y': multi('Y', 2);	return 0;
		case 'b': multi('b', 1);	return 0;
		case 'c': multi('c', 1);	return 0;
		case 'd': core_clrtobos();	return 1;
		case 'e': core_cursor(1);	return 0;
		case 'f': core_cursor(0);	return 0;
		case 'j': core_save_pos(0);	return 0;
		case 'k': core_save_pos(1);	return 1;
		case 'l': core_clrline(core_y);	return 1;
		case 'o': core_clrtobol();	return 1;
		case 'p': core_setattr(  AT_REVERSE);   return 0;
		case 'q': core_clearattr(AT_REVERSE);   return 0;
		case 'r': core_setattr(  AT_UNDERSCORE); return 0;
		case 's': core_setattr(  AT_BLINK);     return 0;
		case 't': core_clearattr(AT_BLINK);     return 0;
		case 'u': core_clearattr(AT_UNDERSCORE); return 0;
		case 'w': core_wrap = 1; 	return 0;
		case 'v': core_wrap = 0;	return 0;
		case 'x': core_24x80();		return 1;
		case 'y': core_fullscr();	return 1;	
		default:  core_addch(c);	return 1; /* Unknown */
	}
	else if (escm)		/* 2nd, 3rd char etc. of escape code */
	{
		esc = esc1;	/* Assume more to come */
		mptr[0] = c;
		++mptr;
		--escm;
		if (escm) return 0;
		
		/* Multi-byte code read in */	
		esc = 0;
		switch(esc1)
		{
			case 'b': core_setfg(vt52_colour(escode[0])); return 1;
			case 'c': core_setbg(vt52_colour(escode[0])); return 1;
			case 'X': vt52_win(escode); return 1;
			case 'Y': core_move(escode[1] - 32, escode[0] - 32);
				  return 1;
		}

	}
	core_addch(c);
	return 1;	/* Character not processed */
}


static void vt52_init(void)
{
	core_init();
}


static int vt52_char(char c)
{
	if (esc) return do_esc(c);

	switch(c)
	{
		case  1: core_move(0,0);	  return 1;
		case  2: core_clearattr(AT_BRIGHT);	  return 0;
		case 26:
		case  4: clear(); core_move(0,0); return 1;
		case  8: core_left(core_wrap);	  return 1;
		case  9: core_tab();		  return 1;
		case 10: core_down(1);		  return 1;
		case 11: core_up(0);		  return 1;
		case 12: clear();		  return 1;
		case 13: core_cr();		  return 1;
		case 24: core_clrtoeol();	  return 1;
		case 27: esc = c;		  return 0;
		case 30: core_move(0,0);          return 1;
		case 31: core_cr(); core_down(1); return 1;
		default: core_addch(c);		  return 1;
	}
	return 0;
}

int vt52_term(int func, int param)
{
	switch(func)
	{
		case CPM_TERM_INIT:  vt52_init(); return 1;
		case CPM_TERM_CHAR:  return vt52_char(param);
		default: return core_term(func, param);
	}
	return 0;
}


