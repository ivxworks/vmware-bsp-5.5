/* ln97xEnd.c - END style AMD Am79C97X PCnet-PCI Ethernet driver */

/* Copyright 1984-1998 Wind River Systems, Inc., and Cetia Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01c,08dec98,snk  made architecture independant, by teamF1 Inc.
		 tested with x86 & power Pc architectures.
01b,21sep98,dat  modified to stand alone
01a,01aug98,dmb  written from if_lnPci.c, ver 01e
*/

/*
DESCRIPTION  
This module implements the Advanced Micro Devices Am79C971
Am79C972 and Am79C973 PCnet-PCI Ethernet 32 bit network interface driver.

The PCnet-PCI ethernet controller is inherently little endian because
the chip is designed to operate on a PCI bus which is a little endian
bus. The software interface to the driver is divided into three parts.
The first part is the PCI configuration registers and their set up. 
This part is done at the BSP level in the various BSPs which use this
driver. The second and third part are dealt in the driver. The second
part of the interface comprises of the I/O control registers and their
programming. The third part of the interface comprises of the descriptors
and the buffers. 

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  To
achieve this, the driver must be given several target-specific parameters,
and some external support routines must be provided. These target-specific
values and the external support routines are described below.

This driver supports multiple units per CPU.  The driver can be
configured to support big-endian or little-endian architectures.  It
contains error recovery code to handle known device errata related to DMA
activity. 

Big endian processors can be connected to the PCI bus through some controllers
which take care of hardware byte swapping. In such cases all the registers 
which the chip DMA s to have to be swapped and written to, so that when the
hardware swaps the accesses, the chip would see them correctly. The chip still
has to be programmed to operated in little endian mode as it is on the PCI bus.
If the cpu board hardware automatically swaps all the accesses to and from the
PCI bus, then input and output byte stream need not be swapped. 

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The only external interface is the ln97xEndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

<unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecNum>:<intLvl>:<memAdrs>:
<memSize>:<memWidth>:<csr3b>:<offset>:<flags>

The ln97xEndLoad() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.

.IP <devMemAddr>
This parameter in the memory base address of the device registers in the
memory map of the CPU. It indicates to the driver where to find the
RDP register.
The LANCE presents two registers to the external interface, the RDP (register
data port) and RAP (register address port) registers.  This driver assumes 
that these two registers occupy two unique addresses in a memory space
that is directly accessible by the CPU executing this driver.  The driver
assumes that the RDP register is mapped at a lower address than the RAP
register; the RDP register is therefore derived from the "base address."
This parameter should be equal to NONE if memory map is not used.

.IP <devIoAddr>
This parameter in the IO base address of the device registers in the
IO map of some CPUs. It indicates to the driver where to find the RDP
register. If both <devIoAddr> and <devMemAddr> are given then the device
chooses <devMemAddr> which is a memory mapped register base address.
This parameter should be equal to NONE if IO map is not used.

.IP <pciMemBase>
This parameter is the base address of the CPU memory as seen from the
PCI bus. This parameter is zero for most intel architectures.

.IP <vecNum>
This parameter is the vector associated with the device interrupt.
This driver configures the LANCE device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the LANCE interrupt.

.IP <intLvl>
Some targets use additional interrupt controller devices to help organize
and service the various interrupt sources.  This driver avoids all
board-specific knowledge of such devices.  During the driver's
initialization, the external routine sysLan97xIntEnable() is called to
perform any board-specific operations required to allow the servicing of a
LANCE interrupt.  For a description of sysLan97xIntEnable(), see "External
Support Requirements" below.

.IP <memAdrs>
This parameter gives the driver the memory address to carve out its
buffers and data structures. If this parameter is specified to be
NONE then the driver allocates cache coherent memory for buffers
and descriptors from the system pool.
The LANCE device is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems
that directly share memory between the CPU and the LANCE.  It
assumes that this shared memory is directly available to it
without any arbitration or timing concerns.

.IP <memSize>
This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.IP <memWidth>
Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

.IP <csr3b>
The LANCE control register #3 determines the bus mode of the device,
allowing the support of big-endian and little-endian architectures.
This parameter, defined as "UINT32 lnCSR_3B", is the value that will
be placed into LANCE control register #3.  The default value supports
Motorola-type buses.  For information about changing this parameter, see 
the manual. Normally for devices on the PCI bus this should always be
little endian. This value is zero normally

.IP <offset>
This parameter specifies the offset from which the packet has to be
loaded from the begining of the device buffer. Normally this parameter is
zero except for architectures which access long words only on aligned
addresses. For these architectures the value of this offset should be 2.

.IP <flags>
This is parameter is used for future use, currently its value should be
zero.

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDrvCtrl, routine, arg)
    SYS_INT_DISCONNECT (pDrvCtrl, routine, arg)
    SYS_INT_ENABLE(pDrvCtrl)
    SYS_INT_DISABLE(pDrvCtrl)
    SYS_OUT_BYTE(pDrvCtrl, reg, data)
    SYS_IN_BYTE(pDrvCtrl, reg, data)
    SYS_OUT_WORD(pDrvCtrl, reg, data)
    SYS_IN_WORD(pDrvCtrl, reg, data)
    SYS_OUT_LONG(pDrvCtrl, reg, data)
    SYS_IN_LONG(pDrvCtrl, reg, data)
    SYS_ENET_ADDR_GET(pDrvCtrl, pAddress)    
    sysLan97xIntEnable(pDrvCtrl->intLevel) 
    sysLan97xIntDisable(pDrvCtrl->intLevel) 
    sysLan97xEnetAddrGet(pDrvCtrl, enetAdrs)
.CE

There are default values in the source code for these macros.  They presume
memory mapped accesses to the device registers and the normal intConnect(),
and intEnable() BSP functions.  The first argument to each is the device
controller structure. Thus, each has access back to all the device-specific
information.  Having the pointer in the macro facilitates the addition 
of new features to this driver.

The macros SYS_INT_CONNECT, SYS_INT_DISCONNECT, SYS_INT_ENABLE and
SYS_INT_DISABLE allow the driver to be customized for BSPs that use special
versions of these routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this is a dummy routine that
returns OK.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization.  It calls an
external board level routine sysLan97xIntEnable(). 

The macro SYS_INT_DISABLE is used to disable the interrupt level for the
end device.  It is called during stop.  It calls an
external board level routine sysLan97xIntDisable(). 

The macro SYS_ENET_ADDR_GET is used get the ethernet hardware of the
chip. This macro calls an external board level routine namely
sysLan97xEnetAddrGet() to get the ethernet address.

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 13288 bytes in text for a I80486 target
    - 64 bytes in the initialized data section (data)
    - 0 bytes in the uninitialized data section (BSS)

The driver allocates clusters of size 1520 bytes for receive frames and
and transmit frames.

INCLUDES:
end.h endLib.h etherMultiLib.h ln97xEnd.h

SEE ALSO: muxLib, endLib, netBufLib
.I "Writing and Enhanced Network Driver"
.I "Advanced Micro Devices PCnet-PCI Ethernet Controller for PCI."
*/

#include "vxWorks.h"
#include "wdLib.h"
#include "stdlib.h"
#include "taskLib.h"
#include "logLib.h"
#include "intLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "iv.h"
#include "memLib.h"
#include "semLib.h"
#include "cacheLib.h"
#include "sys/ioctl.h"
#include "etherLib.h"

#ifndef DOC             /* don't include when building documentation */
#include "net/mbuf.h"
#endif  /* DOC */

#include "net/protosw.h"
#include "sys/socket.h"
#include "errno.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "m2Lib.h"

#include "etherMultiLib.h"              /* multicast stuff. */
#include "end.h"                        /* Common END structures. */
#include "netBufLib.h"
#include "muxLib.h"

#undef END_MACROS

#include "endLib.h"
#include "lstLib.h"                     /* Needed to maintain protocol list. */
#include "ln97xEnd.h"			/* modify by frank */

/* local defines */

/*
 * If LN_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE is there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global lnKickStartTx below.
 */

#define LN_KICKSTART_TX TRUE

/* Cache macros */

#define LN_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define LN_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define LN_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/* memory to PCI address translation macros */

#define PCI_TO_MEM_PHYS(pciAdrs) \
    ((pciAdrs) - (pDrvCtrl->pciMemBase))
	
#define MEM_TO_PCI_PHYS(memAdrs) \
    ((memAdrs) + (pDrvCtrl->pciMemBase))
    
/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS sysIntConnect(); \
    *pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec), \
                             rtn, (int)arg); \
    }
#endif /*SYS_INT_CONNECT*/

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    *pResult = OK; /* HELP: need a real routine */ \
    }
#endif /*SYS_INT_DISCONNECT*/

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE() \
    { \
    IMPORT STATUS sysLan97xIntEnable(); \
    sysLan97xIntEnable (pDrvCtrl->ilevel); \
    }
#endif /* SYS_INT_ENABLE*/

