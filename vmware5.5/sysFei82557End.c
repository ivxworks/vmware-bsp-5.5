/* sysFei82557End.c - system configuration module for fei82557End driver */
 
/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,16jul02,jln  Added PCI device ID for 82559 chip in I82845 (spr 79781) 
01c,29may02,pai  Added workaround for H/W errata relating to 82562 PHY
                 integrated in i82801BA/M ICH2.
01b,07nov01,pai  Updated documentation and routines for new device discovery
                 algorithm (SPR# 35716).
01a,11oct01,pai  Written from sysNetif.  Added dmh 8255x device discovery
                 (SPR# 29068).
*/
 
/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks
fei82557End (fei) END driver.  It has routines for initializing device
resources and provides BSP-specific fei82557End driver routines for any
Intel 82557, 82558, 82559, and 82562 fast Ethernet PCI bus controllers
found on the system.

The number of supported devices that can be configured for a particular
system is finite and is specified by the FEI_MAX_UNITS configuration
constant.  This value, and the internal data structures using it, can be
modified in this file for specific implementations.

SEE ALSO: muxLib, endLib, ifLib,

\tb "Intel 82557 User's Manual,"

\tb "Intel 82558 Fast Ethernet PCI Bus Controller with Integrated PHY,"

\tb "Intel 82559 Fast Ethernet Multifunction PCI/Cardbus Controller,"

\tb "Intel 82559ER Fast Ethernet PCI Controller,"

\tb "Intel PRO100B PCI Adapter Driver Technical Reference."

INTERNAL
The 8255x MII management interface allows the CPU control over the PHY unit
via a control regsiter in the 8255X.  This register, called the Management
Data Interface (MDI) Control Register, allows driver software to place the
PHY in specific modes and query the PHY unit for link status.  The structure
of the MDI Control Register is described in the following figure.

    +-----+--+--+-----+----------+----------+----------------------+
    |31 30|29|28|27 26|25      21|20      16|15                   0|
    +-----+--+--+-----+----------+----------+----------------------+
    | 0  0| I| R|  OP |  PHYADD  |  REGADD  |         DATA         |
    +-----+--+--+-----+----------+----------+----------------------+

Where:

    Bits     Name          
    -----------------------------
     0-15    Data
    16-20    PHY Register Address
    21-25    PHY Address
    26-27    Opcode
    28       Ready
    29       Interrupt Enable
    30-31    Reserved

In a write command, software places the data bits in the "Data" field,
and the 8255x shifts them out to the PHY unit.  In a read command the
8255x reads these bits serially from the PHY unit, and software can
read them from this location.  The "PHY Register Address" field holds
the PHY register address.  The "PHY Address" field holds the PHY address.
The "Opcode" field has valid values:

    01 - MDI write
    10 - MDI read

Any other values for the Opcode field are reserved.  The "Ready" field is
set to '1' by the 8255x at the end of an MDI transaction (for example, a
read or a write has been completed).  It should be reset to '0' by software
at the same time the command is written.  The "Interrupt Enable" field,
when set to '1' by software, will cause the 8255x to assert an interrupt
to indicate the end of an MDI cycle.  The "Reserved" field should always
be set to 00b.

This configuration module uses local routines sys557mdioRead() and
sys557mdioWrite() as an interface for reading and writing MDI data.  BSP
users should not be using these routines to adjust the PHY independent of
the driver and configuration routines contained herein.

ERRATA
From the Errata secion of Intel document number 298242-015, 'Intel 82801BA
I/O Controller Hub 2 (ICH2) and Intel 82801BAM I/O Controller Hub 2 Mobile
(ICH2-M) Specification Update':

    "30.  LAN Microcontroller PCI Protocol Violation

     Problem:

     When the ICH2/ICH2-M (using the 82562ET PLC) is receiving large files
     from a peer LAN device using the 10 Mbps data rate, the ICH2/ICH2-M
     can cause a system lock-up.  Specifically, if the LAN controller has
     Standby Enable set (EEPROM Word 0Ah bit-1 = 1), while receiving large
     files using the 10 Mbps data rate and receives a CU_RESUME command
     when it is just entering IDLE state, the ICH2/ICH2-M will cause a PCI
     protocol violation (typically by asserting FRAME# and IRDY# together)
     within the next few PCI cycles.  This will cause the PCI bus to
     lock-up, further resulting in system lock-up. 

     Implication:

     Large file transfers to the ICH2/ICH2-M using 10 Mpbs can cause the
     receiving system to lock-up. 

     Workaround:

     Clear EEPROM Word 0Ah bit-1 to 0.  This will result in an increase
     power consumption of the ICH2/ICH2-M of ~ 40 mW. 

     Status:

     There are no plans to fix this erratum."

The sys557Init() routine implements the specified workaround for this
errata.
*/


#if defined(INCLUDE_FEI_END)

/* includes */

#include <end.h>
#include <drv/end/fei82557End.h>


/* defines */

/* BSP specific FEI ethernet device type constants */

#define TYPE_PRO100B_PCI  (1)           /* Intel EtherExpress PRO-100B PCI */
#define TYPE_I82557_PCI   (2)           /* Intel 82557 - 82559 */
#define TYPE_I82559_PCI   (3)           /* Intel "InBusiness" model */
#define TYPE_I82559ER_PCI (4)           /* Intel 82559ER */
#define TYPE_I82562_PCI   (5)           /* Intel ICH2 integrated 82562 */
#define TYPE_I82562ET_PCI (6)           /* Intel 82562, PCI Revs 1 & 3 */

/* EEPROM control bits */

#define EE_SK             (0x01)        /* shift clock */
#define EE_CS             (0x02)        /* chip select */
#define EE_DI             (0x04)        /* chip data in */
#define EE_DO             (0x08)        /* chip data out */

/* EEPROM opcode */

#define EE_CMD_WRITE      (0x05)        /* WRITE opcode, 101 */
#define EE_CMD_READ       (0x06)        /* READ  opcode, 110 */
#define EE_CMD_ERASE      (0x07)        /* ERASE opcode, 111 */

/* EEPROM misc. defines */

#define EE_CMD_BITS       (3)           /* number of opcode bits */
#define EE_ADDR_BITS      (6)           /* number of address bits */
#define EE_DATA_BITS      (16)          /* number of data bits */
#define EE_SIZE           (0x40)        /* 0x40 WORDS */
#define EE_SIZE_BITS      (6)
#define EE_CHECKSUM       (0xbaba)      /* checksum */

/* Management Data Interface (MDI) Register */

#define MDI_OPC_READ      (0x08000000)  /* MDI Read command opcode */
#define MDI_OPC_WRITE     (0x04000000)  /* MDI Write command opcode */

/* form an MDI Read command */

#define MDI_COMMAND_RD(phyAddr, regAddr) \
    (MDI_OPC_READ | ((phyAddr) << 21) | ((regAddr) << 16))

/* form an MDI Write command */

#define MDI_COMMAND_WR(phyAddr, regAddr, val) \
    (MDI_OPC_WRITE | ((phyAddr) << 21) | ((regAddr) << 16) | (val))

/* test the MDI "Ready" field */

#define MDI_READY_SET(mdiReg)  ((mdiReg) & 0x10000000)

/* get the content of the MDI "Data" field */

#define MDI_DATA_GET(mdiReg)   ((UINT16)((UINT32)(mdiReg) & 0x0000ffff))


/* PCI Vendor IDs for NICs supported by fei82557End */

#define FEI_VENDORID_INTEL     (0x8086)      /* Intel PCI vendor ID */

#ifndef INTEL_PCI_VENDOR_ID
#define INTEL_PCI_VENDOR_ID    (0x8086)      /* Intel PCI vendor ID */
#endif /* INTEL_PCI_VENDOR_ID */

/* short list of PCI Device IDs for NICs supported by fei82557End */

#define FEI_DEVICEID_i82557        (0x1229)      /* 82557 - 82559 */
#define FEI_DEVICEID_i82559        (0x1030)      /* The "InBusiness" model */
#define FEI_DEVICEID_i82559ER      (0x1209)      /* 82559ER */
#define FEI_DEVICEID_i82562        (0x2449)      /* chipset integrated 82562 */
#define FEI_DEVICEID_i82559_I82845 (0x103a)      /* 82559 compatible chip in I82845 */  

/* typedefs */

typedef struct feiResource             /* FEI_RESOURCE */
    {
    UINT16           eeprom[EE_SIZE];  /* Ethernet Address of this unit */
    INT32            timeout;          /* timeout for the self-test */
    INT32            str[6];           /* storage for the self-test result */
    volatile INT32 * pResults;         /* pointer to the self-test result */
    BOOL             initDone;         /* driver has called sys557Init() */

    } FEI_RESOURCE;


/* locals */

LOCAL UINT32 feiUnits = 0;        /* number of FEIs we found */

/* This table defines board extended resources */

LOCAL FEI_RESOURCE feiResources [FEI_MAX_UNITS] =
    {
    {{NONE}, NONE, {NONE}, NULL, FALSE},
    {{NONE}, NONE, {NONE}, NULL, FALSE},
    {{NONE}, NONE, {NONE}, NULL, FALSE},
    {{NONE}, NONE, {NONE}, NULL, FALSE}
    };

/* This table defines board PCI resources */

LOCAL PCI_BOARD_RESOURCE feiPciResources [FEI_MAX_UNITS] =
    {
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE},
     (void * const)(&feiResources[0])
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE},
     (void * const)(&feiResources[1])
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE},
     (void * const)(&feiResources[2])
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE},
     (void * const)(&feiResources[3])
    }
    };

