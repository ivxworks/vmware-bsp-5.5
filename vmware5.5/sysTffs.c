/* sysTffs.c - PC Pentium/Pentium2/Pentium3 system-dependent TrueFFS library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/* FAT-FTL Lite Software Development Kit
 * Copyright (C) M-Systems Ltd. 1995-1997	*/

/*
modification history
--------------------
01t,30apr01,yp  removing assumption that Disk On Chip support will always be
                 present
01s,14dec00,mks  removed references to pc386 and pc486. SPR 63032.
01r,29nov00,yp  cleaning up so we can build as BSP stub
01q,31may99,yp  Added comments suggested in SPR #25319
01p,21apr98,yp   added tffs to files included from there
01o,26mar98,hdn  added DOC2_XXX macros along with windowBaseAddress().
01n,11mar98,yp   made including tffsConfig.c conditional so man page
		 generation does not include it.
01m,09mar98,kbw  made man page edits to fix problems found by QE
01l,04mar98,kbw  made man page edits
01k,13feb98,hdn  commented out pcVccOff() to fix multi drive testing.
01j,19jan98,hdn  fixed pcVccXX, pcVppXX. added timeout in while loop.
01i,18dec97,hdn  added comment.  cleaned up.
01h,11dec97,hdn  added our PCMCIA library support.
01g,05dec97,hdn  added flDelayMsecs(), socketTable[]. cleanup.
01f,25nov97,hdn  made each interface functions configurable.
01e,24nov97,hdn  made each socket interface configurable.
01d,11nov97,hdn  made flDelayLoop() dummy function.
01c,11nov97,hdn  fixed comments, copyright.
01b,05nov97,hdn  cleaned up.
01a,09oct97,and  written by Andray in M-Systems
*/

/*
DESCRIPTION
This library provides board-specific hardware access routines for TrueFFS.  
In effect, these routines comprise the socket component driver (or drivers)
for your flash device hardware.  At socket registration time, TrueFFS stores 
pointers to the functions of this socket component driver in an 'FLSocket' 
structure.  When TrueFFS needs to access the flash device, it uses these 
functions.  

Because this file is, for the most part, a device driver that exports its 
functionality by registering function pointers with TrueFFS, very few of the 
functions defined here are externally callable.  For the record, these 
external functions are flFitInSocketWindow() and flDelayLoop().  You should 
never have any need to call these functions.  

However, one of the most import functions defined in this file is neither
referenced in an 'FLSocket' structure, nor is it externally callable.  This
function is sysTffsInit().  TrueFFS calls this function at initialization 
time to register socket component drivers for all the flash devices attached 
to your target.  It is this call to sysTffs() that results in assigning 
drive numbers to the flash devices on your target hardware.  Drive numbers 
are assigned by the order in which the socket component drivers are registered.
The first to be registered is drive 0, the second is drive 1, and so on up to 
4.  As shipped, TrueFFS supports up to five flash drives.  

After registering socket component drivers for a flash device, you may 
format the flash medium even though there is not yet a block device driver
associated with the flash (see the reference entry for the tffsDevCreate() 
routine).  To format the flash medium for use with TrueFFS, 
call tffsDevFormat() or, for some BSPs, sysTffsFormat().  

The sysTffsFormat() routine is an optional but BSP-specific externally 
callable helper function.  Internally, it calls tffsDevFormat() with a 
pointer to a 'FormatParams' structure initialized to values that leave a 
space on the flash device for a boot image. This space is outside the 
region managed by TrueFFS.  This special region is necessary for boot 
images because the normal translation and wear-leveling services of TrueFFS 
are incompatible with the needs of the boot program and the boot image it 
relies upon.  To write a boot image (or any other data) into this area, 
use tffsBootImagePut().  

INCLUDE FILES: flsocket.h

SEE ALSO : tffsDevFormat tffsRawio
*/


/* includes */

#include "vxWorks.h"
#include "config.h"
#include "tffs/flsocket.h"
#include "tffs/pcic.h"


/* defines */
#define INCLUDE_MTD_I28F016
#define INCLUDE_MTD_I28F008
#define INCLUDE_MTD_AMD
#undef  INCLUDE_MTD_CFISCS
#undef  INCLUDE_MTD_WAMD
#define INCLUDE_TL_FTL
#undef  INCLUDE_TL_SSFDC       

