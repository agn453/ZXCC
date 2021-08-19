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

static char cpm_waiting;	/* Character waiting for conin */
static time_t last_refresh;	/* Set to the time the screen was last 
                                   refreshed */
static int do_refresh;		/* Set to 1 if the screen has been written
                                 * since last refresh() */
int cpmio_using_curses;		/* Using curses or termios? */
static int terminal;		/* Terminal type */

typedef int  (*TERMFUNC)(int what, int c);
typedef void (*VOIDFUNC)(void);

/* Terminals. If you want to add one, add it to these two lines */
static char     *term_desc[] = {"TERMIOS", "RAW",    "GENERIC",    
                                "ANSI",    "VT52",    NULL};
static TERMFUNC term_funcs[] = {termios_term, raw_term,  generic_term, 
                                ansi_term,    vt52_term, NULL};

static int filen;

static struct termios ts, ots;

static void curses_off(void);
static char key_xlt(int key);

static void curses_on(void)
{
	if (cpmio_using_curses) return;

	/* Switch to raw mode, not cbreak. The program will want ^Z ^C etc. */
	initscr(); raw(); noecho();

	start_color();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	last_refresh = time(NULL);
	do_refresh   = 0;

	cpmio_using_curses = 1;
}


void cpm_scr_init(void)
{
	char *s;

	fflush(stdin);
	filen = fileno(stdin);
        cpm_waiting = 0;
        terminal = 0;
	curses_off();
        tcgetattr(filen, &ts);
 	tcgetattr(filen, &ots); 
	cfmakeraw(&ts);
	tcsetattr(filen, TCSANOW, &ts);

	s = getenv("CPMTERM");
	if (s) 
	{
		if (cpm_set_terminal(s)) 
		{
			fprintf(stderr, "No ZXCC driver for CPMTERM=%s; assuming CPMTERM=%s\r\n",
				s, term_desc[0]);
			cpm_set_terminal(term_desc[0]);
		}
	}
	else cpm_set_terminal(term_desc[0]);

	cpm_console_bdos_init();
}



static void curses_off(void)
{
	if (!cpmio_using_curses) return;
	endwin();
	cpmio_using_curses = 0;
}


void cpm_scr_unit(void)
{
	curses_off();
	tcsetattr(filen, TCSANOW, &ots);
}


char termios_const(void)
{
	int i;
	fd_set rfds;
	struct timeval tv;

	if (cpm_waiting) return 1;

	fflush(stdout);
	FD_ZERO(&rfds);
	FD_SET(filen, &rfds);	

	tv.tv_sec = tv.tv_usec = 0;

	i = select(1, &rfds, NULL, NULL, &tv);	

	if (i) return 1;	/* Data ready */
	else return 0;		/* Data not ready */
}


char termios_conin(void)
{
	char c;
	int i;

	if (cpm_waiting)
	{
		c = cpm_waiting;
		cpm_waiting = 0;
		return c;
	}

	fflush(stdout);
	do
	{
		i = read(filen, &c, 1);
	} while (i < 0);
	return c;
}


char cpm_const(void)
{
	int c;

	if (!cpmio_using_curses) return termios_const();

        if (do_refresh && last_refresh != time(NULL))
        {
                if (terminal) refresh();
                do_refresh = 0;
		last_refresh = time(NULL);
        }

	if (cpm_waiting) return 1;

	nodelay(stdscr, 1);
	c = getch();
	nodelay(stdscr, 0);
	raw();

	if (c == ERR) return 0;
	cpm_waiting = key_xlt(c);
	return 1;
}

char cpm_conin(void)
{
	int ch;

	if (!cpmio_using_curses) return termios_conin();

        if (do_refresh && last_refresh != time(NULL))
        {
                if (terminal) refresh();
                do_refresh = 0;
		last_refresh = time(NULL);
        }

	if (cpm_waiting)
	{
		char c = cpm_waiting;
		cpm_waiting = 0;	
		return c;
	}
	
	ch = getch();
	return key_xlt(ch);
}


int cpm_term_direct(int func, int param)
{
        TERMFUNC tf = term_funcs[terminal];

        return ((*tf)(func, param));
}




void cpm_conout(char c)
{
	TERMFUNC tf = term_funcs[terminal];
	
	if ((*tf)(CPM_TERM_CHAR, c) == 0) return;

	do_refresh = 1;
	if (last_refresh != time(NULL))
	{
		if (terminal) refresh();
		do_refresh = 0;
		last_refresh = time(NULL);
	}
}


