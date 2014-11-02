/* sysEl3c90xEnd.c - system configuration module for el3c90xEnd driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,07mar02,pai  Specify pciIntConnect() as the default driver interrupt
                 connect routine (SPR# 73501).
01g,28nov01,pai  Added code to determine whether 32-bit NPF memory decoder is
                 implemented.
01f,18oct01,pai  Updated documentation and routines for new device discovery
                 algorithm (SPR# 35716).
01e,11oct01,bur  Added more device ids to el3c90xBrds[] for different 
		 versions of 3COM 3c90x network cards.
01d,01oct01,pai  Removed INCLUDE_MMU_BASIC conditional-compilation block.
                 Fixed function comment-headers.
01c,11sep01,hdn  replaced "irq + EXT_INTERRUPT_BASE" with INT_NUM_GET(irq)
01b,12mar99, jkf renamed boardResource and pciResources to dev specific.
01a,12mar99, cn  created.
*/

/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks
el3c90xEnd (elPci) END driver.  It has routines for initializing
device resources and provides BSP-specific el3c90xEnd driver routines
for 3Com EtherLink and Fast EtherLink PCI network interface cards.
 
The number of supported devices that can be configured for a particular
system is finite and is specified by the EL_3C90X_MAX_DEV configuration
constant found is this file.  This configuration constant, and the data
structures using it, can be modified in this file for specific
implementations.
*/


#if defined(INCLUDE_EL_3C90X_END)

/* includes */

#include "end.h"
#include "drv/end/el3c90xEnd.h"


/* defines */

/* specify the maximum number of physical devices to configure */

#define EL_3C90X_MAX_DEV        (8)

#define THREECOM_PCI_VENDOR_ID   (0x10b7)   /* 3COM PCI vendor ID */

/* BSP specific 3Com ethernet device type constants */

#define TYPE_BOOMERANG_10BT           (1)   /* 3COM 3c900-TPO */
#define TYPE_BOOMERANG_10BT_COMBO     (2)   /* 3COM 3c900-COMBO */
#define TYPE_BOOMERANG_10_100BT       (3)   /* 3COM 3c905-TX */
#define TYPE_BOOMERANG_100BT4         (4)   /* 3COM 3c905-T4 */
#define TYPE_CYCLONE_10BT             (5)   /* 3COM 3c900B-TPO */
#define TYPE_CYCLONE_10BT_COMBO       (6)   /* 3COM 3c900B-COMBO */
#define TYPE_CYCLONE_10_100BT         (7)   /* 3COM 3c905B-TX */
#define TYPE_CYCLONE_10_100BT4        (8)   /* 3COM 3c905B-T4 */
#define TYPE_CYCLONE_10_100FX         (9)   /* 3COM 3c980-TX */
#define TYPE_CYCLONE_10_100BT_SERV   (10)   /* 3COM 3c980-TX */
#define TYPE_CYCLONE_10FL            (11)   /* 3COM 3c900B-FL */
#define TYPE_CYCLONE_10_100_COMBO    (12)   /* 3COM 3c905B-COMBO */
#define TYPE_KRAKATOA_10BT_TPC       (13)   /* 3COM 3c900B-TPC */
#define TYPE_TORNADO_10_100BT        (14)   /* 3COM 3c920-TX */
#define TYPE_TORNADO_10_100BT_SERV   (15)   /* 3COM 3c980-TX */
#define TYPE_TORNADO_HOMECONNECT     (16)   /* 3COM Home Connect */
#define TYPE_HURRICANE_SOHO100TX     (17)   /* 3COM Soho */

#define EL_3C90X_END_FLAGS      (0)
#define EL_3C90X_BUFF_MTPLR     (NONE)


/* imports */

IMPORT STATUS     sysMmuMapAdd (void * address, UINT len,
                                UINT initialStateMask,
                                UINT initialState);

IMPORT END_OBJ *  el3c90xEndLoad (char *);
IMPORT FUNCPTR    el3c90xIntConnectRtn;


/* locals */

LOCAL UINT32 etherLinkUnits = 0;     /* the number of physical units found */

/* This string table stores English descriptions of supported devices.
 * TYPE_XXX device type constants index the table to get descriptions.
 */