/* Macro to disable the appropriate interrupt level */

#ifndef SYS_INT_DISABLE
#   define SYS_INT_DISABLE(pDrvCtrl) \
    { \
    IMPORT STATUS sysLan97xIntDisable (); \
    sysLan97xIntDisable (pDrvCtrl->ilevel); \
    }
#endif

#ifndef SYS_OUT_LONG
#define SYS_OUT_LONG(pDrvCtrl,addr,value) \
    { \
    *((ULONG *)(addr)) = (value); \
    }
#endif /* SYS_OUT_LONG */

#ifndef SYS_IN_LONG
#define SYS_IN_LONG(pDrvCtrl,addr,data) \
    { \
    ((data) = *((ULONG *)(addr))); \
    }
#endif /* SYS_IN_LONG */

#ifndef SYS_OUT_SHORT
#define SYS_OUT_SHORT(pDrvCtrl,addr,value) \
    { \
    *((USHORT *)(addr)) = (value); \
    }
#endif /* SYS_OUT_SHORT*/

#ifndef SYS_IN_SHORT
#define SYS_IN_SHORT(pDrvCtrl,addr,data) \
    { \
    ((data) = *((USHORT *)(addr))); \
    }      
#endif /* SYS_IN_SHORT*/

#ifndef SYS_OUT_BYTE
#define SYS_OUT_BYTE(pDrvCtrl,addr,value) \
    { \
    *((UCHAR *)(addr)) = (value); \
    }
#endif /* SYS_OUT_BYTE */

#ifndef SYS_IN_BYTE
#define SYS_IN_BYTE(pDrvCtrl,addr,data) \
    { \
    ((data) = *((UCHAR *)(addr))); \
    }
#endif /* SYS_IN_BYTE */

#ifndef SYS_ENET_ADDR_GET
#define SYS_ENET_ADDR_GET(pDrvCtrl, pAddress) \
    { \
    IMPORT STATUS sysLan97xEnetAddrGet (LN_97X_DRV_CTRL *pDrvCtrl, \
                                     char * enetAdrs); \
    sysLan97xEnetAddrGet (pDrvCtrl, pAddress); \
    }
#endif /* SYS_ENET_ADDR_GET */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd) \
                ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
                ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define END_FLAGS_ISSET(pEnd, setBits) \
            ((pEnd)->flags & (setBits))

/* externs */

IMPORT int endMultiLstCnt (END_OBJ *);

#ifdef DRV_DEBUG	/* if debugging driver */

int      ln97xDebug = DRV_DEBUG_LOAD | DRV_DEBUG_INT | DRV_DEBUG_TX;
NET_POOL pLan97xNetPool;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6) \
        if (ln97xDebug & FLG) \
            logMsg((char *)X0, (int)X1, (int)X2, (int)X3, (int)X4, \
		    (int)X5, (int)X6);

#define DRV_PRINT(FLG,X) \
        if (ln97xDebug & FLG) printf X;

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)

#endif /*DRV_DEBUG*/

/* locals */

LOCAL int lnTsize = LN_TMD_TLEN;    /* deflt xmit ring size as power of 2 */
LOCAL int lnRsize = LN_RMD_RLEN;    /* deflt recv ring size as power of 2 */

LOCAL BOOL lnKickStartTx = LN_KICKSTART_TX;

/* forward static functions */

LOCAL int 	ln97xReset (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void 	ln97xInt (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void 	ln97xHandleRecvInt (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS 	ln97xRecv (LN_97X_DRV_CTRL * pDrvCtrl, LN_RMD *rmd);
LOCAL LN_RMD *	ln97xFullRMDGet (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void 	ln97xCsrWrite (LN_97X_DRV_CTRL * pDrvCtrl, int reg,
                               UINT32 value);
LOCAL void 	ln97xRestart (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS 	ln97xRestartSetup (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void 	ln97xAddrFilterSet (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL UINT32 	ln97xCsrRead (LN_97X_DRV_CTRL * pDrvCtrl, int reg);
LOCAL void	ln97xBcrWrite (LN_97X_DRV_CTRL * pDrvCtrl, int reg,
                               UINT32 value);
LOCAL void 	ln97xTRingScrub (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void	ln97xConfig (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS 	ln97xMemInit (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL UINT32 	ln97xBcrRead (LN_97X_DRV_CTRL * pDrvCtrl, int reg);

/* END Specific interfaces. */

LOCAL STATUS    ln97xStart (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xStop (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xUnload (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL int       ln97xIoctl (LN_97X_DRV_CTRL * pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    ln97xSend (LN_97X_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    ln97xMCastAddrAdd (LN_97X_DRV_CTRL* pDrvCtrl, char * pAddress);
LOCAL STATUS    ln97xMCastAddrDel (LN_97X_DRV_CTRL * pDrvCtrl,
                                   char * pAddress);
LOCAL STATUS    ln97xMCastAddrGet (LN_97X_DRV_CTRL * pDrvCtrl,
                                    MULTI_TABLE * pTable);
LOCAL STATUS    ln97xPollSend (LN_97X_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    ln97xPollReceive (LN_97X_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    ln97xPollStart (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xPollStop (LN_97X_DRV_CTRL * pDrvCtrl);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS ln97xFuncTable =
    {
    (FUNCPTR) ln97xStart,               /* Function to start the device. */
    (FUNCPTR) ln97xStop,                /* Function to stop the device. */
    (FUNCPTR) ln97xUnload,		/* Unloading function for the driver. */
    (FUNCPTR) ln97xIoctl,               /* Ioctl function for the driver. */
    (FUNCPTR) ln97xSend,                /* Send function for the driver. */
    (FUNCPTR) ln97xMCastAddrAdd,        /* Multicast address add  */
    (FUNCPTR) ln97xMCastAddrDel,	/* Multicast address delete */
    (FUNCPTR) ln97xMCastAddrGet,	/* Multicast table retrieve */
    (FUNCPTR) ln97xPollSend,            /* Polling send function  */
    (FUNCPTR) ln97xPollReceive,		/* Polling receive function */
    endEtherAddressForm,       		/* Put address info into a packet.  */
    endEtherPacketDataGet,     		/* Get a pointer to packet data. */
    endEtherPacketAddrGet      		/* Get packet addresses. */
    };

/******************************************************************************
*
* ln97xEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecnum>:<intLvl>:<memAdrs>
* :<memSize>:<memWidth>:<csr3b>:<offset>:<flags>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "lnPci") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ * ln97xEndLoad
    (
    char * initString            /* string to be parse by the driver */
    )
    {
    LN_97X_DRV_CTRL *	pDrvCtrl;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading ln97x...debug @ 0X%X\n",
             (int)&ln97xDebug, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);

    if (initString [0] == NULL)
        {
        bcopy ((char *)LN_97X_DEV_NAME, initString, LN_97X_DEV_NAME_LEN);
	DRV_LOG (DRV_DEBUG_LOAD, "Returning string...\n", 1, 2, 3, 4, 5, 6);
        return ((END_OBJ *)OK);
        }

    DRV_LOG (DRV_DEBUG_LOAD, "lnstring: [%s]\n",
	    (int)initString, 2, 3, 4, 5, 6);

    /* allocate the device structure */

    pDrvCtrl = (LN_97X_DRV_CTRL *)calloc (sizeof (LN_97X_DRV_CTRL), 1);

    if (pDrvCtrl == NULL)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "DrvControl : 0x%X\n", (int)pDrvCtrl,
             2, 3, 4, 5, 6);

    /* parse the init string, filling in the device structure */

    if (ln97xInitParse (pDrvCtrl, initString) == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Parse failed ...\n", 1, 2, 3, 4, 5, 6);
        goto errorExit;
	}

    /* Have the BSP hand us our address. */

    SYS_ENET_ADDR_GET (pDrvCtrl, &(pDrvCtrl->enetAddr [0]));

    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl, LN_97X_DEV_NAME,
                      pDrvCtrl->unit, &ln97xFuncTable,
                      "AMD 79C970 Lance PCI Enhanced Network Driver") == ERROR
     || END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
                      LN_SPEED)
                    == ERROR)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "END init done ...\n", 1, 2, 3, 4, 5, 6);

    /* Perform memory allocation */

    if (ln97xMemInit (pDrvCtrl) == ERROR)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "Malloc done ...\n", 1, 2, 3, 4, 5, 6);

    /* Perform memory distribution and reset and reconfigure the device */

    if (ln97xRestartSetup (pDrvCtrl) == ERROR)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "Restart setup done ...\n", 1, 2, 3, 4, 5, 6);

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj,
                    IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
                    | IFF_MULTICAST | IFF_SIMPLEX);

    DRV_LOG (DRV_DEBUG_LOAD, "Done loading ln97x...\n", 1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->endObj);

errorExit:
    if (pDrvCtrl != NULL)
        free ((char *)pDrvCtrl);

    return ((END_OBJ *)NULL);
    }

/*******************************************************************************
*
* ln97xInitParse - parse the initialization string
*
* Parse the input string. This routine is called from ln97xEndLoad() which
* intializes some values in the driver control structure with the values
* passed in the intialization string.
*
* The initialization string format is:
* <unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecNum>:<intLvl>:<memAdrs>
* :<memSize>:<memWidth>:<csr3b>:<offset>:<flags>
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <devMemAddr>
* Device register base memory address
* .IP <devIoAddr>
* Device register base IO address
* .IP <pciMemBase>
* Base address of PCI memory space
* .IP <vecNum>
* Interrupt vector number.
* .IP <intLvl>
* Interrupt level.
* .IP <memAdrs>
* Memory pool address or NONE.
* .IP <memSize>
* Memory pool size or zero.
* .IP <memWidth>
* Memory system size, 1, 2, or 4 bytes (optional).
* .IP <CSR3>
* Value of CSR3 (for endian-ness mainly)
* .IP <offset>
* Offset of starting of data in the device buffers.
* .IP <flags>
* Device specific flags, for future use.
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/

STATUS ln97xInitParse
    (
    LN_97X_DRV_CTRL *	pDrvCtrl,	/* pointer to the control structure */
    char * 		initString	/* initialization string */
    )
    {
    char*       tok;
    char**      holder = NULL;
    UINT32      devMemAddr;
    UINT32      devIoAddr;

    DRV_LOG (DRV_DEBUG_LOAD, "Parse starting ...\n", 1, 2, 3, 4, 5, 6);

    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", holder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->unit = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "Unit : %d ...\n", pDrvCtrl->unit, 2, 3, 4, 5, 6);

    /* devAdrs address. */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    devMemAddr = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "devMemAddr : 0x%X ...\n", devMemAddr,
             2, 3, 4, 5, 6);

    /* devIoAddrs address */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    devIoAddr = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "devIoAddr : 0x%X ...\n", devIoAddr,
             2, 3, 4, 5, 6);

    /* always use memory mapped IO if provided, else use io map */
    
    if ((devMemAddr == NONE) && (devIoAddr == NONE))
        {
        DRV_LOG (DRV_DEBUG_LOAD, "No memory or IO base specified ...\n",
                 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }
    else if (devMemAddr != NONE)
        {
        pDrvCtrl->devAdrs = devMemAddr;
        pDrvCtrl->flags   |= LS_MODE_MEM_IO_MAP;
        }
    else
        pDrvCtrl->devAdrs = devIoAddr;
        
    /* PCI memory base address as seen from the CPU */
    
    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->pciMemBase = strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "Pci : 0x%X ...\n", pDrvCtrl->pciMemBase,
             2, 3, 4, 5, 6);

    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ivec = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "ivec : 0x%X ...\n", pDrvCtrl->ivec,
             2, 3, 4, 5, 6);
    /* Interrupt level. */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ilevel = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "ilevel : 0x%X ...\n", pDrvCtrl->ilevel,
             2, 3, 4, 5, 6);

    /* Caller supplied memory address. */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memAdrs = (char *)strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "memAdrs : 0x%X ...\n", (int)pDrvCtrl->memAdrs,
             2, 3, 4, 5, 6);

    /* Caller supplied memory size. */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memSize = strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "memSize : 0x%X ...\n", pDrvCtrl->memSize,
             2, 3, 4, 5, 6);

    /* Caller supplied memory width. */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memWidth = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "memWidth : 0x%X ...\n", pDrvCtrl->memWidth,
             2, 3, 4, 5, 6);

    /* CSR3B value */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->csr3B = strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "CSR3b : 0x%X ...\n", pDrvCtrl->csr3B,
             2, 3, 4, 5, 6);

    /* Caller supplied alignment offset. */
    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->offset = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "Offset : 0x%X ...\n", pDrvCtrl->offset,
             2, 3, 4, 5, 6);

    /* caller supplied flags */

    tok = strtok_r (NULL, ":", holder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->flags |= strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "flags : 0x%X ...\n", pDrvCtrl->flags,
             2, 3, 4, 5, 6);

    return OK;
    }