#undef	INCLUDE_SOCKET_DOC		/* DOC socket interface */
#define	INCLUDE_SOCKET_PCIC0		/* PCIC socket interface 0 */
#define	INCLUDE_SOCKET_PCIC1		/* PCIC socket interface 1 */
#define INCLUDE_TFFS_BOOT_IMAGE		/* include tffsBootImagePut() */
#define	WINDOW_ID	0		/* PCIC window used (0-4) */
#define	VPP_DELAY_MSEC	100		/* Millisecs to wait for Vpp ramp up */
#define	DOC2_SCAN_ADRS_0 0xc8000	/* start of mem range to scan for DOC2 */
#define	DOC2_SCAN_ADRS_1 0xf0000	/* end of mem range to scan for DOC2 */
#define	PC_BASE_ADRS_0	0xd8000		/* base addr for socket 0 */
#define	PC_BASE_ADRS_1	0xda000		/* base addr for socket 1 */
#define KILL_TIME_FUNC	 ((iz * iz) / (iz + iz)) / ((iy + iz) / (iy * iz))
#define PC_WINDOW	1		/* PCIC window no. used by TFFS */
#define PC_EXTRAWS	1		/* PCIC wait state used by TFFS */
#define PC_SOCKET_NAME_DOC "DOC"	/* DOC socket name for DOC */
#define PC_SOCKET_NAME_0 "PCMCIA-0"	/* PCIC socket name for socket 0 */
#define PC_SOCKET_NAME_1 "PCMCIA-1"	/* PCIC socket name for socket 1 */


/* externs */

#ifdef INCLUDE_SOCKET_DOC
IMPORT unsigned windowBaseAddress (unsigned driveNo, unsigned long startAddr,
                                   unsigned long endAddr);   /* nfdc2148.c */
#endif /* INCLUDE_SOCKET_DOC */
IMPORT PCMCIA_CTRL pcmciaCtrl;


/* globals */

char pcDriveNo[2] = {NONE, NONE};	/* drive number of the sockets */


/* locals */

LOCAL UINT32 sysTffsMsecLoopCount = 0;	/* loop count to consume milli sec */
#ifndef	INCLUDE_PCMCIA
LOCAL FLMutex flPcicMutex = NULL;	/* protects PCIC register access  */
                                        /* in multi-threaded environments */
#endif	/* INCLUDE_PCMCIA */


/* forward declarations */

#ifdef	INCLUDE_SOCKET_DOC
LOCAL FLStatus		docRegister (void);
LOCAL unsigned		docWindowBaseAddress (unsigned driveNo);
LOCAL FLBoolean		docCardDetected (FLSocket vol);
LOCAL void		docVccOn (FLSocket vol);
LOCAL void		docVccOff (FLSocket vol);
#ifdef	SOCKET_12_VOLTS
LOCAL FLStatus		docVppOn (FLSocket vol);
LOCAL void		docVppOff (FLSocket vol);
#endif	/* SOCKET_12_VOLTS */
LOCAL FLStatus		docInitSocket (FLSocket vol);
LOCAL void		docSetWindow (FLSocket vol);
LOCAL void		docSetMappingContext (FLSocket vol, unsigned page);
LOCAL FLBoolean		docGetAndClearCardChangeIndicator (FLSocket vol);
LOCAL FLBoolean		docWriteProtected (FLSocket vol);
#ifdef	EXIT
LOCAL void		docFreeSocket (FLSocket vol);
#endif	/* EXIT */
#endif	/* INCLUDE_SOCKET_DOC */

#if	defined (INCLUDE_SOCKET_PCIC0) || defined (INCLUDE_SOCKET_PCIC1)
LOCAL FLStatus		pcRegister (int socketNo, unsigned int baseAddress);
#ifndef	INCLUDE_PCMCIA
LOCAL unsigned char	flInportb (unsigned portId);
LOCAL void		flOutportb (unsigned portId, unsigned char value);
LOCAL unsigned char	get365 (FLSocket vol, unsigned char reg);
LOCAL void		set365 (FLSocket vol, unsigned char reg, 
				unsigned char value);
#endif	/* INCLUDE_PCMCIA */
LOCAL FLBoolean		pcCardDetected (FLSocket vol);
LOCAL void		pcVccOn (FLSocket vol);
LOCAL void		pcVccOff (FLSocket vol);
#ifdef	SOCKET_12_VOLTS
LOCAL FLStatus		pcVppOn (FLSocket vol);
LOCAL void		pcVppOff (FLSocket vol);
#endif	/* SOCKET_12_VOLTS */
LOCAL FLStatus		pcInitSocket (FLSocket vol);
LOCAL void		pcSetWindow (FLSocket vol);
LOCAL void		pcSetMappingContext (FLSocket vol, unsigned page);
LOCAL FLBoolean		pcGetAndClearCardChangeIndicator (FLSocket vol);
LOCAL FLBoolean		pcWriteProtected (FLSocket vol);
#ifdef	EXIT
LOCAL void		pcFreeSocket (FLSocket vol);
#endif	/* EXIT */
#endif	/* defined (INCLUDE_SOCKET_PCIC0) || defined (INCLUDE_SOCKET_PCIC1) */

#ifndef DOC
#include "tffs/tffsConfig.c"
#endif /* DOC */


/*******************************************************************************
*
* sysTffsInit - board level initialization for TFFS
*
* This routine calls the socket registration routines for the socket component
* drivers that will be used with this BSP. The order of registration signifies
* the logical drive number given to the drive associated with the socket.
*
* RETURNS: N/A
*/