LOCAL const char * elDescription [] =
    {
    "3COM 3c90X Fast Etherlink Endhanced Network Driver."
    "3COM 3c900-TPO Etherlink Endhanced Network Driver."
    "3COM 3c900-COMBO Etherlink Endhanced Network Driver."
    "3COM 3c905-TX Etherlink Endhanced Network Driver."
    "3COM 3c905-T4 Etherlink Endhanced Network Driver."
    "3COM 3c900B-TPO Etherlink Endhanced Network Driver."
    "3COM 3c900B-COMBO Etherlink Endhanced Network Driver."
    "3COM 3c905B-TX Etherlink Endhanced Network Driver."
    "3COM 3c905B-T4 Etherlink Endhanced Network Driver."
    "3COM 3c980-TX Etherlink Endhanced Network Driver."
    "3COM 3c980-TX Etherlink Endhanced Network Driver."
    "3COM 3c900B-FL Etherlink Endhanced Network Driver."
    "3COM 3c905B-COMBO Etherlink Endhanced Network Driver."
    "3COM 3c900B-TPC Etherlink Endhanced Network Driver."
    "3COM 3c920-TX Etherlink Endhanced Network Driver."
    "3COM 3c980-TX Etherlink Endhanced Network Driver."
    "3COM 3c90X Home Connect Etherlink Endhanced Network Driver."
    "3COM 3c90X Soho Etherlink Endhanced Network Driver."
    };

/*
 * This array defines the board-specific PCI resources.  Each table entry
 * stores this information for specific physical devices found on the system
 * bus.  There is a unique END unit associated with each unique physical
 * device recorded in this table.  The END unit number is equivalent to an
 * index into this table.
 */

LOCAL PCI_BOARD_RESOURCE elPciResources [EL_3C90X_MAX_DEV] =
    {
    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    }
    };


/* forward declarations */

LOCAL UINT32 sys3comDevToType (UINT32, UINT32, UINT8);
LOCAL UINT32 sys3comMmioGet (UINT32, UINT32, UINT32);



/*******************************************************************************
*
* sysEl3c90xPciInit - initialize a 3c90x PCI ethernet device
*
* This routine performs basic PCI initialization for 3c90x PCI ethernet
* devices supported by the el3c90xEnd END driver.  Parameters to this
* routine specify a PCI function, including PCI ID registers, to
* initialize.  If supported, the device memory and I/O addresses are
* mapped into the local CPU address space and an internal board-specific
* PCI resources table is updated with information on the board type,
* memory address, and IO address.
*
* CAVEATS
* This routine must be performed prior to MMU initialization, usrMmuInit().
* If the number of supported 3c90x physical device instances installed
* on the PCI bus exceeds EL_3C90X_MAX_DEV, then the extra devices will not be
* initialized in this routine.
*
* RETURNS:
* OK, or ERROR if the specified device is not supported, or if
* the device could not be mapped into the local CPU memory space.
*/
STATUS sysEl3c90xPciInit
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

    UINT32  ioBase;      /* IO base address (BAR 0) */
    UINT32  memIo32;     /* memory-mapped IO address (BAR 1) */
    UINT8   irq;         /* interrupt line number (IRQ) for device */


    /* number of physical units exceeded the number supported ? */

    if (etherLinkUnits >= EL_3C90X_MAX_DEV)
        {
        return (ERROR);
        }

    if ((boardType = sys3comDevToType (vendorId, deviceId, revisionId))
        == BOARD_TYPE_UNKNOWN)
        {
        return (ERROR);
        }


    pciConfigInLong  (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_0, &ioBase);

    ioBase &= PCI_IOBASE_MASK;


    /* supported 3Com devices may or may not implement memory mapped IO */

    if ((memIo32 = sys3comMmioGet (pciBus, pciDevice, pciFunc)) != NONE)
        {
        memIo32 &= PCI_MEMBASE_MASK;

        /* map a 4Kb 32-bit non-prefetchable memory address decoder */

        if (sysMmuMapAdd ((void *)(memIo32 & PCI_DEV_MMU_MSK),
           PCI_DEV_ADRS_SIZE, VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_PCI) == ERROR)
            {
            return (ERROR);
            }
        }


    /* get the interrupt line number (IRQ) for the device */

    pciConfigInByte (pciBus, pciDevice, pciFunc,
                     PCI_CFG_DEV_INT_LINE, &irq);


    /* update the board-specific resource table */

    elPciResources[etherLinkUnits].bar[0]     = ioBase;
    elPciResources[etherLinkUnits].bar[1]     = memIo32;
    elPciResources[etherLinkUnits].irq        = irq;
    elPciResources[etherLinkUnits].irqvec     = INT_NUM_GET (irq);

    elPciResources[etherLinkUnits].vendorID   = vendorId;
    elPciResources[etherLinkUnits].deviceID   = deviceId;
    elPciResources[etherLinkUnits].revisionID = revisionId;
    elPciResources[etherLinkUnits].boardType  = boardType;


    /* enable mapped memory and IO decoders */

    pciConfigOutWord (pciBus, pciDevice, pciFunc, PCI_CFG_COMMAND,
                      PCI_CMD_MEM_ENABLE | PCI_CMD_IO_ENABLE |
                      PCI_CMD_MASTER_ENABLE);

    /* disable sleep mode */

    pciConfigOutByte (pciBus, pciDevice, pciFunc, PCI_CFG_MODE,
                      SLEEP_MODE_DIS);


    ++etherLinkUnits;  /* increment number of units initialized */

    /* Bind the driver-specific PCI interrupt connection routine. */

    el3c90xIntConnectRtn = (FUNCPTR) pciIntConnect;

    return (OK);
    }