/* English descriptions of supported PHY devices */

LOCAL const char * phyNames [] = 
    {
    "None", "i82553-A/B", "i82553-C", "i82503",
    "DP83840", "80c240", "80c24", "i82555",
    "unknown-8", "unknown-9", "DP83840A", "unknown-11",
    "unknown-12", "unknown-13", "unknown-14", "unknown-15"
    };

enum phyChips 
    {
    NonSuchPhy=0, I82553AB, I82553C, I82503,
    DP83840, S80C240, S80C24, I82555, DP83840A=10, UndefinedPhy
    };

LOCAL const char * connectors [] = {" RJ45", " BNC", " AUI", " MII"};


/* forward declarations */

LOCAL UINT16  sys557eepromRead (int unit, int loc);
LOCAL void    sys557eepromWrite (int unit, int loc, UINT16 data);
LOCAL void    sys557eepromWriteBits (int unit, UINT16 data, int bitlen);
LOCAL void    sys557eepromChecksumSet (int unit);
LOCAL UINT16  sys557mdioRead   (int unit, int phyId, int loc);
LOCAL UINT16  sys557mdioWrite  (int unit, int phyId, int loc, UINT16 value);
LOCAL int     sys557IntEnable  (int unit);
LOCAL int     sys557IntDisable (int unit);
LOCAL int     sys557IntAck     (int unit);
LOCAL UINT32  sysFeiDevToType  (UINT32, UINT32, UINT8);

 
/* imports */