LOCAL void sysTffsInit (void)
    {
    UINT32 ix = 0;
    UINT32 iy = 1;
    UINT32 iz = 2;
    int oldTick;

    /* we assume followings:
     *   - no interrupts except timer is happening.
     *   - the loop count that consumes 1 msec is in 32 bit.
     * it should be done in the early stage of usrRoot() in tffsDrv().  */

    oldTick = tickGet();
    while (oldTick == tickGet())	/* wait for next clock interrupt */
	;

    oldTick = tickGet();
    while (oldTick == tickGet())	/* loop one clock tick */
	{
	iy = KILL_TIME_FUNC;		/* consume time */
	ix++;				/* increment the counter */
	}
    
    sysTffsMsecLoopCount = ix * sysClkRateGet() / 1000;

#ifdef	INCLUDE_SOCKET_DOC
    (void) docRegister ();			/* Disk On Chip */
#endif	/* INCLUDE_SOCKET_DOC */

#ifdef	INCLUDE_SOCKET_PCIC0
    (void) pcRegister (0, PC_BASE_ADRS_0);	/* flash card on socket 0 */
#endif	/* INCLUDE_SOCKET_PCIC0 */

#ifdef	INCLUDE_SOCKET_PCIC1
    (void) pcRegister (1, PC_BASE_ADRS_1);	/* flash card on socket 1 */
#endif	/* INCLUDE_SOCKET_PCIC1 */
    }

#ifdef	INCLUDE_SOCKET_DOC
/*******************************************************************************
*
* docRegister - registration routine for M-Systems Disk On Chip (DOC) 
*		socket component driver
*
* This routine populates the 'vol' structure for a logical drive with the
* socket component driver routines for the M-System DOC. All socket routines
* are referanced through the 'vol' structure and never from here directly
*
* RETURNS: flOK, or flTooManyComponents if there're too many drives,
*                or flAdapterNotFound if there's no controller.
*/

LOCAL FLStatus docRegister (void)
    {
    FLSocket vol;

    if (noOfDrives >= DRIVES)
        return (flTooManyComponents);

    pVol = flSocketOf (noOfDrives);

    vol.window.baseAddress =	docWindowBaseAddress (vol.volNo);
    if (vol.window.baseAddress == 0)
        return (flAdapterNotFound);

    vol.cardDetected =		docCardDetected;
    vol.VccOn =			docVccOn;
    vol.VccOff =		docVccOff;
#ifdef SOCKET_12_VOLTS
    vol.VppOn =			docVppOn;
    vol.VppOff =		docVppOff;
#endif
    vol.initSocket =		docInitSocket;
    vol.setWindow =		docSetWindow;
    vol.setMappingContext =	docSetMappingContext;
    vol.getAndClearCardChangeIndicator = docGetAndClearCardChangeIndicator;
    vol.writeProtected =	docWriteProtected;
#ifdef EXIT
    vol.freeSocket =		docFreeSocket;
#endif

    tffsSocket[noOfDrives] = PC_SOCKET_NAME_DOC;
    noOfDrives++;

    return (flOK);
    }
 
/*******************************************************************************
*
* docWindowBaseAddress - Return the host base address of the DOC2 window
*
* This routine Return the host base address of the window.
* It scans the host address range from DOC2_SCAN_ADRS_0 to DOC2_SCAN_ADRS_1
* (inclusive) attempting to identify DiskOnChip 2000 memory window.
*
* RETURNS: Host physical address of window divided by 4 KB
*/

LOCAL unsigned docWindowBaseAddress
    (
    unsigned driveNo		/* drive number */
    )
    {

    return (windowBaseAddress (driveNo, DOC2_SCAN_ADRS_0, DOC2_SCAN_ADRS_1));
    }

/*******************************************************************************
*
* docCardDetected - detect if a card is present (inserted)
*
* This routine detects if a card is present (inserted).
*
* RETURNS: TRUE, or FALSE if the card is not present.
*/

LOCAL FLBoolean docCardDetected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (TRUE);
    }

/*******************************************************************************
*
* docVccOn - turn on Vcc (3.3/5 Volts)
*
* This routine turns on Vcc (3.3/5 Volts).  Vcc must be known to be good
* on exit.
*
* RETURNS: N/A
*/

LOCAL void docVccOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

/*******************************************************************************
*
* docVccOff - turn off Vcc (3.3/5 Volts)
*
* This routine turns off Vcc (3.3/5 Volts). 
*
* RETURNS: N/A
*/

LOCAL void docVccOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

#ifdef SOCKET_12_VOLTS

/*******************************************************************************
*
* docVppOn - turns on Vpp (12 Volts)
*
* This routine turns on Vpp (12 Volts). Vpp must be known to be good on exit.
*
* RETURNS: flOK always
*/

LOCAL FLStatus docVppOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (flOK);
    }

/*******************************************************************************
*
* docVppOff - turns off Vpp (12 Volts)
*
* This routine turns off Vpp (12 Volts).
*
* RETURNS: N/A
*/

LOCAL void docVppOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

#endif	/* SOCKET_12_VOLTS */

