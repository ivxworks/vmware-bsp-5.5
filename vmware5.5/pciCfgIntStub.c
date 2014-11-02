/* pciCfgIntStub.c - pcPentium BSP stub for PCI shared interrupts */

/* Copyright 2001-2002, Wind River Systems, Inc. */

/*
modification history
--------------------
01f,12mar02,hdn  added ICH3, updated IOAPIC support for HTT (spr 73738)
01e,03oct01,hdn  added i82801BA ICH2 support
01d,23oct01,pai  Added prototype for pciIntLibInit().
01c,18sep00,dat  fixing param names
01b,07sep00,dat  fix to pci stubs
01a,06aug00,dat  written
*/

/*
For the pciIntLib module, the following macros customize how the code is
compiled.  For the generic pc platform, all these macros take on their
default values.  For special hardware, just edit these macros in this BSP
specific stub file as needed. (Please do not edit the global stub file in
target/config/comps/src).

.IP PCI_INT_LINES
This macro represents the number of PCI interrupt vectors that may be shared.
The default number is 32.  At startup time a linked list is created for each
possible shared vector.

.IP PCI_INT_BASE
This macro represents the base vector number for the first shared PCI vector.
It is used in the default mapping of system vector numbers to shared PCI vector
numbers.  The default value is the standard macro INT_NUM_IRQ0, or zero
if that macro is not defined. This macro is used with the PCI_INT_VECTOR_TO_IRQ
macro described below.

.IP PCI_INT_VECTOR_TO_IRQ(vector)
This macro accepts a system vector number as input and returns a PCI irq number
as output.  This value is used as the index into the list of shared interrupts.
The resulting PCI irq number must lie in the range of zero to
(PCI_INT_LINES - 1).  By default, the macro evaluates to:
.CS
	(IVEC_TO_INUM(vector) - PCI_INT_BASE)
.CE

.IP PCI_INT_HANDLER_BIND(vector, rtn, arg, pResult)
This macro is used by the module to bind the master PCI interrupt handling
routine to the actual system vector.  By default it is mapped to:
.CS
	*pResult = intConnect (vector, rtn, arg);
.CE

.IP PCI_INT_HANDLER_UNBIND(vector, rtn, arg, pResult)
This macro is used by the module to unbind the master PCI interrupt handling
routine pciInt() from the actual system vector.  This will only happen when
the last shared interrupt handler is disconnected from a shared interrupt.
The default mapping is a no-op function that returns OK.
Note: without a functional mapping, a small memory leak will exist if PCI
interrupts are connected and disconnected repeatedly.  If present, the leak
involves the code stub generated as part of the default binding routine, 
intConnect().
*/


/* macros */

/* 
 * PCI_INT_LINES: number of PCI interrupt/IRQ is
 * - number of IRQs [0-15] in the PIC or VIRTUAL_WIRE mode
 * - number of IRQs [0-15,A-H] in the SYMMETRIC_IO mode
 */

#ifdef	SYMMETRIC_IO_MODE
#   define PCI_INT_LINES	(N_PIC_IRQS + N_IOAPIC_PIRQS)
#else
#   define PCI_INT_LINES	(N_PIC_IRQS)
#endif  /* SYMMETRIC_IO_MODE */

/* 
 * PCI_INT_BASE: PCI base IRQ number (not intNum) is
 * - IRQ 0 in the PIC or VIRTUAL_WIRE mode
 * - IRQ 0 in the SYMMETRIC_IO mode
 */

#define PCI_INT_BASE		(0)

/*
 * This maps a system vector to a PCI irq number, in the range
 * 0 to (PCI_INT_LINES - 1). The output is used as an index
 * into the array of linked lists used for sharing.
 */

#define PCI_INT_VECTOR_TO_IRQ(vector)	(sysPciIvecToIrq((int)vector))

/*
 * Provide intConnect via a macro so that an alternate interrupt binding
 * mechanism can be specified
 */

#define PCI_INT_HANDLER_BIND(vector, routine, param, pResult)		\
    do {								\
    IMPORT STATUS intConnect();						\
    *pResult = intConnect ( (vector),(routine), (int)(param) );		\
    } while (0)

/*
 * Provide an unbind macro to remove an interrupt binding.  The default 
 * is a no-op.  This can result in a memory leak if there
 * is a lot of pciIntConnect, pciIntDisconnect activity.
 */

