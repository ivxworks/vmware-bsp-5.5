/* sysNet.c - Intel Architecture BSP network susbsystem support */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,23apr02,pai  Made DEC and GEI END driver config names consistent with
                 other END driver config names.
01a,24oct01,pai  written.
*/

/*
DESCRIPTION
This library contains BSP-specific routines for network subsystems,
including network interface card device driver configuration and
support routines.  Generic network interface drivers are not provided
in source code form.  As a result, this module does not #include the
actual device drivers via the C-preprocessor.

Note that the routines implemented in this file will not necessarily
be included in future versions of Wind River Systems' Intel Architecture
BSPs.

SEE ALSO:
.pG "Configuration"
*/


/* includes */

#ifdef  INCLUDE_END
#   include "sysDec21x40End.c"          /* dec21x40End support routines */
#   include "sysEl3c90xEnd.c"           /* el3c90xEnd support routines  */
#   include "sysElt3c509End.c"          /* elt3c509End support routines */
#   include "sysFei82557End.c"          /* fei82557End support routines */
#   include "sysGei82543End.c"          /* gei82543End support routines */
#   include "sysLn97xEnd.c"             /* ln97xEnd support routines    */
#   include "sysNe2000End.c"            /* ne2000End support routines   */
#   include "sysUltraEnd.c"             /* ultraEnd support routines    */
#endif  /* INCLUDE_END */

#ifdef  INCLUDE_BSD
#   include "sysNetif.c"                /* if_eex32 support routines    */
#endif /* INCLUDE_BSD */


#if defined(INCLUDE_PCI)            /* PCI Ethernet Device Support Code */


/* typedefs */

typedef STATUS (* INIT_FUNCPTR) (UINT32, UINT32, UINT32, UINT32, UINT32, UINT8);

typedef struct vendIdDesc               /* VEND_ID_DESC */
    {
    UINT32              vendorId;       /* PCI Vendor ID */
    INIT_FUNCPTR        pPciInitRtn;    /* associated pciInit() routine */

    } VEND_ID_DESC;


/* locals */

/* This table maps END PCI driver initialization routines to
 * the PCI Vendor IDs for supported (tested) device types.
 */

LOCAL VEND_ID_DESC vendorIdEnet [] =
    {
    #if defined(INCLUDE_DEC21X40_END)
    {DEC_PCI_VENDOR_ID,      sysDec21x40PciInit},
    #endif /* INCLUDE_DEC21X40_END */

    #if defined(INCLUDE_LN_97X_END)
    {AMD_PCI_VENDOR_ID,      sysLan97xPciInit},
    #endif /* INCLUDE_LN_97X_END */

    #if defined(INCLUDE_EL_3C90X_END)
    {THREECOM_PCI_VENDOR_ID, sysEl3c90xPciInit},
    #endif /* INCLUDE_EL_3C90X_END */

    #if defined(INCLUDE_GEI8254X_END)
    {INTEL_PCI_VENDOR_ID,    sys543PciInit},
    #endif /* INCLUDE_GEI8254X_END */

    #if defined(INCLUDE_FEI_END)
    {INTEL_PCI_VENDOR_ID,    sys557PciInit},
    #endif /* INCLUDE_FEI_END */

    {0xffffffff, NULL}        /* last entry */
    };