/*******************************************************************************
*
* docInitSocket - perform all necessary initializations of the socket
*
* This routine performs all necessary initializations of the socket.
*
* RETURNS: flOK always
*/

LOCAL FLStatus docInitSocket
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (flOK);
    }

/*******************************************************************************
*
* docSetWindow - set current window attributes, Base address, size, etc
*
* This routine sets current window hardware attributes: Base address, size,
* speed and bus width.  The requested settings are given in the 'vol.window' 
* structure.  If it is not possible to set the window size requested in
* 'vol.window.size', the window size should be set to a larger value, 
* if possible. In any case, 'vol.window.size' should contain the 
* actual window size (in 4 KB units) on exit.
*
* RETURNS: N/A
*/

LOCAL void docSetWindow
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

/*******************************************************************************
*
* docSetMappingContext - sets the window mapping register to a card address
*
* This routine sets the window mapping register to a card address.
* The window should be set to the value of 'vol.window.currentPage',
* which is the card address divided by 4 KB. An address over 128MB,
* (page over 32K) specifies an attribute-space address. On entry to this 
* routine vol.window.currentPage is the page already mapped into the window.
* (In otherwords the page that was mapped by the last call to this routine.)
*
* The page to map is guaranteed to be on a full window-size boundary.
*
* RETURNS: N/A
*/

LOCAL void docSetMappingContext
    (
    FLSocket vol,		/* pointer identifying drive */
    unsigned page		/* page to be mapped */
    )
    {
    }

/*******************************************************************************
*
* docGetAndClearCardChangeIndicator - return the hardware card-change indicator
*
* This routine returns the hardware card-change indicator and clears it if set.
*
* RETURNS: FALSE, or TRUE if the card has been changed
*/

LOCAL FLBoolean docGetAndClearCardChangeIndicator
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (FALSE);
    }

/*******************************************************************************
*
* docWriteProtected - return the write-protect state of the media
*
* This routine returns the write-protect state of the media
*
* RETURNS: FALSE, or TRUE if the card is write-protected
*/

LOCAL FLBoolean docWriteProtected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (FALSE);
    }

#ifdef EXIT

/*******************************************************************************
*
* docFreeSocket - free resources that were allocated for this socket.
*
* This routine free resources that were allocated for this socket.
* This function is called when FLite exits.
*
* RETURNS: N/A
*/

LOCAL void docFreeSocket
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

#endif  /* EXIT */
#endif	/* INCLUDE_SOCKET_DOC */

#if	defined (INCLUDE_SOCKET_PCIC0) || defined (INCLUDE_SOCKET_PCIC1)
/*******************************************************************************
*
* pcRegister - install routines for the PCIC socket controller.
*
* This routine installs necessary functions for the PCIC socket controller.
* This routine also determines the window base addresses for the
* sockets registered. these values are received as parameters. if 0 is
* received, default values are selected (D8000h and DA000h).
*
* RETURNS: flOK, or flTooManyComponents if there're too many drivers,
*		 or flAdapterNotFound if there's no controller.
*/

LOCAL FLStatus pcRegister 
    (
    int socketNo,		/* socket number */
    unsigned int baseAddress	/* base addr of socket, 4KB aligned */
    )
    {
    FLSocket vol = flSocketOf (noOfDrives);

    if (noOfDrives >= DRIVES)
        return (flTooManyComponents);

    vol.serialNo = socketNo;
    if (socketNo == 0)
      {
      vol.window.baseAddress = baseAddress ? 
			       baseAddress >> 12 : PC_BASE_ADRS_0 >> 12 ;
      tffsSocket[noOfDrives] = PC_SOCKET_NAME_0;
      }
    else if (socketNo == 1)
      {
      vol.window.baseAddress = baseAddress ? 
			       baseAddress >> 12 : PC_BASE_ADRS_1 >> 12 ;
      tffsSocket[noOfDrives] = PC_SOCKET_NAME_1;
      }
    else
      return (flAdapterNotFound);

    pcDriveNo[socketNo] = noOfDrives;	/* drive no. for the socket */

    vol.cardDetected =		pcCardDetected;
    vol.VccOn =			pcVccOn;
    vol.VccOff =		pcVccOff;
#ifdef SOCKET_12_VOLTS
    vol.VppOn =			pcVppOn;
    vol.VppOff =		pcVppOff;
#endif
    vol.initSocket =		pcInitSocket;
    vol.setWindow =		pcSetWindow;
    vol.setMappingContext =	pcSetMappingContext;
    vol.getAndClearCardChangeIndicator = pcGetAndClearCardChangeIndicator;
    vol.writeProtected =	pcWriteProtected;
#ifdef EXIT
    vol.freeSocket =		pcFreeSocket;
#endif
    noOfDrives++;

    return (flOK);
    }

#ifdef	INCLUDE_PCMCIA

/*******************************************************************************
*
* pcCardDetected - detect if a card is present (inserted)
*
* This routine detects if a card is present (inserted).
*
* RETURNS: TRUE, or FALSE if the flash card is not present.
*/

