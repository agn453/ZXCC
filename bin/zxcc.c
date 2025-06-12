#include "zxcc.h"

/* Global variables */

char *progname;
char **argv;
int argc;

byte cpm_drive;	
byte cpm_user;
extern byte cpm_error;

char bindir80[CPM_MAXPATH] = { BINDIR80 };
char libdir80[CPM_MAXPATH] = { LIBDIR80 };
char incdir80[CPM_MAXPATH] = { INCDIR80 };

byte RAM[65536]; /* The Z80's address space */

void load_comfile(void);	/* Forward declaration */

static int deinit_term, deinit_gsx;
static void mkpath(char* fullpath, char* path, char* subdir);


void dump_regs(FILE *fp, byte a, byte b, byte c, byte d, byte e, byte f,
             byte h, byte l, word pc, word ix, word iy)
{
    fprintf(fp, "\tAF=%02x%02x BC=%02x%02x DE=%02x%02x HL=%02x%02x\n"
                    "\tIX=%04x IY=%04x PC=%04x\n",
            a,f,b,c,d,e,h,l,pc,ix,iy);
}



char *parse_to_fcb(char *s, int afcb)
{
    byte *fcb = &RAM[afcb+1];

    RAM[afcb] = 0;
    memset(fcb, ' ', 11);

    while (s[0]==' ')	/* skip leading spaces */
    {
        s++;
    }
    while (1)
    {
        if (s[0] == 0) break;	
        if (s[0] == ' ') break;
        if (s[1] == ':')
        {
            RAM[afcb] = s[0] - '@';
            if (RAM[afcb] > 16) RAM[afcb] -= 0x20;
            s+=2;
            continue;
        }
        if (s[0] == '.')
        {
            ++s;
            fcb = &RAM[afcb+9];
            continue;
        }
        *fcb = *s;  if (islower(*fcb)) *fcb = toupper(*fcb);
        ++s;
        ++fcb;	
        if (fcb >= &RAM[afcb+12]) break;
    }
    return s;
}


void Msg(char *s, ...)
{
#ifdef DEBUG
    va_list ap;

        va_start(ap, s);
    printf("%s trace: ", progname);
    vprintf(s, ap);
    fflush(stdout);
    if (s[strlen(s) - 1] == '\n') putchar('\r');
    va_end(ap);
#endif
}


void ed_fe(byte *a, byte *b, byte *c, byte *d, byte *e, byte *f,
             byte *h, byte *l, word *pc, word *ix, word *iy)
{
    switch(*a)
    {
        case 0xC0:
        cpmbdos(a,b,c,d,e,f,h,l,pc,ix,iy);
        break;

        case 0xC1:
        load_comfile();
        break;

        case 0xC2:
        fprintf(stderr,"%s: Incompatible BIOS.BIN\n", progname);
        zxcc_term();
        zxcc_exit(1);

        case 0xC3:
        cpmbios(a,b,c,d,e,f,h,l,pc,ix,iy);
        break;

        default:
        fprintf(stderr, "%s: Z80 encountered invalid trap\n", progname);
        dump_regs(stderr,*a,*b,*c,*d,*e,*f,*h,*l,*pc,*ix,*iy);
        zxcc_term();
        zxcc_exit(1);

    }
}


/* 
 * load_bios() loads the minimal CP/M BIOS and BDOS.
 *
 */

void load_bios(void)
{
    char dir[CPM_MAXPATH + 1], fname[CPM_MAXPATH + 1];
    char* q;
    size_t bios_len;

    FILE* fp = fopen("bios.bin", "rb");
    if (!fp)
    {
        strcpy(fname, bindir80);
        strcat(fname, "bios.bin");
        fp = fopen(fname, "rb");
    }
    if (!fp)
    {
#ifdef _MSC_VER
        dir[0] = 0;	/* use strncat in case the path is very long */
        strncat(dir, _pgmptr, CPM_MAXPATH - 8);   /* copy the executable path */
#elif defined(__APPLE__)
	uint32_t size = CPM_MAXPATH - 8;
	_NSGetExecutablePath(dir, &size);
#else
        readlink("/proc/self/exe", dir, CPM_MAXPATH - 8);   /* allow room for bios.bin */
#endif
        q = strrchr(dir, DIRSEPCH);
        *++q = 0;
        strcpy(fname, dir);
        strcat(fname, "bios.bin");
        fp = fopen(fname, "rb");
    }
    if (!fp)
    {
        fprintf(stderr, "%s: Cannot locate bios.bin\n", progname);
        zxcc_term();
        zxcc_exit(1);
    }
    bios_len = fread(RAM + 0xFE00, 1, 512, fp);
    if (bios_len < 1 || ferror(fp))
    {
        fclose(fp);
        fprintf(stderr, "%s: Cannot load bios.bin\n", progname);
        zxcc_term();
        zxcc_exit(1);
    }
    fclose(fp);

    Msg("Loaded %d bytes of BIOS\n", bios_len);
}
/*
 * try_com() attempts to open file, file.com, file.COM, file.cpm and file.CPM
 *
 */