/*******************************************************************************
*
* ln97xMemInit - initialize memory for Lance chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln97xMemInit
    (
    LN_97X_DRV_CTRL * pDrvCtrl /* device to be initialized */
    )
    {
    UINT32      sz;             /* temporary size holder */
    int         ix;
    LN_RMD *    pRmd;
    void *      pTemp;
    char *      pTempBuf;

    /* Remember register addresses */

    pDrvCtrl->pRdp   = LN_97X_RDP;
    pDrvCtrl->pRap   = LN_97X_RAP;
    pDrvCtrl->pReset = LN_97X_RST;
    pDrvCtrl->pBdp   = LN_97X_BDP;

    /***** Establish size of shared memory region we require *****/

    if ((int) pDrvCtrl->memAdrs != NONE)  /* specified memory pool */
	{
        /*
         * With a specified memory pool we want to maximize
         * lnRsize and lnTsize
         */

        sz = (pDrvCtrl->memSize - (RMD_SIZ + TMD_SIZ + sizeof (LN_IB)))
               / ((2 * LN_BUFSIZ) + RMD_SIZ + TMD_SIZ);

        sz >>= 1;               /* adjust for roundoff */

        for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
            ;

        lnTsize = lnRsize;      /* lnTsize = lnRsize for convenience */
        }

    /* limit ring sizes to reasonable values */

    lnRsize = max (lnRsize, LN_RMD_MIN); /* 4 Rx buffers is reasonable min */
    lnRsize = min (lnRsize, LN_RMD_MAX); /* 128 Rx buffers is max for chip */
    lnTsize = max (lnTsize, LN_TMD_MIN); /* 4 Tx buffers is reasonable min */
    lnTsize = min (lnTsize, LN_TMD_MAX); /* 128 Tx buffers is max for chip */

    /* Add it all up */

    sz = (((1 << lnRsize) + 1) * RMD_SIZ) +
         (((1 << lnTsize) + 1) * TMD_SIZ) + IB_SIZ + 0x10;

    /***** Establish a region of shared memory *****/

    /* OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ((int) pDrvCtrl->memAdrs)
	{
	default :       /* caller provided memory */
	    if (pDrvCtrl->memSize < sz)     /* not enough space */
		{
		DRV_LOG (DRV_DEBUG_LOAD, "ln97x: not enough memory provided\n"
			 "ln97x: need %ul got %d\n",
			 sz, pDrvCtrl->memSize, 3, 4, 5, 6);
		return (NULL);
                }

	    /* set the beginning of pool */

	    pDrvCtrl->pShMem = pDrvCtrl->memAdrs;

	    /* assume pool is cache coherent, copy null structure */

	    pDrvCtrl->cacheFuncs = cacheNullFuncs;
	    DRV_LOG (DRV_DEBUG_LOAD, "Memory checks out\n", 1, 2, 3, 4, 5, 6);
	    break;

        case NONE :     /* get our own memory */

	    /* Because the structures that are shared between the device
	     * and the driver may share cache lines, the possibility exists
	     * that the driver could flush a cache line for a structure and
	     * wipe out an asynchronous change by the device to a neighboring
	     * structure. Therefore, this driver cannot operate with memory
	     * that is not write coherent.  We check for the availability of
	     * such memory here, and abort if the system did not give us what
	     * we need.
	     */

	    if (!CACHE_DMA_IS_WRITE_COHERENT ())
		{
		printf ( "ln97x: device requires cache coherent memory\n" );
		return (ERROR);
		}

	    pDrvCtrl->pShMem = (char *) cacheDmaMalloc (sz);

	    if ((int)pDrvCtrl->pShMem == NULL)
		{
		printf ( "ln97x: system memory unavailable\n" );
		return (ERROR);
		}

	    /* copy the DMA structure */

	    pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |         (sizeof (LN_IB))            |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | (1 << lnRsize) * sizeof (LN_RMD)|
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | (1 << lnTsize) * sizeof (LN_TMD)|
     *             |-------------------------------------|
     */

    /* Save some things */

    pDrvCtrl->memBase  = (char *)((UINT32)pDrvCtrl->pShMem & 0xff000000);

    if ((int) pDrvCtrl->memAdrs == NONE)
        pDrvCtrl->flags |= LS_MEM_ALLOC_FLAG;

    /* first let's clear memory */

    bzero ( (char *) pDrvCtrl->pShMem, (int) sz );

    /* setup Rx memory pointers */

    pDrvCtrl->pRring    = (LN_RMD *) ((int)pDrvCtrl->pShMem + IB_SIZ);
    pDrvCtrl->rringLen  = lnRsize;
    pDrvCtrl->rringSize = 1 << lnRsize;
    pDrvCtrl->rmdIndex  = 0;

    /* setup Tx memory pointers. */

    /* Note: +2 is to round up to alignment. */

    pDrvCtrl->pTring = (LN_TMD *) (int)(pDrvCtrl->pShMem + IB_SIZ +
                        ((1 << lnRsize) + 1) * RMD_SIZ + 0xf);
    pDrvCtrl->pTring = (LN_TMD *) (((int)pDrvCtrl->pTring + 0xf) & ~0xf);

    pDrvCtrl->tringSize = 1 << lnTsize;
    pDrvCtrl->tringLen  = lnTsize;
    pDrvCtrl->tmdIndex  = 0;
    pDrvCtrl->tmdIndexC = 0;

    /* Set up the structures to allow us to free data after sending it. */

    for (ix = 0; ix < pDrvCtrl->rringSize; ix++)
	{
	pDrvCtrl->freeRtn[ix] = NULL;
	pDrvCtrl->freeData[ix].arg1 = NULL;
	pDrvCtrl->freeData[ix].arg2 = NULL;
	}

    /* allocate pool structure for mblks, clBlk, and clusters */

    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
	return (ERROR);

#ifdef DRV_DEBUG
    pLan97xNetPool = pDrvCtrl->endObj.pNetPool;
#endif
    
    pDrvCtrl->clDesc.clNum    = pDrvCtrl->rringSize * 2;
    pDrvCtrl->mClCfg.clBlkNum = pDrvCtrl->clDesc.clNum;
    pDrvCtrl->mClCfg.mBlkNum  = pDrvCtrl->mClCfg.clBlkNum * 2;

    /* total memory size for mBlks and clBlks */
    
    pDrvCtrl->mClCfg.memSize =
        (pDrvCtrl->mClCfg.mBlkNum  *  (MSIZE + sizeof (long))) +
        (pDrvCtrl->mClCfg.clBlkNum * (CL_BLK_SZ + sizeof (long)));

    /* total memory for mBlks and clBlks */

    if ((pDrvCtrl->mClCfg.memArea =
         (char *) memalign (sizeof(long), pDrvCtrl->mClCfg.memSize)) == NULL)
        return (ERROR);

    /* total memory size for all clusters */

    pDrvCtrl->clDesc.clSize  = LN_BUFSIZ;
    pDrvCtrl->clDesc.memSize =
        (pDrvCtrl->clDesc.clNum * (pDrvCtrl->clDesc.clSize + 8)) + sizeof(int);

    /* Do we hand over our own memory? */

    if (pDrvCtrl->memAdrs != (char *)NONE)
        {
        pDrvCtrl->clDesc.memArea =
            (char *)(pDrvCtrl->pTring + pDrvCtrl->tringSize);
        }
    else
        {
        pDrvCtrl->clDesc.memArea = cacheDmaMalloc (pDrvCtrl->clDesc.memSize);
            
        if ((int)pDrvCtrl->clDesc.memArea == NULL)
            {
            DRV_LOG(DRV_DEBUG_LOAD,
                    "system memory unavailable\n", 1, 2, 3, 4, 5, 6);
            return (ERROR);
            }
        }

    /* initialize the device net pool */
    
    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &pDrvCtrl->mClCfg,
                     &pDrvCtrl->clDesc, 1, NULL) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Could not init buffering\n",
                 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }
    
    /* Store the cluster pool id as others need it later. */

    pDrvCtrl->pClPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                       LN_BUFSIZ, FALSE);

    /* Longword align rmd ring */

    pDrvCtrl->pRring = (LN_RMD *) (((int)pDrvCtrl->pRring + 0xf) & ~0xf);
    pDrvCtrl->pRring = (LN_RMD *) (((int)pDrvCtrl->pRring + 0xf) & ~0xf);
    pRmd = pDrvCtrl->pRring;

    DRV_LOG (DRV_DEBUG_LOAD, "Using %d RX buffers from 0x%X\n",
             pDrvCtrl->rringSize, (int)pRmd, 3, 4, 5, 6);

    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, pRmd++)
        {
        if ((pTempBuf = (char *)netClusterGet (pDrvCtrl->endObj.pNetPool,
                                               pDrvCtrl->pClPoolId)) == NULL)
            {
            DRV_LOG (DRV_DEBUG_LOAD, "Could not get a buffer\n",
                     1, 2, 3, 4, 5, 6);
            return (ERROR);
            }
        pTempBuf += pDrvCtrl->offset;
        LN_RMD_BUF_TO_ADDR (pRmd, pTemp, pTempBuf);
        }

    return OK;
    }