LOCAL FLBoolean pcCardDetected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];

    if ((pCard->type == PCCARD_FLASH) && (pCard->detected))
	return (TRUE);
    else
	return (FALSE);
    }

/*******************************************************************************
*
* pcVccOn - turn on Vcc (3.3/5 Volts)
*
* This routine turns on Vcc (3.3/5 Volts). Vcc must be known to be good 
* on exit.
*
* RETURNS: N/A
*/

LOCAL void pcVccOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];
    int flag		= (*pChip->flagGet) (vol.serialNo);
    int status		= (PC_READY | PC_POWERON);
    UINT32 timeout	= flMsecCounter + 2000;

    if ((pCard->type != PCCARD_FLASH) || (!pCard->detected))
	return;

    flag = (flag & ~PC_VCC_MASK) | (PC_PWR_AUTO | PC_VCC_5V);
    (*pChip->flagSet) (vol.serialNo, flag);
    while ((((*pChip->status) (vol.serialNo) & status) != status) &&
	   (flMsecCounter < timeout))
	;
    }

/*******************************************************************************
*
* pcVccOff - turn off Vcc (3.3/5 Volts)
*
* This routine turns off Vcc (3.3/5 Volts).
*
* RETURNS: N/A
*/

LOCAL void pcVccOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
#if	FALSE
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];
    int flag		= (*pChip->flagGet) (vol.serialNo);

    if ((pCard->type != PCCARD_FLASH) || (!pCard->detected))
	return;

    flag = (flag & ~PC_VCC_MASK) | PC_PWR_AUTO;
    (*pChip->flagSet) (vol.serialNo, flag);
#endif
    }

#ifdef SOCKET_12_VOLTS

/*******************************************************************************
*
* pcVppOn - turn on Vpp (12 Volts)
*
* This routine turns on Vpp (12 Volts). Vpp must be known to be good on exit.
*
* RETURNS: flOK, or flDriveNotAvailable if it failed
*/

LOCAL FLStatus pcVppOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];
    int flag		= (*pChip->flagGet) (vol.serialNo);

    if ((pCard->type != PCCARD_FLASH) || (!pCard->detected))
	return (flDriveNotAvailable);

    flag = (flag & ~PC_VPP_MASK) | (PC_PWR_AUTO | PC_VPP_12V);
    (*pChip->flagSet) (vol.serialNo, flag);
    flDelayMsecs (VPP_DELAY_MSEC);	/* wait for Vpp to ramp up */

    return (flOK);
    }

/*******************************************************************************
*
* pcVppOff - turn off Vpp (12 Volts)
*
* This routine turns off Vpp (12 Volts).
*
* RETURNS: N/A
*/

LOCAL void pcVppOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];
    int flag		= (*pChip->flagGet) (vol.serialNo);

    if ((pCard->type != PCCARD_FLASH) || (!pCard->detected))
	return;

    flag = (flag & ~PC_VPP_MASK) | (PC_PWR_AUTO | PC_VPP_5V);
    (*pChip->flagSet) (vol.serialNo, flag);
    }

#endif	/* SOCKET_12_VOLTS */

/*******************************************************************************
*
* pcInitSocket - perform all necessary initializations of the socket
*
* This routine performs all necessary initializations of the socket.
*
* RETURNS: flOK, or ERROR if it failed
*/

LOCAL FLStatus pcInitSocket
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    int flag		= (PC_PWR_AUTO | PC_VCC_5V);

    if (pcDriveNo[vol.serialNo] != NONE)
        (*pChip->flagSet) (vol.serialNo, flag);

    return (flOK);
    }

/*******************************************************************************
*
* pcSetWindow - set current window attributes, Base address, size, etc
*
* This routine sets current window hardware attributes: Base address, size,
* speed and bus width.  The requested settings are given in the 'vol.window' 
* structure.  If it is not possible to set the window size requested in
* 'vol.window.size', the window size should be set to a larger value, 
* if possible. In any case, 'vol.window.size' should contain the 
* actual window size (in 4 KB units) on exit.
*
* RETURNS: N/A
*/

LOCAL void pcSetWindow
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];
    PCMCIA_MEMWIN memwin;

    if ((pCard->type != PCCARD_FLASH) || (!pCard->detected))
	return;

    memwin.window = PC_WINDOW;
    if (vol.window.busWidth == 16)
	memwin.flags = MAP_ACTIVE | MAP_16BIT;
    else
	memwin.flags = MAP_ACTIVE;
    memwin.extraws	= PC_EXTRAWS;
    memwin.start	= vol.window.baseAddress << 12;
    memwin.stop		= (vol.window.baseAddress << 12) + vol.window.size - 1;
    memwin.cardstart	= 0;	/* it is set in pcSetMappingContext() */
    (*pChip->memwinSet) (vol.serialNo, &memwin);
    }

