
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

int main(int argc, char **argv)
{	
	int n;

	strcpy(cmdbuf,"zxcc link.com ");
	for (n = 1; n < argc; n++)
	{
		if (argv[n][0] == '-')	/* An option */
		{
			strcat(cmdbuf,"-");

			/* The following options can take filenames */

			if (fname_opt(argv[n], 'o')) continue;
			if (fname_opt(argv[n], 'm')) continue;
			if (fname_opt(argv[n], 'd')) continue;
		}
		strcat(cmdbuf,argv[n]);
		strcat(cmdbuf," ");
	}	
	return system(cmdbuf);
}