IMPORT FUNCPTR feiEndIntConnect;
IMPORT FUNCPTR feiEndIntDisconnect;



/*******************************************************************************
*
* sys557PciInit - initialize a 82557 PCI ethernet device
*
* This routine performs basic PCI initialization for FEI 82557 PCI ethernet
* devices supported by the fei82557End driver.  If supported, the device
* memory and I/O addresses are mapped into the local CPU address space
* and an internal board-specific resource table is updated with information
* on the board type, memory and I/O addresses.
*
* CAVEATS
* This routine must be called before the driver attempts to initialize itself
* and the physical device via sys557Init().  Also, this routine must be done
* prior to MMU initialization, usrMmuInit().
*
* The number of supported devices that can be configured for a particular
* system is finite and is specified by the FEI_MAX_UNITS configuration
* constant.
*
* RETURNS:
* OK, else ERROR when the specified device is not supported, or if
* the device could not be mapped into the local CPU memory space.
*/
STATUS sys557PciInit
    (
    UINT32  pciBus,      /* store a PCI bus number */
    UINT32  pciDevice,   /* store a PCI device number */
    UINT32  pciFunc,     /* store a PCI function number */
    UINT32  vendorId,    /* store a PCI vendor ID */
    UINT32  deviceId,    /* store a PCI device ID */
    UINT8   revisionId   /* store a PCI revision ID */
    )
    {
    UINT32  boardType;   /* store a BSP-specific board type constant */

    UINT32  memIo32;     /* memory-mapped IO address (BAR 0) */
    UINT32  ioBase;      /* IO base address (BAR 1) */
    UINT32  flash32;     /* optional flash memory base (BAR 2) */
    UINT8   irq;         /* interrupt line number (IRQ) for device */


    /* number of physical units exceeded the number supported ? */

    if (feiUnits >= FEI_MAX_UNITS)
        {
        return (ERROR);
        }

    if ((boardType = sysFeiDevToType (vendorId, deviceId, revisionId))
        == BOARD_TYPE_UNKNOWN)
        {
        return (ERROR);
        }


    pciConfigInLong  (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_0, &memIo32);
    pciConfigInLong  (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_1, &ioBase);
    pciConfigInLong  (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_2, &flash32);

    memIo32 &= PCI_MEMBASE_MASK;
    ioBase  &= PCI_IOBASE_MASK;
    flash32 &= PCI_MEMBASE_MASK;

    /* map a 4Kb 32-bit non-prefetchable memory IO address decoder */

    if (sysMmuMapAdd ((void *)(memIo32 & PCI_DEV_MMU_MSK),
        PCI_DEV_ADRS_SIZE, VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_PCI) == ERROR)
        {
        return (ERROR);
        }


    /* read the IRQ number and vector and save to the resource table */

    pciConfigInByte (pciBus, pciDevice, pciFunc,
                     PCI_CFG_DEV_INT_LINE, &irq);


    /* update the board-specific resource table */

    feiPciResources[feiUnits].bar[0]     = memIo32;
    feiPciResources[feiUnits].bar[1]     = ioBase;
    feiPciResources[feiUnits].bar[2]     = flash32;
    feiPciResources[feiUnits].irq        = irq;
    feiPciResources[feiUnits].irqvec     = INT_NUM_GET (irq);

    feiPciResources[feiUnits].vendorID   = vendorId;
    feiPciResources[feiUnits].deviceID   = deviceId;
    feiPciResources[feiUnits].revisionID = revisionId;
    feiPciResources[feiUnits].boardType  = boardType;

    /* the following support legacy interfaces and data structures */

    feiPciResources[feiUnits].pciBus     = pciBus;
    feiPciResources[feiUnits].pciDevice  = pciDevice;
    feiPciResources[feiUnits].pciFunc    = pciFunc;


    /* enable mapped memory and IO decoders */

    pciConfigOutWord (pciBus, pciDevice, pciFunc, PCI_CFG_COMMAND,
                      PCI_CMD_MEM_ENABLE | PCI_CMD_IO_ENABLE |
                      PCI_CMD_MASTER_ENABLE);

    /* disable sleep mode */

    pciConfigOutByte (pciBus, pciDevice, pciFunc, PCI_CFG_MODE,
                      SLEEP_MODE_DIS);


    ++feiUnits;  /* increment number of units initialized */

    return (OK);
    }

