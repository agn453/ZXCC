/*
    CPMREDIR: CP/M filesystem redirector
    Copyright (C) 1998, John Elliott <jce@seasip.demon.co.uk>

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

    This is a sample piece of redirection code, which is designed to work
    with CPMREDIR.RSX (see CPMREDIR.ZSM)
*/

#include "cpmredir.h"

#include "Z80.h"	/* or whatever #includes you need for your emulator */

extern cpm_byte *RAM;	/* or however your emulator emulates RAM. If the TPA */
			/* is not contiguous (eg: some bank-switching schemes) */
			/* then you will have to copy in and copy out */


/* CCP_DRIVE is where the CCP keeps the default drive. This is only used in 
 * the "display directory name" call below, and can be ignored if you don't 
 * know where the SCB is going to end up on your system.
 *
 * On the other hand, it is necessary to know where the BDOS keeps its 
 * current drive (SCB_DRIVE), because the CP/M 3 CCP accesses this variable
 * directly.
 */

#define CCP_DRIVE (RAM[0xFBAF])
#define SCB_DMA   0xFBD8
#define SCB_DRIVE (RAM[0xFBDA])	

static cpm_word peekw(cpm_word a)	/* Read a word from RAM */
{
	return RAM[a] + 256*RAM[a+1];
}


/* Return 1 to continue with BDOS call, 0 to return */

