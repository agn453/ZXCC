
#include "zxcc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char cmdbuf[1024];


int main(int argc, char **argv)
{	
	int n;

	strcpy(cmdbuf,"zxcc objtohex.com ");
	for (n = 1; n < argc; n++)
	{
		if (argv[n][0] == '-')	/* An option */
			strcat(cmdbuf,"-");
		strcat(cmdbuf,argv[n]);
		strcat(cmdbuf," ");
	}	
	return system(cmdbuf);
}