/*******************************************************************************
*
* sys557Init - prepare 82557 PCI ethernet device for initialization
*
* This routine is expected to perform any adapter-specific or target-specific
* initialization that must be done before an 82557 device is initialized
* by the driver.
*
* The 82557 drivers call this routine in the course of initializing driver
* internal structures and the associated underlying physical device.  The
* drivers will call this routine every time there is a requirement to
* [re]initialize the device.
*
* INTERNAL
* The fei82557End driver will call this function in the second pass of the
* xxxEndLoad() routine.
*
* RETURNS:
* OK, or ERROR if the device could not be prepared for initialization.
*/
STATUS sys557Init
    (
    int                  unit,    /* END driver unit number */
    FEI_BOARD_INFO *     pBoard   /* board information for the END driver */
    )
    {
    PCI_BOARD_RESOURCE * pRsrc;   /* point to a board PCI resource entry */
    FEI_RESOURCE *       pReso;   /* point to board extended resources */


    /* alias BOARD_INFO <enetAddr> for a more efficient data copy */

    UINT16 * const pBoardEnetAddr = (UINT16 * const)(pBoard->enetAddr);


    /* is there a PCI resource associated with this driver unit ? */

    if (unit >= feiUnits)
        {
        /* This is an error - no physical devs available to this unit */

        return ERROR;
        }

    /* associate the driver unit number with a PCI resource */

    pRsrc  = &feiPciResources [unit];
    pReso  = (FEI_RESOURCE *)(pRsrc->pExtended);


    /* perform one-time base initialization ? */

    if (pReso->initDone == FALSE)
        {
        /* read the configuration in EEPROM */

        UINT16  sum = 0;
        UINT16  value;
        int     ix;

        for (ix = 0; ix < EE_SIZE; ix++) 
            {
            value = sys557eepromRead (unit, ix);
            pReso->eeprom[ix] = value;
            sum += value;
            }

        if (sum != EE_CHECKSUM)
            {
            printf ("i8255x(%d): Invalid EEPROM checksum %#4.4x\n", unit, sum);
            }


        /* Handle Intel 82801BA/M ICH2 Errata ? */

        if (pRsrc->boardType == TYPE_I82562ET_PCI)
            {
            value = pReso->eeprom[0x0A];

            if (value & 0x02)
                {
                puts ("Found Intel 82562 with Standby Enable set.  "
                      "Fixing and rebooting ...");

                sys557eepromWrite (unit, 0x0A, (value & ~0x02));
                sys557eepromChecksumSet (unit);

                taskDelay (5 * sysClkRateGet ());
                sysToMonitor (BOOT_COLD);
                }
            }


        /* NS DP83840 Physical Layer Device specific setup */

        if (((pReso->eeprom[6] >> 8) & 0x3f) == DP83840)
            {
            UINT16 reg23 = sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 23);
            sys557mdioWrite (unit, pReso->eeprom[6] & 0x1f, 23, reg23 | 0x0420);
            }

        /* perform system self-test */

        pReso->timeout  = 16000;    /* self-test timeout */
        pReso->pResults = (volatile INT32 *)((((int) pReso->str) + 0xf) & ~0xf);

        pReso->pResults[0] = 0;
        pReso->pResults[1] = -1;

        sysOutLong ((pRsrc->bar[1]) + SCB_PORT, (int)(pReso->pResults) | 1);

        do  {
            sysDelay ();        /* delay for one IO read cycle */
            } while ((pReso->pResults[1] == -1)  &&  (--(pReso->timeout) >= 0));


        /* bind driver-specific PCI interrupt connection routines */

        feiEndIntConnect    = (FUNCPTR) pciIntConnect;
        feiEndIntDisconnect = (FUNCPTR) pciIntDisconnect;


        pReso->initDone = TRUE;
        }


    /* initialize the board information structure */

    /* pass along the PCI Device ID to support the fix for SPR# 69298 */

    pBoard->spare1    = pRsrc->deviceID;

    pBoard->vector    = INT_NUM_GET (pRsrc->irq);
    pBoard->baseAddr  = pRsrc->bar[0];

    /* copy the Ethernet address to the board information structure */

    pBoardEnetAddr[0] = pReso->eeprom[0];
    pBoardEnetAddr[1] = pReso->eeprom[1];
    pBoardEnetAddr[2] = pReso->eeprom[2];

    pBoard->intEnable     = sys557IntEnable;
    pBoard->intDisable    = sys557IntDisable;
    pBoard->intAck        = sys557IntAck;
    pBoard->sysLocalToBus = NULL; 
    pBoard->sysBusToLocal = NULL; 