#define PCI_INT_HANDLER_UNBIND(vector, routine, param, pResult)		\
    do { *pResult = OK; } while (0)


/* imports */

IMPORT STATUS 	pciIntLibInit (void);		/* pci/pciIntLib.c */
IMPORT UINT8	sysInumTbl[];			/* IRQ vs intNum table */
IMPORT UINT32	sysInumTblNumEnt;		/* number of IRQs */


/* forward declarations */


/***********************************************************************
*
* sysPciIntInit - PCI interrupt library init
*
* Modify this routine as needed for any special host bridge initialization
* related to interrupt handling.
*/

VOID sysPciIntInit (void)
    {

    /* TODO: add any special pre-initialization code here */

    if (pciIntLibInit () == ERROR)
	{
	sprintf (sysExcMsg, "pciCfgIntStub.c: pciIntLibInit() failed\n");
	sysToMonitor (BOOT_NO_AUTOBOOT);
	}
    }

/*******************************************************************************
*
* sysPciIvecToIrq - get an IRQ(PIC or IOAPIC) number from vector address
*
* This routine gets an IRQ(PIC or IOAPIC) number from vector address.
* Assumptions are following:
*   - IRQ number is 0 - 15 in PIC or VIRTUAL_WIRE mode
*   - IRQ number is 0 - 23 in SYMMETRIC_IO mode
*
* RETURNS: IRQ 0 - 15/23, or -1 if we failed to get it.
*
* ARGSUSED0
*/

int sysPciIvecToIrq 
    (
    int vector		/* vector address */
    )
    {
    UINT32 irq;
    UINT32 intNum = IVEC_TO_INUM (vector);

    /* walk through the sysInumTbl[] to get the match */

    for (irq = PCI_INT_BASE; irq < (PCI_INT_BASE + PCI_INT_LINES); irq++)
	{
	if (sysInumTbl[irq] == intNum)
    	    return (irq);
	}

    return (ERROR);
    }


#ifdef	SYMMETRIC_IO_MODE

#if	defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3)

/* Intel ICH2/3 (IO Controller Hub 2/3) specific stuff */

/* typedefs */

typedef	struct sysPciPirqTbl
    {
    INT16 offset;	/* ICH2/3 (LPC I/F - D31:F0) PIRQ[A-H]_ROUT offset */
    UINT8 pirq;		/* IOAPIC_PIRQ[A-H]_INT_LVL */
    UINT8 irq;		/* default IRQ[0-15] (ISA IRQ 0 - 15) */
    } SYS_PCI_PIRQ_TBL;


/* defines */

/* ICH2/3 (LPC I/F - D31:F0) */

#define ICH2_LPC_PCI_BUSNO	0x0	/* ICH2/3 LPC PCI BusNo */
#define ICH2_LPC_PCI_DEVNO	0x1f	/* ICH2/3 LPC PCI DevNo */
#define ICH2_LPC_PCI_FUNCNO	0x0	/* ICH2/3 LPC PCI FuncNo */
#define ICH2_LPC_VID		0x8086	/* ICH2   LPC PCI vendor ID */
#define ICH2_LPC_DID_S		0x2440	/* ICH2   LPC PCI device ID */
#define ICH2_LPC_DID_M		0x244c	/* ICH2   LPC PCI device ID */
#define ICH3_LPC_VID		0x8086	/* ICH3   LPC PCI vendor ID */
#define ICH3_LPC_DID_S		0x2480	/* ICH3   LPC PCI device ID */
#define ICH3_LPC_DID_M		0x248c	/* ICH3   LPC PCI device ID */

/* ICH2/3 (LPC I/F - D31:F0) GEN_CNTL - General Control Reg */

#define ICH2_LPC_GEN_CNTL	0xd0		/* offset GEN_CNTL */
#define ICH2_APICEN		0x00000100	/* APIC enable */
#define ICH2_XAPIC_EN		0x00000080	/* IO (x) extension enable */

/* ICH2/3 (LPC I/F - D31:F0) PIRQ[n]_ROUT - PIRQ[n] Routing Control */

