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

/* CPMIO: Public interface - client programs should include this */

void cpm_scr_init(void);	/* Initialise */
void cpm_scr_unit(void);	/* Terminate */

/* BIOS functions */
char cpm_const(void);		/* Console status */
char cpm_conin(void);		/* Console input */
void cpm_conout(char c);	/* Console output */

/* BDOS functions */
int           cpm_bdos_1(void);			/* Console input with echo
                                                 * Returns -1 for break,
                                                 * else character  */
int           cpm_bdos_2(char c);		/* Console output.
                                                 * Returns -1 for break,
                                                 * else 0 */
int           cpm_bdos_6(unsigned char c);	/* Direct console access
                                                 * Returns -1 for break, 
                                                 * else E-value */
int           cpm_bdos_9(char *buf);		/* Print a string. Returns
                                                 * -1 for break, else 0 */
unsigned      cpm_bdos_10(unsigned char *buf);	/* Read a line. 
					 * Note: Calling program has to
                                	 * deal with DE=0 parameter, and set 
                                 	 * the "current length" to 0 if DE is 
					 * nonzero. Returns 0 normally, -1 if
                                         * Control-C was pressed. */
int           cpm_bdos_11(void);		/* Poll console. Returns
                                                 * -1 for break, else 0 or 1 */
unsigned int  cpm_bdos_109(unsigned int de);	/* Console mode set/get */
unsigned char cpm_bdos_110(unsigned int de);	/* String delimiter set/get */
int           cpm_bdos_111(char *buf, unsigned int len);
						/* Output fixed-length string */
						/* Returns -1 for break, 0 if
                                                 * OK */

int cpm_bdos_set_get_typeahead(int flag);	/* Set or get Typeahead flag */


char *cpm_get_terminal(void);		/* Gets terminal ID */
int   cpm_set_terminal(char *s);	/* Sets terminal ID, eg "RAW" "VT52" */
void  cpm_enum_terminals(char *s);	/* List all terminal IDs */

int cpm_term_direct(int function, int param); /* Direct control of terminal */

/* Functions for term_direct: */

#define CPM_TERM_INIT      0	/* Initialise a terminal */
#define CPM_TERM_GETATTR   1	/* Get bold/reverse etc */
#define CPM_TERM_SETATTR   2	/* Set bold/reverse etc */
#define CPM_TERM_GETCOLOUR 3	/* Get screen colour as byte */
#define CPM_TERM_SETCOLOUR 4	/* Set screen colour as byte */
#define CPM_TERM_CLEAR     5	/* Clear screen */
#define CPM_TERM_MOVE      6	/* Move cursor */
#define CPM_TERM_IFCOLOUR  7	/* Is this a colour screen? */
#define CPM_TERM_CHAR     20	/* Write character */
#define CPM_TERM_HEIGHT   60	/* Get/set terminal height */
#define CPM_TERM_WRAP     61    /* Get/set wrap mode */
#define CPM_TERM_WIDTH    62    /* Get/set terminal width */