#ifdef FEI_10MB 
    pBoard->phySpeed      = NULL;
    pBoard->phyDpx        = NULL;
#endif


    return (OK);
    }

/*******************************************************************************
*
* sys557IntAck - acknowledge an 82557 PCI ethernet device interrupt
*
* This routine performs any 82557 interrupt acknowledge that may be
* required.  This typically involves an operation to some interrupt
* control hardware.
*
* This routine gets called from the 82557 driver's interrupt handler.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if the interrupt could not be acknowledged.
*/
LOCAL STATUS sys557IntAck
    (
    int      unit                /* END driver unit number */
    )
    {
    return (OK);
    }

/*******************************************************************************
*
* sys557IntEnable - enable 82557 PCI ethernet device interrupts
*
* This routine enables 82557 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82557 driver calls this routine throughout normal operation to terminate
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be enabled.
*/
LOCAL STATUS sys557IntEnable
    (
    int    unit                /* END driver unit number */
    )
    {
    return ((unit >= feiUnits) ? ERROR :
           (sysIntEnablePIC (feiPciResources[unit].irq)));
    }

/*******************************************************************************
*
* sys557IntDisable - disable 82557 PCI ethernet device interrupts
*
* This routine disables 82557 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82557 driver calls this routine throughout normal operation to enter
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be disabled.
*/
LOCAL STATUS sys557IntDisable
    (
    int    unit                /* END driver unit number */
    )
    {
    return ((unit >= feiUnits) ? ERROR :
           (sysIntDisablePIC (feiPciResources[unit].irq)));
    }

/*******************************************************************************
*
* sys557eepromRead - read a word from 82557 PCI ethernet device EEPROM
*
* RETURNS: The EEPROM data word read in.
*
* NOMANUAL
*/
LOCAL UINT16 sys557eepromRead
    (
    int      unit,           /* END driver unit number */
    int      location        /* address of word to read */
    )
    {
    UINT16   iobase = (feiPciResources[unit].bar[1]);

    UINT16   retval = 0;
    UINT16   dataval;
    int      ix;

    volatile UINT16 dummy;

    /* enable EEPROM */

    sysOutWord (iobase + SCB_EEPROM, EE_CS);

    /* write the READ opcode */

    for (ix = EE_CMD_BITS - 1; ix >= 0; ix--)
        {
        dataval = (EE_CMD_READ & (1 << ix)) ? EE_DI : 0;
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval);
        sysDelay ();        /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval | EE_SK);
        sysDelay ();        /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval); 
        sysDelay ();        /* delay for one IO READ cycle */
        }

    /* write the location */

    for (ix = EE_ADDR_BITS - 1; ix >= 0; ix--)
        {
        dataval = (location & (1 << ix)) ? EE_DI : 0;
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval);
        sysDelay ();        /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval | EE_SK);
        sysDelay ();        /* delay for one IO READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval); 
        sysDelay ();        /* delay for one IO READ cycle */
        dummy = sysInWord (iobase + SCB_EEPROM);
        }

    if ((dummy & EE_DO) == 0)
        {
        ;  /* dummy read */
        }

    /* read the data */

    for (ix = EE_DATA_BITS - 1; ix >= 0; ix--)
        {
        sysOutWord (iobase + SCB_EEPROM, EE_CS | EE_SK);

        sysDelay ();        /* delay for one IO READ cycle */

        retval = (retval << 1) | 
                 ((sysInWord (iobase + SCB_EEPROM) & EE_DO) ? 1 : 0);

        sysOutWord (iobase + SCB_EEPROM, EE_CS);

        sysDelay ();        /* delay for one IO READ cycle */
        }
 
    /* disable EEPROM */

    sysOutWord (iobase + SCB_EEPROM, 0x00);

    return (retval);
    }