/*******************************************************************************
*
* ln97xStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS ln97xStart
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    STATUS 		result;

    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->txBlocked  = FALSE;

    SYS_INT_CONNECT (pDrvCtrl, ln97xInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
        return ERROR;

    DRV_LOG (DRV_DEBUG_LOAD, "Interrupt connected.\n", 1, 2, 3, 4, 5, 6);

    SYS_INT_ENABLE ();

    DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* ln97xInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void ln97xInt
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT32	   	stat;

    /* Read the device status register */

    stat = ln97xCsrRead (pDrvCtrl, 0);

    DRV_LOG (DRV_DEBUG_INT, "i=0x%x:\n", stat, 2, 3, 4, 5, 6);

    /* If false interrupt, return. */

    if (! (stat & CSR0_INTR))
        {
        DRV_LOG (DRV_DEBUG_INT, "False interrupt.\n", 1, 2, 3, 4, 5, 6);
        return;
        }

    /*
     * enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     * Writing back what was read clears all interrupts
     */

    ln97xCsrWrite (pDrvCtrl, 0, stat);

    /* Check for errors */

    if (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR))
	{
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

        if (stat & CSR0_BABL)
            {
            pDrvCtrl->lastError.errCode = END_ERR_WARN;
            pDrvCtrl->lastError.pMesg = "Babbling";
            netJobAdd ((FUNCPTR) muxError, (int) &pDrvCtrl->endObj,
                       (int) &pDrvCtrl->lastError,
                       0, 0, 0);
            DRV_LOG (DRV_DEBUG_INT, "Babbling\n", 1, 2, 3, 4, 5, 6);
            }
        if (stat & CSR0_MISS)
            {
            pDrvCtrl->lastError.errCode = END_ERR_WARN;
            pDrvCtrl->lastError.pMesg = "Missing";
            netJobAdd ((FUNCPTR) muxError, (int) &pDrvCtrl->endObj,
                       (int) &pDrvCtrl->lastError,
                       0, 0, 0);
            DRV_LOG (DRV_DEBUG_INT, "Missing\n", 1, 2, 3, 4, 5, 6);
            }

        /* restart chip on fatal error */

        if (stat & CSR0_MERR)        /* memory error */
	    {
            pDrvCtrl->lastError.errCode = END_ERR_RESET;
            pDrvCtrl->lastError.pMesg = "Memory error.";
            netJobAdd ((FUNCPTR)muxError, (int) &pDrvCtrl->endObj,
                       (int)&pDrvCtrl->lastError,
                       0, 0, 0);
            DRV_LOG (DRV_DEBUG_INT, "Memory error, restarting.\n",
                     1, 2, 3, 4, 5, 6);
            END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));
            ln97xRestart (pDrvCtrl);
            return;
	    }
	}

    /* Have netTask handle any input packets */

    if ((stat & CSR0_RINT) && (stat & CSR0_RXON))
	{
        if (!(pDrvCtrl->flags & LS_RCV_HANDLING_FLAG))
	    {
            pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;
            (void) netJobAdd ((FUNCPTR)ln97xHandleRecvInt, (int)pDrvCtrl,
                              0,0,0,0);
	    }
	}

    /*
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff
     */

    if (!(stat & CSR0_TINT))
        return;

    DRV_LOG (DRV_DEBUG_INT, "t ", 1, 2, 3, 4, 5, 6);

    if (!pDrvCtrl->txCleaning)
        {
        pDrvCtrl->txCleaning = TRUE;
        netJobAdd ((FUNCPTR)ln97xTRingScrub, (int) pDrvCtrl, 0, 0, 0, 0);
        }

    if (pDrvCtrl->txBlocked)    /* cause a restart */
        {
        pDrvCtrl->txBlocked = FALSE;
        netJobAdd ((FUNCPTR)muxTxRestart, (int) &pDrvCtrl->endObj, 0, 0, 0, 0);
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    return;
    }

/*******************************************************************************
*
* ln97xHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void ln97xHandleRecvInt
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    LN_RMD * 		pRmd = (LN_RMD *)NULL;

    do
        {
        pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;

        while ((pRmd = ln97xFullRMDGet (pDrvCtrl)) != (LN_RMD *)NULL) 
	    {
	    LN_CACHE_INVALIDATE (pRmd, RMD_SIZ);
            ln97xRecv (pDrvCtrl, pRmd);
	    };

        /*
         * There is a RACE right here.  The ISR could add a receive packet
         * and check the boolean below, and decide to exit.  Thus the
         * packet could be dropped if we don't double check before we
         * return.
         */

        pDrvCtrl->flags &= ~LS_RCV_HANDLING_FLAG;
	}
    while (ln97xFullRMDGet (pDrvCtrl) != NULL);
    /* this double check solves the RACE */
    }

