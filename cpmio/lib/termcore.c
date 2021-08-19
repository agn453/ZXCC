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

/* General terminal emulation library. The different terminal "personalities"
   call this module to achieve their effects.  */

#define TERM_CORE_C
#include "termcore.h"

#define DOMOVE move(core_y, core_x)


void core_scroll_down(void)
{
	scrollok(stdscr, 1);
	setscrreg(core_wy, core_wy + core_wh);
	scrl(1);
	scrollok(stdscr, 0);

}


void core_scroll_up(void)
{
	scrollok(stdscr, 1);
	setscrreg(core_wy, core_wy + core_wh);
	scrl(0);
	scrollok(stdscr, 0);
}


void core_up(int scrl)
{
	--core_y;
	if (core_y >= core_wy) 
	{
		DOMOVE; 
		return; 
	}
	++core_y;
	if (scrl) core_scroll_up();

	DOMOVE;	
}


void core_down(int scrl)
{
        ++core_y;
        if (core_y < (core_wy + core_wh)) 
	{
		DOMOVE;
		return;
	}
        --core_y;
        if (scrl) core_scroll_down();

	DOMOVE;
}

void core_left(int wrap)
{
        --core_x;
        if (core_x >= core_wx) 
	{
		DOMOVE;
		return;
	}
        if (!wrap) ++core_x;
        else 
	{
		core_x = core_wx + core_ww - 1;
		core_up(1);
	}
	DOMOVE;
}



void core_right(int wrap)
{
        ++core_x;
        if (core_x < (core_wx + core_ww)) 
	{
		DOMOVE;
		return;
	}
        if (!wrap) --core_x;
        else 
	{
		core_x = core_wx;
		core_down(1);
	} 
	DOMOVE;
}


void core_addch(char c)
{
	DOMOVE;
	addch(c);
	core_right(core_wrap);

	DOMOVE;
}

void core_tab(void)
{
	do
	{
		addch(' ');
	}	while ((core_x - core_wx) & 7);
}


void core_cr(void)
{
	core_x = core_wx;
	DOMOVE;
}




void core_move(int x, int y)
{
	if (x < core_wx)             x = core_wx;
	if (x >= core_wx + core_ww)  x = core_wx + core_ww - 1;
	if (y < core_wy)             y = core_wy;
	if (y >= core_wy + core_wh)  y = core_wy + core_wh - 1;

	core_x = x + core_wx;
	core_y = y + core_wy;
	DOMOVE;
}

void core_save_pos(int restore)
{
	static int ox, oy;

	if (restore) core_move(ox, oy);
	else	
	{
		ox = core_x;
		oy = core_y;
	}
}

static int limity(int y) 
{ 
	if (y < 0) y = 0; 
	if (y >= LINES) y = LINES - 1;
	return y;
}

static int limitx(int x) 
{ 
	if (x < 0) x = 0; 
	if (x >= COLS) x = COLS - 1;
	return x;
}


void core_win(int tr, int lc, int h, int w)
{
	core_wy = limity(tr);
	core_wx = limitx(lc);
	core_wh = h;
	core_ww = w;

	if (core_wy == LINES - 1) --core_wy;
	if (core_wx == COLS - 1) --core_wh;

	core_wh = limity(core_wh + core_wy) - core_wy;
	core_ww = limitx(core_ww + core_wx) - core_wx;

	if (core_x < core_wx)            core_x = core_wx;
	if (core_x >= core_wx + core_ww) core_x = core_wx + core_ww - 1;
	if (core_y < core_wy)            core_y = core_wy;
	if (core_y >= core_wy + core_wh) core_y = core_wy + core_wh - 1;
	
	DOMOVE;
}

void core_24x80(void)
{
	core_win(0, 0, 24, 80);
}


void core_fullscr(void)
{
	core_win(0, 0, LINES, COLS);
}


void core_clrtobol(void)
{
	int l;

	move(core_y, core_wx);
	for (l = core_wx; l < core_x; l++) addch(' ');
	
	DOMOVE;
}