/*******************************************************************************
*
* sys557eepromWrite - write a word to 82557 PCI ethernet device EEPROM
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void sys557eepromWrite
    (
    int     unit,           /* END driver unit number */
    int     location,       /* address of word to write */
    UINT16  data            /* the data to write */
    )
    {
    UINT16  iobase = (feiPciResources[unit].bar[1]);

    int i;
    int j;
    
    /* Enable the EEPROM for writing */

    sysOutWord (iobase + SCB_EEPROM, FEI_EECS);

    sys557eepromWriteBits (unit, 0x4, 3);
    sys557eepromWriteBits (unit, 0x03 << (EE_SIZE_BITS - 2), EE_SIZE_BITS);

    sysOutWord (iobase + SCB_EEPROM, 0);
    sysDelay ();

    /* (1) Activite EEPROM by writing '1' to the EECS bit */

    sysOutWord (iobase + SCB_EEPROM, FEI_EECS);

    /* (2) Write the WRITE Opcode */

    sys557eepromWriteBits (unit, EE_CMD_WRITE, 3);

    /* (3) Write the Address field */

    sys557eepromWriteBits (unit, location, EE_SIZE_BITS);

    /* (4) Write the data field */

    sys557eepromWriteBits (unit, data, 16);

    /* (5) Deactivate the EEPROM */

    sysOutWord (iobase + SCB_EEPROM, 0);
    sysDelay ();

    /* delay */

    sysOutWord (iobase + SCB_EEPROM, FEI_EECS);
    sysDelay ();

    for (i = 0; i < 1000; ++i)
        {
        if (sysInWord (iobase + SCB_EEPROM) & FEI_EEDO)
            break;

        for (j = 0; j < 50; ++j)
            sysDelay ();
        }

    sysOutWord (iobase + SCB_EEPROM, 0);
    sysDelay ();

    /* Disable the write access */

    sysOutWord (iobase + SCB_EEPROM, FEI_EECS);

    sys557eepromWriteBits (unit, 0x4, 3);
    sys557eepromWriteBits (unit, 0, EE_SIZE_BITS);

    sysOutWord (iobase + SCB_EEPROM, 0);
    sysDelay ();
    }

/*******************************************************************************
*
* sys557eepromWriteBits - write bits to 82557 PCI ethernet device EEPROM
*
* This routine writes a specified <data> item of <bitlen> length-in-bits
* to the serial EEPROM of an Intel 8255x device associated with an
* END driver <unit> number.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void sys557eepromWriteBits
    (
    int     unit,           /* END driver unit number */
    UINT16  data,           /* the data to write */
    int     bitlen          /* the data length in bits */
    )
    {
    UINT16  iobase = (feiPciResources[unit].bar[1]);
    UINT16  reg;
    UINT16  mask;

    /* write the data, MSB first */

    for (mask = 1 << (bitlen - 1); mask; mask >>= 1) 
        {
        /* if mask and data then set the EEDI bit on */

        reg = ((data & mask) ? (FEI_EECS | FEI_EEDI) : (FEI_EECS));

        /* (a) write the bit to the EEDI bit */

        sysOutWord (iobase + SCB_EEPROM, reg);
        sysDelay();

        /* (b) write a '1' to the EESK bit then wait the minumum SK high time */

        sysOutWord (iobase + SCB_EEPROM, reg | FEI_EESK);
        sysDelay();

        /* (c) write a '0' to the EESK bit then wait the minimum SK low time */

        sysOutWord (iobase + SCB_EEPROM, reg);
        sysDelay();
        }
    }

