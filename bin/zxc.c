
#include "zxcc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char cmdbuf[1024];
char incdir80[CPM_MAXPATH] = { INCDIR80 " "};

static void mkpath(char* fullpath, char* path, char* subdir);
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
	/* modified to use environment variables if defined */
	char* tmpenv = getenv("INCDIR80");
	if (tmpenv)
		mkpath(incdir80, tmpenv, " ");
	else if (tmpenv = getenv("CPMDIR80"))
		mkpath(incdir80, tmpenv, INC80 " ");

	strcpy(cmdbuf, "zxcc c.com --I +");
	strcat(cmdbuf, incdir80);

	for (n = 1; n < argc; n++)
	{
		if (argv[n][0] == '-')	/* An option */
		{
			strcat(cmdbuf,"-");

			/* The following options can take filenames */

			if (fname_opt(argv[n], 'e')) continue; /* executable */
			if (fname_opt(argv[n], 'f')) continue; /* symbol */
			if (fname_opt(argv[n], 'i')) continue; /* include dir */
			if (fname_opt(argv[n], 'm')) continue; /* map */
			if (cref_opt(argv[n])) continue; /* cross-referencce */
		}
		strcat(cmdbuf,argv[n]);
		strcat(cmdbuf," ");
	}	
	return system(cmdbuf);
}

/* helper function to build full path */
/* make sure that a / or \ is present at the end of path
 * before appending the subdir
 */
static void mkpath(char* fullpath, char* path, char* subdir) {
	char* s;
	strcpy(fullpath, path);
	s = strchr(fullpath, '\0');
	if (*fullpath && !ISDIRSEP(s[-1]))  /* make sure we have dir sep */
		*s++ = '/';
	strcpy(s, subdir);
}