/*******************************************************************************
*
* pcSetMappingContext - sets the window mapping register to a card address
*
* This routine sets the window mapping register to a card address.
* The window should be set to the value of 'vol.window.currentPage',
* which is the card address divided by 4 KB. An address over 128MB,
* (page over 32K) specifies an attribute-space address.
*
* The page to map is guaranteed to be on a full window-size boundary.
*
* RETURNS: N/A
*/

LOCAL void pcSetMappingContext
    (
    FLSocket vol,		/* pointer identifying drive */
    unsigned page		/* page to be mapped */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];
    PCMCIA_MEMWIN memwin;
    unsigned int mapRegValue = page & 0x3fff;

    if ((pCard->type != PCCARD_FLASH) || (!pCard->detected))
	return;

    memwin.window = PC_WINDOW;
    if (vol.window.busWidth == 16)
	memwin.flags = MAP_ACTIVE | MAP_16BIT;
    else
	memwin.flags = MAP_ACTIVE;
    if (page & ATTRIBUTE_SPACE_MAPPED)
	memwin.flags |= MAP_ATTRIB;
    memwin.extraws	= PC_EXTRAWS;
    memwin.start	= vol.window.baseAddress << 12;
    memwin.stop		= (vol.window.baseAddress << 12) + vol.window.size - 1;
    memwin.cardstart	= mapRegValue << 12;
    (*pChip->memwinSet) (vol.serialNo, &memwin);
    }

/*******************************************************************************
*
* pcGetAndClearCardChangeIndicator - return the hardware card-change indicator
*
* This routine returns the hardware card-change indicator and clears it if set.
*
* RETURNS: FALSE, or TRUE if the card has been changed
*/

LOCAL FLBoolean pcGetAndClearCardChangeIndicator
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CARD * pCard	= &pCtrl->card[vol.serialNo];

    if (pCard->cardStatus & PC_DETECT)
        return (TRUE);
    else 
        return (FALSE);
    }

/*******************************************************************************
*
* pcWriteProtected - return the write-protect state of the media
*
* This routine returns the write-protect state of the media
*
* RETURNS: FALSE, or TRUE if the card is write-protected
*/

LOCAL FLBoolean pcWriteProtected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    PCMCIA_CTRL * pCtrl	= &pcmciaCtrl;
    PCMCIA_CHIP * pChip	= &pCtrl->chip;
    int status         	= (*pChip->status) (vol.serialNo);

    if (status & PC_WRPROT)
	return (TRUE);
    else
	return (FALSE);
    }

#else	/* INCLUDE_PCMCIA */

/*******************************************************************************
*
* flInportb - read byte to I/O port
*
* This routine read byte to I/O port.
*
* RETURNS: data byte read from I/O port
*/

LOCAL unsigned char flInportb 
    (
    unsigned portId		/* I/O port number */
    )
    {
    return (sysInByte (portId));
    }

/*******************************************************************************
*
* flOutportb - write byte to I/O port
*
* This routine write byte to I/O port.
*
* RETURNS: N/A
*/

LOCAL void flOutportb 
    (
    unsigned portId, 
    unsigned char value
    )
    {
    sysOutByte (portId, value);
    }

/*******************************************************************************
*
* get365 - read an 82365SL register
*
* This routine read an 82365SL register
*
* RETURNS: N/A
*/

LOCAL unsigned char get365
    (
    FLSocket vol, 		/* pointer identifying drive */
    unsigned char reg		/* register index */
    )
    {
    unsigned char value;

#if DRIVES > 1
    if (vol.serialNo == 1)
        reg += 0x40;
#endif
    flStartCriticalSection (&flPcicMutex);
    flOutportb (INDEX_REG,reg);
    value = flInportb (DATA_REG);
    flEndCriticalSection (&flPcicMutex);

    return (value);
    }

/*******************************************************************************
*
* set365 - write an 82365SL register
*
* This routine write an 82365SL register
*
* RETURNS: N/A
*/

LOCAL void set365
    (
    FLSocket vol, 		/* pointer identifying drive */
    unsigned char reg,		/* register index */
    unsigned char value		/* value to set */
    )
    {
#if DRIVES > 1
    if (vol.serialNo == 1)
        reg += 0x40;
#endif
    flStartCriticalSection (&flPcicMutex);
    flOutportb (INDEX_REG, reg);
    flOutportb (DATA_REG, value);
    flEndCriticalSection (&flPcicMutex);
    }

/*******************************************************************************
*
* pcCardDetected - detect if a card is present (inserted)
*
* This routine detects if a card is present (inserted).
*
* RETURNS: TRUE, or FALSE if the card is not present.
*/

LOCAL FLBoolean pcCardDetected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return ((~get365 (&vol, INTERFACE) & 
	    (CARD_DETECT_1 | CARD_DETECT_2)) == 0);
    }

/*******************************************************************************
*
* pcVccOn - turn on Vcc (3.3/5 Volts)
*
* This routine turns on Vcc (3.3/5 Volts). Vcc must be known to be good 
* on exit.
*
* RETURNS: N/A
*/