FILE *try_com(char *s)
{
    char fname[CPM_MAXPATH + 1];
    FILE *fp;
    
    strcpy(fname, s);
                                fp = fopen(s, "rb"); if (fp) return fp;
    sprintf(s,"%s.com", fname); fp = fopen(s, "rb"); if (fp) return fp;
    sprintf(s,"%s.COM", fname); fp = fopen(s, "rb"); if (fp) return fp;
    sprintf(s,"%s.cpm", fname); fp = fopen(s, "rb"); if (fp) return fp;
    sprintf(s,"%s.CPM", fname); fp = fopen(s, "rb"); if (fp) return fp;

    strcpy(s, fname);
    return NULL;
}

/*
 * load_comfile() loads the COM file whose name was passed as a parameter.
 *
 */


void load_comfile(void)
{
    size_t com_len;
    char fname[CPM_MAXPATH + 1];
    FILE *fp;

    /* Look in current directory first */
    strcpy(fname, argv[1]);
    fp = try_com(fname);
    if (!fp) 
    {
        strcpy(fname, bindir80);
        strcat(fname, argv[1]);
        fp = try_com(fname);
    }
    if (!fp)
    {
        fprintf(stderr,"%s: Cannot locate %s, %s.com, %s.COM, %s.cpm _or_ %s.CPM\r\n", 
                progname, argv[1], argv[1], argv[1], argv[1], argv[1]);
        zxcc_term();
        zxcc_exit(1);
    }
    com_len = fread(RAM + 0x0100, 1, 0xFD00, fp);
    if (com_len < 1 || ferror(fp))
    {
        fclose(fp);
        fprintf(stderr,"%s: Cannot load %s\n", progname, fname);
        zxcc_term();
        zxcc_exit(1);
    }
    fclose(fp);
    
    Msg("Loaded %d bytes from %s\n", com_len, fname);
}

unsigned int in(int a, int b, int c) { return 0; }
unsigned int out(int a, int b, int c, int d) { return 0; }



/* 
 * xltname: Convert a unix filepath into a CP/M compatible drive:name form.
 *          The unix filename must be 8.3 or the CP/M code will reject it.
 *
 * This uses the library xlt_name to do the work, and then just strcat()s 
 * the result to the command line.
 */

void zxcc_xltname(char *name, char *pcmd)
{
    char nbuf[CPM_MAXPATH + 1];

    xlt_name(pcmd, nbuf);

    strcat(name, nbuf);
}

/* main() parses the arguments to CP/M form. argv[1] is the name of the CP/M
  program to load; the remaining arguments are arguments for the CP/M program.

  main() also loads the vestigial CP/M BIOS and does some sanity checks 
         on the endianness of the host CPU and the sizes of data types.
 */

