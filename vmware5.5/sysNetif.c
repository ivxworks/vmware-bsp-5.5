/* sysNetif.c - system network interface support library */

/* Copyright 1989-2001 Wind River Systems, Inc.  */

/*
modification history
--------------------
01v,16oct01,pai  Moved FEI 82557 END driver configuration to sysFei82557End.c
01u,02oct01,pai  Moved AMD 79c97x END driver configuration code to
                 sysLn97xEnd.c
01t,11sep01,hdn  replaced "irq + INT_NUM_IRQ0" with INT_NUM_GET (irq)
01s,28jan00,jkf  enabling 557 interrupt after connecting ISR, SPR#30132.
01s,03may00,mks  Give driver an interrupt level to use, when choosen PCI forced
                 config. This modification will hold good for SFL booted board.
01r,07sep99,stv  macros for including END driver components are 
		 conditioned according to SPR# 26296.	
01q,12mar99,jkf  renamed boardResource and pciResources to dev specific.
01p,09mar99,sbs  moved sysEltIntEnable and sysEltIntDisable to
                 sysElt3c509End.c
                 corrected previous version number.
01o,08feb99,jkf  removed PCI_CFG_* definitions, now set in configAll.h
01n,01feb99,jkf  added support for AMD 7997x PCI card.
01m,26nov98,ms_  add support for end enabled elt3c509
01l,27aug98,dat  fixed conflicting symbols from if_fei.h and fei82557
01k,16apr98,cn   added import of feiIntConnect, feiEndIntConnect and 
		 feiEndIntDisconnect and their initialisation.
01g,31mar98,cn   Added enhanced network driver support, changed BOARD_INFO 
		 in FEI_BOARD_INFO if INCLUDE_FEI_END is defined.
01f,17mar98,sbs  moved PRO100B PCI definitions to if_fei.h.
                 documentation corrections.
01e,03mar98,sbs  removed update of mmu table entries.
                 using sysMmuMapAdd() for adding mmu entries.
                 added extra members to feiResource structure.
                 changed initialization of feiResources arrray. 
01d,03dec96,hdn  changed UINT32 to INT32 for timeout, str[6], *pResults.
		 added sys557PciInit(). added configType to the resource.
01c,20nov96,dat  chg'd name to sysNetif.c, incorporated #defines
		 and struct typedefs from header file. Combines old
		 if_eex32.c and if_i82557.c files.
01b,07nov96,hdn  re-written.
01a,31aug96,dzb  written, based on v01a of src/drv/netif/if_iep.c.
*/

/*
DESCRIPTION
Ths is the WRS-supplied configuration module for the VxWorks Intel
EtherExpress Flash 32 i82596 Netif driver (if_eex32).  It has routines for
initializing device resources and provides BSP-specific driver routines.

The number of supported devices that can be configured for a particular
system is finite and is specified by the MAX_UNITS configuration constant
in this file.  This value, and the internal data structures using it, can
be modified in this file for specific implementations.
*/


#if defined(INCLUDE_EEX32)

/* includes */

#include "drv/netif/if_eex32.h"


/* defines */

#undef		EEX32_DEBUG		/* Compiles debug output */
#define 	MAX_UNITS	4	/* maximum units to support */

/* LAN Board Types */

#define		TYPE_UNKNOWN	0	/* initial value */
#define		TYPE_EEX32	1	/* Intel EtherExpress Flash 32 */

/* typedefs */

typedef struct i596Info			/* extra information struct */
    {
    UINT	port;			/* I/O port base address */
    UINT	type;			/* Type of LAN board this unit is */
    int		ilevel;			/* Interrupt level of this unit */
    } I596_INFO;


/* globals */

STATUS 	sysEnetAddrGet	(int unit, char addr[]);
STATUS 	sys596Init	(int unit);
STATUS 	sys596IntAck	(int unit);
STATUS 	sys596IntEnable (int unit);
void   	sys596IntDisable (int unit);
void   	sys596Port	(int unit, int cmd, UINT32 addr);
void   	sys596ChanAtn	(int unit);

/* locals */

LOCAL I596_INFO i596Info [MAX_UNITS];