/*******************************************************************************
*
* ln97xFullRMDGet - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/

LOCAL LN_RMD * ln97xFullRMDGet
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    LN_RMD *		pRmd;

    pRmd = pDrvCtrl->pRring + pDrvCtrl->rmdIndex;  /* form ptr to Rx desc */

    /* If receive buffer has been released to us, return it */

    if ((PCI_SWAP (pRmd->rBufRmd1) & RMD1_OWN) == 0)
        return (pRmd);
    else
        return ((LN_RMD *) NULL);
    }

/*******************************************************************************
*
* ln97xRecv - process the next incoming packet
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS ln97xRecv
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    LN_RMD *		pRmd
    )
    {
    int         	len;
    M_BLK_ID    	pMblk;
    char *       	pCluster;
    char *       	pNewCluster;
    char *       	pTemp;
    CL_BLK_ID   	pClBlk;
    UINT32		rmd1Tmp;

    /* Packet must be checked for errors, Read rmd1 once only */

    rmd1Tmp = PCI_SWAP (pRmd->rBufRmd1);

    DRV_LOG (DRV_DEBUG_TX, "Recv : rmd1 = %X index = %d\n", rmd1Tmp,
             pDrvCtrl->rmdIndex, 3, 4, 5, 6);

    /* If error flag OR if packet is not completely in one buffer */

    if  ((rmd1Tmp & RMD1_ERR) ||
          (rmd1Tmp & (RMD1_STP | RMD1_ENP)) != (RMD1_STP | RMD1_ENP))
	{
        DRV_LOG (DRV_DEBUG_RX, "RMD error!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;                          /* skip to clean up */
	}

    /* If we cannot get a buffer to loan then bail out. */

    pNewCluster = netClusterGet (pDrvCtrl->endObj.pNetPool,
                                 pDrvCtrl->pClPoolId);

    if (pNewCluster == NULL)
        {
        DRV_LOG (DRV_DEBUG_RX, "Cannot loan!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    if ((pClBlk = netClBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)) == NULL)
        {
        netClFree (pDrvCtrl->endObj.pNetPool, pNewCluster);
        DRV_LOG (DRV_DEBUG_RX, "Out of Cluster Blocks!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    /*
     * OK we've got a spare, let's get an M_BLK_ID and marry it to the
     * one in the ring.
     */

    if ((pMblk = mBlkGet(pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA))
        == NULL)
        {
        netClBlkFree (pDrvCtrl->endObj.pNetPool, pClBlk);
        netClFree (pDrvCtrl->endObj.pNetPool, pNewCluster);
        DRV_LOG (DRV_DEBUG_RX, "Out of M Blocks!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

    len = LN_PKT_LEN_GET (pRmd);	/* get packet length */

    LN_RMD_TO_ADDR (pRmd, pCluster);	/* Get pointer to packet */

    pCluster -= pDrvCtrl->offset;
    
    DRV_LOG (DRV_DEBUG_RX, "Packet @ 0x%X for %d bytes!\n", pCluster,
             len, 3, 4, 5, 6);

    /* Join the cluster to the MBlock */

    netClBlkJoin (pClBlk, pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    /* make the packet data coherent */

    LN_CACHE_INVALIDATE (pMblk->mBlkHdr.mData, len);

    pMblk->mBlkHdr.mData  += pDrvCtrl->offset;
    pMblk->mBlkHdr.mLen   = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    DRV_LOG (DRV_DEBUG_RX, "Calling upper layer!\n", 1, 2, 3, 4, 5, 6);

    /* Deal with memory alignment. */

    pNewCluster += pDrvCtrl->offset;

    /* Give receiver a new buffer */

    LN_RMD_BUF_TO_ADDR (pRmd, pTemp, pNewCluster);

    /* Call the upper layer's receive routine. */

    END_RCV_RTN_CALL(&pDrvCtrl->endObj, pMblk);

cleanRXD:
    /* clear status bits */

    LN_CLEAN_RXD (pRmd);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    return (OK);
    }

/*******************************************************************************
*
* lnSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln97xSend
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl,  /* device to be initialized */
    M_BLK_ID 		pMblk      /* data to send */
    )
    {
    LN_TMD *		pTmd;
    UINT32		ltmd1;
    void *       	pTemp;
    char *       	pBuf;
    char *       	pOrig;
    int         	level;
    int         	len = 0;

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    if (!(pDrvCtrl->flags & LS_POLLING))
        END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;

    DRV_LOG (DRV_DEBUG_TX, "Send : index %d tmd = 0x%X\n",
             pDrvCtrl->tmdIndex, (int)pTmd, 3, 4, 5, 6);

    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    ltmd1 = PCI_SWAP (pTmd->tBufTmd1);
    
    if ((ltmd1 & TMD1_OWN) ||
	(((pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1))
         == pDrvCtrl->tmdIndexC))
        {
        if (!(pDrvCtrl->flags & LS_POLLING))
            END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        /* Are we still on the first chunk? */

        DRV_LOG (DRV_DEBUG_TX, "Out of TMDs!\n", 1, 2, 3, 4, 5, 6);

        level = intLock ();

        pDrvCtrl->txBlocked = TRUE;

        intUnlock (level);
        return (END_ERR_BLOCK);
        }

    DRV_LOG (DRV_DEBUG_TX, "before cluster get %d %d\n",
             pDrvCtrl->endObj.pNetPool,
             pDrvCtrl->pClPoolId, 3, 4, 5, 6);

    pOrig = pBuf = netClusterGet (pDrvCtrl->endObj.pNetPool,
                                  pDrvCtrl->pClPoolId);

    DRV_LOG (DRV_DEBUG_TX, "after cluster get pBuf = 0x%X\n",
             (int)pBuf, 2, 3, 4, 5, 6);

    if (pBuf == NULL)
        {
        netMblkClChainFree(pMblk);
        return (ERROR);
        }

    pBuf += pDrvCtrl->offset;	/* take care of the alignment */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    netMblkClChainFree(pMblk);
    LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pBuf);
    len = max (len, ETHERSMALL);
    pTmd->tBufTmd2 = 0;         /* clear buffer error status */

    ltmd1 = TMD1_STP | TMD1_ENP | TMD1_CNST;
    ltmd1 |= (TMD1_BCNT_MSK & -len);
    pTmd->tBufTmd1 = PCI_SWAP (ltmd1);

    CACHE_PIPE_FLUSH ();
    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    ltmd1 |= TMD1_OWN;

    DRV_LOG (DRV_DEBUG_TX, "TMD1 = 0x%X\n", ltmd1, 2, 3, 4, 5, 6);
    
    /* write to actual register */

    pTmd->tBufTmd1 = PCI_SWAP (ltmd1);

    pDrvCtrl->freeRtn [pDrvCtrl->tmdIndex] = (FUNCPTR)netClFree;
    pDrvCtrl->freeData [pDrvCtrl->tmdIndex].arg1 = pDrvCtrl->endObj.pNetPool;
    pDrvCtrl->freeData [pDrvCtrl->tmdIndex].arg2 = pOrig;

    /* Advance our management index */

    pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    if (lnKickStartTx)
        {
        if (!(pDrvCtrl->flags & LS_POLLING))
            ln97xCsrWrite (pDrvCtrl, 0, (CSR0_INTMASK | CSR0_TDMD));
        else
            ln97xCsrWrite (pDrvCtrl, 0, CSR0_TDMD);
        }

    if (!(pDrvCtrl->flags & LS_POLLING))
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    return (OK);
    }

/*******************************************************************************
*
* ln97xIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS OK or ERROR value
*/

LOCAL int ln97xIoctl
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int 		cmd,	 /* ioctl command to execute */
    caddr_t 		data	 /* date to get or set */
    )
    {
    long 		value;
    int 		error = 0;

    switch (cmd)
        {
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->endObj),
		   END_HADDR_LEN (&pDrvCtrl->endObj));
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR (&pDrvCtrl->endObj), (char *)data,
		    END_HADDR_LEN (&pDrvCtrl->endObj));
            break;

        case EIOCSFLAGS:
	    value = (long)data;
            
	    if (value < 0)
		{
		value = -value;
		value--;		/* HELP: WHY ??? */
		END_FLAGS_CLR (&pDrvCtrl->endObj, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->endObj, value);
		}
	    ln97xConfig (pDrvCtrl);
            break;
        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET (&pDrvCtrl->endObj);
            break;

	case EIOCPOLLSTART:
	    error = ln97xPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    error = ln97xPollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->endObj.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;
        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = LN_MIN_FBUF;
            break;
        case EIOCGMWIDTH:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = pDrvCtrl->memWidth;
            break;
        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = 14;
            break;
        default:
            error = EINVAL;
        }
    return (error);
    }

/*******************************************************************************
*
* ln97xReset - hardware reset of chip (stop it)
*
* This routine is responsible for resetting the device and switching into
* 32 bit mode.
*
* RETURNS: OK/ERROR
*/

LOCAL int ln97xReset
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT32 		resetTmp;

    /* Enable 32 bit access by doing a 32 bit write */

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRdp, 0);

    ln97xCsrWrite (pDrvCtrl, CSR(0), CSR0_STOP);
    
    /* Generate a soft-reset of the controller */

    SYS_IN_LONG(pDrvCtrl, pDrvCtrl->pReset, resetTmp);

    /* This isn't a real test - it just stops the compiler ignoring the read */

    if (resetTmp == 0x12345678)
	return (ERROR);

    ln97xBcrWrite (pDrvCtrl, BCR(20), BCR20_SWSTYLE_PCNET);

    /* autoselect port tye - 10BT or AUI */
    ln97xBcrWrite (pDrvCtrl, BCR(2), BCR2_AUTO_SELECT); 

    /* read BCR */
    resetTmp = ln97xBcrRead (pDrvCtrl, BCR(20));

    return (OK);
    }