LOCAL void pcVccOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    unsigned char interface;
    unsigned long timeout	= flMsecCounter + 2000;

    set365 (&vol, POWER, get365 (&vol, POWER) | CARD_POWER_ENABLE);
    do {
        interface = get365 (&vol, INTERFACE);
        } while (pcCardDetected (&vol) && 
		 (~interface & (CARD_POWER_ACTIVE | CARD_READY)) &&
	         (flMsecCounter < timeout));
    }

/*******************************************************************************
*
* pcVccOff - turn off Vcc (3.3/5 Volts)
*
* This routine turns off Vcc (3.3/5 Volts).
*
* RETURNS: N/A
*/

LOCAL void pcVccOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
#if	FALSE
    set365 (&vol, POWER, get365 (&vol, POWER) & ~CARD_POWER_ENABLE);
#endif
    }

#ifdef SOCKET_12_VOLTS

/*******************************************************************************
*
* pcVppOn - turn on Vpp (12 Volts)
*
* This routine turns on Vpp (12 Volts). Vpp must be known to be good on exit.
*
* RETURNS: flOK always
*/

LOCAL FLStatus pcVppOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    set365 (&vol, POWER, 
	(get365 (&vol, POWER) & ~(VPP1_CONTROL | VPP2_CONTROL)) | VPP_ON_12V);
    flDelayMsecs (VPP_DELAY_MSEC);	/* wait for Vpp to ramp up */

    return (flOK);
    }

/*******************************************************************************
*
* pcVppOff - turn off Vpp (12 Volts)
*
* This routine turns off Vpp (12 Volts).
*
* RETURNS: N/A
*/

LOCAL void pcVppOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    set365 (&vol, POWER, 
	    (get365(&vol, POWER) & ~(VPP1_CONTROL | VPP2_CONTROL)) | VPP_ON_5V);
    }

#endif	/* SOCKET_12_VOLTS */

/*******************************************************************************
*
* pcInitSocket - perform all necessary initializations of the socket
*
* This routine performs all necessary initializations of the socket.
*
* RETURNS: flOK, or flGeneralFailure if it failed to create a mutex semaphore,
*                or FALSE if the PCIC is neither STEP_A nor STEP_B.
*/

LOCAL FLStatus pcInitSocket
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    unsigned char identification;

    /* create mutex protecting PCIC registers access */

    if ((flPcicMutex == NULL) && (flCreateMutex (&flPcicMutex) != flOK))
	{
#ifdef DEBUG_PRINT
        DEBUG_PRINT ("Debug: failed creating Mutex for PCIC.\n");
#endif
        return (flGeneralFailure);
        }

    identification = get365 (&vol, IDENTIFICATION);
    if (identification != PCIC_STEP_A && identification != PCIC_STEP_B)
        return (FALSE);

    set365 (&vol, POWER, DISABLE_RESUME_RESETDRV | ENABLE_OUTPUTS);
    set365 (&vol, INTERRUPT, 0);			/* reset */
    set365 (&vol, INTERRUPT, PC_CARD_NOT_RESET);	/* enough reset */
    set365 (&vol, CARD_STATUS_INTERRUPT, 0);		/* no CSC interrupt */
    set365 (&vol, ADDRESS_WINDOW_ENABLE, MEMCS16_DECODE);

    return (flOK);
    }

/*******************************************************************************
*
* pcSetWindow - set current window attributes, Base address, size, etc
*
* This routine sets current window hardware attributes: Base address, size,
* speed and bus width.  The requested settings are given in the 'vol.window' 
* structure.  If it is not possible to set the window size requested in
* 'vol.window.size', the window size should be set to a larger value, 
* if possible. In any case, 'vol.window.size' should contain the 
* actual window size (in 4 KB units) on exit.
*
* RETURNS: N/A
*/

LOCAL void pcSetWindow
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    set365 (&vol, ADDRESS_WINDOW_ENABLE,
	   MEMCS16_DECODE | (MEMORY_WINDOW_0_ENABLE << WINDOW_ID));
    set365 (&vol, WINDOW_0_START_LO + WINDOW_ID * 8,
	   vol.window.baseAddress);
    set365 (&vol, WINDOW_0_START_HI + WINDOW_ID * 8,
	   vol.window.busWidth == 16 ? DATA_16_BITS : 0);
    set365 (&vol, WINDOW_0_STOP_LO + WINDOW_ID * 8,
	   vol.window.baseAddress + (vol.window.size / 0x1000) - 1);
    set365 (&vol, WINDOW_0_STOP_HI + WINDOW_ID * 8,0);	/* no wait states */
    }

/*******************************************************************************
*
* pcSetMappingContext - sets the window mapping register to a card address
*
* This routine sets the window mapping register to a card address.
* The window should be set to the value of 'vol.window.currentPage',
* which is the card address divided by 4 KB. An address over 128MB,
* (page over 32K) specifies an attribute-space address.
*
* The page to map is guaranteed to be on a full window-size boundary.
*
* RETURNS: N/A
*/