int cpm_set_terminal(char *s)
{
	int t;
	TERMFUNC tf;

	for (t = 0; term_desc[t]; ++t)
	{
		if (!strcmp(s, term_desc[t])) 
		{
			tf = term_funcs[t];
			if (terminal != t) 
			{
				if (t) curses_on(); else curses_off();
				(*tf)(CPM_TERM_INIT, 0);
			}
			terminal = t;
			return 0;
		}
	}
	return -1;
}



char *cpm_get_terminal(void)
{
	return term_desc[terminal];
}


void cpm_enum_terminals(char *c)
{
	int l;
	int t = 0;
	char *desc = term_desc[t];

	while (desc != NULL)
	{
		l = 1 + strlen(desc);
		memcpy(c, desc, l);
		c += l;
		desc = term_desc[++t];
	}
	*(c++) = '\0';
	*c     = '\0';	
}


/**********************************************************************/
/* "Termios" terminal (bypassing ncurses) */

int termios_term(int func, int param)
{
	if (func == CPM_TERM_CHAR) 
	{
		putchar(param);
		return 1;
	}
	return 0;
}

/**********************************************************************/
/* "Raw" terminal */


int raw_term(int func, int param)
{
	switch(func)
	{
		case CPM_TERM_INIT:  core_init();  return 1; 
		case CPM_TERM_CHAR:  addch(param); return 1;
	}  
	return core_term(func, param);
}


int raw_char(char c)
{
	addch(c);
	return 1;	/* Screen was changed */
}


/*********************************************************************/
/*  Billy Chen's Wordstar translations. These translate PC extended  */
/*  keycodes to Wordstar keys, and some of them seem to be unique to */
/*  PDCURSES. I've included a subset for NCurses afterwards.         */
/*********************************************************************/

char key_xlt(int ch)
{
	char a = ch;
#ifdef __PDCURSES__
	switch(ch)
	{
		case KEY_DOWN:  a = 0x0018; break; /* Down -> ^X */
		case KEY_UP:    a = 0x0005; break; /* Up   -> ^E */
		case KEY_LEFT:  a = 0x0013; break; /* Left -> ^S */
		case KEY_RIGHT: a = 0x0004; break; /* Right-> ^D */
		case KEY_HOME:  a = 0x0011; ungetch(0x053); break; /* Home -> ^QS */
		case KEY_DC:    a = 0x0007; break; /* Del -> ^G */
		case KEY_IC:    a = 0x0016; break; /* Ins -> ^V */
		case KEY_NPAGE: a = 0x0003; break; /* PgDn -> ^C */
		case KEY_PPAGE: a = 0x0012; break; /* PgUp -> ^R */
		case KEY_END:   a = 0x0011; ungetch(0x0044); break; /* End -> ^QD */
		case CTL_LEFT:  a = 0x0001; break; /* ctl-left  -> ^A */
		case CTL_RIGHT: a = 0x0006; break; /* ctl-right -> ^F */
		case CTL_PGUP:  a = 0x0011; ungetch(0x0012); break; /* ctl-pgup */
		case CTL_PGDN:  a = 0x0011; ungetch(0x0003); break; /* ctl-pgdn */
		case CTL_HOME:  a = 0x0011; ungetch(0x0005); break; /* ctl-home */
		case CTL_END:   a = 0x0011; ungetch(0x0018); break; /* ctl-end */

		case KEY_B2:    a = 0x0001; break; /* center -> ^A */
		case PADSLASH:  a = 0x002f; break; /* grey / */
		case PADENTER:  a = 0x000d; break; /* grey enter */
		case PADSTAR:   a = 0x002a; break; /* grey * */
		case PADMINUS:  a = 0x002d; break; /* grey - */
		case PADPLUS:   a = 0x002b; break; /* grey + */
		default: break;
	}
#else /* def __PDCURSES__ */
	switch(ch)
	{
		case KEY_DOWN:  a = 0x0018; break; /* Down -> ^X */
		case KEY_UP:    a = 0x0005; break; /* Up   -> ^E */
		case KEY_LEFT:  a = 0x0013; break; /* Left -> ^S */
		case KEY_RIGHT: a = 0x0004; break; /* Right-> ^D */
		case KEY_HOME:  a = 0x0011; ungetch(0x053); break; /* Home -> ^QS */
		case KEY_DC:    a = 0x0007; break; /* Del -> ^G */
		case KEY_IC:    a = 0x0016; break; /* Ins -> ^V */
		case KEY_NPAGE: a = 0x0003; break; /* PgDn -> ^C */
		case KEY_PPAGE: a = 0x0012; break; /* PgUp -> ^R */
		case KEY_END:   a = 0x0011; ungetch(0x0044); break; /* End -> ^QD */
		case KEY_B2:    a = 0x0001; break; /* center -> ^A */
		default: break;
	}
#endif /* def __PDCURSES__ */

	return a;
}
