/*

    CPMIO: CP/M console emulation library
    Copyright (C) 1998 - 1999,2003, John Elliott <jce@seasip.demon.co.uk>

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
#ifdef __MSDOS__
#include <dos.h>
#else
# ifdef USE_READLINE 
#  include <readline/readline.h>
#  include <readline/history.h>
# endif
#endif

static void update (int n, int c, char *s);

static unsigned int console_mode = 0;
static char delimiter = '$';
static char kbchar = 0;
static int func1 = 0;
static int list_echo = 0;
static int cpm_typeahead = 0;

static char conin(void);
static int constat(void);
static int conout(char c);
static int printable(char c);
static int on_ctls(void);
static int check_ctls(int called_by_func_11);

extern int cpmio_using_curses;

void cpm_console_bdos_init(void)
{
	func1 = 0;
	kbchar = 0;
	delimiter = '$';
	list_echo = 0;
	console_mode = 0;
	cpm_typeahead = 0;
}

/* BDOS functions */
int cpm_bdos_1(void)	/* Console input with echo */
{
	char c;
	int r;

	func1 = 1;
	do
	{
		c = conin();

		if (printable(c)) 	/* Printable char. Echo it & return */
		{			/* it. */
			if (conout(c)) r = -1; else r = c;
			func1 = 0;

			return r;
		}
		if (console_mode & 2) break; 	/* If not checking ^S,      */
                                                /* return ctl codes as well */
		if ( c == CTL_S && on_ctls()) 
		{
			func1 = 0;	/* ^S was followed by ^C */
			return -1;
		}
	}
	while (c != CTL_Q && c != CTL_P && c != CTL_S);

	func1 = 0;
	return c;
}

int cpm_bdos_2(char c)
{
	/* TODO: Tab expansion? 
	 *
	 * if (!(console_mode & 0x14)) return tabout(c);
	 *
	 */
	return conout(c);
}



int cpm_bdos_6(unsigned char c)	/* Direct console access */
{
	switch(c)
	{
		case 0xFF:	if (cpm_const()) return cpm_conin(); 
				else             return 0;
		case 0xFE:	return cpm_const();
		case 0xFD:	return cpm_conin(); 
		default: 	cpm_conout(c); break;
	}
	return 0;
}


int cpm_bdos_9(char *buf)
{
	while (*buf != delimiter)
	{
		cpm_bdos_2(*buf++);
	}
	return 0;
}



static void update (int n, int c, char *s);

/* Under DOS, line input uses the DOS line input */
#ifdef __MSDOS__
int cpm_bdos_10(unsigned char *buffer)
{
	if (!cpmio_using_curses) cpm_scr_unit(); 
	else 
	{
		refresh();
		echo();
	}
	bdosptr(0x0A, buffer, 0);
	if (!cpmio_using_curses) cpm_scr_init(); 
	else
	{
		echo();
	}
	return 0;
}

#else

/* If using readline, line input uses it */
#if USE_READLINE
int cpm_bdos_10(unsigned char *buffer)
{
	char *s;

	if (!cpmio_using_curses) cpm_scr_unit(); 
	else 
	{
		refresh();
		echo();
	}
	s = readline(""); 
	if (!cpmio_using_curses) cpm_scr_init(); 
	else
	{
		echo();
	}
	if (!s)
	{
		buffer[1] = 0;
	}
	if (strlen(s) > buffer[0])
	{
		buffer[1] = buffer[0];
		strncpy(buffer + 2, s, buffer[0]);
	}
	else
	{
		buffer[1] = strlen(s);
		strcpy(buffer + 2, s);
	}
	free(s);
	return 0;
}

#else /* Not using MSDOS or readline */
/* [reported by Andy Parkins] Make this take unsigned char; on systems 
 * where char is signed, we can end up with a -ve buffer size. */
