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

/* This program is not part of the library itself; it tests some features 
  of the VT52 emulation.
 */

#include <stdio.h>
#include <string.h>
#include "cpmio.h"

static int native = 0, termios = 0;

void conout(char c)
{
	if (native) putchar(c);
	else cpm_conout(c);
}


char conin(void)
{
	if (native) return getchar();
	else	    return cpm_conin();
}


void vt52_move(int y, int x)
{
	conout(27);
	conout('Y');
	conout(y+32);
	conout(x+32);
}

void cpm_print(char *s)
{
        while (*s) conout(*s++);
}


void esc(char c)
{
        conout(27); conout(c);
}


void compass(void)
{
	esc('E');
	vt52_move(0,30); cpm_print("NW   N   NE");
	vt52_move(1,30); cpm_print(" \\   |   / ");
	vt52_move(2,30); cpm_print("  \\  |  /  ");
	vt52_move(3,30); cpm_print("   \\ | /   ");
	vt52_move(4,30); cpm_print("    \\|/    ");
	vt52_move(5,30); cpm_print("W----O----E");
	vt52_move(6,30); cpm_print("    /|\\    ");
	vt52_move(7,30); cpm_print("   / | \\   ");
	vt52_move(8,30); cpm_print("  /  |  \\  ");
	vt52_move(9,30); cpm_print(" /   |   \\ ");
	vt52_move(10,30);cpm_print("SW   S   SE");	

	esc('p');
	vt52_move(5,35); conout('*');
	esc('q');
	vt52_move(15,0);
}

void compass_tests(void)
{
	compass();
	cpm_print("Cursor positioning");
	conin();
	compass();
	vt52_move(5,36); esc('K');
	vt52_move(15,0); cpm_print("Delete to EOL");
	conin();
	compass();
	vt52_move(5,36); esc('J');
	vt52_move(15,0); cpm_print("Delete to EOS");
	conin();
	compass();
	vt52_move(5,35); esc('o');
	vt52_move(15,0); cpm_print("Delete to BOL");
	conin();
	compass();
	vt52_move(5,35); esc('d');
	vt52_move(15,0); cpm_print("Delete to BOS");
	cpm_print("\r\nPress SPACE for scrolling tests");
	conin();
	esc('E');	
}

void scrolling_tests()
{
	char s[20];
	int n;

	esc('H');
	for (n = 0; n < 30; n++) 
	{
		if (n > 23) conin();
		sprintf(s, "%d\r\n", n); 
		cpm_print(s);
	}
	conin();
}



int main(int argc, char **argv)
{
	char c;

	if (argc > 1 && (!strcmp(argv[1], "-native"))) native = 1;
	if (argc > 1 && (!strcmp(argv[1], "-termios"))) termios = 1;

	if (!native) 
		{
		cpm_scr_init();
		if (termios) cpm_set_terminal("TERMIOS");
		else cpm_set_terminal("VT52");
		}
	cpm_print("Press SPACE to do the compass rose tests");
	conin();
	compass_tests();
	scrolling_tests();
	
	cpm_print("Hello ");
	esc('p');
	cpm_print("World");
	esc('q');
	cpm_print("!");
	esc('Y');
	cpm_print("$*");
	esc('r');
	cpm_print("<<-->>");
	esc('u');
	esc('s');
	cpm_print("@@@@@");
	esc('t');

	esc('(');
	cpm_print("*****");
	esc(')');

	cpm_print("Press SPACE to do keyboard input tests");

	conin();

	esc('E');
	esc('H');
	cpm_print("Press keypad keys to see the characters they produce. SPACE"
                  " finishes.\r\n");
	while((c = conin()) != ' ')
	{
		if (c < 32) { conout('^'); conout(c + '@'); }
		else	conout(c);
	}

	if (!native) cpm_scr_unit();	
	return 0;
}