/*******************************************************************************
*
* sysEl3c90xIntEnable - enable 3c90x ethernet device interrupts
*
* This routine enables el3c90x interrupts.  This may involve operations on
* interrupt control hardware.
*
* RETURNS: OK or ERROR for invalid arguments.
*/
STATUS sysEl3c90xIntEnable
    (
    int    level        /* level number */
    )
    {
    return (sysIntEnablePIC (level));
    }

/*******************************************************************************
*
* sysEl3c90xIntDisable - disable 3c90x ethernet device interrupts
*
* This routine disables el3c90x interrupts.  This may involve operations on
* interrupt control hardware.
*
* RETURNS: OK or ERROR for invalid arguments.
*/
STATUS sysEl3c90xIntDisable
    (
    int    level        /* level number */
    )
    {
    return (sysIntDisablePIC (level));
    }

/******************************************************************************
*
* sysEl3c90xEndLoad - construct a load string and load an el3c90xEnd device
*
* This routine will be invoked by the MUX for the purpose of loading an
* el3c90xEnd (elPci) device with initial parameters.  This routine is
* constructed as an interface wrapper for the driver load routine.  Thus,
* the arguments and return values are consistent with any xxxEndLoad()
* routine defined for an END driver and the MUX API.
*
* INTERNAL
* The muxDevLoad() operation calls this routine twice.  A zero length
* <pParamStr> parameter string indicates that this is the first time
* through this routine.  The driver load routine should return the
* driver name in <pParamStr>.
*
* On the second pass though this routine, the initialization parameter
* string is constructed.  Note that on the second pass, the <pParamStr>
* consists of a colon-delimeted END device unit number and rudimentary
* initialization string (often empty) constructed from entries in the
* BSP END Device Table such that:
*
*     <pParamStr> = "<unit>:<default initialization string>"
*
* In the process of building the rest of <pParamStr>, the prepended unit
* number must be preserved and passed to the driver load routine.  The
* <default initialization string> portion mentioned above is discarded,
* but future versions of this routine may use it.
*
* The complete el3c90xEnd driver load string has format:
*
*     <unit>:<devMemAddr>:<devIoAddr>:<pciMemBase>:<vecnum>:<intLvl>:
*     <memAdrs>:<memSize>:<memWidth>:<flags>:<buffMultiplier>
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <pParamStr> was NULL.
*
* SEE ALSO: el3c90xEndLoad()
*/
END_OBJ * sysEl3c90xEndLoad
    (
    char *    pParamStr,   /* pointer to initialization parameter string */
    void *    unused       /* unused optional argument */
    )
    {
    END_OBJ * pEnd;
    char      paramStr [END_INIT_STR_MAX];

    static const char * const paramTemplate = 
        "%d:0x%x:0x%x:0x%x:%d:%d:-1:-1:-1:0x%x:0x%x:%p";

    /* point to 3c90x board resource table */

    PCI_BOARD_RESOURCE * const pRsrc = elPciResources;


    if (strlen (pParamStr) == 0)
        {
        /* PASS (1)
         * The driver load routine returns the driver name in <pParamStr>.
         */

        pEnd = el3c90xEndLoad (pParamStr);
        }
    else
        {
        /* PASS (2)
         * The END <unit> number is prepended to <pParamStr>.  Construct
         * the rest of the driver load string based on physical devices
         * discovered in sysEl3c90xPciInit().  When this routine is called
         * to process a particular END <unit> number, use the END <unit> as
         * an index into the PCI "resources" table to build the driver
         * parameter string.
         */

        int    typeIdx;   /* index to the string resource table */

        char * holder  = NULL;
        int    endUnit = atoi (strtok_r (pParamStr, ":", &holder));


        /* is there a PCI resource associated with this END unit ? */

        if (endUnit >= etherLinkUnits)
            {
            return NULL;
            }


        /* construct an index into the string resource table */

        typeIdx = (pRsrc[endUnit].boardType);


        /* finish off the initialization parameter string */

        sprintf (paramStr, paramTemplate,
                 endUnit,                   /* END unit number */
                 pRsrc[endUnit].bar[1],     /* memory-mapped IO base */
                 pRsrc[endUnit].bar[0],     /* IO address space base */
                 PCI2DRAM_BASE_ADRS,        /* host PCI mem. base */
                 pRsrc[endUnit].irqvec,     /* IRQ vector */
                 pRsrc[endUnit].irq,        /* IRQ number */
                 EL_3C90X_END_FLAGS,        /* flags for type */
                 EL_3C90X_BUFF_MTPLR,       /* buff alloc factor */
                 &elDescription[typeIdx]    /* device description */
                );

        if ((pEnd = el3c90xEndLoad (paramStr)) == (END_OBJ *) NULL)
            {
            printf ("Error el3c90xEndLoad:  failed to load driver.\n");
            }
        }

    return (pEnd);
    }