int cpm_bdos_10(unsigned char *buffer)
{
	char c;
	int maxlen, curlen, curpos, editing, n;
        static char prev[20][257];
        int prevn = 0;

	curlen = buffer[1];
	maxlen = buffer[0];
	buffer[curlen + 2] = 0;
	editing = 1;
	curpos = curlen;

	update(curlen, curpos, buffer + 2);
	while (editing)
	{	
		c = cpm_conin();

		switch(c)
		{
			case '\n':
			case '\r':
				editing = 0;
				break;	
			case 8:
			case 127:	/* Delete & Backspace */
				if (!curpos || !curlen) break;
				for (n = curpos + 1; n <= curlen + 2; n++)
				{
					buffer[n] = buffer[n+1];
				}
				--curlen;
				update(curlen, curpos, buffer + 2);
				--curpos;
				break;

			case 'W' - '@':
			case 'E' - '@':
				if (c == 'E' - '@')
				{
					if (!prevn) prevn = 19; else --prevn;
				}
				n = prev[prevn][0];
				if (n > buffer[0]); n = buffer[0];
				++n;
				memcpy(buffer + 1, &prev[prevn][0], n);
				if (curlen > buffer[1]) 
				{
					memset(buffer + 2 + buffer[1],
					       curlen - buffer[1], ' ');
					update(curlen, curpos, buffer + 2);
				}
				curlen = buffer[1];
				update(curlen, curpos, buffer + 2);
                                if (curpos > buffer[1]) curpos = buffer[1];
				if (c == 'W' - '@') 
				{
					if (prevn < 20) ++prevn; else prevn = 0;
				}
				break;	

                        case 3: /* Control-C */
                                if (curpos == 0) return -1;
				break;

			default:
				if (c < 32 && c != 26) break;
				if (curlen >= maxlen) break;
				
				if (curpos < curlen)
				{
					for (n = curlen; n > curpos; --n)
					{
						buffer[n + 2] = buffer[n + 1];
					}
				}
				buffer[curpos + 2]   = c;
				++curlen;
				update(curlen, curpos, buffer + 2);
				++curpos;
				break;
		}
	}
	buffer[1] = curlen;
	for (n = 19; n > 0; n--) memcpy(&prev[n][0], &prev[n - 1][0], 257);
	memcpy(&prev[0][0], buffer + 1, buffer[1] + 1);
	return 0;
}
#endif /* no readline */
#endif /* not DOS */


int cpm_bdos_11(void)			/* Poll console */
{
	char ch;

	if (console_mode & 1)
	{
		if (kbchar == 3) return 1;
		if (!cpm_const()) return 0;
		ch = cpm_conin();
		if (ch == 3) return 1;
		else kbchar = ch;
		return 0;
	}
	if (console_mode & 2) return constat();

	/* Mimic undocmented "typeahead" behaviour */	
	if ((cpm_typeahead == 0xFF) && !kbchar) return constat();

	return check_ctls(1);
}


unsigned int  cpm_bdos_109(unsigned int de)	/* Console mode set/get */
{
	if (de != 0xFFFF) console_mode = de;

	return console_mode;
}


unsigned char cpm_bdos_110(unsigned int de)	/* String delimiter set/get */
{
	if (de != 0xFFFF) delimiter = (de & 0xFF);

	return delimiter;
}

int cpm_bdos_111(char *buf, unsigned int len)
{
	while(len) if (cpm_bdos_2(*buf++)) return -1;

	return 0;
}

/* Set/clear the SCB "Typeahead" byte */
int cpm_bdos_set_get_typeahead(int flag)
{
	if (flag > 0) cpm_typeahead = flag;

	return cpm_typeahead;
}


/* Additional functions */

static void update (int n, int c, char *s)
{
        while (c--) cpm_conout(8);
        while (*s && (n--)) cpm_conout(*(s++));
        cpm_conout(' ');
        cpm_conout(8);
}

/* Get key */

static char conin(void)
{
        char ch;

        if (kbchar)
        {
                ch = kbchar;
                kbchar = 0;
                return ch;
        }
        return cpm_conin();
}

/* Output character */
static int conout(char c)
{
	if (!func1 && (check_ctls(0) == -1)) return -1;

	cpm_conout(c);

/* TODO: Printer echo? 
 *
 *	if (!(console_mode & 0x14) && list_echo) cpm_list(c);
 * 
 */
	return 0;
}


/* Is character "c" printable? */
static int printable(char c)
{
	return (c == '\r' || c == '\n' || c == 9 || c == 8 || c >= 32);
}




static int on_ctls(void)
{
	char c;

	do
	{
		c = cpm_conin();
		if (c == CTL_C && (!(console_mode & 8))) return -1;
	
		if (c == CTL_Q) return 0;
		if (c == CTL_P) 
		{ 
			list_echo ^= 1; 
			if (list_echo) cpm_conout(7);
		}
	} 
	while (c != CTL_Q && c != CTL_P);

	return 0;
}


static int check_ctls(int called_by_func_11)
				/* Returns -1 to exit, 
                                 *         0 no char waiting
                                 *         1 char waiting
                                 */          
{
	char ch;

	if (called_by_func_11)
	{
		if (console_mode & 2) return constat();
	}
	else if (console_mode & 2) return 0;	/* Not checking ^S */

	ch = kbchar;
	if (!(called_by_func_11 && kbchar))
	{
		if (ch != CTL_S)
		{
			if (!cpm_const()) return 0; 	/* Nothing waiting */
			ch = cpm_conin();	/* Something was waiting   */
		}
	}

	if (ch == CTL_S)
	{
		if (kbchar == ch) kbchar = 0;

		/* Got a ^S */

		if (on_ctls()) return -1;
	}
	if (!(console_mode & 1) && kbchar == 3) return 1;
	if (ch != CTL_Q && ch != CTL_P) 
	{
		kbchar = ch; 
		return 1;
	}
	return 0;
}


static int constat(void)
{
	if (kbchar) return 1;
	if (cpm_const()) return 1;
	return 0;
}
