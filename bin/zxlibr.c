#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char cmdbuf[1024];

int main(int argc, char **argv)
{	
	int n;

	strcpy(cmdbuf,"zxcc libr.com ");
	for (n = 1; n < argc; n++)
	{
		if (n == 1)	/* 1st argument is an option */
		{
			strcat(cmdbuf,"-");
		}
		strcat(cmdbuf,argv[n]);
		strcat(cmdbuf," ");
	}
	return system(cmdbuf);
}
