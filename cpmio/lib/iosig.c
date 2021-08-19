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

#include <stdio.h>
#include <signal.h>
#include "cpmio_i.h"


RETSIGTYPE on_sigint(int s)		/* Called if there's a fatal signal */
{
	cpm_scr_unit();		/* Clear screen, end ncurses */
	signal(s, SIG_DFL);
	raise(s);		/* Re-throw */
}

