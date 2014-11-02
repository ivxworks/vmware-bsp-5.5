/* sysLn97xEnd.c - system configuration module for AMD 79C97x END driver */
 
/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,11oct01,pai  Now using VM_STATE_MASK_FOR_ALL and VM_STATE_FOR_PCI in
                 sysMmuMapAdd call.  Updated documentation and routines
                 for new device discovery algorithm (SPR# 35716).
01c,09oct01,pai  Corrected variable usage in sysLn97xEndLoad and
                 sysLan97xPciInit.  Conditionally compile PCI_DEV_MMU_MSK
                 and PCI_DEV_ADRS_SIZE.
01b,05oct01,pai  Removed inaccurate commentary in sysLan97xPciInit().
01a,02oct01,pai  Written from sysNetif.  Implemented a new BSP driver load
                 routine that processes multiple physical devices and END
                 units.  This replaces sysLan97xInitStrCook (SPR #35716).
*/

/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks
ln97xEnd (lnPci) END driver.  It initializes device resources and
provides BSP-specific ln97xEnd driver routines for any Am79C970A,
Am79C971, Am79C972, and Am79C973 PCnet-PCI ethernet devices found on
the system.

The number of supported devices that can be configured for a particular
system is finite and is specified by the LN97X_MAX_DEV configuration
constant in this file.  This value, and the internal data structures
using it, can be modified in this file for specific implementations.
*/


#if defined(INCLUDE_LN_97X_END)

/* namespace collisions */

#undef CSR  /* redefined in ln97xEnd.h (temporary fix) */


/* includes */

#include "end.h"
#include "ln97xEnd.h"			/* modify by frank */


/* defines */

/* specify the maximum number of physical devices to configure */

#define LN97X_MAX_DEV           (8)

/* AMD 79C97x 10/100Base-TX PCI Ethernet Board Types */

#define AMD_PCI_VENDOR_ID       (0x1022)  /* AMD PCI vendor ID */
#define LN97X_PCI_VENDOR_ID     (0x1022)  /* AMD PCI vendor ID */
#define LN97X_PCI_DEVICE_ID     (0x2000)  /* Am79c97x device ID */

#define LN970_PCI_REV_MASK      (0x10)    /* Am79c970A PCI rev mask */
#define LN971_PCI_REV_MASK      (0x20)    /* Am79c971  PCI rev mask */
#define LN972_PCI_REV_MASK      (0x30)    /* Am79c972  PCI rev mask */
#define LN973_PCI_REV_MASK      (0x40)    /* Am79c973  PCI rev mask */

/* LN_TYPE_97x values are equivalent to (LN97x_PCI_REV_MASK >> 4) */

#define LN_TYPE_970             (1)       /* Am97c970A PCNet-PCI II */
#define LN_TYPE_971             (2)       /* Am97c971  PCNet-Fast   */
#define LN_TYPE_972             (3)       /* Am97c972  PCNet-Fast+  */
#define LN_TYPE_973             (4)       /* Am97c973  PCNet-Fast III */

/* driver configuration flags */

/* The <offset> parameter specifies the offset from which a packet has
 * to be loaded from the beginning of a device buffer.  Normally, this
 * parameter is zero except for architectures which access long words
 * only on aligned addresses.  For these architectures, the value of
 * this <offset> should be 2.
 */

#define LN97X_OFFS_VALUE        (0)       /* driver <offset> value */
#define LN97X_CSR3_VALUE        (0)       /* CSR3 register value */
#define LN97X_RSVD_FLAGS        (0)       /* driver <flags> value */


/* imports */

IMPORT STATUS    sysMmuMapAdd (void * address, UINT len,
                               UINT initialStateMask,
                               UINT initialState);

IMPORT END_OBJ * ln97xEndLoad (char *);


/* locals */
 
LOCAL int ln97XUnits = 0;  /* the number of physical units found */

/* This string table stores English descriptions of supported devices.
 * LN_TYPE_97x values index the table to obtain board descriptions.
 */

LOCAL const char * ln97xStrDesc [] =
    {
    "AMD 79C97x PCI Enhanced Network Driver",
    "AMD Am79C970A PCnet-PCI II Enhanced Network Driver",
    "AMD Am79C971 PCnet-FAST Enhanced Network Driver",
    "AMD Am79C972 PCnet-FAST+ Enhanced Network Driver",
    "AMD Am79C973 PCnet-FAST III Enhanced Network Driver"
    };

/*
 * This array defines the board-specific PCI resources.  There is one
 * unique END unit associated with one unique physical device recorded
 * in this table.  The END unit number is equivalent to an index into
 * this table.
 */
 
LOCAL PCI_BOARD_RESOURCE ln97xPciResources [LN97X_MAX_DEV] =
    {
    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    },

    {NONE, NONE, NONE, AMD_PCI_VENDOR_ID, LN97X_PCI_DEVICE_ID,
     LN970_PCI_REV_MASK, LN_TYPE_970, NONE, NONE,
    {NONE, NONE, NONE, NONE, NONE, NONE}, NULL
    }
    };


/* forward declarations */

LOCAL UINT32 sysLn97xDev2Type (UINT32, UINT32, UINT8);



/******************************************************************************
*
* sysLn97xEndLoad - construct a load string and load an Am79C97x device
*
* This routine will be invoked by the MUX for the purpose of loading
* an Am79C97x (lnPci) device with initial parameters.  This routine is
* constructed as an interface wrapper for the driver load routine.
* Thus, the arguments and return values are consistent with any
* xxxEndLoad() routine defined for an END driver and the MUX API.
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
* The complete ln97xEnd driver load string has format:
*
*     <unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecNum>:<intLvl>:
*     <memAdrs>:<memSize>:<memWidth>:<csr3b>:<offset>:<flags>
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <pParamStr> was NULL.
*
* SEE ALSO: ln97xEndLoad()
*/
END_OBJ * sysLn97xEndLoad
    (
    char *      pParamStr,   /* pointer to initialization parameter string */
    void *      unused       /* unused optional argument */
    )
    {
    END_OBJ *   pEnd;
    char        paramStr [END_INIT_STR_MAX];

    static const char * const paramTemplate = 
        "%d:0x%x:0x%x:0x%x:%d:%d:-1:-1:-1:0x%x:%d:0x%x:%p";


    /* alias local PCI and board resource table addresses */

    PCI_BOARD_RESOURCE * const pciRsrc = ln97xPciResources;


    if (strlen (pParamStr) == 0)
        {
        /* PASS (1)
         * The driver load routine returns the driver name in <pParamStr>.
         */

        pEnd = ln97xEndLoad (pParamStr);
        }
    else
        {
        /* PASS (2)
         * The END <unit> number is prepended to <pParamStr>.  Construct
         * the rest of the driver load string based on physical devices
         * discovered in sysLan97xPciInit().  When this routine is called
         * to process a particular END <unit> number, use the END <unit>
         * as an index into the PCI "resources" table to build the driver
         * parameter string.
         */

        int    typeIdx;   /* index to the string resource table */

        char * holder  = NULL;
        int    endUnit = atoi (strtok_r (pParamStr, ":", &holder));


        /* is there a PCI resource associated with this END unit ? */

        if (endUnit >= ln97XUnits)
            {
            /* This is an error - no physical devs available to this unit */

            return NULL;
            }


        /* construct an index into the string resource table */

        typeIdx = (pciRsrc[endUnit].boardType);


        /* construct the initialization parameter string */

        sprintf (paramStr, paramTemplate,
                 endUnit,                     /* END unit number */
                 NONE, /* pciRsrc[endUnit].bar[1],   modify by frank */  /* memory-mapped IO base */
                 pciRsrc[endUnit].bar[0],     /* IO address space base */
                 PCI2DRAM_BASE_ADRS,          /* host PCI mem. base */
                 pciRsrc[endUnit].irqvec,     /* IRQ vector */
                 pciRsrc[endUnit].irq,        /* IRQ number */
                 LN97X_CSR3_VALUE,            /* csr3 register value */
                 LN97X_OFFS_VALUE,            /* offset */
                 LN97X_RSVD_FLAGS,            /* flags (reserved) */
                 &ln97xStrDesc[typeIdx]       /* device description */
                );

        if ((pEnd = ln97xEndLoad (paramStr)) == (END_OBJ *) NULL)
            {
            printf ("Error sysLn97xEndLoad: device failed.\n");
            }
        }

    return (pEnd);
    }

/*******************************************************************************
*
* sysLn97xDev2Type - convert a PCI Revision ID to a board type
*
* Given <vendorId>, <deviceId>, and <revisionId> values read from PCI
* configuration space, this routine will attempt to map the ID to a board
* type value defined in this file.
*
* INTERNAL
* The PCI Vendor and Device IDs are the same for all driver supported
* Am79c97x devices.  When an instance is located, the PCI Revision ID is
* used to determine whether or not the driver can support the device.
*
* The shift and mask operations used to implement this routine are based
* upon the following table which documents the PCI Revison ID register
* values one can expect for the devices we are supporting:
*
* ---------------------------------------------------------------------------
*      PCI Configuration Space Offset: 0x08 PCI Revision ID Register
*
* 31               16  15                0
* xxxx xxxx xxxx xxxx  xxxx xxxx RRRR RRRR
* xxxx xxxx xxxx xxxx  xxxx xxxx 0000 xxxx 0x  Am79C970  PCnet-PCI
* xxxx xxxx xxxx xxxx  xxxx xxxx 0001 xxxx 1x  Am79C970A PCnet-PCI II
* xxxx xxxx xxxx xxxx  xxxx xxxx 0010 xxxx 2x  Am79C971  PCnet-FAST
* xxxx xxxx xxxx xxxx  xxxx xxxx 0011 xxxx 3x  Am79C972  PCnet-FAST+
* xxxx xxxx xxxx xxxx  xxxx xxxx 0100 xxxx 4x  Am79C973  PCnet-FAST III
* xxxx xxxx xxxx xxxx  xxxx xxxx 0100 xxxx 4x  Am79C975  PCnet-FAST III
* xxxx xxxx xxxx xxxx  xxxx xxxx 0101 xxxx 5x  Am79C978  PCnet-Home
* ---------------------------------------------------------------------------
*
* While the driver documentation is not specific on the matter, the
* revision ID is required because the driver supports Am79C970A
* (PCnet-PCI II), but not Am79C970 (PCnet-PCI).  Moreover, AMD's
* documentation indicates that Am79C973 devices and Am79C975 devices
* will be identified as the same revision.
*
* RETURNS:
* A board type value which will be one of
*
* .IP
* LN_TYPE_970
* .IP
* LN_TYPE_971
* .IP
* LN_TYPE_972
* .IP
* LN_TYPE_973
* .LP
*
* BOARD_TYPE_UNKNOWN will be returned if the Revision ID does not map to
* a supported board type.
*
* NOMANUAL
*/
LOCAL UINT32 sysLn97xDev2Type
    (
    UINT32 vendorId,    /* specifies a PCI vendor ID value */
    UINT32 deviceId,    /* specifies a PCI device ID value */
    UINT8  revisionId   /* specifies a PCI revision ID value */
    )
    {
    if ((vendorId == AMD_PCI_VENDOR_ID) &&
        (deviceId == LN97X_PCI_DEVICE_ID))
        {
        /* An AMD Am79C97x board was found.  Check the
         * Revision ID and make sure we support it.
         */

        if ((revisionId & LN970_PCI_REV_MASK) ||
            (revisionId & LN971_PCI_REV_MASK) ||
            (revisionId & LN972_PCI_REV_MASK) ||
            (revisionId & LN973_PCI_REV_MASK))
            {
            /* assume the shifted Revision ID is one of LN_TYPE_97x */

            return (UINT32)(revisionId >> 4);
            }
        }

    return (BOARD_TYPE_UNKNOWN);
    }

/*******************************************************************************
*
* sysLan97xPciInit - initialize a Am79C97x PCI ethernet device
*
*
* This routine performs basic PCI initialization for Am79C97x ethernet
* devices supported by the ln97xEnd END driver.  If supported,  the device
* memory and I/O addresses are mapped into the local CPU address space and
* an internal board-specific PCI resources table is updated with
* information on the board type, memory address, and IO address.
*
* CAVEATS
* This routine must be performed prior to MMU initialization, usrMmuInit().
* If the number of supported Am79c97x physical device instances installed
* on the PCI bus exceeds LN97X_MAX_DEV, then the extra devices will not be
* initialized in this routine.
*
* RETURNS:
* OK, else ERROR when the specified device is not supported, or if
* the device could not be mapped into the local CPU memory space.
*/
STATUS sysLan97xPciInit
    (
    UINT32  pciBus,           /* store a PCI bus number */
    UINT32  pciDevice,        /* store a PCI device number */
    UINT32  pciFunc,          /* store a PCI function number */
    UINT32  vendorId,         /* store a PCI vendor ID */
    UINT32  deviceId,         /* store a PCI device ID */
    UINT8   revisionId        /* store a PCI revision ID */
    )
    {
    UINT32  boardType;        /* store a BSP-specific board type constant */

    UINT32  ioBase;           /* IO base address (BAR 0) */
    UINT32  memIo32;          /* memory-mapped IO address (BAR 1) */
    UINT8   irq;              /* interrupt line number (IRQ) for device */


    /* number of physical units exceeded the number supported ? */

    if (ln97XUnits >= LN97X_MAX_DEV)
        {
        return (ERROR);
        }

    if ((boardType = sysLn97xDev2Type (vendorId, deviceId, revisionId))
        == BOARD_TYPE_UNKNOWN)
        {
        return (ERROR);
        }


    pciConfigInLong  (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_0, &ioBase);
    pciConfigInLong  (pciBus, pciDevice, pciFunc,
                      PCI_CFG_BASE_ADDRESS_1, &memIo32);

    memIo32 &= PCI_MEMBASE_MASK;
    ioBase  &= PCI_IOBASE_MASK;

    /* map a 4Kb 32-bit non-prefetchable memory address decoder */

    if (sysMmuMapAdd ((void *)(memIo32 & PCI_DEV_MMU_MSK),
        PCI_DEV_ADRS_SIZE, VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_PCI) == ERROR)
        {
        return (ERROR);
        }


    /* read the IRQ number and vector and save to the resource table */

    pciConfigInByte (pciBus, pciDevice, pciFunc,
                     PCI_CFG_DEV_INT_LINE, &irq);


    /* update the board-specific resource table */

    ln97xPciResources[ln97XUnits].bar[0]     = ioBase;
    ln97xPciResources[ln97XUnits].bar[1]     = memIo32;
    ln97xPciResources[ln97XUnits].irq        = irq;
    ln97xPciResources[ln97XUnits].irqvec     = INT_NUM_GET (irq);

    ln97xPciResources[ln97XUnits].vendorID   = vendorId;
    ln97xPciResources[ln97XUnits].deviceID   = deviceId;
    ln97xPciResources[ln97XUnits].revisionID = revisionId;
    ln97xPciResources[ln97XUnits].boardType  = boardType;

    /* enable mapped memory and IO decoders */

    pciConfigOutWord (pciBus, pciDevice, pciFunc, PCI_CFG_COMMAND,
                      PCI_CMD_MEM_ENABLE | PCI_CMD_IO_ENABLE |
                      PCI_CMD_MASTER_ENABLE);

    /* disable sleep mode */

    pciConfigOutByte (pciBus, pciDevice, pciFunc, PCI_CFG_MODE,
                      SLEEP_MODE_DIS);


    ++ln97XUnits;  /* increment number of units initialized */

    return (OK);
    }

/*******************************************************************************
*
* sysLan97xIntEnable - enable Am79C97x ethernet device interrupts
*
* This routine enables Am79C97x interrupts.  This may involve operations on
* interrupt control hardware.
*
* RETURNS: OK or ERROR for invalid arguments.
*/
STATUS sysLan97xIntEnable
    (
    int level           /* level number */
    )
    {
    return (sysIntEnablePIC (level));
    }

/*******************************************************************************
*
* sysLan97xIntDisable - disable Am79C97x ethernet device interrupts
*
* This routine disables Am79C97x interrupts.  This may involve operations on
* interrupt control hardware.
*
* RETURNS: OK or ERROR for invalid arguments.
*/
STATUS sysLan97xIntDisable
    (
    int level           /* level number */
    )
    {
    return (sysIntDisablePIC (level));
    }

/*******************************************************************************
*
* sysLan97xEnetAddrGet - get Am79C97x Ethernet (IEEE station) address
*
* This routine provides a target-specific interface for accessing an
* Am79C97x device Ethernet (IEEE station address) address in the device's
* Address PROM (APROM).  A handle to the specific device control structure
* is specified in the <pDrvCtrl> parameter.  The 6-byte IEEE station
* address will be copied to the memory location specified by the
* <enetAdrs> parameter.
*
* INTERNAL
* The 6 bytes of the IEEE station address occupy the first 6 locations of
* the Address PROM space.  The driver must copy the station address from
* the Address PROM space to the initialization block or to CSR12-14 in order
* for the receiver to accept unicast frames directed to the device.
*
* Bytes 14 and 15 of APROM should each be ASCII 'W' (57h).  The above
* requirements must be met in order to be compatible with AMD driver
* software.
*
* The APROM is 32 bytes, and there is no need to read all of that here.
* So, this routine reads half of the APROM to get the Ethernet address
* and test for ASCII 'W's.
*
* RETURNS: OK, or ERROR if could not be obtained.
*/
STATUS sysLan97xEnetAddrGet
    (
    LN_97X_DRV_CTRL *  pDrvCtrl,   /* Driver control */
    char *             enetAdrs
    )
    {
    char          aprom [LN_97X_APROM_SIZE];  /* copy of address PROM space */

    int           numBytes = (LN_97X_APROM_SIZE >> 1);
    register int  ix;


    /* get IO address of unit */

    UINT8 * const ioaddr = (UINT8 * const)(pDrvCtrl->devAdrs);


    /* load APROM into an array */

    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP)
        {
        for (ix = 0; ix < numBytes; ++ix)
            {
            aprom[ix] = *(ioaddr + ix);
            }
        }
    else
        {
        for (ix = 0; ix < numBytes; ++ix)
            {
            aprom[ix] = sysInByte ((int)(ioaddr + ix));
            }
        }

		/* modify by frankzhou to support in VMware */
#define	VXWORKS_RUN_ON_VMWARE
#ifndef	VXWORKS_RUN_ON_VMWARE
    /* check for ASCII 'W's at APROM bytes 14 and 15 */
    if ((aprom [0xe] != 'W') || (aprom [0xf] != 'W'))
        {
        logMsg ("sysLn97xEnetAddrGet:  W's not stored in aprom\n",
                0, 1, 2, 3, 4, 5);

        return ERROR;
        }
#endif
#ifdef	VXWORKS_RUN_ON_VMWARE
	aprom[0]=0x00;
	aprom[1]=0x0c;
	aprom[2]=0x29;
	aprom[3]=0x5a;
	aprom[4]=0x23;
	aprom[5]=0xaf;
#endif
		/* end by frankzhou */
    bcopy (aprom, enetAdrs, 6);

    return (OK);
    }

#endif /* INCLUDE_LN_97X_END */