/*******************************************************************************
*
* sys3comDevToType - convert PCI Vendor and Device IDs to a device type
*
* Given <vendorId>, <deviceId>, and <revisionId> values read from PCI Vendor
* and Device ID registers in PCI configuration space, this routine will
* attempt to map the IDs to an 3c90x device type value defined in this file.
*
* RETURNS:
* A board type value which will be one of
*
* .IP
* TYPE_BOOMERANG_10BT
* .IP
* TYPE_BOOMERANG_10BT_COMBO
* .IP
* TYPE_BOOMERANG_10_100BT
* .IP
* TYPE_BOOMERANG_100BT4
* .IP
* TYPE_CYCLONE_10BT
* .IP
* TYPE_CYCLONE_10BT_COMBO
* .IP
* TYPE_CYCLONE_10_100BT
* .IP
* TYPE_CYCLONE_10_100BT4
* .IP
* TYPE_CYCLONE_10_100FX
* .IP
* TYPE_CYCLONE_10_100BT_SERV
* .IP
* TYPE_CYCLONE_10FL
* .IP
* TYPE_CYCLONE_10_100_COMBO
* .IP
* TYPE_KRAKATOA_10BT_TPC
* .IP
* TYPE_TORNADO_10_100BT
* .IP
* TYPE_TORNADO_10_100BT_SERV
* .IP
* TYPE_TORNADO_HOMECONNECT
* .IP
* TYPE_HURRICANE_SOHO100TX
* .LP
*
* BOARD_TYPE_UNKNOWN will be returned if the Device ID does not map to
* a supported board type.
*
* NOMANUAL
*/
LOCAL UINT32 sys3comDevToType
    (
    UINT32 vendorId,    /* specifies a PCI Vendor ID value */
    UINT32 deviceId,    /* specifies a PCI Device ID value */
    UINT8  revisionId   /* specifies a PCI Revision ID values */
    )
    {
    /* At the moment, we are only supporting vendor 3Com */

    if (vendorId == THREECOM_PCI_VENDOR_ID)
        {
        switch (deviceId)
            {
            case TC_DEVICEID_BOOMERANG_10BT:
                return TYPE_BOOMERANG_10BT;

            case TC_DEVICEID_BOOMERANG_10BT_COMBO:
                return TYPE_BOOMERANG_10BT_COMBO;

            case TC_DEVICEID_BOOMERANG_10_100BT:
                return TYPE_BOOMERANG_10_100BT;

            case TC_DEVICEID_BOOMERANG_100BT4:
                return TYPE_BOOMERANG_100BT4;

            case TC_DEVICEID_CYCLONE_10BT:
                return TYPE_CYCLONE_10BT;

            case TC_DEVICEID_CYCLONE_10BT_COMBO:
                return TYPE_CYCLONE_10BT_COMBO;

            case TC_DEVICEID_CYCLONE_10_100BT:
                return TYPE_CYCLONE_10_100BT;

            case TC_DEVICEID_CYCLONE_10_100BT4:
                return TYPE_CYCLONE_10_100BT4;

            case TC_DEVICEID_CYCLONE_10_100FX:
                return TYPE_CYCLONE_10_100FX;

            case TC_DEVICEID_CYCLONE_10_100BT_SERV:
                return TYPE_CYCLONE_10_100BT_SERV;

            case TC_DEVICEID_CYCLONE_10FL:
                return TYPE_CYCLONE_10FL;

            case TC_DEVICEID_CYCLONE_10_100_COMBO:
                return TYPE_CYCLONE_10_100_COMBO;

            case TC_DEVICEID_KRAKATOA_10BT_TPC:
                return TYPE_KRAKATOA_10BT_TPC;

            case TC_DEVICEID_TORNADO_10_100BT:
                return TYPE_TORNADO_10_100BT;

            case TC_DEVICEID_TORNADO_10_100BT_SERV:
                return TYPE_TORNADO_10_100BT_SERV;

            case TC_DEVICEID_TORNADO_HOMECONNECT:
                return TYPE_TORNADO_HOMECONNECT;

            case TC_DEVICEID_HURRICANE_SOHO100TX:
                return TYPE_HURRICANE_SOHO100TX;

            default:
                break;
            }
        }

    return (BOARD_TYPE_UNKNOWN);
    }