/*******************************************************************************
*
* sysNetPciInit - initialize PCI network devices
*
* This routine invokes the driver or BSP-specific PCI initialization routine
* for a Network Class, Ethernet Subclass, device specified by the <pciBus>,
* <pciDevice>, and <pciFunc> parameters.
*
* The interface is constructed such that this function can be invoked
* via pciConfigForeachFunc().  Internally, this routine tests the 24-bit
* PCI class code value (class/subclass/prog_if) to ensure that the
* specified function is an ethernet controller.
*
* INTERNAL
* This callback will terminate pciConfigForeachFunc() if it returns ERROR.
* So, this routine returns OK always such that a failure to config one
* ethernet device will not prevent attempts to initialize other ethernet
* devices.
*
* RETURNS: OK always.
*
* NOMANUAL
*/
STATUS sysNetPciInit
    (
    UINT32              pciBus,        /* store a PCI bus number */
    UINT32              pciDevice,     /* store a PCI device number */
    UINT32              pciFunc,       /* store a PCI function number */
    void *              pArg           /* reserved argument */
    )
    {
    UINT32              classCodeReg;  /* store a 24-bit PCI class code */


    pciConfigInLong (pciBus, pciDevice, pciFunc, PCI_CFG_REVISION,
                     &classCodeReg);

    if (((classCodeReg >> 8) & 0x00ffffff) == PCI_NET_ETHERNET_CLASS)
        {
        UINT32        vendorId;    /* store a PCI vendor ID */
        UINT32        deviceId;    /* store a PCI device ID */
        UINT8         revisionId;  /* store a PCI revision ID */
        int           i;           /* index Vendor ID --> pciInit() table */


        /* get the PCI Vendor and Device IDs */

        pciConfigInLong (pciBus, pciDevice, pciFunc,
                         PCI_CFG_VENDOR_ID, &vendorId);

        /* get the PCI Revision ID */

        pciConfigInByte (pciBus, pciDevice, pciFunc,
                         PCI_CFG_REVISION, &revisionId);

        /* test for driver support of the specified Ethernet function */

        deviceId = ((vendorId >> 16) & 0x0000ffff);
        vendorId = (vendorId & 0x0000ffff);

        /* find & exec. a PCI initialization routine for the device */

        for (i = 0; (vendorIdEnet[i].pPciInitRtn) != NULL; ++i)
            {
            if ((vendorId == vendorIdEnet[i].vendorId) &&
                (*(vendorIdEnet[i].pPciInitRtn))
                (pciBus, pciDevice, pciFunc,
                 vendorId, deviceId, revisionId) == OK)
                {
                /* initialized the device - move on */

                return OK;
                }
            }
        }

    return (OK);
    }

#endif /* INCLUDE_PCI */


#if (defined(INCLUDE_DEC21X40_END) || defined(INCLUDE_ENE_END))

/*******************************************************************************
*
* sysLanIntEnable - enable 21x4x or Ne2000 ethernet device interrupts
*
* This routine enables DEC 21x4x or Ne2000 interrupts.  This may involve
* operations on interrupt control hardware.
*
* NOTE
* This is a legacy END driver support routine.  Early END driver
* implementations tended to use identical names for external support
* routines defined in the BSP.  As a result, a BSP configuration that
* selects such drivers for a build will fail with multiple definitions
* if the routine is defined separately for each driver.  Such namespace
* collisions will be resolved in future versions of the Wind River END
* drivers.
*
* RETURNS: Ok or ERROR for invalid arguments.
*/
STATUS sysLanIntEnable
    (
    int level        /* interrupt level to enable */
    )
    {
    return (sysIntEnablePIC (level));
    }

/*******************************************************************************
*
* sysLanIntDisable - disable 21x4x or Ne2000 ethernet device interrupts
*
* This routine disables DEC 21x4x or Ne2000 interrupts.  This may involve
* operations on interrupt control hardware.
*
* NOTE
* This is a legacy END driver support routine.  Early END driver
* implementations tended to use identical names for external support
* routines defined in the BSP.  As a result, a BSP configuration that
* selects such drivers for a build will fail with multiple definitions
* if the routine is defined separately for each driver.  Such namespace
* collisions will be resolved in future versions of the Wind River END
* drivers.
*
* RETURNS: OK or ERROR for invalid arguments.
*/
STATUS sysLanIntDisable
    (
    int level        /* interrupt level to disable */
    )
    {
    return (sysIntDisablePIC (level));
    }

#endif /* INCLUDE_DEC21X40_END || INCLUDE_ENE_END */