#define ICH2_LPC_PIRQA		0x60		/* offset PIRQA */
#define ICH2_LPC_PIRQB      	0x61		/* offset PIRQB */
#define ICH2_LPC_PIRQC		0x62		/* offset PIRQC */
#define ICH2_LPC_PIRQD		0x63		/* offset PIRQD */
#define ICH2_LPC_PIRQE		0x68		/* offset PIRQE */
#define ICH2_LPC_PIRQF		0x69		/* offset PIRQF */
#define ICH2_LPC_PIRQG		0x6a		/* offset PIRQG */
#define ICH2_LPC_PIRQH		0x6b		/* offset PIRQH */
#define ICH2_IRQ_DIS		0x80		/* ISA IRQ routing disable */
#define ICH2_IRQ_MASK		0x0f		/* ISA IRQ routing mask */


/* locals */

/* PIRQ[A-H] (Programmable IRQ) vs IRQ[0-15] routing table */

LOCAL SYS_PCI_PIRQ_TBL sysPciPirqTbl [N_IOAPIC_PIRQS] = {
    {ICH2_LPC_PIRQA, IOAPIC_PIRQA_INT_LVL + 0, 0},
    {ICH2_LPC_PIRQB, IOAPIC_PIRQA_INT_LVL + 1, 0},
    {ICH2_LPC_PIRQC, IOAPIC_PIRQA_INT_LVL + 2, 0},
    {ICH2_LPC_PIRQD, IOAPIC_PIRQA_INT_LVL + 3, 0},
    {ICH2_LPC_PIRQE, IOAPIC_PIRQA_INT_LVL + 4, 0},
    {ICH2_LPC_PIRQF, IOAPIC_PIRQA_INT_LVL + 5, 0},
    {ICH2_LPC_PIRQG, IOAPIC_PIRQA_INT_LVL + 6, 0},
    {ICH2_LPC_PIRQH, IOAPIC_PIRQA_INT_LVL + 7, 0}
    };


#   if defined (INCLUDE_D815EEA) || defined (INCLUDE_D850GB)

/* Intel Mother Board D815EEA/D850GB specific stuff */

/* defines */

/* PCI bus slot : PCI bus/dev/func no., etc */

#   define MOTHER_PCI_BUSNO_SLOT	0x02	/* PCI BusNo for PCI slot */
#   define MOTHER_PCI_DEVNO_SLOT0	0x09	/* PCI DevNo for slot0 */
#   define MOTHER_PCI_DEVNO_SLOT1	0x0a	/* PCI DevNo for slot1 */
#   define MOTHER_PCI_DEVNO_SLOT2	0x0b	/* PCI DevNo for slot2 */
#   define MOTHER_PCI_DEVNO_SLOT3	0x0c	/* PCI DevNo for slot3 */
#   define MOTHER_PCI_DEVNO_SLOT4	0x0d	/* PCI DevNo for slot4 */
#   define MOTHER_PCI_FUNCNO_SLOT	0x00	/* PCI FuncNo for slotX */
#   define N_PCI_SLOTS			5	/* number of PCI slots */
#   define MOTHER_PCI_SLOT_GET(pciDev)	(pciDev - MOTHER_PCI_DEVNO_SLOT0)


/* locals */

/* PIRQ[A-H] (Programmable IRQ) vs INT[A-D] wiring table */

LOCAL UINT8 sysPciIntTbl [N_PCI_SLOTS][4] = {
    {IOAPIC_PIRQF_INT_LVL, 		/* slot1(J4E1) INTA */
     IOAPIC_PIRQG_INT_LVL, 		/* slot1(J4E1) INTB */
     IOAPIC_PIRQH_INT_LVL,		/* slot1(J4E1) INTC */
     IOAPIC_PIRQB_INT_LVL},		/* slot1(J4E1) INTD */
    {IOAPIC_PIRQG_INT_LVL, 		/* slot2(J4D1) INTA */
     IOAPIC_PIRQH_INT_LVL, 		/* slot2(J4D1) INTB */
     IOAPIC_PIRQB_INT_LVL,		/* slot2(J4D1) INTC */
     IOAPIC_PIRQF_INT_LVL},		/* slot2(J4D1) INTD */
    {IOAPIC_PIRQH_INT_LVL, 		/* slot3(J4C1) INTA */
     IOAPIC_PIRQB_INT_LVL, 		/* slot3(J4C1) INTB */
     IOAPIC_PIRQF_INT_LVL,		/* slot3(J4C1) INTC */
     IOAPIC_PIRQG_INT_LVL},		/* slot3(J4C1) INTD */
    {IOAPIC_PIRQB_INT_LVL, 		/* slot4(J4B1) INTA */
     IOAPIC_PIRQF_INT_LVL, 		/* slot4(J4B1) INTB */
     IOAPIC_PIRQG_INT_LVL,		/* slot4(J4B1) INTC */
     IOAPIC_PIRQH_INT_LVL},		/* slot4(J4B1) INTD */
    {IOAPIC_PIRQF_INT_LVL, 		/* slot5(J4A1) INTA */
     IOAPIC_PIRQG_INT_LVL, 		/* slot5(J4A1) INTB */
     IOAPIC_PIRQH_INT_LVL,		/* slot5(J4A1) INTC */
     IOAPIC_PIRQB_INT_LVL}};		/* slot5(J4A1) INTD */