LOCAL unsigned char eex32IdString [] =	/* Fixed-length, no NUL terminator! */
    {
    EEX32_EISA_ID0,
    EEX32_EISA_ID1,
    EEX32_EISA_ID2,
    EEX32_EISA_ID3
    };

LOCAL unsigned char eex32IntLevel [8] = /* first 4 are PLX, rest are FLEA */
    {
    5, 9, 10, 11, 3, 7, 12, 15
    };

/* forward function declarations */

LOCAL STATUS sysInitNecessary (int unit);
LOCAL STATUS sysFindEisaBoard (I596_INFO *pI596Info, UCHAR *idString);

/*******************************************************************************
*
* sysEnetAddrGet - retrieve net unit's Ethernet address
*
* The driver expects this routine to provide the six byte Ethernet address
* that will be used by this unit.  This routine must copy the six byte
* address to the space provided by pCopy.  This routine is expected to
* return OK on success, or ERROR.
*
* The driver calls this routine, once per unit, from the eiattach() routine.
* In if_ei.c version 03c,15jan93, this routine is the first 82596 support
* routine called.  In the PC environment, it is necessary for this routine
* then to find the hardware corresponding to the given unit number, before
* it can return the Ethernet address!  This is done by the function
* sysInitNecessary() (q.v.).
*
* Here we blithely assume that sysInitNecessary() finds a unit for us,
* as if no one would have attched the unit if it didn't exist!
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
*/

STATUS sysEnetAddrGet
    (
    int unit,
    char addr[]
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];
    UINT charIndex;

    /* 
     * Find a physical board to go with this unit and set up the
     * I596_INFO structure to refer to that board.
     */

    if (sysInitNecessary (unit) != OK)
	return (ERROR);

    for (charIndex = 0; charIndex < 6; charIndex++)
	addr[charIndex] = sysInByte (pI596Info->port + NET_IA0 + charIndex);

    return (OK);
    }

/*******************************************************************************
*
* sys596Init - prepare a LAN board for Ethernet initialization
*
* This routine performs target-specific initialization that must occur
* before the Intel 82596 Ethernet chip is initialized.  Typically, this
* routine is empty.
*
* The driver calls this routine from the eiattach() routine, once per unit,
* immediately before starting up the Intel 82596.  We here call
* sysInitNecessary() again in case eiattach() hasn't called sysInetAddrGet()
* yet.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: eiattach(), sysInitNecessary()
*/

STATUS sys596Init
    (
    int unit  /* unit number */
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];
    UCHAR intIndex;

    if (sysInitNecessary (unit) != OK)
	return (ERROR);

    if ( (sysInByte (pI596Info->port + IRQCTL) & IRQ_EXTEND) == 0)
        {
        intIndex = (sysInByte (pI596Info->port + PLX_CONF0) &
                    (IRQ_SEL0 | IRQ_SEL1)) >> 1;
        }
    else
        {
        intIndex = ( (sysInByte (pI596Info->port + IRQCTL) &
                      (IRQ_SEL0 | IRQ_SEL1)) >> 1) + 4;
        }

    pI596Info->ilevel = eex32IntLevel[intIndex];

    sysIntEnablePIC (pI596Info->ilevel);

    return (OK);
    }

/*******************************************************************************
*
* sys596IntAck - acknowledge an Ethernet chip interrupt 
*
* This routine acknowledges any specified non-Intel 82596 Ethernet chip
* interrupt.  Typically, this involves an operation to some interrupt
* control hardware.
*
* The interrupt signal from the 82596 behaves in an "edge-triggered" mode;
* therefore, this routine clears a latch within the control
* circuitry.
*
* NOTE: The driver calls this routine from the interrupt service routine.
*
* RETURNS: OK, or ERROR if the  
*/

STATUS sys596IntAck
    (
    int unit  /* unit number */
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];

    if (pI596Info->type == TYPE_UNKNOWN)
	return (ERROR);

    sysOutByte (pI596Info->port + IRQCTL,
                sysInByte (pI596Info->port + IRQCTL) | IRQ_LATCH);
    return (OK);
    }

/*******************************************************************************
*
* sys596IntEnable - enable an interrupt from an Intel 82596 Ethernet chip
*
* This routine manipulates i386/i486 board hardware to permit an interrupt.
*
* The Ethernet chip driver calls this routine throughout normal operation to
* terminate critical sections of code.
*
* RETURNS: OK, or ERROR if
*
* SEE ALSO: sys596IntDisable
*/