/*******************************************************************************
*
* sys3comMmioGet - get a 3Com EtherLink memory mapped IO decoder value
*
* This routine gets the memory mapped IO decoder, if any, for a 3Com
* EtherLink or Fast EtherLink PCI network controller specified by PCI
* bus, device, and function numbers.  Many of the WRS supported PCI
* ethernet devices support both memory mapped IO and IO address space
* decoders.  This assumption cannot be made in the case of supported
* 3Com EtherLink PCI devices.  If a memory mapped IO decoder is
* implemented, assume it is in BAR 1.
*
* RETURNS: The 32-bit memory decoder value read from BAR 1, else NONE.
*
* NOMANUAL
*/
LOCAL UINT32 sys3comMmioGet
    (
    UINT32  pciBus,         /* store a PCI bus number */
    UINT32  pciDevice,      /* store a PCI device number */
    UINT32  pciFunc         /* store a PCI function number */
    )
    {
    UINT16  cmdSave;        /* saves 16-bit PCI command word register */
    UINT32  barSave;        /* saves 32-bit PCI base address register 1 */
    UINT32  barRead;        /* memory decoder (if any) read from BAR 1 */

    UINT32  retVal = NONE;  /* assume the BAR is not implemented */


    /* disable PCI device memory decode */

    pciConfigInWord (pciBus, pciDevice, pciFunc,
                     PCI_CFG_COMMAND, &cmdSave);

    pciConfigOutWord (pciBus, pciDevice, pciFunc,
                      PCI_CFG_COMMAND, (cmdSave & (~PCI_CMD_MEM_ENABLE)));


    /* save the BAR and determine whether it specifies a memory decoder */

    pciConfigInLong (pciBus, pciDevice, pciFunc,
                     PCI_CFG_BASE_ADDRESS_1, &barSave);

    pciConfigOutLong (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_1, 0xffffffff);

    pciConfigInLong (pciBus, pciDevice, pciFunc,
                     PCI_CFG_BASE_ADDRESS_1, &barRead);


    /* this BAR specifies a memory decoder? */

    if (barRead != 0)
        {
        retVal = barSave;

        pciConfigOutLong (pciBus, pciDevice, pciFunc,
                          PCI_CFG_BASE_ADDRESS_1, barSave);
        }


    /* re-enable PCI device memory decode */

    pciConfigOutWord (pciBus, pciDevice, pciFunc,
                      PCI_CFG_COMMAND, cmdSave);


    return (retVal);
    }

#endif /* INCLUDE_EL_3C90X_END */