/* PCI slot[n] device no. table */

LOCAL UINT8 sysPciSlotTbl [N_PCI_SLOTS] = {
    MOTHER_PCI_DEVNO_SLOT0,		/* PCI DevNo for slot0 */
    MOTHER_PCI_DEVNO_SLOT1,		/* PCI DevNo for slot1 */
    MOTHER_PCI_DEVNO_SLOT2,		/* PCI DevNo for slot2 */
    MOTHER_PCI_DEVNO_SLOT3,		/* PCI DevNo for slot3 */
    MOTHER_PCI_DEVNO_SLOT4		/* PCI DevNo for slot4 */
    };

#   else

#       define INCLUDE_UNKNOWN_MOTHER	/* unknown mother board */

#   endif /* defined (INCLUDE_D815EEA) || defined (INCLUDE_D850GB) */

#else

#   define INCLUDE_UNKNOWN_ICH		/* unknown ICH */
#   define INCLUDE_UNKNOWN_MOTHER	/* unknown mother board */

#endif	/* defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3) */


#ifdef	INCLUDE_UNKNOWN_MOTHER

/* defines */

#   define N_PCI_NETS		8	/* number of PCI network devs */


/* typedefs */

typedef	struct sysPciNetTbl
    {
    INT32 pciBus;	/* PCI bus no */
    INT32 pciDev;	/* PCI device no */
    INT32 pciFunc;	/* PCI func no */
    INT16 vendorId;	/* PCI vendor id */
    INT16 deviceId;	/* PCI device id */
    INT8  classCode;	/* PCI class code */
    INT8  intPin;	/* PCI interrupt pin */
    INT8  intLine;	/* PCI interrupt line */
    } SYS_PCI_NET_TBL;


/* globals */

SYS_PCI_NET_TBL sysPciNetTbl [N_PCI_NETS] =
    {
    {0, 0, 0, 0, 0, 0, 0, 0}
    };

INT32 sysPciNnets = 0;

#endif	/* INCLUDE_UNKNOWN_MOTHER */


/***********************************************************************
*
* sysPciIoApicEnable - enable or disable the IO APIC
*
* This routine enables or disables the IO APIC.  This routine is 
* called by ioApicEnable() in the SYMMETRIC IO mode.
*
* RETURNS: N/A
*/

VOID sysPciIoApicEnable 
    (
    BOOL enable		/* TRUE to enable, FALSE to disable */
    )
    {

#if	defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3)

    INT32 pciBusLpc	= ICH2_LPC_PCI_BUSNO;	/* bus# of ICH2/3 LPC */
    INT32 pciDevLpc	= ICH2_LPC_PCI_DEVNO;	/* dev# of ICH2/3 LPC */
    INT32 pciFuncLpc	= ICH2_LPC_PCI_FUNCNO;	/* func# of ICH2/3 LPC */
    INT32 value;				/* PCI config reg value */
    UINT16 vendorId;
    UINT16 deviceId;

    /* is there ICH2 or ICH3? */

    pciConfigInWord (pciBusLpc, pciDevLpc, pciFuncLpc, 
	PCI_CFG_VENDOR_ID, &vendorId);

    pciConfigInWord (pciBusLpc, pciDevLpc, pciFuncLpc, 
	PCI_CFG_DEVICE_ID, &deviceId);

    if (!(((vendorId == ICH2_LPC_VID) &&
           ((deviceId == ICH2_LPC_DID_S) || (deviceId == ICH2_LPC_DID_M))) ||
          ((vendorId == ICH3_LPC_VID) &&
           ((deviceId == ICH3_LPC_DID_S) || (deviceId == ICH3_LPC_DID_M)))))
       return;

    /* enables or disables 1) IOAPIC address decode 2) IO XAPIC extensions */

    pciConfigInLong (pciBusLpc, pciDevLpc, pciFuncLpc, 
	ICH2_LPC_GEN_CNTL, &value);

    if (enable)
        value |= (ICH2_APICEN | ICH2_XAPIC_EN);
    else
        value &= ~(ICH2_APICEN | ICH2_XAPIC_EN);

    pciConfigOutLong (pciBusLpc, pciDevLpc, pciFuncLpc, 
	ICH2_LPC_GEN_CNTL, value);

#endif	/* defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3) */

    }