int main(int ac, char **av)
{
    int n;
    char *pCmd, *str;

    argc = ac;
    argv = av;
#ifdef __PACIFIC__ 		/* Pacific C doesn't support argv[0] */
    progname="ZXCC";
#endif
    progname = argv[0];
    
    /* DJGPP includes the whole path in the program name, which looks 
         * untidy...
         */
    while ((str = strpbrk(progname, DIRSEP)))
        progname = str + 1;


    if (sizeof(int) > 8 || sizeof(byte) != 1 || sizeof(word) != 2)
    {
        fprintf(stderr,"%s: type lengths incorrect; edit typedefs "
                               "and recompile.\n", progname);
        zxcc_exit(1); 
    }

    if (argc < 2)
    {
        fprintf(stderr,"%s: No CP/M program name provided.\n",progname);
        zxcc_exit(1);
    }

    /* Parse arguments. An argument can be either:

           * preceded by a '-', in which case it is copied in as-is, less the
             dash;
       * preceded by a '+', in which case it is parsed as a filename and
            then concatenated to the previous argument;
           * preceded by a '+-', in which case it is concatenated without 
            parsing;
           * not preceded by either, in which case it is parsed as a filename.

          So, the argument string "--a -b c +-=q --x +/dev/null" would be rendered
        into CP/M form as "-a b p:c=q -xd:null"  */

    if (!fcb_init())
    {
        fprintf(stderr, "Could not initialise CPMREDIR library\n");
        zxcc_exit(1);
    }

        /* allow environment variables to override default locations */
    /* two options are supported, explicit overrides for each directory
     * (BINDIR80, LIBDIR80, INCDIR80)
     * or a common directory prefix override (CPMDIR80)
     * the explict override takes precedence
     */
    char* tmpenv;
    if ((tmpenv = getenv("CPMDIR80"))) {
        mkpath(bindir80, tmpenv, BIN80);    /* use CPMDIR80 & std subdirs */
        mkpath(libdir80, tmpenv, LIB80);
        mkpath(incdir80, tmpenv, INC80);
    }
    if ((tmpenv = getenv("BINDIR80")))
        mkpath(bindir80, tmpenv, "");

    if ((tmpenv = getenv("LIBDIR80")))
        mkpath(libdir80, tmpenv, "");

    if ((tmpenv = getenv("INCDIR80")))
        mkpath(incdir80, tmpenv, "");

	xlt_map(0, bindir80);	/* Establish the 3 fixed mappings */
	xlt_map(1, libdir80);
	xlt_map(2, incdir80);
	
    pCmd = (char *)RAM + 0x81;

    for (n = 2; n < argc; n++)
    {
        if (argv[n][0] == '+' && argv[n][1] == '-')
        {
            /* Append, no parsing */
            strcat(pCmd, argv[n] + 2);
        }
        else if (!argv[n][0] || argv[n][0] == '-')
        {
            /* Append with space; no parsing. */
            strcat(pCmd, " ");
            strcat(pCmd, argv[n] + 1);
        }
        else if (argv[n][0] == '+')
        {
            zxcc_xltname(pCmd, argv[n]+1);
        }
        else /* Translate a filename */
        {
            strcat(pCmd, " ");
            zxcc_xltname(pCmd, argv[n]);
        }

    }
    pCmd[0x7F] = 0;	/* Truncate to fit the buffer */
    RAM[0x80] = (byte)strlen(pCmd);

    str = parse_to_fcb(pCmd, 0x5C);
    parse_to_fcb(str, 0x6C);	

/* This statement is very useful when creating a client like zxc or zxas

    printf("Command tail is %s\n", pCmd);
*/
    load_bios();

    memset(RAM + 0xFE9C, 0, 0x64);	/* Zap the SCB */
    RAM[0xFE98] = 0x06; 
    RAM[0xFE99] = 0xFE;		/* FE06, BDOS entry */
    RAM[0xFEA1] = 0x31;		/* BDOS 3.1 */
    RAM[0xFEA8] = 0x01;		/* UK date format */
    RAM[0xFEAF] = 0x0F;		/* CCP drive */

#ifdef USE_CPMIO
    RAM[0xFEB6] = cpm_term_direct(CPM_TERM_WIDTH,  -1);
    RAM[0xFEB8] = cpm_term_direct(CPM_TERM_HEIGHT, -1);
#else
    RAM[0xFEB6] = 79;
    RAM[0xFEB8] = 23;
#endif
    RAM[0xFED1] = 0x80;	/* Buffer area */
    RAM[0xFED2] = 0xFF;
    RAM[0xFED3] = '$';
    RAM[0xFED6] = 0x9C;
    RAM[0xFED7] = 0xFE;	/* SCB address */
    RAM[0xFED8] = 0x80;	/* DMA address */
    RAM[0xFED9] = 0x00;
    RAM[0xFEDA] = 0x0F;	/* P: */
    RAM[0xFEE6] = 0x01;	/* Multi sector count */
    RAM[0xFEFE] = 0x06;
    RAM[0xFEFF] = 0xFE;	/* BDOS */

    cpm_drive = 0x0F;	/* Start logged into P: */
    cpm_user  = 0;		/* and user 0 */

#ifdef USE_CPMIO
    cpm_scr_init(); deinit_term = 1;
#endif
#ifdef USE_CPMGSX
    gsx_init();	deinit_gsx = 1;
#endif

    /* Start the Z80 at 0xFF00, with stack at 0xFE00 */
    mainloop(0xFF00, 0xFE00);

    return zxcc_term();
}

void zxcc_exit(int code)
{
#ifdef USE_CPMIO
    if (deinit_term) cpm_scr_unit();
#endif
#ifdef USE_CPMGSX
    if (deinit_gsx) gsx_deinit();
#endif
    exit(code);
}

int zxcc_term(void)
{
    word n;


    n = RAM[0x81];		  /* Get the return code. This is Hi-Tech C */
    n = (n << 8) | RAM[0x80]; /* specific and fails with other COM files */

    putchar('\n');

    if (cpm_error != 0)	/* The CP/M "set return code" call was used */
    {			/* (my modified Hi-Tech C library uses this */
        n = cpm_error;	/*  call) */
    }	
    if (n < 256 || n == 0xFFFF)
    {
        Msg("Return code %d\n", n);
        return n;
    }
    else	return 0;
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