/*******************************************************************************
*
* ln97xCsrWrite - select and write a CSR register
*
* This routine selects a register to write, through the RAP register and
* then writes the CSR value to the RDP register.
*
* RETURNS: N/A
*/

LOCAL void ln97xCsrWrite
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int 		reg,	 /* CSR register to select */
    UINT32 		value	 /* CSR value to write */
    )
    {
    int 		level;

    level = intLock ();

    /* select CSR */

    reg &= 0xff;

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP (reg));

    CACHE_PIPE_FLUSH ();

    value &=0xffff;

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRdp, PCI_SWAP (value));

    CACHE_PIPE_FLUSH ();

    intUnlock (level);
    }

/*******************************************************************************
*
* ln97xCsrRead - select and read a CSR register
*
* This routine selects a register to read, through the RAP register and
* then reads the CSR value from the RDP register.
*
* RETURNS: N/A
*/

LOCAL UINT32 ln97xCsrRead
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int			reg  	 /* register to select */
    )
    {
    int 		level;
    UINT32 		result;
    
    level = intLock ();
    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP (reg));
    CACHE_PIPE_FLUSH ();
    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pRdp, result);
    intUnlock (level);

    return (PCI_SWAP (result) & 0x0000FFFF);
    }

/*******************************************************************************
*
* ln97xBcrWrite - select and write a BCR register
*
* This routine writes the bus configuration register. It first selects the
* BCR register to write through the RAP register and then it writes the value
* to the BDP register.
*
* RETURNS: N/A
*/

LOCAL void ln97xBcrWrite
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int 		reg,     /* BCR register to select */
    UINT32 		value    /* BCR value to write */
    )
    {
    int 		level;

    reg &= 0xff;

    value &=0xffff;

    level = intLock ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP(reg));
    CACHE_PIPE_FLUSH ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pBdp, PCI_SWAP(value));
    intUnlock (level);
    }

/*******************************************************************************
*
* ln97xBcrRead - select and read a BCR register
*
* This routine reads the bus configuration register. It first selects the
* BCR register to read through the RAP register and then it reads the value
* from the BDP register.
*
* RETURNS: N/A
*/

LOCAL UINT32 ln97xBcrRead
    (
    LN_97X_DRV_CTRL *	pDrvCtrl,		/* driver control */
    int		reg			/* register to select */
    )
    {
    int level;
    UINT32 result;
    
    level = intLock ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP (reg));

    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pBdp, result);

    intUnlock (level);

    return (PCI_SWAP (result) & 0x0000FFFF);
    }

/*******************************************************************************
*
* ln97xRestartSetup - setup memory descriptors and turn on chip
*
* This routine initializes all the shared memory structures and turns on
* the chip.
*
* RETURNS OK/ERROR
*/

LOCAL STATUS ln97xRestartSetup
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {  
    int         	rsize;	/* recv ring length */
    int         	tsize;	/* xmit ring length */
    LN_TMD *    	pTmd;

    /* reset the device */

    ln97xReset (pDrvCtrl);

    /* setup Rx buffer descriptors  - align on 000 boundary */

    rsize = pDrvCtrl->rringLen;

    /* 
     * setup Tx buffer descriptors  - 
     *      save unaligned location and align on 000 boundary 
     */

    pTmd = pDrvCtrl->pTring;
    tsize = pDrvCtrl->tringLen;

    /* setup the initialization block */

    pDrvCtrl->ib = (LN_IB *)pDrvCtrl->pShMem;
    
    DRV_LOG (DRV_DEBUG_LOAD, "Init block @ 0x%X\n",
             (int)(pDrvCtrl->ib), 2, 3, 4, 5, 6);

    bcopy ((char *) END_HADDR(&pDrvCtrl->endObj), pDrvCtrl->ib->lnIBPadr, 6);

    CACHE_PIPE_FLUSH ();

    /* point to Rx ring */

    LN_ADDR_TO_IB_RMD (pDrvCtrl->pRring, pDrvCtrl->ib, rsize);

    /* point to Tx ring */

    LN_ADDR_TO_IB_TMD (pDrvCtrl->pTring, pDrvCtrl->ib, tsize);

    DRV_LOG (DRV_DEBUG_LOAD, "Memory setup complete\n", 1, 2, 3, 4, 5, 6);

    /* reconfigure the device */

    ln97xConfig (pDrvCtrl);

    return (OK);
    }

/*******************************************************************************
*
* ln97xRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*
* RETURNS: N/A
*/

LOCAL void ln97xRestart
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    ln97xReset (pDrvCtrl);

    ln97xRestartSetup (pDrvCtrl);

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj,
                    IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
                    | IFF_MULTICAST);
    }

/******************************************************************************
*
* ln97xConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A
*/

LOCAL void ln97xConfig
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT16 		stat;
    void * 		pTemp;
    LN_RMD *		pRmd;
    LN_TMD *		pTmd;
    int 		ix;
    int 		timeoutCount = 0;

    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_PROMISC)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Setting promiscuous mode on!\n",
                 1, 2, 3, 4, 5, 6);

        /* chip will be in promiscuous mode */

        pDrvCtrl->ib->lnIBMode |= 0x00800000;
        }
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Setting promiscuous mode off!\n",
                 1, 2, 3, 4, 5, 6);
        }

    CACHE_PIPE_FLUSH ();

    ln97xCsrWrite (pDrvCtrl, 0, CSR0_STOP);     /* set the stop bit */

    /* Set up address filter for multicasting. */

    if (END_MULTI_LST_CNT (&pDrvCtrl->endObj) > 0)
        {
        ln97xAddrFilterSet (pDrvCtrl);
        }

    /* set the bus mode to little endian */

    ln97xCsrWrite (pDrvCtrl, 3, pDrvCtrl->csr3B);

    /* set the Bus Timeout to a long time */
    /* This allows other stuff to hog the bus a bit more */

    ln97xCsrWrite (pDrvCtrl, 100, BUS_LATENCY_COUNT  );

    pRmd = pDrvCtrl->pRring;

    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, pRmd++)
        {
        LN_CLEAN_RXD (pRmd);
        }
    pDrvCtrl->rmdIndex = 0;

    pTmd = pDrvCtrl->pTring;

    for (ix = 0; ix < pDrvCtrl->tringSize; ix++, pTmd++)
        {
        pTmd->tBufAddr = 0;                    /* no message byte count yet */
	pTemp = (void *)(TMD1_CNST | TMD1_ENP | TMD1_STP);
        pTmd->tBufTmd1 = (UINT32) PCI_SWAP (pTemp);
        LN_TMD_CLR_ERR (pTmd);
        }

    pDrvCtrl->tmdIndex = 0;
    pDrvCtrl->tmdIndexC = 0;

    /* Point the device to the initialization block */

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->ib);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));

    ln97xCsrWrite (pDrvCtrl, CSR(2), (((ULONG)pTemp >> 16) & 0x0000ffff));
    ln97xCsrWrite (pDrvCtrl, CSR(1), ((ULONG)pTemp & 0x0000ffff));
    ln97xCsrWrite (pDrvCtrl, CSR(0), CSR0_INIT);    /* init chip (read IB) */

    /* hang until Initialization DONe, ERRor, or timeout */

    while (((stat = ln97xCsrRead (pDrvCtrl, 0)) & (CSR0_IDON | CSR0_ERR)) == 0)
        {
        if (timeoutCount++ > 0x100)
            break;

        taskDelay (2 * timeoutCount);
        }

    DRV_LOG (DRV_DEBUG_LOAD, "Timeoutcount %d\n", timeoutCount,
             2, 3, 4, 5, 6);

    /* log chip initialization failure */

    if (((stat & CSR0_ERR) == CSR0_ERR) || (timeoutCount >= 0x10000))
        {
        DRV_LOG (DRV_DEBUG_LOAD, "%s: Device initialization failed\n",
                 (int)END_DEV_NAME(&pDrvCtrl->endObj), 0,0,0,0,0);
        return;
        }

   /* Setup LED controls */

    ln97xBcrWrite (pDrvCtrl, BCR(4), 0x0090);
    ln97xBcrWrite (pDrvCtrl, BCR(5), 0x0084);
    ln97xBcrWrite (pDrvCtrl, BCR(7), 0x0083);

    if (!(pDrvCtrl->flags & LS_POLLING))
        ln97xCsrWrite (pDrvCtrl, 0, CSR0_INTMASK | CSR0_STRT);
    else
        ln97xCsrWrite (pDrvCtrl, 0, CSR0_STRT);
    
    }