/***********************************************************************
*
* sysPciPirqEnable - enable or disbale PCI PIRQ direct handling 
*
* This routine enables or disbales the PCI PIRQ direct handling.
* This routine is called by ioApicEnable() in the SYMMETRIC IO mode.
*
* RETURNS: N/A
*/

VOID sysPciPirqEnable 
    (
    BOOL enable		/* TRUE to enable, FALSE to disable */
    )
    {
    static BOOL sysPciFirstTime = TRUE;

    if (sysPciFirstTime)
        {

#   if defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3)

        {
        INT32 pciBusLpc	 = ICH2_LPC_PCI_BUSNO;
        INT32 pciDevLpc	 = ICH2_LPC_PCI_DEVNO;
        INT32 pciFuncLpc = ICH2_LPC_PCI_FUNCNO;
        INT32 pirq;
        UINT8 irq;

        /* saves the default PIRQ[A-H] routing info */

        for (pirq = 0; pirq < N_IOAPIC_PIRQS; pirq++)
	    {
	    pciConfigInByte (pciBusLpc, pciDevLpc, pciFuncLpc,
	        sysPciPirqTbl[pirq].offset, &irq);
	    sysPciPirqTbl[pirq].irq = (irq & ~ICH2_IRQ_DIS);
	    }
        }

#   endif /* defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3) */

#   ifdef INCLUDE_UNKNOWN_MOTHER

	{
        INT32  pciBus;
        INT32  pciDev;
        UINT16 vendorId;
        UINT16 deviceId;
        UINT8  classCode;
        UINT8  intPin;
        UINT8  intLine;
        INT32  ix;

        /* collect the PCI network device info */

        ix = 0;
        for (pciBus = 0; pciBus < PCI_MAX_BUS; pciBus++)
	    {
	    for (pciDev = 0; pciDev < PCI_MAX_DEV; pciDev++)
	        {
		/* just PCI function 0 for now */

	        pciConfigInWord (pciBus, pciDev, 0, PCI_CFG_VENDOR_ID, 
		    &vendorId);
                pciConfigInWord (pciBus, pciDev, 0, PCI_CFG_DEVICE_ID, 
		    &deviceId);
                pciConfigInByte (pciBus, pciDev, 0, PCI_CFG_CLASS, 
		    &classCode);
                pciConfigInByte (pciBus, pciDev, 0, PCI_CFG_DEV_INT_PIN, 
		    &intPin);
                pciConfigInByte (pciBus, pciDev, 0, PCI_CFG_DEV_INT_LINE, 
		    &intLine);
	    
	        /* just PCI network card for now */

	        if ((vendorId != 0xffff) && (classCode == 0x2))
	            {
	            if (ix < N_PCI_NETS)
	                {
	                sysPciNetTbl[ix].pciBus    = pciBus;
	                sysPciNetTbl[ix].pciDev    = pciDev;
	                sysPciNetTbl[ix].pciFunc   = 0;
	                sysPciNetTbl[ix].vendorId  = vendorId;
	                sysPciNetTbl[ix].deviceId  = deviceId;
	                sysPciNetTbl[ix].classCode = classCode;
	                sysPciNetTbl[ix].intPin    = intPin;
	                sysPciNetTbl[ix].intLine   = intLine;
	                ix++;
	    	        }
	            sysPciNnets++;
	            }
    	        }
    	    }
    	}

#   endif /* INCLUDE_UNKNOWN_MOTHER */

        sysPciFirstTime = FALSE;
        }

    /* enables or disables the PIRQ direct handling */

#   if defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3)

    {
    INT32 pciBusLpc  = ICH2_LPC_PCI_BUSNO;
    INT32 pciDevLpc  = ICH2_LPC_PCI_DEVNO;
    INT32 pciFuncLpc = ICH2_LPC_PCI_FUNCNO;
    INT32 pirq;

    for (pirq = 0; pirq < N_IOAPIC_PIRQS; pirq++)
	{
	if (enable)
	    {
	    pciConfigOutByte (pciBusLpc, pciDevLpc, pciFuncLpc,
	        sysPciPirqTbl[pirq].offset, 
		(sysPciPirqTbl[pirq].irq | ICH2_IRQ_DIS));
	    }
	else
	    {
	    pciConfigOutByte (pciBusLpc, pciDevLpc, pciFuncLpc,
	        sysPciPirqTbl[pirq].offset, 
		(sysPciPirqTbl[pirq].irq & ~ICH2_IRQ_DIS));
	    }
	}
    }

#   endif /* defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3) */

    /* set the PIRQ[A-H] to PCI_CFG_DEV_INT_LINE for direct handling */

#   ifdef INCLUDE_UNKNOWN_MOTHER

    {
    INT32 ix;

    if (enable)
	{

        /* 
	 * INT_LINE should have IRQ number of PIRQ[A-H] (16 - 23).
	 * Since INT[A-D] to PIRQ[A-H] wiring info is unknown, value to set
	 * is unknown too.  So set PIRQA to PCI_CFG_DEV_INT_LINE for now.
	 * Note, setting PIRQA does not mean the device generates PIRQA.
	 */

        for (ix = 0; ix < sysPciNnets; ix++)
            {
	    pciConfigOutByte (sysPciNetTbl[ix].pciBus, sysPciNetTbl[ix].pciDev, 
		sysPciNetTbl[ix].pciFunc, PCI_CFG_DEV_INT_LINE, 
		IOAPIC_PIRQA_INT_LVL);
            }

	}
    else
	{
        /* restore the original IRQn to PCI_CFG_DEV_INT_LINE */

        for (ix = 0; ix < sysPciNnets; ix++)
            {
	    pciConfigOutByte (sysPciNetTbl[ix].pciBus, sysPciNetTbl[ix].pciDev, 
		sysPciNetTbl[ix].pciFunc, PCI_CFG_DEV_INT_LINE, 
		sysPciNetTbl[ix].intLine);
            }
	}
    }

#   elif defined (INCLUDE_D815EEA) || defined (INCLUDE_D850GB)

    {
    INT32 pciBusSlot	= MOTHER_PCI_BUSNO_SLOT;
    INT32 pciFuncSlot	= MOTHER_PCI_FUNCNO_SLOT;
    INT32 pciBusLpc	= ICH2_LPC_PCI_BUSNO;
    INT32 pciDevLpc	= ICH2_LPC_PCI_DEVNO;
    INT32 pciFuncLpc	= ICH2_LPC_PCI_FUNCNO;
    UINT8 intPin;	/* PCI INT[A-D] from PCI Int Pin Reg */
    UINT8 irq;		/* IRQ[0-15] of the device */
    INT32 vendorId;	/* PCI vendor Id */
    INT32 pirq;
    UINT32 index;
    INT32 ix;

    for (ix = 0; ix < N_PCI_SLOTS; ix++)
	{
	/* skip if the slot is empty */

        pciConfigInLong (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
	    PCI_CFG_VENDOR_ID, &vendorId);

	if ((vendorId & 0x0000ffff) == 0x0000ffff)
	    continue;

	/* get the PCI INT[A-D] of the device */

        pciConfigInByte (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
	    PCI_CFG_DEV_INT_PIN, &intPin);

	/* get the PIRQ[A-H] */

	pirq = sysPciIntTbl[ix][intPin - 1];

	/* get the original IRQ[0-15] */

	index = pirq - IOAPIC_PIRQA_INT_LVL;
	irq   = sysPciPirqTbl[index].irq;

        if (enable)
            {
	    /* set the PIRQ[A-H] to the PCI_CFG_DEV_INT_LINE */

            pciConfigOutByte (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
		PCI_CFG_DEV_INT_LINE, pirq);
	    }
        else
	    {
            /* restore the original IRQ[0-15] to PCI_CFG_DEV_INT_LINE */

            pciConfigOutByte (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
	        PCI_CFG_DEV_INT_LINE, irq);
	    }
	}
    }

#   endif /* INCLUDE_UNKNOWN_MOTHER */

    }