/*******************************************************************************
*
* sys557eepromChecksumSet - set an Intel 825xx EEPROM checksum
*
* This routine computes and writes the checksum for the serial EEPROM
* on an Intel 82557, 82558, 82559, or 82562 device associated with the
* specified END driver <unit> number.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void sys557eepromChecksumSet
    (
    int    unit           /* END driver unit number */
    )
    {
    int    ix;
    UINT16 sum = 0;

    for (ix = 0; ix < EE_SIZE - 1; ix++) 
        {
        sum += sys557eepromRead (unit, ix);
        }
  
    sys557eepromWrite (unit, EE_SIZE - 1, (EE_CHECKSUM - sum));
    }

/*******************************************************************************
*
* sys557mdioRead - read from 82557 PCI ethernet Media Data Interface (MDI)
*
* RETURNS: The value read.
*
* NOMANUAL
*/
LOCAL UINT16 sys557mdioRead
    (
    int    unit,           /* END driver unit number */
    int    phyId,          /* PHY ID (PHY address) */
    int    location        /* location to read (PHY register address) */
    )
    {
    UINT16 iobase  = (feiPciResources[unit].bar[1]);

    int    timeout = (64 * 4);  /* < 64 usec. to complete, typ 27 ticks */
    int    mdi;

    /* send command to MDI register and poll for completion */

    sysOutLong (iobase + SCB_MDI, MDI_COMMAND_RD(phyId, location));

    do  {
        sysDelay ();        /* delay for one IO READ cycle */
        mdi = sysInLong (iobase + SCB_MDI);

        if (--timeout < 0)
            printf ("sys557mdioRead() timed out with MDI = %8.8x.\n", mdi);

        } while (!MDI_READY_SET(mdi));

    return MDI_DATA_GET(mdi);
    }

/*******************************************************************************
*
* sys557mdioWrite - write to 82557 PCI ethernet Media Data Interface (MDI)
*
* RETURNS: The value written.
*
* NOMANUAL
*/
LOCAL UINT16 sys557mdioWrite
    (
    int    unit,            /* END driver unit number */
    int    phyId,           /* PHY ID (PHY address ) */
    int    location,        /* location to write (PHY registe address) */
    UINT16 value            /* value to write */
    )
    {
    UINT16 iobase  = (feiPciResources[unit].bar[1]);

    int    timeout = (64 * 4);  /* < 64 usec. to complete, typ 27 ticks */
    int    mdi;

    /* send command to MDI register and poll for completion */

    sysOutLong (iobase + SCB_MDI, MDI_COMMAND_WR(phyId, location, value));

    do  {
        sysDelay ();        /* delay for one IO READ cycle */
        mdi = sysInLong (iobase + SCB_MDI);

        if (--timeout < 0)
            printf ("sys557mdioWrite() timed out with MDI = %8.8x.\n", mdi);

        } while (!MDI_READY_SET(mdi));

    return MDI_DATA_GET(mdi);
    }