/******************************************************************************
*
* ln97xAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the ln97xAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A
*/

LOCAL void ln97xAddrFilterSet
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    ETHER_MULTI * 	pCurr;
    LN_IB *		pIb;
    UINT8 *		pCp;
    UINT8 		byte;
    UINT32 		crc;
    int 		len;
    int 		count;

    pIb = pDrvCtrl->ib;

    LN_ADDRF_CLEAR (pIb);

    pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);

    while (pCurr != NULL)
	{
	pCp = (UINT8 *)&pCurr->addr;

	crc = 0xffffffff;

	for (len = LN_LA_LEN; --len >= 0;)
	    {
	    byte = *pCp++;

            for (count = 0; count < LN_LAF_LEN; count++)
		{
		if ((byte & 0x01) ^ (crc & 0x01))
		    {
		    crc >>= 1;
		    crc = crc ^ LN_CRC_POLYNOMIAL;
		    }
		else
		    {
		    crc >>= 1;
		    }
		byte >>= 1;
		}
	    }

	/* Just want the 6 most significant bits. */

	crc = LN_CRC_TO_LAF_IX (crc);

	/* Turn on the corresponding bit in the filter. */

	LN_ADDRF_SET (pIb, crc);

	pCurr = END_MULTI_LST_NEXT(pCurr);
	}
    }

/*******************************************************************************
*
* ln97xPollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device. This routine return OK if it is successful in getting the packet
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS ln97xPollReceive
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    M_BLK_ID 		pMblk
    )
    {
    LN_RMD *		pRmd;
    UINT16 		stat;
    char * 		pPacket;
    int 		len;

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX b\n", 1, 2, 3, 4, 5, 6);

    /* Read the device status register */

    stat = ln97xCsrRead (pDrvCtrl, CSR(0));

    /* Check for errors */

    if (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR))
        {
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad error\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

        /* restart chip on fatal error */

        if (stat & CSR0_MERR)        /* memory error */
            {
	    END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));
            DRV_LOG (DRV_DEBUG_POLL_RX, "PRX restart\n", 1, 2, 3, 4, 5, 6);
            ln97xRestart (pDrvCtrl);
            }

	return (EAGAIN);
        }
    /* If no interrupt then return. */

    if (!(stat & CSR0_RINT))
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX no rint\n", 1, 2, 3, 4, 5, 6);
        return (EAGAIN);
	}

    /*
     * clear receive interrupts, and clear any errors that may be set.
     */

    ln97xCsrWrite (pDrvCtrl, 0, (stat & (CSR0_INTMASK | CSR0_STRT)));


    /* Packet must be checked for errors. */

    pRmd = ln97xFullRMDGet (pDrvCtrl);

    if (pRmd == NULL)
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX no rmd\n", 1, 2, 3, 4, 5, 6);
	return (EAGAIN);
	}

    if  (LN_RMD_ERR (pRmd))
	{
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad rmd\n", 1, 2, 3, 4, 5, 6);
	goto cleanRXD;
	}

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
    len = LN_PKT_LEN_GET (pRmd);		/* get packet length */

    /* Get pointer to packet */

    LN_RMD_TO_ADDR (pRmd, pPacket);
    
    LN_CACHE_INVALIDATE (pPacket, len);   /* make the packet coherent */

    /* Upper layer provides the buffer. */

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad mblk len:%d flags:%d\n",
                 pMblk->mBlkHdr.mLen, pMblk->mBlkHdr.mFlags, 3, 4, 5, 6);
	return (EAGAIN);
	}

    bcopy (pPacket, pMblk->mBlkHdr.mData, len);
    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    /* Done with descriptor, clean up and give it to the device. */
cleanRXD:
    LN_CLEAN_RXD (pRmd);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX ok\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* ln97xPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS ln97xPollSend
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    M_BLK_ID 		pMblk
    )
    {
    LN_TMD *		pTmd;
    void *       	pTemp;
    int         	len;
    int         	oldLevel;
    ULONG		ltmd1;
    UINT16		stat;
    char *       	pBuf = NULL;
    char *       	pOrig = NULL;

    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX b\n", 1, 2, 3, 4, 5, 6);

    /* See if next TXD is available */

    pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;

    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    if ((pTmd->tBufTmd1 & TMD1_OWN) || (((pDrvCtrl->tmdIndex + 1) &
        (pDrvCtrl->tringSize - 1)) == pDrvCtrl->tmdIndexC))
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "Out of tmds.\n", 1, 2, 3, 4, 5, 6);

	if (!pDrvCtrl->txCleaning)
	    netJobAdd ((FUNCPTR)ln97xTRingScrub, (int) pDrvCtrl, 0, 0, 0, 0);

	return (EAGAIN);
        }

    /*
     * If we don't have alignment issues then we can transmit
     * directly from the M_BLK otherwise we have to copy.
     */
    
    if ((pDrvCtrl->offset == 0) && (pMblk->mBlkHdr.mNext == NULL))
        {
        len = max (ETHERSMALL, pMblk->m_len);
        LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pMblk->m_data);
        }
    else
        {
        pOrig = pBuf = netClusterGet (pDrvCtrl->endObj.pNetPool,
                                      pDrvCtrl->pClPoolId);
        if (pBuf == NULL)
            return (EAGAIN);

        pBuf += pDrvCtrl->offset;
        len = netMblkToBufCopy (pMblk, pBuf, NULL);
        LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pBuf);
        }

    len = max (len, ETHERSMALL);

    /* place a transmit request */

    oldLevel = intLock ();          /* disable ints during update */

    pTmd->tBufTmd2 = 0;                     /* clear buffer error status */
    pTmd->tBufTmd3 = 0;

    ltmd1 = TMD1_STP | TMD1_ENP | TMD1_CNST;

    ltmd1 |= TMD1_BCNT_MSK & -len;
    pTmd->tBufTmd1 = PCI_SWAP (ltmd1);

    CACHE_PIPE_FLUSH ();
    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    ltmd1 |= TMD1_OWN;
    pTmd->tBufTmd1 = PCI_SWAP (ltmd1);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    intUnlock (oldLevel);   /* now ln97xInt won't get confused */

    /* Advance our management index */

    pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);

    /* kick start the transmitter, if selected */

    if (lnKickStartTx)
	ln97xCsrWrite (pDrvCtrl, 0, CSR0_TDMD);

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Read the device status register */

    stat = ln97xCsrRead (pDrvCtrl, CSR(0));

    /*
     * Spin until we think we've sent it.  since we're single threaded
     * now it must be us who talked.
     */
    
    while (!(stat & CSR0_TINT))
        {
        stat = ln97xCsrRead (pDrvCtrl, CSR(0));
        }

    /*
     * We now believe that this frame has been transmitted.  SInce we
     * may have allocated/copied it we need to free it.
     */

    if (pOrig != NULL)
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "pOrig != NULL\n", 1, 2, 3, 4, 5, 6);
        netClFree (pDrvCtrl->endObj.pNetPool, pOrig);
        }
    
    if (!pDrvCtrl->txCleaning)
        {
        pDrvCtrl->txCleaning = TRUE;
        ln97xTRingScrub (pDrvCtrl);
        }
    
    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX e\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*****************************************************************************
*
* ln97xMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK
*/

LOCAL STATUS ln97xMCastAddrAdd
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    char * 		pAddress
    )
    {
    int 		error;

    if ((error = etherMultiAdd (&pDrvCtrl->endObj.multiList,
		pAddress)) == ENETRESET)
	    ln97xConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* ln97xMCastAddrDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK.
*/

LOCAL STATUS ln97xMCastAddrDel
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    char * 		pAddress
    )
    {
    int 		error;

    if ((error = etherMultiDel (&pDrvCtrl->endObj.multiList,
	     (char *)pAddress)) == ENETRESET)
	    ln97xConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* ln97xMCastAddrGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS ln97xMCastAddrGet
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    MULTI_TABLE * 	pTable
    )
    {
    int 		error;

    error = etherMultiGet (&pDrvCtrl->endObj.multiList, pTable);

    return (error);
    }

