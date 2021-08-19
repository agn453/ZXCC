
#include "zxcc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char cmdbuf[1024];

int fname_opt(char *arg, char c)
{
	if ((arg[1] == c || arg[1] == toupper(c)) && arg[2])
	{
		sprintf(cmdbuf + strlen(cmdbuf), "-%c +%s", c, arg + 2);
		strcat(cmdbuf," ");
		return 1;	
	}
	return 0;
}

int cref_opt(char *arg)
{
	if ((arg[1] == 'c' || arg[1] == 'C') &&
            (arg[2] == 'r' || arg[2] == 'r') && arg[3])
	{
		strcat(cmdbuf, "-cr +");
		strcat(cmdbuf, arg + 3);
		strcat(cmdbuf, " ");
		return 1;
	}
	return 0;
}



int main(int argc, char **argv)
{	
	int n;

	strcpy(cmdbuf,"zxcc c.com --I +" INCDIR80 " " );
	for (n = 1; n < argc; n++)
	{
		if (argv[n][0] == '-')	/* An option */
		{
			strcat(cmdbuf,"-");

			/* The following options can take filenames */

			if (fname_opt(argv[n], 'i')) continue;
			if (fname_opt(argv[n], 'o')) continue;
			if (fname_opt(argv[n], 'm')) continue;
			if (fname_opt(argv[n], 'h')) continue;
			if (fname_opt(argv[n], 'g')) continue;
			if (cref_opt(argv[n])) continue;
		}
		strcat(cmdbuf,argv[n]);
		strcat(cmdbuf," ");
	}	
	return system(cmdbuf);
}