static int redirector(Z80 *R)
{
	/* DMA address */
	static cpm_byte *pdma = NULL;
	static cpm_word adma  = 0;

	/* Parameters as various routines want them... */
	cpm_byte e    = R->DE.B.l;	/*  E as byte */
	cpm_word de   = R->DE.W;	/* DE as word */
	cpm_byte *pde = RAM + de;	/* DE as pointer */
	cpm_byte *rsx = RAM + R->HL.W;	/* Address of RSX */
	cpm_byte *drv = rsx + 0x1B;	/* 16-byte table of redirected drives */
	cpm_byte *ptr1;			/* Pointer for misc. use */
	int retc = 1;			/* Return value */
	static int idrv = 0;		/* Current "default" drive */
	cpm_byte drive;			/* Current FCB drive */
	char dname[41];			/* Directory name */
	static int lastdrv;		/* Drive in last call 0x11 */

	if (!pdma) pdma = RAM + 0x80;

	if (adma != SCB_DMA)
	{
		adma = SCB_DMA;
		pdma = RAM + adma;
	}

	/* Has the "Current drive" setting been changed manually? */
	if (idrv != SCB_DRIVE)
	{
		fcb_drive(SCB_DRIVE);
		idrv = SCB_DRIVE;
	}

	/* Convert FCB "drive" byte to real drivenumber */
	drive = *pde & 0x7F;
	if (!drive || drive == '?') drive = idrv; else --drive;

	/* Various functions... */
	switch(R->BC.B.l)
	{
/* The first case in this switch deals with calls made to the "Communicate 
  with RSX" function. This program supports three: "initialise", "mount"
  and "umount". 

  In this function, subfunction numbers can be arbitrarily chosen by 
  the RSX authors. A common convention is to make the first parameter
  point at a copy of the RSX name; this has been followed in the 
  calls 0x77 - 0x79 below.
*/

		case 0x3C:	/* RSX calls... */

		/* Initialise - subfunction 0x79, 1 parameter */

		if (pde[0] == 0x79 && pde[1] == 1)	
		{
			/* RSXPB parameter 1 should be the RSX name */
			ptr1 = RAM + peekw(de + 2);
			if (memcmp(ptr1, rsx + 0x10, 8)) return 1;
			fcb_init();
			retc = 0;
		}

		/* Mount host directory as CP/M drive -
                   subfunction 0x78, 3 parameters */

		if (pde[0] == 0x78 && pde[1] == 3)
		{
			ptr1 = RAM + peekw(de + 2);
			if (memcmp(ptr1, rsx + 0x10, 8)) return 1;
			ptr1 = RAM + peekw(de + 6);
			R->HL.W = xlt_map(peekw(de + 4), (char *)ptr1);
			if (R->HL.W == 1) drv[peekw(de + 4)] = 1;
			retc = 0;	
		}

		/* Unmount host directory as CP/M drive -
                   subfunction 0x77, 2 parameters */

		if (pde[0] == 0x77 && pde[1] == 2)
		{
			ptr1 = RAM + peekw(de + 2);
			if (memcmp(ptr1, rsx + 0x10, 8)) return 1;
			R->HL.W = xlt_umap(peekw(de + 4));
			if (R->HL.W == 1) drv[peekw(de + 4)] = 0;
			retc = 0;	
		}
		/* This subfunction (0x41) is not necessary to the functioning
                  of CPMREDIR, but is quite useful. Enhanced CCPs such as 
                  ZCCP call it while printing the prompt (after "A" and 
                  before ">") and its purpose is to display the name of the 
                  current directory. Therefore we can use it to display the
                  directory a drive is using. */

		if (pde[0] == 0x41)	/* Get dir name */
		{
			/* This function has no parameter checking */

			if (drv[CCP_DRIVE])	/* Is current drive 
                                                   redirected? */
			{
				/* If so, output its name 
                                   (fitting it in 40 characters) */

				R->BC.B.l = 9;	/* Change BDOS function to 
                                                   "print string" */
				R->DE.W   = R->HL.W + 0x13b; 
                                /* DE = Buffer in RSX for the name */
                                                            
				sprintf(dname, "=%-.38s", 
                                                  xlt_getcwd(CCP_DRIVE));
				/* Append a CP/M end-of-line */
				strcat(dname, "$");

				/* Copy into the RSX */
				memcpy(RAM + R->DE.W, dname, 40);
				retc = 1;
			}
		}
		break;

/* The other cases in this switch are, on the whole, straight passthroughs
  (passes through?) to CPMREDIR */
		
		case 0x0D:		/* Reset discs */
		R->HL.W = fcb_reset();
		idrv    = 0;
		break;

		case 0x0E:		/* Select drive */
		if (drv[e]) 
		{ 
			R->HL.W = fcb_drive(e); 
			if (!R->HL.W) SCB_DRIVE = e; /* Write it back into */
			retc = 0; 		     /* the SCB */
		}
		idrv = e;
		break; 

		case 0x0F:		/* Open file */
		if (drv[drive]) { R->HL.W = fcb_open(pde, pdma); retc = 0; }
		break;

		case 0x10:		/* Close file */
		if (drv[drive]) { R->HL.W = fcb_close(pde); retc = 0; }
		break;
	
		case 0x11:		/* Search 1st */
		lastdrv = drv[drive];
		if (drv[drive]) { R->HL.W = fcb_find1(pde, pdma); retc = 0; }
		break;

		case 0x12:		/* Search next */
		if (lastdrv) { R->HL.W = fcb_find2(pde, pdma); retc = 0; }
		break;

		case 0x13:		/* Erase file */
		if (drv[drive]) { R->HL.W = fcb_unlink(pde, pdma); retc = 0; }
		break;

		case 0x14:		/* Read sequential */
		if (drv[drive]) { R->HL.W = fcb_read(pde, pdma); retc = 0; }
		break;
		
		case 0x15:		/* Write sequential */
		if (drv[drive]) { R->HL.W = fcb_write(pde, pdma); retc = 0; }
		break;

		case 0x16:		/* Create file */
		if (drv[drive]) { R->HL.W = fcb_creat(pde, pdma); retc = 0; }
		break;

		case 0x17:		/* Rename file */
		if (drv[drive]) { R->HL.W = fcb_rename(pde, pdma); retc = 0; }
		break;

		case 0x1A:		/* Set DMA */
		adma = de;
		pdma = pde;
		break;	

		case 0x1B:		/* Get allocation vector */
		if (drv[idrv])
		{
			R->HL.W   +=  0x13b; 
			fcb_getalv(RAM + R->HL.W, 40);
			retc = 0;
		}
		break;

		case 0x1C:		/* Make disc R/O */
		if (drv[idrv]) { fcb_rodisk(); retc = 0; }
		break;

		case 0x1E:		/* Set attributes */
                if (drv[drive]) { R->HL.W = fcb_chmod(pde, pdma); retc = 0; }
		break;

		case 0x1F:		/* Get DPB */
		if (drv[idrv]) 
		{
			fcb_getdpb(rsx + 0x2B + (0x11 * idrv));
			R->HL.W += 0x2B + (0x11 * idrv);
			retc = 0;
		} 
		break;

		case 0x20:		/* Set/get user */
		fcb_user(e);
		break;

                case 0x21:		/* Read random */
                if (drv[drive]) { R->HL.W = fcb_randrd(pde, pdma); retc = 0; }
                break;

                case 0x22:		/* Write random */
                if (drv[drive]) { R->HL.W = fcb_randwr(pde, pdma); retc = 0; }
                break;

		case 0x23:		/* Get file size */
		if (drv[drive]) { R->HL.W = fcb_stat(pde); retc = 0; }
		break;

                case 0x24:		/* Get file position */
                if (drv[drive]) { R->HL.W = fcb_tell(pde); retc = 0; }
                break;

		case 0x25:		/* Reset R/O drives */
		fcb_resro(de);
		break;

		case 0x28:		/* Write random with 0 fill */
                if (drv[drive]) { R->HL.W = fcb_randwz(pde, pdma); retc = 0; }
                break;
		
		case 0x2C:		/* Set multisector count */
		fcb_multirec(e);
		break;

		case 0x2E:		/* Get free space */
		if (drv[e]) { fcb_dfree(e, pdma); retc = 0; }
		break;

		case 0x30:		/* Empty buffers */
		fcb_sync(e);
		break;

		case 0x62:		/* Free temporary blocks */
		fcb_purge();
		break;	

		case 0x63:		/* Truncate file */
                if (drv[drive]) { R->HL.W = fcb_trunc(pde, pdma); retc = 0; }
		break;

		case 0x64:		/* Set label */
		if (drv[drive]) { R->HL.W = fcb_setlbl(pde, pdma); retc = 0; }
		break;

		case 0x65:		/* Get label byte */
		if (drv[e]) { R->HL.W = fcb_getlbl(e); retc = 0; }
		break;

                case 0x66:		/* Get date */
                if (drv[drive]) { R->HL.W = fcb_date(pde); retc = 0; }
                break;

		case 0x67:		/* Set password */
		if (drv[drive]) { R->HL.W = fcb_setpwd(pde, pdma); retc = 0; }
		break;

		case 0x6A:		/* Set default password */
		fcb_defpwd(pde);
		break;

		case 0x74:		/* Set stamps */
                if (drv[drive]) { R->HL.W = fcb_sdate(pde, pdma); retc = 0; }
		break;
	}
	R->AF.B.h = R->HL.B.l;
	R->BC.B.h = R->HL.B.h;
	return retc;

}


/* Actual entry from the emulator */

void cpmredir_intercept(Z80 *R)
{
	if (redirector(R)) R->AF.B.l |= 0x01;	/* Carry set, call main BDOS */
	else               R->AF.B.l &= 0xFE;	/* Carry clear, return */
}