/*******************************************************************************
*
* sys557Show - show 82557 PCI ethernet device configuration 
*
* This routine shows Intel 82557 and compatible device configuration.
*
* RETURNS: N/A
*/
void sys557Show
    (
    int        unit        /* END driver unit number */
    )
    {
    UINT16     ioBase;
    UINT32     memBase;
    UINT32     flashBase = NONE;

    UCHAR      etheraddr[6];

    int        ix;
    int        iy;

    PCI_BOARD_RESOURCE const * pRsrc;
    FEI_RESOURCE const *       pReso;


    if (unit >= feiUnits)
        {
        printf ("none\n");
        return;
        }


    pRsrc     = &feiPciResources [unit];
    pReso     = (FEI_RESOURCE *)(pRsrc->pExtended);

    ioBase    = (UINT16)(pRsrc->bar[1]);
    memBase   = (pRsrc->bar[0]);
    flashBase = (pRsrc->bar[2]);


    for (ix = 0, iy = 0; ix < 3; ix++) 
        {
        etheraddr[iy++] = pReso->eeprom[ix];
        etheraddr[iy++] = pReso->eeprom[ix] >> 8;
        }

    printf ("i8255x(%d): Intel EtherExpress Pro 10/100 at %#3x ", unit, ioBase);

    for (ix = 0; ix < 5; ix++)
        printf ("%2.2X:", etheraddr[ix]);

    printf ("%2.2X\n", etheraddr[ix]);
    printf ("CSR mem base address = %x, Flash mem base address = %x\n\n", 
            memBase, flashBase);

    printf ("PCI bus no. = %x, device no. = %x, function no. = %x, IRQ = %d\n",
            pRsrc->pciBus, pRsrc->pciDevice, pRsrc->pciFunc, pRsrc->irq);
    printf ("PCI Device ID = 0x%x\n\n", pRsrc->deviceID);

    if (pReso->eeprom[3] & 0x03)
        printf ("Receiver lock-up bug exists -- enabling work-around.\n");

    printf ("Board assembly %4.4x%2.2x-%3.3d, Physical connectors present:",
        pReso->eeprom[8], pReso->eeprom[9]>>8, pReso->eeprom[9] & 0xff);

    for (ix = 0; ix < 4; ix++)
        if (pReso->eeprom[5] & (1 << ix))
            printf ("%s", connectors [ix]);

    printf ("\nPrimary interface chip %s PHY #%d.\n",
        phyNames[(pReso->eeprom[6]>>8)&15], pReso->eeprom[6] & 0x1f);

    if (pReso->eeprom[7] & 0x0700)
        printf ("Secondary interface chip %s.\n", 
                phyNames[(pReso->eeprom[7]>>8)&7]);


    /* We do not show PHY specific register info at this time. */

#if  FALSE
    for (ix = 0; ix < 2; ix++)
        printf ("MDIO register %d is %4.4x.\n",
                ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
    for (ix = 5; ix < 7; ix++)
        printf ("MDIO register %d is %4.4x.\n",
                ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
    printf ("MDIO register %d is %4.4x.\n",
            25, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 25));
#endif  /* FALSE */


    if (pReso->timeout < 0) 
        {                /* Test optimized out. */
        printf ("Self test failed, status %8.8x:\n"
                " Failure to initialize the i8255x.\n"
                " Verify that the card is a bus-master capable slot.\n",
                pReso->pResults[1]);
        }
    else 
        {
        printf ("General self-test: %s.\n"
                " Serial sub-system self-test: %s.\n"
                " Internal registers self-test: %s.\n"
                " ROM checksum self-test: %s (%#8.8x).\n",
                pReso->pResults[1] & 0x1000 ? "failed" : "passed",
                pReso->pResults[1] & 0x0020 ? "failed" : "passed",
                pReso->pResults[1] & 0x0008 ? "failed" : "passed",
                pReso->pResults[1] & 0x0004 ? "failed" : "passed",
                pReso->pResults[0]);
        }
    }

/*******************************************************************************
*
* sysFeiDevToType - convert PCI Vendor and Device IDs to a device type
*
* Given <vendorId>, <deviceId>, and <revisionId> values read from PCI Vendor
* and Device ID registers in PCI configuration space, this routine will
* attempt to map the IDs to an 8255x device type value defined in this file.
*
* RETURNS:
* A board type value which will be one of
*
* .IP
* TYPE_I82557_PCI
* .IP
* TYPE_I82559_PCI
* .IP
* TYPE_I82559ER_PCI
* .IP
* TYPE_I82562_PCI
* .IP
* TYPE_I82562ET_PCI
* .LP
*
* NOTE
* The TYPE_I82562ET_PCI constant is used specifically to identify the
* Intel 82562ET LAN controller integrated in i82801BA (ICH2) and i82801BAM
* (ICH2-M) devices.  As noted in the Specification Update for these
* devices, document number 298242-015, the PCI Revsion ID values for the
* integrated LAN controller are 1 and 3.
*
* BOARD_TYPE_UNKNOWN will be returned if the Device ID does not map to
* a supported board type.
*
* NOMANUAL
*/
LOCAL UINT32 sysFeiDevToType
    (
    UINT32 vendorId,    /* specifies a PCI Vendor ID value */
    UINT32 deviceId,    /* specifies a PCI Device ID value */
    UINT8  revisionId   /* specifies a PCI Revision ID value */
    )
    {
    /* At the moment, we are only supporting vendor Intel */

    if (vendorId == FEI_VENDORID_INTEL)
        {
        switch (deviceId)
            {
            case FEI_DEVICEID_i82557:
                return (TYPE_I82557_PCI);

            case FEI_DEVICEID_i82559:
            case FEI_DEVICEID_i82559_I82845:  
                return (TYPE_I82559_PCI);

            case FEI_DEVICEID_i82559ER:
                return (TYPE_I82559ER_PCI);

            case FEI_DEVICEID_i82562:
                if ((revisionId == 1) || (revisionId == 3))
                    {
                    return (TYPE_I82562ET_PCI);
                    }

                return (TYPE_I82562_PCI);

            default:
                break;
            }
        }

    return (BOARD_TYPE_UNKNOWN);
    }

#endif /* INCLUDE_FEI_END */