LOCAL void pcSetMappingContext
    (
    FLSocket vol,		/* pointer identifying drive */
    unsigned page		/* page to be mapped */
    )
    {
    unsigned mapRegValue = page - vol.window.baseAddress;

    mapRegValue &= 0x3fff;
    if (page & ATTRIBUTE_SPACE_MAPPED)
        mapRegValue |= (REG_ACTIVE << 8);
    set365 (&vol, WINDOW_0_ADDRESS_LO + WINDOW_ID * 8, mapRegValue);
    set365 (&vol, WINDOW_0_ADDRESS_HI + WINDOW_ID * 8, mapRegValue >> 8);
    }

/*******************************************************************************
*
* pcGetAndClearCardChangeIndicator - return the hardware card-change indicator
*
* This routine returns the hardware card-change indicator and clears it if set.
*
* RETURNS: FALSE, or TRUE if the card has been changed
*/

LOCAL FLBoolean pcGetAndClearCardChangeIndicator
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    /* Note: On the 365, the indicator is turned off by the act of reading */
    return (get365 (&vol, CARD_STATUS_CHANGE) & CARD_DETECT_CHANGE);
    }

/*******************************************************************************
*
* pcWriteProtected - return the write-protect state of the media
*
* This routine returns the write-protect state of the media
*
* RETURNS: FALSE, or TRUE if the card is write-protected
*/

LOCAL FLBoolean pcWriteProtected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return ((~get365 (&vol, INTERFACE) & 
	    (MEMORY_WRITE_PROTECT | CARD_DETECT_1 | CARD_DETECT_2)) == 0);
    }

#endif	/* INCLUDE_PCMCIA */

#ifdef EXIT

/*******************************************************************************
*
* pcFreeSocket - free resources that were allocated for this socket.
*
* This routine free resources that were allocated for this socket.
* This function is called when FLite exits.
*
* RETURNS: N/A
*/

LOCAL void pcFreeSocket
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    flDeleteMutex (&flPcicMutex);
    }

#endif /* EXIT */
#endif	/* defined (INCLUDE_SOCKET_PCIC0) || defined (INCLUDE_SOCKET_PCIC1) */

/*******************************************************************************
*
* flFitInSocketWindow - check whether the flash array fits in the socket window
*
* This routine checks whether the flash array fits in the socket window.
*
* RETURNS: A chip size guaranteed to fit in the socket window.
*/

long int flFitInSocketWindow 
    (
    long int chipSize,		/* size of single physical chip in bytes */
    int      interleaving,	/* flash chip interleaving (1,2,4 etc) */
    long int windowSize		/* socket window size in bytes */
    )
    {
    /* x86 architectures use sliding windows for flash arrays */
    /* so this check is irrelevant for them                   */

    return (chipSize);
    }

#if	FALSE
/*******************************************************************************
*
* sysTffsCpy - copy memory from one location to another
*
* This routine copies <size> characters from the object pointed
* to by <source> into the object pointed to by <destination>. If copying
* takes place between objects that overlap, the behavior is undefined.
*
* INCLUDE FILES: string.h
*
* RETURNS: A pointer to <destination>.
* 
* NOMANUAL
*/

void * sysTffsCpy
    (
    void *       destination,   /* destination of copy */
    const void * source,        /* source of copy */
    size_t       size           /* size of memory to copy */
    )
    {
    bcopy ((char *) source, (char *) destination, (size_t) size);
    return (destination);
    }

/*******************************************************************************
*
* sysTffsSet - set a block of memory
*
* This routine stores <c> converted to an `unsigned char' in each of the
* elements of the array of `unsigned char' beginning at <m>, with size <size>.
*
* INCLUDE FILES: string.h
*
* RETURNS: A pointer to <m>.
* 
* NOMANUAL
*/

void * sysTffsSet
    (
    void * m,                   /* block of memory */
    int    c,                   /* character to store */
    size_t size                 /* size of memory */
    )
    {
    bfill ((char *) m, (int) size, c);
    return (m);
    }
#endif	/* FALSE */

/*******************************************************************************
*
* flDelayMsecs - wait for specified number of milliseconds
*
* This routine waits for specified number of milliseconds.
*
* RETURNS: N/A
* 
* NOMANUAL
*/

void flDelayMsecs
    (
    unsigned milliseconds       /* milliseconds to wait */
    )
    {
    UINT32 ix;
    UINT32 iy = 1;
    UINT32 iz = 2;

    /* it doesn't count time consumed in interrupt level */

    for (ix = 0; ix < milliseconds; ix++)
        for (ix = 0; ix < sysTffsMsecLoopCount; ix++)
	    {
	    tickGet ();			/* dummy */
	    iy = KILL_TIME_FUNC;	/* consume time */
	    }
    }

/*******************************************************************************
*
* flDelayLoop - consume the specified time
*
* This routine consumes the specified time.
*
* RETURNS: N/A
*/

void flDelayLoop 
    (
    int  cycles
    )
    {
    }
