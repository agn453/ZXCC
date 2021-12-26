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
    /* this version does not use fgets as it introduces complexities
     * as it needs a trailing '\0' and does not translate \r\n
     * under unix / linux
     */
	int maxlen = RAM[line];
    int len = 0;
    int c;
    while ((c = cpm_conin()) != '\n' && !eof_conin) {
        if (c != '\r' && len < maxlen) {
            RAM[line + 2 + len++] = c;
            if (file_conin) {
                if (c < 0x20) {
                    cpm_conout(c);
                    c += 0x40;
                }
                cpm_conout(c);
            }
        }
    }
    if (file_conin)
        cpm_conout('\r');
    RAM[line + 1] = len;
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