STATUS sys596IntEnable
    (
    int unit  /* unit number */
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];

    if (pI596Info->type == TYPE_UNKNOWN)
	return (ERROR);

    sysOutByte (pI596Info->port + IRQCTL,
		sysInByte (pI596Info->port + IRQCTL) & ~IRQ_FORCE_LOW);

    return (OK);
    }

/*******************************************************************************
*
* sys596IntDisable - disable an interrupt from an Intel 82596 Ethernet chip
*
* This routine manipulates i386/i486 board hardware to prevent interrupts.
*
* The Ethernet chip driver calls this routine throughout normal operation to
* protect critical sections of code from interrupt service routine
* intervention.
*
* RETURNS: N/A
*
* SEE ALSO: sys596IntEnable()
*/

void   sys596IntDisable
    (
    int unit  /* unit number */
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];

    if (pI596Info->type == TYPE_UNKNOWN)
	return;

    sysOutByte (pI596Info->port + IRQCTL,
		sysInByte (pI596Info->port + IRQCTL) | IRQ_FORCE_LOW);
    }

/*******************************************************************************
*
* sys596Port - issue PORT command to 82596
*
* This routine provides access to the special port function of the Intel
* 82596 Ethernet chip.  The driver expects this routine to deliver the
* command and address arguments to the port of the specified unit.
*
* The Ethernet chip driver calls this routine primarily during
* initialization, but may also call it during error recovery procedures.
* Because it is called during initialization, this routine can be called
* before the board for this unit has been found, depending on how eiattach()
* might be modified in the future.
*
* RETURNS: N/A
*/

void   sys596Port
    (
    int unit,
    int cmd,
    UINT32 addr
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];

    if (sysInitNecessary (unit) != OK)	/* Just in case not called yet! */
	return;

    /* PORT command wants to see 16 bits at a time, low-order first */

    sysOutWord (pI596Info->port + PORT, (cmd + addr) & 0xffff);
    sysOutWord (pI596Info->port + PORT + 2, ((cmd + addr) >> 16) & 0xffff);
    }

/*******************************************************************************
*
* sys596ChanAtn - assert channel attention signal to an Intel 82596
*
* This routine provides the channel attention signal to the Intel 82596
* Ethernet chip for a specified unit.
*
* The driver calls this routine frequently throughout all phases of
* operation.
*
* RETURNS: N/A
*/

void   sys596ChanAtn
    (
    int unit  /* unit number */
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];

    if (pI596Info->type == TYPE_UNKNOWN)
	return;

    sysOutByte (pI596Info->port + CA, 0);
    }

/*******************************************************************************
*
* sysInitNecessary - if unit is undefined, locate and set up the next board
*
*/

LOCAL STATUS sysInitNecessary
    (
    int unit  /* unit number */
    )
    {
    I596_INFO *pI596Info = &i596Info[unit];

    if (pI596Info->type == TYPE_UNKNOWN)
	{
	if (sysFindEisaBoard (pI596Info, eex32IdString) == ERROR)
	    return (ERROR);

	pI596Info->type = TYPE_EEX32;
	}

    /* Set up next unit structure in case there's another board */

    if (unit < (MAX_UNITS - 1))
        i596Info[unit + 1].port = i596Info[unit].port;

    return (OK);
    }

/*******************************************************************************
*
* sysFindEisaBoard - scan selected EISA slots for a board with given ID bytes
*
*/

LOCAL STATUS sysFindEisaBoard
    (
    I596_INFO *pI596Info,		/* Start at I/O address in here */
    UCHAR *idString
    )
    {
    UINT byteNumber;
    BOOL failed;

    do
        {
	failed = FALSE;
	for (byteNumber = 0; (byteNumber < 4) && !failed; byteNumber++)
	    {
	    failed = (sysInByte (pI596Info->port + EISA_ID0 + byteNumber)
		     != idString[byteNumber]);
	    }
	if (!failed)
	    return (OK);

	pI596Info->port += 0x1000;
        }
    while (pI596Info->port != 0xf000);

    return (ERROR);
    }

#endif /* INCLUDE_EEX32 */