/*******************************************************************************
*
* ln97xStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS ln97xStop
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    STATUS 		result = OK;

    /* Stop the device. */

    ln97xCsrWrite (pDrvCtrl, 0, CSR0_STOP);     	/* set the stop bit */

    SYS_INT_DISABLE (pDrvCtrl);

    SYS_INT_DISCONNECT (pDrvCtrl, ln97xInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Could not diconnect interrupt!\n",
                 1, 2, 3, 4, 5, 6);
	}

    return (result);
    }

/******************************************************************************
*
* ln97xUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function. The controller
* structure should be free by who ever is calling this function.
*
* RETURNS: OK
*/

LOCAL STATUS ln97xUnload
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* Free the shared DMA memory. */

    if (pDrvCtrl->flags & LS_MEM_ALLOC_FLAG)
	cacheDmaFree (pDrvCtrl->pShMem);

    /* Free the shared DMA memory allocated for clusters */
    
    if (pDrvCtrl->flags & LS_MEM_ALLOC_FLAG)
        cacheDmaFree (pDrvCtrl->clDesc.memArea);

    /* Free the memory allocated for mBlks and clBlks */
    
    if (pDrvCtrl->mClCfg.memArea != NULL)
        free (pDrvCtrl->mClCfg.memArea);

    /* Free the memory allocated for driver pool structure */

    if (pDrvCtrl->endObj.pNetPool != NULL)
        free (pDrvCtrl->endObj.pNetPool);

    return (OK);
    }


/*******************************************************************************
*
* ln97xPollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln97xPollStart
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT16        	stat;
    int         	oldLevel;

    /* Read the device status register */

    oldLevel = intLock ();          /* disable ints during update */

    stat = ln97xCsrRead (pDrvCtrl, 0);

    /* Rewrite the register minus the INEA bit to turn off interrupts */

    ln97xCsrWrite (pDrvCtrl, 0, ((stat & (CSR0_BABL | CSR0_CERR | CSR0_MISS | \
                                          CSR0_MERR | CSR0_RINT | CSR0_TINT))));

    pDrvCtrl->flags |= LS_POLLING;

    intUnlock (oldLevel);   /* now ln97xInt won't get confused */

    DRV_LOG (DRV_DEBUG_POLL, "STARTED\n", 1, 2, 3, 4, 5, 6);

    ln97xConfig (pDrvCtrl);	/* reconfigure device */ 

    return (OK);
    }

/*******************************************************************************
*
* ln97xPollStop - stop polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln97xPollStop
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT16        	stat;
    int         	oldLevel;

    oldLevel = intLock ();          /* disable ints during update */

    stat = ln97xCsrRead (pDrvCtrl, CSR(0));

    /* Rewrite the register minus the INEA bit to turn off interrupts */

    ln97xCsrWrite (pDrvCtrl, 0, ((stat & (CSR0_BABL | CSR0_CERR | CSR0_MISS | \
                                          CSR0_MERR | CSR0_RINT | CSR0_TINT))
                                 | CSR0_INEA));

    pDrvCtrl->flags &= ~LS_POLLING;

    intUnlock (oldLevel);   	/* now ln97xInt won't get confused */

    DRV_LOG (DRV_DEBUG_POLL, "STOPPED\n", 1, 2, 3, 4, 5, 6);

    ln97xConfig (pDrvCtrl);	/* reconfigure device */
    
    return (OK);
    }


/******************************************************************************
*
* ln97xTRingScrub - clean the transmit ring
*
* RETURNS: N/A
*
*/

LOCAL void ln97xTRingScrub
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    int 		oldLevel;
    LN_TMD * 		pTmd;
    UINT32 		tmpTmd2 = 0;
    
    pDrvCtrl->txCleaning = TRUE;
    
    DRV_LOG (DRV_DEBUG_TX, "Scrub : indexes %d %d\n", pDrvCtrl->tmdIndex,
             pDrvCtrl->tmdIndexC, 3, 4, 5, 6);

    while (pDrvCtrl->tmdIndexC != pDrvCtrl->tmdIndex)
        {
        /* disposal has not caught up */

        pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndexC;

        /* if the buffer is still owned by LANCE, don't touch it */

        LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

        if (PCI_SWAP (pTmd->tBufTmd1) & TMD1_OWN)
            break;

        oldLevel = intLock();

        if (pDrvCtrl->freeRtn [pDrvCtrl->tmdIndexC] != NULL)
            {
            pDrvCtrl->freeRtn [pDrvCtrl->tmdIndexC]
                (pDrvCtrl->freeData [pDrvCtrl->tmdIndexC].arg1,
                 pDrvCtrl->freeData [pDrvCtrl->tmdIndexC].arg2); 
            pDrvCtrl->freeRtn [pDrvCtrl->tmdIndexC] = NULL;
            pDrvCtrl->freeData [pDrvCtrl->tmdIndexC].arg1 = NULL; 
            pDrvCtrl->freeData [pDrvCtrl->tmdIndexC].arg2 = NULL; 
            }

        intUnlock(oldLevel);
        
        /* now bump the tmd disposal index pointer around the ring */

        pDrvCtrl->tmdIndexC = (pDrvCtrl->tmdIndexC + 1) &
			      (pDrvCtrl->tringSize - 1);

        /*
         * TMD1_ERR is an "OR" of LCOL, LCAR, UFLO or RTRY.
         * Note that BUFF is not indicated in lntmd1_ERR.
         * We should therefore check both lntmd1_ERR and lntmd3_BUFF
         * here for error conditions.
         */

        if ((PCI_SWAP (pTmd->tBufTmd1) & TMD1_ERR) ||
	    ((tmpTmd2 = PCI_SWAP (pTmd->tBufTmd2)) & TMD2_BUFF))
            {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);

            /*
	     * If error was due to excess collisions, bump the collision
             * counter.  The LANCE does not keep an individual counter of
             * collisions, so in this driver, the collision statistic is not
             * an accurate count of total collisions.
             */

	    /* assume DRTY bit not set */

            if (tmpTmd2 & TMD2_RTRY)
		END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

            /* check for no carrier */

            if (tmpTmd2 & TMD2_LCAR)
		{
		DRV_LOG (DRV_DEBUG_TX, "%s: no carrier\n",
                         (int)END_DEV_NAME(&pDrvCtrl->endObj), 0,0,0,0,0);
		}

            /*
	     * Restart chip on fatal errors.
             * The following code handles the situation where the transmitter
             * shuts down due to an underflow error.  This is a situation that
             * will occur if the DMA cannot keep up with the transmitter.
             * It will occur if the LANCE is being held off from DMA access
             * for too long or due to significant memory latency.  DRAM
             * refresh or slow memory could influence this.  Many
             * implementations use a dedicated LANCE buffer.  This can be
             * static RAM to eliminate refresh conflicts; or dual-port RAM
             * so that the LANCE can have free run of this memory during its
             * DMA transfers.
             */

            if (tmpTmd2 & (TMD2_BUFF | TMD2_UFLO))
                {
                pDrvCtrl->endObj.flags &= ~(IFF_UP | IFF_RUNNING);
		ln97xRestart (pDrvCtrl);
                }
            }

	LN_TMD_CLR_ERR (pTmd);
        }
    pDrvCtrl->txCleaning = FALSE;
    
    }

#if FALSE /* use this if no board level support */

/*******************************************************************************
*
* ln97XEnetAddrGet - get Ethernet address
*
* This routine provides a target-specific interface for accessing a
* device Ethernet address.
*
* NOMANUAL

* RETURNS: OK or ERROR if could not be obtained.
*/

STATUS  ln97XEnetAddrGet
    (
    LN_97X_DRV_CTRL *  pDrvCtrl,   /* Driver control */
    char *            enetAdrs
    )
    {
    char 	aprom [LN_97X_APROM_SIZE]; /* copy of address prom space */
    char * 	ioaddr;
    int  	ix;

    /* get IO address of unit */
    ioaddr = (char *)(pDrvCtrl->devAdrs);

    /* load aprom into an array */
    for (ix=0; ix<32; ix++)
        {
         SYS_IN_BYTE(pDrvCtrl,(ioaddr + ix), aprom [ix]);
        }

    /* check for 'w's at end of list */
    if ((aprom [0xe] != 'W') || (aprom [0xf] != 'W'))
        {
        /* should set errno here ? ldt */
        DRV_LOG (DRV_DEBUG_LOAD,
                 "sysLn97XEnetAddrGet W's not stored in aprom\n",
                 0, 1, 2, 3, 4, 5);
        return ERROR;
        }

    bcopy (aprom, enetAdrs, 6);

    DRV_LOG (DRV_DEBUG_LOAD,
             "sysLn97XEnetAddrGet %02x:%02x:%02x:%02x:%02x:%02x\n",
             (enetAdrs[0] & 0xff), (enetAdrs[1] & 0xff),
             (enetAdrs[2] & 0xff), (enetAdrs[3] & 0xff),
             (enetAdrs[4] & 0xff), (enetAdrs[5] & 0xff));
    
    return (OK);
    }
#endif /* XXX */