#ifdef	INCLUDE_SHOW_ROUTINES

/*******************************************************************************
*
* sysPciPirqShow - show the PCI PIRQ[A-H] to IRQ[0-15] routing status
*
* This routine shows the PCI PIRQ[A-H] to IRQ[0-15] routing status
*
* RETURNS: N/A
*/

void sysPciPirqShow (void)
    {

#   if defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3)

    {
    INT32 ix;

    /* show the PIRQ[A-H] - IRQ[0-15] routing table */

    for (ix = 0; ix < N_IOAPIC_PIRQS; ix++)
	{
        printf ("PIRQ%c: offset=0x%04x, PIRQ=0x%02x, IRQ=0x%02x\n",
	    'A' + ix,
            sysPciPirqTbl[ix].offset,
            sysPciPirqTbl[ix].pirq,
            sysPciPirqTbl[ix].irq);
	}
    }

#   endif /* defined (INCLUDE_ICH2) || defined (INCLUDE_ICH3) */

#   ifdef INCLUDE_UNKNOWN_MOTHER

    {
    INT32 ix;

    /* show the network device table */

    for (ix = 0; ix < sysPciNnets; ix++)
	{
        printf ("bus=0x%04x dev=0x%04x venId=0x%04x "
            "devId=0x%04x intPin=0x%02x intLine=0x%02x\n",
            sysPciNetTbl[ix].pciBus,
            sysPciNetTbl[ix].pciDev,
            sysPciNetTbl[ix].vendorId,
            sysPciNetTbl[ix].deviceId,
            sysPciNetTbl[ix].intPin,
            sysPciNetTbl[ix].intLine);
	}
    }

#   elif defined (INCLUDE_D815EEA) || defined (INCLUDE_D850GB)

    {
    INT32 pciBusSlot	= MOTHER_PCI_BUSNO_SLOT;
    INT32 pciFuncSlot	= MOTHER_PCI_FUNCNO_SLOT;
    FUNCPTR defIsr;	/* ISR of the default IRQ */
    FUNCPTR curIsr;	/* ISR of the current IRQ */
    INT32 idtType;	/* IDT type */
    INT32 selector;	/* CS selector */
    INT32 vendorId;	/* PCI vendor Id */
    UINT8 intPin;	/* PCI INT[A-D] from PCI Int Pin Reg */
    UINT8 defIrq;	/* default IRQ */
    UINT8 curIrq;	/* current IRQ */
    UINT8 pirq;		/* PIRQ[n] */
    INT32 ix;

    for (ix = 0; ix < N_PCI_SLOTS; ix++)
	{
	/* skip if the slot is empty */

        pciConfigInLong (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
			 PCI_CFG_VENDOR_ID, &vendorId);

	if ((vendorId & 0x0000ffff) == 0x0000ffff)
	    {
	    printf ("slot %d: empty \n", ix);
	    continue;
	    }

	/* get the PCI INT[A-D] and IRQ[0-15] of the device */

        pciConfigInByte (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
	    PCI_CFG_DEV_INT_PIN, &intPin);
        pciConfigInByte (pciBusSlot, sysPciSlotTbl[ix], pciFuncSlot, 
	    PCI_CFG_DEV_INT_LINE, &curIrq);

	/* get the PIRQ[A-H] of the device */

	pirq = sysPciIntTbl[ix][intPin - 1];

	/* if sysPciPirqTbl[] table is not initialized, use the curIrq */

	defIrq = sysPciPirqTbl[pirq - IOAPIC_PIRQA_INT_LVL].irq; 

	/* get the default Isr and current Isr */

	intVecGet2 ((FUNCPTR *)INUM_TO_IVEC (INT_NUM_GET (defIrq)), 
	    &defIsr, &idtType, &selector);
	intVecGet2 ((FUNCPTR *)INUM_TO_IVEC (INT_NUM_GET (curIrq)), 
	    &curIsr, &idtType, &selector);

	/* show the default IRQ and the current IRQ setting */

	printf ("slot %d: def-IRQ(ISR) = 0x%04x(0x%08x)  "
		"cur-IRQ(ISR) = 0x%04x(0x%08x)\n",
		ix, defIrq, (int)defIsr, curIrq, (int)curIsr);
	}
    }

#   endif /* INCLUDE_UNKNOWN_MOTHER */

    }

#endif	/* INCLUDE_SHOW_ROUTINES */

#endif	/* SYMMETRIC_IO_MODE */