void core_clrline(int row)
{
	int l;

	move (row, core_wx);
	for (l = 0; l < core_ww; l++) addch(' ');
	
	DOMOVE;
}


void core_clrtobos(void)
{
	int l;

	for (l = core_wy; l < core_y; l++)
	{
		core_clrline(l);
	}
	core_clrtobol();
}


void core_clrtoeol(void)
{
	int l;

	DOMOVE;
	
	for (l = core_x; l < (core_wx + core_ww); l++) addch(' ');
	
	DOMOVE;
}


void core_clrtoeos(void)
{
	int l;
	
	core_clrtoeol();
	for (l = core_y + 1; l < (core_wy + core_wh); l++) core_clrline(l);
}


void core_init(void)
{
	core_x = 0;
	core_y = 0;
	move(0,0);
	core_wx = 0;
	core_wy = 0;
	core_ww = COLS;
	core_wh = LINES;
	core_wrap = 1;
	core_attr = 0;
	core_pair = 1;
	init_pair(core_pair, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(core_pair));
	bkgdset(COLOR_PAIR(core_pair));
}



void core_setattr(int attr)
{
	if (attr & AT_BRIGHT)     attron(A_BOLD);
	if (attr & AT_UNDERSCORE) attron(A_UNDERLINE);
	if (attr & AT_BLINK)      attron(A_BLINK);
	if (attr & AT_REVERSE)    attron(A_REVERSE);	
	core_attr |= attr;
}

void core_clearattr(int attr)
{
	if (attr & AT_BRIGHT)     attroff(A_BOLD);
	if (attr & AT_UNDERSCORE) attroff(A_UNDERLINE);
	if (attr & AT_BLINK)      attroff(A_BLINK);   
	if (attr & AT_REVERSE)    attroff(A_REVERSE);
	core_attr &= ~attr;
}

void core_setfg(int fg)
{
	core_fg = fg;
	init_pair(core_pair, core_fg, core_bg);
}


void core_setbg(int bg)
{
	core_bg = bg;
	init_pair(core_pair, core_fg, core_bg);
	bkgdset(COLOR_PAIR(core_pair));
}	


void core_cursor(int type)
{
	/* not implemented */
}


void core_ansi_begin(void)
{
	core_ansi_count = 0;
	core_ansi_buf[0] = 0;
}

int core_ansi(char c)
{
	if (c >= '0' && c <= '9')
	{
		core_ansi_buf[core_ansi_count] = (c - '0') + (10 * core_ansi_buf[core_ansi_count]);
		return 0;
	}
	if (c == ';')
	{
		++core_ansi_count;
		if (core_ansi_count == 20) --core_ansi_count;
		core_ansi_buf[core_ansi_count] = 0;
		return 0;
	}
	else
	{
		core_ansi_char = c;
		return 1;
	}
}



int core_term(int func, int param)
{
	switch(func)
	{
		case CPM_TERM_INIT:     core_init(); return 1;
		case CPM_TERM_GETATTR:  return core_attr;
		case CPM_TERM_SETATTR:  core_setattr(param);
					core_clearattr(~param);
					return 1;

		case CPM_TERM_GETCOLOUR: return (core_bg << 4) | core_fg;
		case CPM_TERM_SETCOLOUR: core_setbg((param >> 4) & 0x0F);
					 core_setfg (param & 0x0F);
					 return 1;	
		case CPM_TERM_CLEAR:    clear(); refresh(); return 1;
		case CPM_TERM_MOVE:     core_move(CPM_LO(param), CPM_HI(param));
				        return 1; 
		case CPM_TERM_IFCOLOUR: return has_colors();
		case CPM_TERM_HEIGHT:	if (param > 0 && param < LINES)
						core_win(0, core_wx,
							 core_ww, param);
					return core_wh;
		case CPM_TERM_WIDTH:	if (param > 0 && param < COLS)
						core_win(core_wy, 0, 
                                                         param, core_wh);
					return core_ww;

		case CPM_TERM_WRAP:	if (param == 1 || param == 0) 
						core_wrap = param;
					return core_wrap;
	}
	return 0;
}


