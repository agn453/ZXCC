#include "zxcc.h"
#include "zxbdos.h"
#include "zxcbdos.h"

#ifdef __MSDOS__
#include <conio.h>
#endif

/* Line input */
/* modified to allow <file even is USE_CPMIO is defined */
void bdos_rdline(word line, word *PC)
{
	if (!line) line = cpm_dma;
	else RAM[line + 1] = 0;

#if USE_CPMIO
    if (!file_conin) {
        if (cpm_bdos_10(&RAM[line])) *PC = 0;
        return;
    }
#endif
    /* if USE_CPMIO is not defined or stdin is redirected from a file */
    /* cpm rdline accepts up to specified number of characters
     * the terminating \r or \n is excluded from this count
     * additionally using fgets adds a \0 at the end of line
     * which should also be excluded from the count
     * to support this the fgets reads to a temporary buffer
     * that has two bytes in case the specified number of bytes
     * are on the line, before the \n
     * unix also doesn't convert \r\n to \n so additional
     * processing is required to skip the \r
     */
	int maxlen = RAM[line];
    char buf[257];     /* allow for 255 byte input + eol + '\0' */
    int len;

	if (fgets(buf, maxlen + 2, stdin)) {    /* allow for \0 terminator */
        len = strlen(buf);
        if (len && buf[len - 1] == '\n') {
            if (--len && buf[len - 1] == '\r')  /* fix \r\n under unix */
                len--;
        } else if (len && buf[len - 1] == '\r') {  /* max line under unix */
            int c;
            if ((c = getc(stdin)) != '\n')  /* absorb the \n */
                ungetc(c, stdin);
            len--;
        } else if (len > maxlen)    /* line longer than maxlen */
            ungetc(buf[--len], stdin);   /* so unget extra char */
    } else
        len = 0;
	RAM[line + 1] = len;
    if (len)
        memcpy(&RAM[line + 2], buf, len);
    if (file_conin) {      /* echo to the console if from file */
        for (int i = 0; i < len; i++)
            cpm_conout(buf[i]);
        cpm_conout('\r');
    } 
	Msg("Input: [%d] %-*.*s\n", RAM[line + 1], RAM[line + 1], RAM[line +1], (char *)(RAM+line+2));
}

#ifndef USE_CPMIO


int cpm_bdos_6(byte e)
{
	int c;

	switch(e)
	{
#ifdef __MSDOS__
		case 0xFF:	/* Not implemented */
		if (kbhit()) return getch();
		return 0;

		case 0xFE:
		return kbhit();	

		case 0xFD:
		c = getch();
		return c;

		default:
		putch(e);
		break;
#else /* __MSDOS__ */
		case 0xFF:	/* Not implemented */
		return 0;

		case 0xFE:
		return 0;	

		case 0xFD:
		c = getchar();
		return c;

		default:
		putchar(e);
		break;
#endif
	}
	return 0;
}
#endif


