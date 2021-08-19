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

/* ANSI/GENERIC Terminal emulation library.

  This interprets the escape codes and implements them as operations for the 
  terminal core library.

  */


static int esc  = 0;
static int escm = 0;
static int ansi_mode = 0;

static char escode[40];
static char *mptr;


static int ansi_attr(int i)
{
	int a = 0;

	switch (i & 15)
	{
		case 0: a = AT_REVERSE; break;
		case 1: a = AT_BRIGHT;  break;
		case 2: a = AT_BLINK;   break;
	}
	return a;
}


static int ansi_attr2(int i)
{
        int a = 0;

        if (i & 1) a |= AT_UNDERSCORE;
	if (i & 2) a |= AT_BRIGHT;
	if (i & 4) a |= AT_REVERSE;
	if (i & 8) a |= AT_BLINK;
        return a;
}



static void multi(char ch, int n)
{
	mptr = escode;
	escm = n;
	esc  = ch;
}


static int ansi_ops(char c)
{
	int n;
	int p = core_ansi_buf[0], q = core_ansi_buf[1];

	if (!p) ++p;
	if (!q) ++p;

	switch(c)
	{
		case 'A': for (n = 0; n < p; n++) core_up(0); return 1;
		case 'B': for (n = 0; n < p; n++) core_down(0); return 1;
		case 'C': for (n = 0; n < p; n++) core_right(0); return 1;
		case 'D': for (n = 0; n < p; n++) core_left(0); return 1;
		case 'f':
		case 'H': core_move(q - 1, p - 1); return 1;
		case 'J': if (p == 0) core_clrtoeos();
			  if (p == 1) core_clrtobos();
			  if (p == 2) clear();
                          return 1;
		case 'K': if (p == 0) core_clrtoeol();
			  if (p == 1) core_clrtobol();
                          if (p == 2) core_clrline(core_y);
			  return 1;
		case 'P': for (n = 0; n < p; n++) delch();
			  return 1;
		case 's': core_save_pos(0);
		case 'u': core_save_pos(1);

		case 'm':
			switch(core_ansi_buf[0])
			{
				case 0: core_clearattr(15);		break;
				case 1:	core_setattr(AT_BRIGHT);	break;
				case 4: core_setattr(AT_UNDERSCORE);	break;
				case 5: core_setattr(AT_BLINK);		break;
				case 7: core_setattr(AT_REVERSE);	break;
			}
	}
	return 0;
}



static int do_esc(char c)
{
	int esc1 = esc;
	esc = 0;

	if (esc1 == 27) switch(c)	/* 1st char of escape code */
	{
		case '"': multi('"', 1);		return 0;
		case '.': multi('.', 1);		return 0;
		case ';': multi(';', 1);		return 0;
		case '(': core_setattr(AT_BRIGHT);      return 0;
		case ')': core_clearattr(AT_BRIGHT);	return 0;
		case '=': multi('=', 2);	return 0;

		case 'A': core_up(0);		return 1;
		case 'B': multi('B', 1);	return 0;
		case 'C': multi('C', 1);	return 0;
		case 'D': delch();		return 1;
		case '*': case '+': case ',': case ':': 
			  clear();		return 1;
		case 'E': insertln();		return 1;
		case 'G': multi('G', 1);	return 0;		
		case 'H': multi('H', 1);        return 0;
		case 'I': core_setattr(AT_REVERSE);	return 0;
		case 'J': core_clrtoeos();	return 1;
		case 'K': core_clrline(core_y);	return 1;
		case 'L': insertln();		return 1;
		case 'M': deleteln();		return 1;
		case 'N': core_clearattr(AT_REVERSE);	return 0;
		case 'q':
		case 'Q': insch(' ');		return 1;
		case 'r':
		case 'R': deleteln();		return 1;
		case 't':
		case 'T': core_clrtoeol();	return 1;
		case 'w':
		case 'W': delch();		return 1;
		case 'y':
		case 'Y': core_clrtoeos();	return 1;

		case '[': ansi_mode = 1;	return 0; 

		case 'd': multi('d', 1);	return 0;
		case 'e': core_cursor(1);	return 0;
		case 'f': core_cursor(0);	return 0;
		case 'j': core_save_pos(0);	return 0;
		case 'k': core_save_pos(1);	return 1;
		case 'l': core_setattr(AT_BRIGHT);	return 0;
		case 'm': core_clearattr(AT_BRIGHT);	return 0;
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
			case '"': escode[0] &= 7;
				  if      (escode[0] == 6) core_cursor(0);
				  else if (escode[0] <  3) core_cursor(1);
				  else                     core_cursor(2);
				  return 0;
			case '.': escode[0] &= 7;
				  core_cursor(escode[0] == 1   ? 0 : 1);
				  return 1;
			case ';': core_cursor(escode[0] == 'A' ? 0 : 1);
			case '=': core_move(escode[1] - 32, escode[0] - 32);
				  return 1;
			case 'B': escode[0] &= 7;
				  if (escode[0] < 4) 
					core_setattr(ansi_attr(escode[0]));
				  else  core_cursor(1);
				  return 0;
			case 'C': escode[0] &= 7;
				  if (escode[0] < 4)
					core_clearattr(ansi_attr(escode[0]));
				  else  core_cursor(0);
				  return 0;
			case 'G': escode[0] = ansi_attr2(escode[0]);
				  core_clearattr(~escode[0]); 
				  core_setattr(escode[0]);
				  return 0;
			case 'H': return 0;	
			case 'd': escode[0] &= 7;
				  core_cursor(escode[0] == 0 ? 0 : 1); 
				  return 1;
		}

	}
	core_addch(c);
	return 1;	/* Character not processed */
}


static void ansi_init(int wrap)
{
	core_init();
	core_wrap = wrap;
}


static int ansi_char(char c)
{
	if (ansi_mode)
	{
		if (!core_ansi(c)) return 0;	/* Not at end of code */
		ansi_mode = 0;
		return ansi_ops(core_ansi_char);
	}

	if (esc) return do_esc(c);

	switch(c)
	{
		case  1: core_setattr(AT_BRIGHT);	  return 0;
		case  2: core_clearattr(AT_BRIGHT);	  return 0;
		case 26:
		case  4: clear(); core_move(0,0); return 1;
		case  8: core_left(core_wrap);	  return 1;
		case  9: core_tab();		  return 1;
		case 10: core_down(1);		  return 1;
		case 11: core_up(0);		  return 1;
		case 12: core_right(core_wrap);	  return 1;
		case 13: core_cr();		  return 1;
		case 24: core_clrtoeol();	  return 1;
		case 27: esc = c;		  return 0;
		case 30: core_move(0,0);          return 1;
		case 31: core_cr(); core_down(1); return 1;
		default: core_addch(c);		  return 1;
	}
	return 0;
}

int ansi_term(int func, int param)
{
	switch(func)
	{
		case CPM_TERM_INIT:  ansi_init(0); return 1;
		case CPM_TERM_CHAR:  return ansi_char(param);
		default: return core_term(func, param);
	}
	return 0;
}

int generic_term(int func, int param)
{
	switch(func)
	{
		case CPM_TERM_INIT: ansi_init(1); return 1;
		case CPM_TERM_CHAR: return ansi_char(param);
		default: return core_term(func, param);
	}
}

