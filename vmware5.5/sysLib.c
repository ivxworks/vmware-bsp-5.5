/* sysLib.c - PC [34]86/Pentium/Pentium[234] system-dependent library */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
05g,25mar03,zmm  Fix SPR 86838. The vxShowInit() will be called for every x86 CPU. 
05f,05nov02,hdn  made sysCpuProbe() execution conditional (spr 76279)
05e,01oct02,hdn  added Debug Store support for Pentium4
05d,09sep02,hdn  added Thermal Monitoring support for Pentium4
05c,21jun02,pai  Linker-defined <end> symbol is now cast to an unsigned
                 integer instead of a signed integer where needed.
                 Updated commentary and page-sized stride in sysPhysMemTop()
                 memory auto-size algorithm (SPR 79525).
05b,12jul02,dmh  include sysNvRam.c based value of NV_RAM_SIZE instead of
                 IACSF
05a,11jul02,dmh  remove SFL_START_ADDR for iacsfl-based BSPs.
04z,04jul02,hdn  added the checksum to the FAST_REBOOT mechanism.
04y,18jun02,hdn  cleaned up sysPhysMemDesc[] and new reboot mechanism.
04x,03jun02,pai  Updated sysMmuMapAdd() documentation.  Modified file I/O
                 status messages in sysToMonitor().
04w,05jun02,hdn  added 2MB page size support for 36Bit MMU
04v,25may02,hdn  updated sysMmuMapAdd to align VM_PAGE_SIZE (spr 77530)
		 added intLock/Unlock in sysIntLock/Unlock (spr 75694)
04u,24may02,hdn  replaced PAGE_SIZE with VM_PAGE_SIZE for 4MB page
04t,23may02,pai  Add WindML device initialization to sysHwInit().
04s,20may02,rhe  added USB support for ohci cards. SPR# 76308
04r,09may02,hdn  added Pentium4 Asymmetric Multi Processor configuration
04q,29apr02,pai  Complete the fix for (SPR 8385).  Modified sysPhysMemTop()
                 will now auto-size a larger address space.
04p,28apr02,hdn  added optional new i8259 PIC specific features (spr 76411)
04o,22apr02,pai  Removed configuration related to obsolete SMC Ultra Netif
                 driver.
04n,09apr02,pai  Reworked ATA configuration and documentation (SPR# 73848).
                 Added sysHwInit0() for the (SPR 74951) fix implementation.
04m,01apr02,jkf  Added _WRS_BSP_DEBUG_NULL_ACCESS to generate exception when
                 code accesses to lower page of memory, null ptr, occur.
04l,25mar02,pai  Conditionally include MTRR table based upon PENTIUM2/3/4 CPU
                 types - a modification to the version 04k implementation.
04k,22mar02,pai  Do not include <sysMtrr> table for P5/PENTIUM builds (SPR#
                 73939).
04j,21mar02,pai  Make pciConfigShow configurable based upon the
                 INCLUDE_PCI_CFGSHOW component (SPR 74274).
04i,12mar02,hdn  replaced redTable[] with sysInumTbl[] for HTT (spr 73738)
04h,07mar02,hdn  excluded MTRR code from P5 (spr 73939)
04g,03dec01,jkf  sysToMonitor: checks if device exists before creation,
                 and using new macros for device and file name strings.
04f,27nov01,dmh  move include of iacsfl.h from sysLib.c to config.h
04e,16nov01,ahm  added power management mode initialization (SPR#32599)
04d,06nov01,hdn  added PIRQ[n] and IOAPIC support for Pentium4
04c,02nov01,dmh  add nvram for iacsfl boards. SPR# 70433
04b,01nov01,hdn  added MSRs init, reset PM, enabled MCA w pentiumMcaEnable()
04a,01nov01,jln  added support for gei82543End driver
03z,22oct01,dmh  replace iPiix4 support with pciAutoConfig. SPR# 65192.
03y,17oct01,hdn  removed duplicate sysMmuMapAdd() prototype.
		 moved enabling MC exception into excArchLib.
		 added LOCAL_MEM_AUTOSIZE in sysPhysMemTop().
		 made LOWER_MEM_TOP configurable to preserve the MP table.
03x,11oct01,pai  Updating details related to BSP NIC driver support.  Removed
                 INCLUDE_EEX32 and INCLUDE_FEI definition conditional on
                 DOC macro.  Updated description of BSP driver config
                 routines.  Added PCI library customization and BSP-specific
                 PCI support.  Implemented new PCI Ethernet controller
                 initialization (SPR# 35716) and (SPR# 69775).
03w,01oct01,hdn  replaced INT_NUM_GET(LPT0_INT_LVL) with INT_NUM_LPT0
03v,26sep01,pai  Added support for dec21x40End driver.  Added sysLn97xEnd
                 driver configuration file.  Added WindML BSP routines.
                 sysToMonitor() now opens bootrom.sys as RDONLY (SPR 23057).
03u,24sep01,pai  Updated LPT resource table and macros (SPR 30067).
03t,11sep01,hdn  renamed XXX_INT_VEC to INT_NUM_XXX as vector no of XXX
		 updated sysIntEoiGet() as APIC's INT_LVL got cleaned up
		 updated CPUID structure for PENTIUM4
		 removed sysCodeSelector.
03s,16aug01,hdn  added PENTIUM2/3/4 support
03r,15aug01,hdn  included iacsfl.h if INCLUDE_IACSFL is defined.
03q,15jun00,jkf  release floppy disk on reboot(), SPR#30280
03p,15jun00,jkf  sysPhysMemTop() loop changed to report all found memory.
03o,03may00,mks  modified sysHwInit and sysToMonitor to support SFL based
                 boot process.
03n,07sep99,stv  macros for including END driver components are 
		 conditioned according to SPR# 26296.
03m,21apr99,hdn  added conditional tffsDrv.h inclusion (SPR# 26922)
03l,12mar99,cn   added support for el3c90xEnd driver (SPR# 25327).
03k,09mar99,sbs  added support for ne2000End driver
                 added support for ultraEnd driver
                 added support for elt3c509End driver
03j,01feb99,jkf  added END support for AMD 7997x PCI card.
03i,26jan99,jkf  changed sysHwInit2 to use INCLUDE_ADD_BOOTMEM and
                 removed ATA_MEM_DOSFS from sysToMonitor.  SPR#21338
03h,31mar98,cn   Added Enhanced Network Driver support.
03g,04jun98,hdn  made sysIntLevel() faster. added sysIntCount[] for debug.
03f,28may98,hdn  added support for APIC.
03e,12may98,hdn  merged with pcPentium/sysLib.c. obsolete INCLUDE_IDE.
03d,23apr98,yp   merged TrueFFS support.
03c,16apr98,hdn  added sysCpuid[] for sysCpuProbe().
03b,17mar98,sbs  using macro for dummy mmu entry.
                 added forward declaration for sysMmuMapAdd().
                 changed sysIntIdtType and sysWarmType to use macros.  
                 documentation update.
03a,12mar98,sbs  moved PCI initialization from sysPhysMemTop() to sysHwInit().
                 changed the PCI library calls to the new updated library API. 
                 moved sys557PciInit() from sysPhysMemTop() to sysHwInit().
                 added sysAic7880PciInit().
02z,02mar98,sbs  removed device specific mmu entries and added dynamic entries
                 in sysPhysMemDesc table.
                 added sysMmuMapAdd().
                 added initialization of sysPhysMemDescNumEnt in sysHwInit().
02y,06jan98,hdn  included tffsDrv.h.
02x,15dec97,hdn  added support for warm start from TFFS device.
02w,10jul97,dds  added SCSI-2 support.
02v,24mar97,mas  added sysPhysMemTop(); removed NOMANUAL from sysHwInit2();
                 parameterized the sysWarm* reboot variables (SPR 7806, 7850).
02u,03dec96,hdn  added sys557PciInit().  
		 moved PCI initialization from sysHwInit2() to sysMemTop().
02t,22nov96,dat  added sysNetif.c, for all network support rtns. (if_eex32.c
		 and if_i82557 were combined into sysNetif.c)
02s,20nov96,db   conditionally defined INCLUDE_EEX32 for man page(SPR #6190).
02r,20nov96,hdn  added support for PRO100B.  
02q,01nov96,hdn  added support for PCMCIA.  
02p,21oct96,hdn  removed lptTimeout, added lptResources[].
02o,14oct96,dat  removed ref to i8253TimerTS.c, merged from windview102.
02n,24sep96,hdn  fixed by removing IMPORT ATA_RESOURCE ataResources[].
02m,03sep96,hdn  added the compression support. 
		 changed constants to ROM_WARM_HIGH and ROM_WARM_LOW.
02l,09aug96,hdn  renamed INT_VEC_IRQ0 to INT_NUM_IRQ0.
02k,26jul96,hdn  shut off warning message: "implicit declaration of function"
02j,18jul96,hdn  added support for INCLUDE_ATA.
02i,19jul96,wlf  doc: cleanup.
02h,25jun96,hdn  added support for TIMESTAMP timer.
02g,17jun96,hdn  initialized sysProcessor to NONE
02f,14jun96,hdn  added support for PCI bus.
02e,28may96,hdn  renamed PIT_INT_xxx to PIT0_INT_xxx.
02d,28sep95,dat  new BSP revision id
02c,27sep95,hdn  fixed a typo by changing IO_ADRS_ULTRA to IO_ADRS_ELC.
02b,14jun95,hdn  added a global variable sysCodeSelector.
		 added a local function sysIntVecSetEnt(), sysIntVecSetExit().
		 renamed pSysEndOfInt to intEOI.
		 moved global function declarations to sysLib.h.
02a,14jun95,myz  moved serial configuration to sysSerial.c
01z,07jan95,hdn  added an accurate memory size checking.
01y,31oct94,hdn  changed sysMemTop() to find out a memory size.
		 deleted sysGDT and used sysGdt in sysALib.s.
		 added the Intel EtherExpress32 driver.
		 deleted a conditional macro for INCLUDE_LPT.
		 swapped 1st and 2nd parameter of fdDevCreate().
		 imported globals to timeout IDE and LPT.
01x,12oct94,hdn  deleted sysBootType.
		 added a conditional macro for INCLUDE_LPT.
01w,29may94,hdn  moved sysCpuProbe() to cacheArchLib.c.
		 added open and read bootrom.dat in sysToMonitor().
01v,22apr94,hdn  moved sysVectorIRQ0 from i8259Pic.c.
		 made new globals sysFdBuf and sysFdBufSize.
		 supported the warm start from the EPROM.
01u,06apr94,hdn  added sysCpuProbe().
01t,17feb94,hdn  deleted memAddToPool() in sysHwInit2().
		 added a conditional statement in sysMemTop().
		 changed sysWarmType 0 to 1.
01s,03feb94,hdn  added MMU conditional macro for the limit in the GDT.
01r,29nov93,hdn  added sysBspRev () routine.
01q,22nov93,hdn  added xxdetach () routine for warm start.
01p,16nov93,hdn  added sysWarmType which controls warm start device.
01o,09nov93,hdn  added warm start (control X).
01n,08nov93,vin  added support pc console drivers.
01m,27oct93,hdn  added memAddToPool stuff to sysHwInit2().
01l,12oct93,hdn  changed PIT_INT_VEC_NUM to PIT_INT_VEC.
01k,05sep93,hdn  moved PIC related functions to intrCtl/i8259Pic.c.
		 added sysDelay ().
01j,12aug93,hdn  changed a global descriptor table sysGDT.
		 deleted sysGDTSet.
01i,11aug93,hdn  added a global sysVectorIRQ0.
01h,03aug93,hdn  changed a mapping IRQ to Vector Table.
01g,26jul93,hdn  added a memory descriptor table sysPhysMemDesc[].
01f,25jun93,hdn  changed sysToMonitor() to call sysReboot.
01e,24jun93,hdn  changed the initialization of PIC.
01d,17jun93,hdn  updated to 5.1.
01c,08apr93,jdi  doc cleanup.
01d,07apr93,hdn  renamed Compaq to PC.
01c,26mar93,hdn  added the global descriptor table, memAddToPool.
		 moved enabling A20 to romInit.s. added cacheClear for 486.
01b,18nov92,hdn  supported nested interrupt.
01a,15may92,hdn  written based on frc386 version.
*/

/*
DESCRIPTION
This library provides board-specific routines.  The device configuration
modules and device drivers included are:

    i8253Timer.c - Intel 8253 timer driver
    i8259Intr.c - Intel 8259 Programmable Interrupt Controller (PIC) library
    ioApicIntr.c - Intel IO APIC/xAPIC driver
    ioApicIntrShow.c - Intel IO APIC/xAPIC driver show routines
    iPiix4Pci.c - low level initalization code for PCI ISA/IDE Xcelerator
    loApicIntr.c - Intel Pentium[234] Local APIC/xAPIC driver
    loApicIntrShow.c - Intel Local APIC/xAPIC driver show routines
    loApicTimer.c - Intel Pentium2/3/4 Local APIC timer library
    nullNvRam.c - null NVRAM library
    nullVme.c - null VMEbus library
    pccardLib.c - PC CARD enabler library
    pccardShow.c - PC CARD show library
    pciCfgStub.c - customizes pciConfigLib for the BSP
    pciCfgIntStub.c - customizes pciIntLib for the BSP
    pciConfigLib.c - PCI configuration space access support for PCI drivers
    pciIntLib.c - PCI shared interrupt support
    pciConfigShow.c - Show routines for PCI configuration library
    sysDec21x40End.c - system configuration module for dec21x40End driver
    sysEl3c509End.c - system configuratin module for elt3c509End driver
    sysEl3c90xEnd.c -  system configuration module for el3c90xEnd driver
    sysFei82557End.c - system configuration module for fei82557End driver
    sysGei82543End.c - system configuration module for gei82543End driver
    sysLn97xEnd.c - system configuration module for ln97xEnd driver
    sysNe2000End.c - system configuration module for ne2000End driver
    sysUltraEnd.c - system configuration module for SMC Elite ultraEnd driver
    sysWindML.c - WindML BSP support routines


INCLUDE FILES: sysLib.h

SEE ALSO:
.pG "Configuration"
*/

/* includes (header file) */

#include "vxWorks.h"
#include "vme.h"
#include "memLib.h"
#include "sysLib.h"
#include "string.h"
#include "intLib.h"
#include "config.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"
#include "errnoLib.h"
#include "dosFsLib.h"
#include "stdio.h"
#include "cacheLib.h"
#include "private/vmLibP.h"
#include "arch/i86/pentiumLib.h"

#ifdef	INCLUDE_TFFS
#   include "tffs/tffsDrv.h"
#endif	/* INCLUDE_TFFS */

#ifdef  INCLUDE_SMCFDC37B78X
#   include "drv/multi/smcFdc37b78x.h"
#   ifndef PRJ_BUILD
#       include "multi/smcFdc37b78x.c"
#   endif /* PRJ_BUILD */
#endif  /* INCLUDE_SMCFDC37B78X */

#if defined(INCLUDE_PC_CONSOLE) && defined(INCLUDE_CTB69000VGA)
#   ifndef PRJ_BUILD
#       include "video/ctB69000Vga.c"
#   endif /* PRJ_BUILD */
#endif  /* INCLUDE_PC_CONSOLE && INCLUDE_CTB69000VGA */


/* defines */

#define ROM_SIGNATURE_SIZE	16
#if	(VM_PAGE_SIZE == PAGE_SIZE_4KB)
#   if	(LOCAL_MEM_LOCAL_ADRS >= 0x00100000)
#       define LOCAL_MEM_SIZE_OS	0x00180000	/* n * VM_PAGE_SIZE */
#   else
#       define LOCAL_MEM_SIZE_OS	0x00080000	/* n * VM_PAGE_SIZE */
#   endif /* (LOCAL_MEM_LOCAL_ADRS >= 0x00100000) */
#else	/* VM_PAGE_SIZE is 2/4MB */
#   define LOCAL_MEM_SIZE_OS		VM_PAGE_SIZE	/* n * VM_PAGE_SIZE */
#endif	/* (VM_PAGE_SIZE == PAGE_SIZE_4KB) */
#if	defined (TGT_CPU) && defined (BOOTCODE_IN_RAM)
#   ifndef FAST_REBOOT
#       define FAST_REBOOT				/* fast reboot */
#   endif
#endif	/* defined (TGT_CPU) && defined (BOOTCODE_IN_RAM) */

/*
 * IA-32 protected mode physical address space 4GB (2^32) and protected
 * mode physical address space extension 64GB (2^36) size constants.
 */

#define SIZE_ADDR_SPACE_32   (0x100000000ull)
#define SIZE_ADDR_SPACE_36   (0x1000000000ull)

/* maximum address space probed when using memory auto-sizing */

#define PHYS_MEM_MAX         (SIZE_ADDR_SPACE_32)

#define HALF(x)              (((x) + 1) >> 1)

/* sysPhysMemTop() memory test patterns */

#define TEST_PATTERN_A       (0x57696E64)
#define TEST_PATTERN_B       (0x52697665)
#define TEST_PATTERN_C       (0x72537973)
#define TEST_PATTERN_D       (0x74656D73)


/* imports */

IMPORT char        end;                       /* linker defined end-of-image */
IMPORT GDT         sysGdt[];                  /* the global descriptor table */
IMPORT void        elcdetach (int unit);
IMPORT VOIDFUNCPTR intEoiGet;                 /* BOI/EOI function pointer */
IMPORT void        intEnt (void);
IMPORT int         sysCpuProbe (void);        /* set a type of CPU family */
IMPORT VOID        sysUsbOhciPciInit (void);  /* USB OHCI Init */


/* globals */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {
    /* adrs and length parameters must be page-aligned (multiples of 4KB/4MB) */

#if	(VM_PAGE_SIZE == PAGE_SIZE_4KB)

    /*
     * Defining _WRS_BSP_DEBUG_NULL_ACCESS for debugging configuration.
     * Doing so explicitly marks some lower memory invalid.
     * Any code access to the invalid address range will generate
     * a MMU exception and the offending task will be suspended.
     * Then use l(), lkAddr, ti(), and tt() to find the NULL access.
     * Defining _WRS_BSP_DEBUG_NULL_ACCESS adds an entry to the front
     * of the sysPhysMemDesc[] array. Any code which alters this table
     * will need adjustment (for example: sysPhysMemTop() is altered
     * to account for another entry into the sysPhysMemDesc[] array
     * when _WRS_BSP_DEBUG_NULL_ACCESS is defined.
     */

    /* lower memory for invalid access */
    {
    (void *) 0x0,
    (void *) 0x0,
    _WRS_BSP_VM_PAGE_OFFSET,

#   ifdef _WRS_BSP_DEBUG_NULL_ACCESS

    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID_NOT  | VM_STATE_WRITABLE_NOT  | VM_STATE_CACHEABLE_NOT

#   else

    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS

#   endif /* _WRS_BSP_DEBUG_NULL_ACCESS */
    },

    /* lower memory for valid access */
    {
    (void *) _WRS_BSP_VM_PAGE_OFFSET,
    (void *) _WRS_BSP_VM_PAGE_OFFSET,
    0xa0000 - _WRS_BSP_VM_PAGE_OFFSET,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS
    },

    /* video ram, etc */
    {
    (void *) 0x000a0000,
    (void *) 0x000a0000,
    0x00060000,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_IO
    },

#   if (LOCAL_MEM_LOCAL_ADRS >= 0x00100000)

    /* upper memory for OS */
    {
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    LOCAL_MEM_SIZE_OS,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS
    },

    /* upper memory for Application */
    {
    (void *) LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE_OS,
    (void *) LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE_OS,
    LOCAL_MEM_SIZE - LOCAL_MEM_SIZE_OS,	/* it is changed in sysMemTop() */
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_APPLICATION
    },

#   else /* LOCAL_MEM_LOCAL_ADRS is 0x0 */

    /* upper memory for OS */
    {
    (void *) 0x00100000,
    (void *) 0x00100000,
    LOCAL_MEM_SIZE_OS,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS
    },

    /* upper memory for Application */
    {
    (void *) 0x00100000 + LOCAL_MEM_SIZE_OS,
    (void *) 0x00100000 + LOCAL_MEM_SIZE_OS,
    LOCAL_MEM_SIZE - (0x00100000 + LOCAL_MEM_SIZE_OS),	/* sysMemTop() fix */
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_APPLICATION
    },

#   endif /* (LOCAL_MEM_LOCAL_ADRS >= 0x00100000) */

#   if defined(INCLUDE_SM_NET) && (SM_MEM_ADRS != 0x0)

    /* upper memory for sm net/obj pool */
    {
    (void *) SM_MEM_ADRS,
    (void *) SM_MEM_ADRS,
    SM_MEM_SIZE + SM_OBJ_MEM_SIZE,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_APPLICATION
    },

#   endif /* defined(INCLUDE_SM_NET) && (SM_MEM_ADRS != 0x0) */

#   ifdef INCLUDE_IACSFL

    {
    (void *) 0xFFF80000,
    (void *) 0xFFF80000,
    0x00080000,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_APPLICATION
    },

#   endif /* INCLUDE_IACSFL */
    
#else	/* VM_PAGE_SIZE is 2/4MB */

    /* 1st 2/4MB: lower mem + video ram etc + sm pool + upper mem */
    {
    (void *) 0x0,
    (void *) 0x0,
    VM_PAGE_SIZE,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS
    },

#   if (LOCAL_MEM_LOCAL_ADRS >= VM_PAGE_SIZE)

    /* 2nd 2/4MB: upper memory for OS */
    {
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    LOCAL_MEM_SIZE_OS,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS
    },

    /* remaining 2/4MB pages: upper memory for Application */
    {
    (void *) LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE_OS,
    (void *) LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE_OS,
    LOCAL_MEM_SIZE - LOCAL_MEM_SIZE_OS,	/* it is changed in sysMemTop() */
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_APPLICATION
    },

#   else /* LOCAL_MEM_LOCAL_ADRS is 0x0 */

    /* 2nd 2/4MB: upper memory for OS */
    {
    (void *) VM_PAGE_SIZE,
    (void *) VM_PAGE_SIZE,
    LOCAL_MEM_SIZE_OS,
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_OS
    },

    /* remaining 2/4MB pages: upper memory for Application */
    {
    (void *) VM_PAGE_SIZE + LOCAL_MEM_SIZE_OS,
    (void *) VM_PAGE_SIZE + LOCAL_MEM_SIZE_OS,
    LOCAL_MEM_SIZE - (VM_PAGE_SIZE + LOCAL_MEM_SIZE_OS), /* sysMemTop() fix */
    VM_STATE_MASK_FOR_ALL,
    VM_STATE_FOR_MEM_APPLICATION
    },

#   endif /* (LOCAL_MEM_LOCAL_ADRS >= VM_PAGE_SIZE) */

#endif	/* (VM_PAGE_SIZE == PAGE_SIZE_4KB) */

    /* entries for dynamic mappings - create sufficient entries */

    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,

    };

int sysPhysMemDescNumEnt; 	/* number Mmu entries to be mapped */

#ifdef	INCLUDE_PC_CONSOLE

PC_CON_DEV	pcConDv [N_VIRTUAL_CONSOLES] = 
    {
    {{{{NULL}}}, FALSE, NULL, NULL}, 
    {{{{NULL}}}, FALSE, NULL, NULL}
    };

#endif	/* INCLUDE_PC_CONSOLE */

#ifdef INCLUDE_FD

IMPORT STATUS usrFdConfig (int type, int drive, char *fileName);
FD_TYPE fdTypes[] =
    {
    {2880,18,2,80,2,0x1b,0x54,0x00,0x0c,0x0f,0x02,1,1,"1.44M"},
    {2400,15,2,80,2,0x24,0x50,0x00,0x0d,0x0f,0x02,1,1,"1.2M"},
    };
UINT    sysFdBuf     = FD_DMA_BUF_ADDR;	/* floppy disk DMA buffer address */
UINT    sysFdBufSize = FD_DMA_BUF_SIZE;	/* floppy disk DMA buffer size */

#endif	/* INCLUDE_FD */

#ifdef	INCLUDE_ATA

IMPORT STATUS usrAtaConfig (int ctrl, int drive, char *fileName);
ATA_TYPE ataTypes[ATA_MAX_CTRLS][ATA_MAX_DRIVES] =
    {
    /* controller zero */
    {  
        /* ctrl 0 drive 0 */
        {   
        ATA_CTRL0_DRV0_CYL,    /* Number of cylinders on device */
        ATA_CTRL0_DRV0_HDS,    /* Number of heads on device */
        ATA_CTRL0_DRV0_SPT,    /* Number of sectors per track on device */
        ATA_CTRL0_DRV0_BPS,    /* Number of bytes per sector on device */
        ATA_CTRL0_DRV0_WPC     /* write precompensation 0xff = dont use */
        },

        /* ctrl 0 drive 1 */
        {   
        ATA_CTRL0_DRV1_CYL,    /* Number of cylinders on device */
        ATA_CTRL0_DRV1_HDS,    /* Number of heads on device */
        ATA_CTRL0_DRV1_SPT,    /* Number of sectors per track on device */
        ATA_CTRL0_DRV1_BPS,    /* Number of bytes per sector on device */
        ATA_CTRL0_DRV1_WPC     /* write precompensation 0xff = dont use */
        },
   },

   /* controller one */
   {
        /* ctrl 1 drive 0 */
        {
        ATA_CTRL1_DRV0_CYL,    /* Number of cylinders on device */
        ATA_CTRL1_DRV0_HDS,    /* Number of heads on device */
        ATA_CTRL1_DRV0_SPT,    /* Number of sectors per track on device */
        ATA_CTRL1_DRV0_BPS,    /* Number of bytes per sector on device */
        ATA_CTRL1_DRV0_WPC     /* write precompensation 0xff = dont use */
        },

        /* ctrl 1 drive 1 */
        {
        ATA_CTRL1_DRV1_CYL,    /* Number of cylinders on device */
        ATA_CTRL1_DRV1_HDS,    /* Number of heads on device */
        ATA_CTRL1_DRV1_SPT,    /* Number of sectors per track on device */
        ATA_CTRL1_DRV1_BPS,    /* Number of bytes per sector on device */
        ATA_CTRL1_DRV1_WPC     /* write precompensation 0xff = dont use */
        }
    }
    };

ATA_RESOURCE ataResources[ATA_MAX_CTRLS] =
    {
    /* ATA controller zero resources */
    {
        /*  PCCARD_RESOURCE */
        { 
        ATA0_VCC,             /* 3-5 volts Vcc */
        ATA0_VPP,             /* 5-12 volts Vpp */
            {
            ATA0_IO_START0,   /* start I/O address 0 */
            ATA0_IO_START1    /* start I/O address 1 */
            },  

            {
            ATA0_IO_STOP0,    /* end I/0 address 0 */
            ATA0_IO_STOP1     /* end I/0 address 1 */
            }, 
        ATA0_EXTRA_WAITS,     /* extra wait states 0-2 */
        ATA0_MEM_START,       /* start host mem address */
        ATA0_MEM_STOP,        /* stop host mem address */
        ATA0_MEM_WAITS,       /* mem extra wait states 0-2 */
        ATA0_MEM_OFFSET,      /* mem offset of card address */
        ATA0_MEM_LENGTH       /* length of memory */
        },
    ATA0_CTRL_TYPE,           /* IDE_LOCAL or ATA_PCMCIA */
    ATA0_NUM_DRIVES,          /* number of drives on controller */ 
    INT_NUM_ATA0,             /* interrupt number of controller */
    ATA0_INT_LVL,             /* interrupt level of controller */
    ATA0_CONFIG,              /* device configuration settings */
    ATA0_SEM_TIMEOUT,         /* semaphore timeout for controller */
    ATA0_WDG_TIMEOUT,         /* watchdog timeout for controller */
    ATA0_SOCKET_TWIN,         /* socket number for twin card */
    ATA0_POWER_DOWN           /* power down mode for this controller */
    },  /* ctrl 0 end */

    /* ATA controller one resources */
    {
        /*  PCCARD_RESOURCE */
        { 
        ATA1_VCC,             /* 3-5 volts Vcc */
        ATA1_VPP,             /* 5-12 volts Vpp */
            {
            ATA1_IO_START0,   /* start I/O address 0 */
            ATA1_IO_START1    /* start I/O address 1 */
            },  

            {
            ATA1_IO_STOP0,    /* end I/0 address 0 */
            ATA1_IO_STOP1     /* end I/0 address 1 */
            }, 
        ATA1_EXTRA_WAITS,     /* extra wait states 0-2 */
        ATA1_MEM_START,       /* start host mem address */
        ATA1_MEM_STOP,        /* stop host mem address */
        ATA1_MEM_WAITS,       /* mem extra wait states 0-2 */
        ATA1_MEM_OFFSET,      /* mem offset of card address */
        ATA1_MEM_LENGTH       /* length of memory */
        },
    ATA1_CTRL_TYPE,           /* IDE_LOCAL or ATA_PCMCIA */
    ATA1_NUM_DRIVES,          /* number of drives on controller */ 
    INT_NUM_ATA1,             /* interrupt number of controller */
    ATA1_INT_LVL,             /* interrupt level of controller */
    ATA1_CONFIG,              /* device configuration settings */
    ATA1_SEM_TIMEOUT,         /* semaphore timeout for controller */
    ATA1_WDG_TIMEOUT,         /* watchdog timeout for controller */
    ATA1_SOCKET_TWIN,         /* socket number for twin card */
    ATA1_POWER_DOWN           /* power down mode for this controller */
    }   /* ctrl 1 end */
    };

#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_LPT

LPT_RESOURCE lptResources [N_LPT_CHANNELS] =
    {
    {LPT0_BASE_ADRS, INT_NUM_LPT0, LPT0_INT_LVL,
    TRUE, 10000, 10000, 1, 1, 0
    },

    {LPT1_BASE_ADRS, INT_NUM_LPT1, LPT1_INT_LVL,
    TRUE, 10000, 10000, 1, 1, 0
    },

    {LPT2_BASE_ADRS, INT_NUM_LPT2, LPT2_INT_LVL,
    TRUE, 10000, 10000, 1, 1, 0
    }
    };

#endif	/* INCLUDE_LPT */

int	sysBus		= BUS;		/* system bus type (VME_BUS, etc) */
int	sysCpu		= CPU;		/* system cpu type (MC680x0) */
char	*sysBootLine	= BOOT_LINE_ADRS; /* address of boot line */
char	*sysExcMsg	= EXC_MSG_ADRS;	/* catastrophic message area */
int	sysProcNum;			/* processor number of this cpu */
int	sysFlags;			/* boot flags */
char	sysBootHost [BOOT_FIELD_LEN];	/* name of host from which we booted */
char	sysBootFile [BOOT_FIELD_LEN];	/* name of file from which we booted */
UINT	sysIntIdtType	= SYS_INT_TRAPGATE; /* IDT entry type */
UINT	sysProcessor	= NONE;		/* 0=386, 1=486, 2=P5, 4=P6, 5=P7 */
UINT	sysCoprocessor	= 0;		/* 0=none, 1=387, 2=487 */
int 	sysWarmType	= SYS_WARM_TYPE;      /* system warm boot type */
int	sysWarmFdDrive	= SYS_WARM_FD_DRIVE;  /* 0 = drive a:, 1 = b: */
int	sysWarmFdType	= SYS_WARM_FD_TYPE;   /* 0 = 3.5" 2HD, 1 = 5.25" 2HD */
int	sysWarmAtaCtrl	= SYS_WARM_ATA_CTRL;  /* controller 0 or 1 */
int	sysWarmAtaDrive	= SYS_WARM_ATA_DRIVE; /* Hd drive 0 (c:), 1 (d:) */
int	sysWarmTffsDrive= SYS_WARM_TFFS_DRIVE; /* TFFS drive 0 (DOC) */
UINT	sysStrayIntCount = 0;		/* Stray interrupt count */
char	*memTopPhys	= NULL;		/* top of memory */
char	*memRom		= NULL;		/* saved bootrom image */
GDT	*pSysGdt	= (GDT *)(LOCAL_MEM_LOCAL_ADRS + GDT_BASE_OFFSET);
CPUID	sysCpuId	= {0,{0},0,0,0,0,0,0,0,0,{0},{0}}; /* CPUID struct. */
BOOL	sysBp		= TRUE;		/* TRUE for BP, FALSE for AP */

#ifdef	SYS_INT_DEBUG			/* element should be > sysInumTblNumEnt */
UINT32 sysIntCount[64];			/* incremented in the EOI routine */
#endif	/* SYS_INT_DEBUG */

#if     defined(VIRTUAL_WIRE_MODE)

UINT8	sysInumTbl[]	= 		/* IRQ vs IntNum table */
    {
    INT_NUM_IRQ0,			/* IRQ  0 Vector No */
    INT_NUM_IRQ0 + 1,			/* IRQ  1 Vector No */
    INT_NUM_IRQ0 + 2,			/* IRQ  2 Vector No */
    INT_NUM_IRQ0 + 3,			/* IRQ  3 Vector No */
    INT_NUM_IRQ0 + 4,			/* IRQ  4 Vector No */
    INT_NUM_IRQ0 + 5,			/* IRQ  5 Vector No */
    INT_NUM_IRQ0 + 6,			/* IRQ  6 Vector No */
    INT_NUM_IRQ0 + 7,			/* IRQ  7 Vector No */
    INT_NUM_IRQ0 + 8,			/* IRQ  8 Vector No */
    INT_NUM_IRQ0 + 9,			/* IRQ  9 Vector No */
    INT_NUM_IRQ0 + 10,			/* IRQ 10 Vector No */
    INT_NUM_IRQ0 + 11,			/* IRQ 11 Vector No */
    INT_NUM_IRQ0 + 12,			/* IRQ 12 Vector No */
    INT_NUM_IRQ0 + 13,			/* IRQ 13 Vector No */
    INT_NUM_IRQ0 + 14,			/* IRQ 14 Vector No */
    INT_NUM_IRQ0 + 15,			/* IRQ 15 Vector No */
    INT_NUM_LOAPIC_TIMER,		/* Local APIC Timer Vector No */
    INT_NUM_LOAPIC_ERROR,		/* Local APIC Error Vector No */
    INT_NUM_LOAPIC_LINT0,		/* Local APIC LINT0 Vector No */
    INT_NUM_LOAPIC_LINT1,		/* Local APIC LINT1 Vector No */
    INT_NUM_LOAPIC_PMC,			/* Local APIC PMC Vector No */
    INT_NUM_LOAPIC_THERMAL,		/* Local APIC Thermal Vector No */
    INT_NUM_LOAPIC_SPURIOUS,		/* Local APIC Spurious Vector No */
    INT_NUM_LOAPIC_SM,			/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_SM + 1,		/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_SM + 2,		/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_SM + 3,		/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_IPI,			/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 1,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 2,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 3,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 4,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 5,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 6,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 7		/* Local APIC IPI Vector No */
    };

#elif   defined(SYMMETRIC_IO_MODE)

UINT8	sysInumTbl[]	=  		/* IRQ vs IntNum table */
    {
    INT_NUM_IOAPIC_IRQ0,		/* IO APIC IRQ  0 Vector No */
    INT_NUM_IOAPIC_IRQ1,		/* IO APIC IRQ  1 Vector No */
    INT_NUM_IOAPIC_IRQ2,		/* IO APIC IRQ  2 Vector No */
    INT_NUM_IOAPIC_IRQ3,		/* IO APIC IRQ  3 Vector No */
    INT_NUM_IOAPIC_IRQ4,		/* IO APIC IRQ  4 Vector No */
    INT_NUM_IOAPIC_IRQ5,		/* IO APIC IRQ  5 Vector No */
    INT_NUM_IOAPIC_IRQ6,		/* IO APIC IRQ  6 Vector No */
    INT_NUM_IOAPIC_IRQ7,		/* IO APIC IRQ  7 Vector No */
    INT_NUM_IOAPIC_IRQ8,		/* IO APIC IRQ  8 Vector No */
    INT_NUM_IOAPIC_IRQ9,		/* IO APIC IRQ  9 Vector No */
    INT_NUM_IOAPIC_IRQA,		/* IO APIC IRQ 10 Vector No */
    INT_NUM_IOAPIC_IRQB,		/* IO APIC IRQ 11 Vector No */
    INT_NUM_IOAPIC_IRQC,		/* IO APIC IRQ 12 Vector No */
    INT_NUM_IOAPIC_IRQD,		/* IO APIC IRQ 13 Vector No */
    INT_NUM_IOAPIC_IRQE,		/* IO APIC IRQ 14 Vector No */
    INT_NUM_IOAPIC_IRQF,		/* IO APIC IRQ 15 Vector No */
    INT_NUM_IOAPIC_PIRQA,		/* IO APIC PIRQ A Vector No */
    INT_NUM_IOAPIC_PIRQB,		/* IO APIC PIRQ B Vector No */
    INT_NUM_IOAPIC_PIRQC,		/* IO APIC PIRQ C Vector No */
    INT_NUM_IOAPIC_PIRQD,		/* IO APIC PIRQ D Vector No */
    INT_NUM_IOAPIC_PIRQE,		/* IO APIC PIRQ E Vector No */
    INT_NUM_IOAPIC_PIRQF,		/* IO APIC PIRQ F Vector No */
    INT_NUM_IOAPIC_PIRQG,		/* IO APIC PIRQ G Vector No */
    INT_NUM_IOAPIC_PIRQH,		/* IO APIC PIRQ H Vector No */
    INT_NUM_LOAPIC_TIMER,		/* Local APIC Timer Vector No */
    INT_NUM_LOAPIC_ERROR,		/* Local APIC Error Vector No */
    INT_NUM_LOAPIC_LINT0,		/* Local APIC LINT0 Vector No */
    INT_NUM_LOAPIC_LINT1,		/* Local APIC LINT1 Vector No */
    INT_NUM_LOAPIC_PMC,			/* Local APIC PMC Vector No */
    INT_NUM_LOAPIC_THERMAL,		/* Local APIC Thermal Vector No */
    INT_NUM_LOAPIC_SPURIOUS,		/* Local APIC Spurious Vector No */
    INT_NUM_LOAPIC_SM,			/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_SM + 1,		/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_SM + 2,		/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_SM + 3,		/* Local APIC SM Vector No */
    INT_NUM_LOAPIC_IPI,			/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 1,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 2,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 3,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 4,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 5,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 6,		/* Local APIC IPI Vector No */
    INT_NUM_LOAPIC_IPI + 7		/* Local APIC IPI Vector No */
    };

#else

UINT8	sysInumTbl[]	=  		/* IRQ vs IntNum table */
    {
    INT_NUM_IRQ0,			/* IRQ  0 Vector No */
    INT_NUM_IRQ0 + 1,			/* IRQ  1 Vector No */
    INT_NUM_IRQ0 + 2,			/* IRQ  2 Vector No */
    INT_NUM_IRQ0 + 3,			/* IRQ  3 Vector No */
    INT_NUM_IRQ0 + 4,			/* IRQ  4 Vector No */
    INT_NUM_IRQ0 + 5,			/* IRQ  5 Vector No */
    INT_NUM_IRQ0 + 6,			/* IRQ  6 Vector No */
    INT_NUM_IRQ0 + 7,			/* IRQ  7 Vector No */
    INT_NUM_IRQ0 + 8,			/* IRQ  8 Vector No */
    INT_NUM_IRQ0 + 9,			/* IRQ  9 Vector No */
    INT_NUM_IRQ0 + 10,			/* IRQ 10 Vector No */
    INT_NUM_IRQ0 + 11,			/* IRQ 11 Vector No */
    INT_NUM_IRQ0 + 12,			/* IRQ 12 Vector No */
    INT_NUM_IRQ0 + 13,			/* IRQ 13 Vector No */
    INT_NUM_IRQ0 + 14,			/* IRQ 14 Vector No */
    INT_NUM_IRQ0 + 15,			/* IRQ 15 Vector No */
    };

#endif	/* defined(VIRTUAL_WIRE_MODE) */

UINT32 sysInumTblNumEnt	= NELEMENTS (sysInumTbl);


/* locals */

#ifdef	INCLUDE_ROMCARD

LOCAL short *sysRomBase[] = 
    {
    (short *)0xce000, (short *)0xce800, (short *)0xcf000, (short *)0xcf800
    };

LOCAL char sysRomSignature[ROM_SIGNATURE_SIZE] = 
    {
    0x55,0xaa,0x01,0x90,0x90,0x90,0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90
    };

#endif	/* INCLUDE_ROMCARD */

#if	(CPU == PENTIUM2) || (CPU == PENTIUM3) || (CPU == PENTIUM4)
    /*
     * The cache control flags and MTRRs operate hierarchically for restricting
     * caching.  That is, if the CD flag is set, caching is prevented globally.
     * If the CD flag is clear, either the PCD flags and/or the MTRRs can be
     * used to restrict caching.  If there is an overlap of page-level caching
     * control and MTRR caching control, the mechanism that prevents caching
     * has precedence.  For example, if an MTRR makes a region of system memory
     * uncachable, a PCD flag cannot be used to enable caching for a page in 
     * that region.  The converse is also true; that is, if the PCD flag is 
     * set, an MTRR cannot be used to make a region of system memory cacheable.
     * If there is an overlap in the assignment of the write-back and write-
     * through caching policies to a page and a region of memory, the write-
     * through policy takes precedence.  The write-combining policy takes
     * precedence over either write-through or write-back.
     */ 
LOCAL MTRR sysMtrr =
    { 					/* MTRR table */
    {0,0},				/* MTRR_CAP register */
    {0,0},				/* MTRR_DEFTYPE register */
    					/* Fixed Range MTRRs */
    {{{MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB}},
     {{MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB, MTRR_WB}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_WC, MTRR_WC, MTRR_WC, MTRR_WC}},
     {{MTRR_WP, MTRR_WP, MTRR_WP, MTRR_WP, MTRR_WP, MTRR_WP, MTRR_WP, MTRR_WP}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}},
     {{MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC, MTRR_UC}}},
    {{0LL, 0LL},			/* Variable Range MTRRs */
     {0LL, 0LL},
     {0LL, 0LL},
     {0LL, 0LL},
     {0LL, 0LL},
     {0LL, 0LL},
     {0LL, 0LL},
     {0LL, 0LL}}
    };
#endif  /* (CPU == PENTIUM[2/3/4]) */


/* forward declarations */

LOCAL void sysStrayInt   (void);
char * sysPhysMemTop	 (void);
STATUS sysMmuMapAdd	 (void * address, UINT len, UINT initialStateMask,
                    	  UINT initialState);
LOCAL void sysIntInitPIC (void);
LOCAL void sysIntEoiGet  (VOIDFUNCPTR * vector, 
			  VOIDFUNCPTR * routineBoi, int * parameterBoi,
			  VOIDFUNCPTR * routineEoi, int * parameterEoi);


/* includes (source file) */

#if (NV_RAM_SIZE != NONE)
#   include "sysNvRam.c"
#else	/* default to nullNvRam */
#   include "mem/nullNvRam.c"
#endif	/* (NV_RAM_SIZE != NONE) */

#include "sysSerial.c"

#if	defined (TGT_CPU) && defined (SYMMETRIC_IO_MODE)
#   include "sysAmp.c"
#else
#   include "vme/nullVme.c"
#endif	/* defined (TGT_CPU) && defined (SYMMETRIC_IO_MODE) */

#if	defined(VIRTUAL_WIRE_MODE)
#   include "intrCtl/loApicIntr.c"
#   include "intrCtl/i8259Intr.c"
#   ifdef INCLUDE_APIC_TIMER
#      include "timer/loApicTimer.c"	/* includes timestamp driver */
#   else
#      include "timer/i8253Timer.c"	/* includes timestamp driver */
#   endif /* INCLUDE_APIC_TIMER */
#   ifdef INCLUDE_SHOW_ROUTINES
#      include "intrCtl/loApicIntrShow.c"
#   endif /* INCLUDE_SHOW_ROUTINES */
#elif	defined(SYMMETRIC_IO_MODE)
#   include "intrCtl/loApicIntr.c"
#   include "intrCtl/i8259Intr.c"
#   include "intrCtl/ioApicIntr.c"
#   ifdef INCLUDE_APIC_TIMER
#      include "timer/loApicTimer.c"	/* includes timestamp driver */
#   else
#      include "timer/i8253Timer.c"	/* includes timestamp driver */
#   endif /* INCLUDE_APIC_TIMER */
#   ifdef INCLUDE_SHOW_ROUTINES
#      include "intrCtl/loApicIntrShow.c"
#      include "intrCtl/ioApicIntrShow.c"
#   endif /* INCLUDE_SHOW_ROUTINES */
#else
#   include "intrCtl/i8259Intr.c"
#   include "timer/i8253Timer.c"	/* includes timestamp driver */
#endif	/* defined(VIRTUAL_WIRE_MODE) */

#ifdef	INCLUDE_PCI                     /* BSP PCI bus & config support */
#   include "pciCfgStub.c"              /* customize pciConfigLib for BSP */
#   include "pci/pciConfigLib.c"
#   include "pciCfgIntStub.c"           /* customize pciIntLib for BSP */
#   include "pci/pciIntLib.c"
#   if (defined(INCLUDE_PCI_CFGSHOW) && !defined(PRJ_BUILD))
#      include "pci/pciConfigShow.c"
#   endif /* (defined(INCLUDE_PCI_CFGSHOW) && !defined(PRJ_BUILD)) */
#if (PCI_CFG_TYPE == PCI_CFG_AUTO)
#   include "pci/pciAutoConfigLib.c"
#   include "sysBusPci.c"
#endif /* (PCI_CFG_TYPE == PCI_CFG_AUTO) */
#endif	/* INCLUDE_PCI */

#ifdef	INCLUDE_PCMCIA
#   include "pcmcia/pccardLib.c"
#   include "pcmcia/pccardShow.c"
#endif	/* INCLUDE_PCMCIA */

#ifdef  INCLUDE_NETWORK
#   include "sysNet.c"                  /* network driver support */
#endif  /* INCLUDE_NETWORK */

#if defined(INCLUDE_SCSI) || defined(INCLUDE_SCSI2)
#    include "sysScsi.c"                /* scsi support */
#endif /* INCLUDE_SCSI || INCLUDE_SCSI2 */

/* include BSP specific WindML configuration */

#if defined(INCLUDE_WINDML)
#    include "sysWindML.c"
#endif /* INCLUDE_WINDML */

#ifdef	INCLUDE_THERM_MONITOR
#   include "sysTherm.c"		/* Thermal Monitor support */
#endif	/* INCLUDE_THERM_MONITOR */

#ifdef	INCLUDE_DEBUG_STORE
#   include "sysDbgStr.c"		/* Debug Store support */
#endif	/* INCLUDE_DEBUG_STORE */


/*******************************************************************************
*
* sysModel - return the model name of the CPU board
*
* This routine returns the model name of the CPU board.
*
* RETURNS: A pointer to the string "PC 386, 486, PENTIUM or PENTIUM[234]".
*/

char *sysModel (void)

    {
#if	(CPU == I80386)
    return ("PC 386");
#elif	(CPU == I80486)
    return ("PC 486");
#elif	(CPU == PENTIUM)
    return ("PC PENTIUM");
#elif	(CPU == PENTIUM2)
    return ("PC PENTIUM2");
#elif	(CPU == PENTIUM3)
    return ("PC PENTIUM3");
#elif	(CPU == PENTIUM4)
    return ("PC PENTIUM4");
#endif	/* (CPU == I80386) */
    }

/*******************************************************************************
*
* sysBspRev - return the BSP version and revision number
*
* This routine returns a pointer to a BSP version and revision number, for
* example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
*
* RETURNS: A pointer to the BSP version/revision string.
*/

char * sysBspRev (void)
    {
    return (BSP_VERSION BSP_REV);
    }

#ifdef INCLUDE_SYS_HW_INIT_0

/*******************************************************************************
*
* sysHwInit0 - BSP-specific hardware initialization
*
* This routine is called from usrInit() to perform BSP-specific initialization
* that must be done before cacheLibInit() is called and/or the BSS is cleared.
*
* The BSP-specific sysCpuProbe() routine is called for the purpose of
* identifying IA-32 target CPU variants, and the features or functions
* supported by the target CPU.  This information must be obtained relatively
* early during system hardware initialization, as some support libraries
* (mmuLib, cacheLib, &c.) will use the processor feature information to
* enable or disable architecture-specific and/or BSP-specific functionality.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysHwInit0 (void)
    {
#ifdef	INCLUDE_CPU_PROBE
    (void) sysCpuProbe ();
#else
    sysProcessor = X86CPU_DEFAULT;
#endif	/* INCLUDE_CPU_PROBE */
    }

#endif  /* INCLUDE_SYS_HW_INIT_0 */

/*******************************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various features of the i386/i486 board.
* It is called from usrInit() in usrConfig.c.
*
* NOTE: This routine should not be called directly by the user application.
*
* RETURNS: N/A
*/

void sysHwInit (void)
    {
    PHYS_MEM_DESC *pMmu;
    int ix = 0;

#if	(CPU == PENTIUM) || (CPU == PENTIUM2) || (CPU == PENTIUM3) || \
	(CPU == PENTIUM4)

    /* initialize the MSRs (Model Specific Registers) */
    
    pentiumMsrInit ();

#   if	(CPU != PENTIUM)

    /* enable the MTRR (Memory Type Range Registers) */

    if ((sysCpuId.featuresEdx & CPUID_MTRR) == CPUID_MTRR)
	{
        pentiumMtrrDisable ();		/* disable MTRR */
#   ifdef INCLUDE_MTRR_GET
        (void) pentiumMtrrGet (&sysMtrr); /* get MTRR initialized by BIOS */
#   else
        (void) pentiumMtrrSet (&sysMtrr); /* set your own MTRR */
#   endif /* INCLUDE_MTRR_GET */
        pentiumMtrrEnable ();		/* enable MTRR */
	}

#   endif /* (CPU != PENTIUM) */

#   ifdef INCLUDE_PMC

    /* enable PMC (Performance Monitoring Counters) */

    pentiumPmcStop ();			/* stop PMC0 and PMC1 */
    pentiumPmcReset ();			/* reset PMC0 and PMC1 */

#   endif /* INCLUDE_PMC */

    /* enable the MCA (Machine Check Architecture) */

    pentiumMcaEnable (TRUE);

#   ifdef INCLUDE_SHOW_ROUTINES

    /* 
     * if excMcaInfoShow is not NULL, it is called in the default
     * exception handler when Machine Check Exception happened
     */

    {
    IMPORT FUNCPTR excMcaInfoShow;
    excMcaInfoShow = (FUNCPTR) pentiumMcaShow;
    }
#   endif /* INCLUDE_SHOW_ROUTINES */

#endif	/* (CPU == PENTIUM) || (CPU == PENTIUM[234]) */

#ifdef INCLUDE_SHOW_ROUTINES
    vxShowInit ();
#endif /* INCLUDE_SHOW_ROUTINES */
    /* initialize the number of active mappings (sysPhysMemDescNumEnt) */

    pMmu = &sysPhysMemDesc[0];

    for (ix = 0; ix < NELEMENTS (sysPhysMemDesc); ix++) 
        if (pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
            pMmu++;
        else
            break;

    sysPhysMemDescNumEnt = ix;

    /* initialize PCI library */

#ifdef  INCLUDE_PCI

    pciConfigLibInit (PCI_MECHANISM_1, PCI_CONFIG_ADDR, PCI_CONFIG_DATA, NONE);
    sysPciIntInit ();			/* it does pciIntLibInit() */

#endif /* INCLUDE_PCI */

    /* initialize the PIC (Programmable Interrupt Controller) */

    sysIntInitPIC ();		/* should be after the PCI init for IOAPIC */
    intEoiGet = sysIntEoiGet;	/* function pointer used in intConnect () */

    /* initialize PCI devices */

#ifdef  INCLUDE_PCI

#if (PCI_CFG_TYPE == PCI_CFG_AUTO)

    /* Some boards don't have a typical BIOS
     * for example, Intel's System Firmware Library needs pciAutoConfig 
     */

    sysPciAutoConfig();

#endif /* (PCI_CFG_TYPE == PCI_CFG_AUTO) */

    /* 
     * PCI-to-PCI bridge initialization should be done here, if it is.
     * It is not necessary for Intel 430HX PCISET, which splits
     * the extended memory area as follows:
     *   - Flash BIOS area from 4GByte to (4GB - 512KB)
     *   - DRAM memory from 1MB to a maximum of 512MB
     *   - PCI memory space from the top of DRAM to (4GB - 512KB)
     */

#ifdef INCLUDE_NETWORK

    /* initialize PCI network controllers starting from Bus 0 */

     pciConfigForeachFunc (0, TRUE, (PCI_FOREACH_FUNC) sysNetPciInit, NULL);

#endif /* INCLUDE_NETWORK */

#if (defined(INCLUDE_SCSI) && defined(INCLUDE_AIC_7880))
    sysAic7880PciInit ();
#endif  /* INCLUDE_SCSI && INCLUDE_AIC_7880 */

#endif /* INCLUDE_PCI */

    /* initialize devices on the board if following SFL boot process */
    
#ifdef INCLUDE_IACSFL
    {
#   ifdef INCLUDE_CTB69000VGA
    extern int  ctB69000VgaInit();
#   endif /* INCLUDE_CTB69000VGA */
    
    /* superIO - basic intialization */
    
#   ifdef INCLUDE_SMCFDC37B78X

    smcFdc37b78xDevCreate ((void *) NULL); /* intialize superIO library */
    
    /* enable only given devices on SuperIO chip */
    smcFdc37b78xInit ((SMCFDC37B78X_FDD_EN | SMCFDC37B78X_COM1_EN |
                       SMCFDC37B78X_COM2_EN | SMCFDC37B78X_LPT1_EN |
                       SMCFDC37B78X_KBD_EN));
    
#   endif /* INCLUDE_SMCFDC37B78X */
    
    /* PC console - initialization */
    
#   if defined(INCLUDE_PC_CONSOLE)
    
#   ifdef INCLUDE_SMCFDC37B78X
    
    smcFdc37b78xKbdInit ();              /* Initialize Kbd on SuperIO */
    
#   endif /* INCLUDE_SMCFDC37B78X */
    
#   ifdef INCLUDE_CTB69000VGA
    
    ctB69000VgaInit ();                  /* Initialize VGA card */
    
#   endif /* INCLUDE_CTB69000VGA */
    
#   endif /* INCLUDE_PC_CONSOLE */
    
    }
#endif /* INCLUDE_IACSFL */

#ifdef INCLUDE_USB
    /*
     * Since the Pentium BSPs do not rely on pciAutoCfg, sysUsbOhciInit
     * must be called to update the MMU mapping for the ohci device.
     * Please Note: INCLUDE_USB is not supported for boot_rom images.
     */
    sysUsbOhciPciInit ();
#endif /* INCLUDE_USB */

    /* initializes the serial devices */

    sysSerialHwInit ();      /* initialize serial data structure */


#ifdef INCLUDE_WINDML

    sysWindMLHwInit ();

#endif /* INCLUDE_WINDML */


#ifdef VX_POWER_MANAGEMENT
    /*
     * initializes Power Management Mode
     * VX_POWER_MODE_DEFAULT is defined in config.h
     */
    vxPowerModeSet(VX_POWER_MODE_DEFAULT);
#endif /* VX_POWER_MANAGEMENT */

    }

/*******************************************************************************
*
* sysHwInit2 - additional system configuration and initialization
*
* This routine connects system interrupts and does any additional
* configuration necessary.
*
* RETURNS: N/A
*/

void sysHwInit2 (void)

    {

#if	defined (INCLUDE_ADD_BOOTMEM)

    /*
     * We memAddToPool some upper memory into any low memory
     * x86 "rom" images pool.  The x86 low memory images reside
     * from 0x8000 to 0xa0000.  By memAddToPool'ing some upper
     * memory here, we allow devices a larger pool to swim within.
     * (SPR#21338).  This is no longer performed in bootConfig.c
     */

#   if (ADDED_BOOTMEM_SIZE != 0x0)
 
    /*
     * if &end (compiler symbol) is in lower memory, then we assume 
     * this is a low memory image, and add some upper memory to the pool.
     */
 
    if ((UINT32)(&end) < 0x100000)
        {
        /* Only do this if there is enough memory. Default is 4MB min. */
 
        if ((UINT32)(memTopPhys) >= (0x00200000 + ADDED_BOOTMEM_SIZE))
            {
            memAddToPool ((char *)memTopPhys - ADDED_BOOTMEM_SIZE,
                          ADDED_BOOTMEM_SIZE);
            }
        }
#   endif /* (ADDED_BOOTMEM_SIZE !=0) */
#endif	/* INCLUDE_ADD_BOOTMEM defined */
 
    /* connect sys clock interrupt and auxiliary clock interrupt*/

#ifdef	INCLUDE_APIC_TIMER
    (void)intConnect (INUM_TO_IVEC (INT_NUM_LOAPIC_TIMER), sysClkInt, 0);
#   ifdef PIT0_FOR_AUX
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (PIT0_INT_LVL)), sysAuxClkInt, 0);
#   else
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (RTC_INT_LVL)), sysAuxClkInt, 0);
#   endif /* PIT0_FOR_AUX */
#else
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (PIT0_INT_LVL)), sysClkInt, 0);
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (RTC_INT_LVL)), sysAuxClkInt, 0);
#endif	/* INCLUDE_APIC_TIMER */

    /* connect serial interrupt */  

    sysSerialHwInit2();

    /* connect stray(spurious/phantom) interrupt */  

#if     defined(VIRTUAL_WIRE_MODE)
    (void)intConnect (INUM_TO_IVEC (INT_NUM_LOAPIC_SPURIOUS), sysStrayInt, 0);
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (LPT_INT_LVL)), sysStrayInt, 0);
#elif   defined(SYMMETRIC_IO_MODE)
    (void)intConnect (INUM_TO_IVEC (INT_NUM_LOAPIC_SPURIOUS), sysStrayInt, 0);
#else
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (LPT_INT_LVL)), sysStrayInt, 0);
    (void)intConnect (INUM_TO_IVEC (INT_NUM_GET (PIC_SLAVE_STRAY_INT_LVL)), 
		      sysStrayInt, 0);
#endif  /* defined(VIRTUAL_WIRE_MODE) */

#ifdef	INCLUDE_PC_CONSOLE

    /* connect keyboard Controller 8042 chip interrupt */

    (void) intConnect (INUM_TO_IVEC (INT_NUM_GET (KBD_INT_LVL)), kbdIntr, 0);

#endif	/* INCLUDE_PC_CONSOLE */

#if	defined (TGT_CPU) && defined (SYMMETRIC_IO_MODE)

    /* init IPI vectors, connect IPI handler up to IPI_MAX_HANDLERS (=8) */

    ipiVecInit (INT_NUM_LOAPIC_IPI);

    ipiConnect ((INT_NUM_LOAPIC_IPI + 0), ipiHandlerShutdown);
    ipiConnect ((INT_NUM_LOAPIC_IPI + 1), ipiHandlerTscReset);
    ipiConnect ((INT_NUM_LOAPIC_IPI + 2), ipiHandlerTlbFlush);

#endif	/* defined (TGT_CPU) && defined (SYMMETRIC_IO_MODE) */

#ifdef	INCLUDE_THERM_MONITOR
    sysThermInit ();
#endif	/* INCLUDE_THERM_MONITOR */

#ifdef	INCLUDE_DEBUG_STORE
    sysDbgStrInit ();
#endif	/* INCLUDE_DEBUG_STORE */

    }

#ifdef	LOCAL_MEM_AUTOSIZE

/*******************************************************************************
*
* WRITE_MEMORY_TEST_PATTERN
*
* This routine writes the memory test pattern used in the sysPhysMemTop()
* memory auto-size algorithm.  12 bytes of data stored at <pTestAddr> are
* written to <pSaveAddr> before a 12-byte test pattern is written to
* <pTestAddr>.
*
* RETURNS: N/a
*
* SEE ALSO:  RESTORE_MEMORY_TEST_ADDRS()
*/
__inline__ static void WRITE_MEMORY_TEST_PATTERN
    (
    int * pTestAddr,
    int * pSaveAddr
    )
    {
    pSaveAddr[0] = pTestAddr[0];
    pSaveAddr[1] = pTestAddr[1];
    pSaveAddr[2] = pTestAddr[2];

    pTestAddr[0] = TEST_PATTERN_A;
    pTestAddr[1] = TEST_PATTERN_B;
    pTestAddr[2] = TEST_PATTERN_C;

    cacheFlush (DATA_CACHE, pTestAddr, 16);
    }

/*******************************************************************************
*
* RESTORE_MEMORY_TEST_ADDRS
*
* This routine restores memory test locations which are modified in the
* sysPhysMemTop() memory auto-size algorithm.  12 bytes of data stored at
* <pSaveAddr> are written to <pTestAddr>.
*
* RETURNS: N/a
*
* SEE ALSO:  WRITE_MEMORY_TEST_PATTERN()
*/
__inline__ static void RESTORE_MEMORY_TEST_ADDRS
    (
    int *       pTestAddr,
    const int * pSaveAddr
    )
    {
    pTestAddr[0] = pSaveAddr[0];
    pTestAddr[1] = pSaveAddr[1];
    pTestAddr[2] = pSaveAddr[2];
    }
#endif	/* LOCAL_MEM_AUTOSIZE */

/*******************************************************************************
*
* sysPhysMemTop - get the address of the top of physical memory
*
* This routine returns the address of the first missing byte of memory,
* which indicates the top of physical memory.
*
* INTERNAL
* The memory auto-size logic assumes that the manifest constant PHYS_MEM_MAX
* specifies the total size in bytes of the processor's physical address space.
* In the case of IA-32 processors, PHYS_MEM_MAX will be 4GB (2^32 bytes) or
* 64GB (2^36 bytes) if the 36-bit Physical Address Extension (PAE) is enabled
* on select processor models.  However, because the tool-chain and sysMemTop()
* API are 32-bit, this routine currently will not auto-size a 36-bit address
* space.  Moreover, this routine will not return the memory top of a platform
* with a memory device using a full 2^32 bytes of address space, as the memory
* top of such a device would be a 33-bit value.
*
* When paging is used, the processor divides the linear address space into
* fixed-size pages (of 4KB, 2MB, or 4MB in length) that can be mapped into
* physical memory and/or disk storage.  The auto-size algorithm organizes
* the physical address space using the same concept.  That is, rather than
* treating the address space as an array of bytes, the memory auto-size
* code treats the address space as an array of equal-sized pages.
*
* The auto-size algorithm attempts to locate the base-address of the first
* non-existant page address in the physical address space.  This is done by
* writing, and then reading, a test pattern to each page base-address in the
* address space.  If the test pattern is not read back from a page, it is
* assumed that the address does not physically exist.
*
* As the installed physical memory could be potentially quite large, the
* auto-size code attempts a few optimizations, chief among these being a
* binary (as opposed to linear) search of the page array (ie. address space).
* An additional optimization is obtained by avoiding a search on memory
* that _must_ exist; namely, the memory storing the VxWorks boot image or
* RTOS image from whence this routine will execute.
*
* In the case of VxWorks boot and RTOS images for IA-32, the last byte of the
* image section loaded highest in memory is assumed to be indicated by the
* address of a symbol, named <end>, which is typically supplied by the linker
* (more precisely, the linker script) used to build the image.  The search
* for remaining extant physical page addresses on the system will use the
* address of the first page following the <end> symbol, or a page-aligned
* address no lower than physical memory location 0x100000 (1Mb), as a lower
* bound on the search.  All memory locations below physical address 0x100000
* are assumed to be reserved existing target memory.
*
* RETURNS:  The address of the top of physical memory.
*/
char * sysPhysMemTop (void)
    {
    PHYS_MEM_DESC * pMmu;       /* points to memory desc. table entries */
    char            gdtr[6];    /* stores a copy of the GDT */

    BOOL            found = FALSE;


    if (memTopPhys != NULL)
        {
        return (memTopPhys);
        }


#ifdef	LOCAL_MEM_AUTOSIZE
    {
    /* Do not use a page-sized stride larger than 4Kb, as the end of usable
     * memory could possibly be within a 2Mb or 4Mb page memory range.
     */

    const UINT32 pageSize = PAGE_SIZE_4KB;

    /* The lower bound for the probe will be the page-aligned VxWorks
     * end-of-image address, or a page-aligned address no less than
     * the 1Mb physical address.
     */

    UINT8 * pPage = (UINT8 *) ROUND_UP (((UINT32)(&end) > 0x100000) ?
                          (UINT32)(&end) : (0x100000), pageSize);


    /* Subtract the number of used pages from the total number of pages
     * possible in the address space.  The resulting value is the total
     * number of pages that could possibly populate the remaining address
     * space above <pPage>.
     */

    const UINT32 pageNoUsed  = ((UINT32) pPage / pageSize);
    const UINT32 pageNoTotal = (PHYS_MEM_MAX / pageSize) - pageNoUsed;
    UINT32       delta       = HALF (pageNoTotal);

    int temp[4];


    /* find out the actual size of the memory (up to PHYS_MEM_MAX) */

    for (pPage += (pageSize * delta); delta != 0; delta >>= 1)
        {
        WRITE_MEMORY_TEST_PATTERN ((int *) pPage, &temp[0]);

        if (*((int *) pPage) != TEST_PATTERN_A)
            {
            /* The test pattern was not written, so assume that <pPage> is the
             * base address of a page beyond available memory.  Test the
             * next lowest page.  If the test pattern is writable there, assume
             * that <pPage> is the address of the first byte beyond the last
             * addressable page.
             */

            UINT8 * pPrevPage = (UINT8 *)((UINT32) pPage - pageSize);

            WRITE_MEMORY_TEST_PATTERN ((int *) pPrevPage, &temp[0]);

            if (*((int *) pPrevPage) == TEST_PATTERN_A)
                {
                RESTORE_MEMORY_TEST_ADDRS ((int *) pPrevPage, &temp[0]);

                memTopPhys = pPage;
                found      = TRUE;
                break;
                }

            pPage -= (pageSize * HALF (delta));
            }
        else
            {
            /* The test pattern was written, so assume that <pPage> is the base
             * address of a page in available memory.  Test the next highest
             * page.  If the test pattern is not writable there, assume that
             * <pNextPage> is the address of the first byte beyond that last
             * addressable page.
             */

            UINT8 * pNextPage = (UINT8 *)((UINT32) pPage + pageSize);

            RESTORE_MEMORY_TEST_ADDRS ((int *) pPage, &temp[0]);

            WRITE_MEMORY_TEST_PATTERN ((int *) pNextPage, &temp[0]);

            if (*((int *) pNextPage) != TEST_PATTERN_A)
                {
                memTopPhys = pNextPage;
                found      = TRUE;
                break;
                }

            RESTORE_MEMORY_TEST_ADDRS ((int *) pNextPage, &temp[0]);

            pPage += (pageSize * HALF (delta));
            }
        }
    }
#endif	/* LOCAL_MEM_AUTOSIZE */


    if (!found)
        {
        memTopPhys = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
        }

    /* copy the global descriptor table from RAM/ROM to RAM */

    bcopy ((char *)sysGdt, (char *)pSysGdt, GDT_ENTRIES * sizeof(GDT));
    *(short *)&gdtr[0]	= GDT_ENTRIES * sizeof(GDT) - 1;
    *(int *)&gdtr[2]	= (int)pSysGdt;

/* 
 * We assume that there are no memory mapped IO addresses
 * above the "memTopPhys" if INCLUDE_PCI is not defined.
 * Thus we set the "limit" to get the General Protection Fault
 * when the memory above the "memTopPhys" is accessed.
 */

#if	!defined (INCLUDE_PCI) && \
	!defined (INCLUDE_MMU_BASIC) && !defined (INCLUDE_MMU_FULL)
    {
    int   ix;
    GDT * pGdt  = pSysGdt;
    int   limit = (((UINT32) memTopPhys) >> 12) - 1;

    for (ix = 1; ix < GDT_ENTRIES; ++ix)
        {
        ++pGdt;
        pGdt->limit00 = limit & 0x0ffff;
        pGdt->limit01 = ((limit & 0xf0000) >> 16) | (pGdt->limit01 & 0xf0);
        }
    }
#endif	/* INCLUDE_PCI */

    /* load the global descriptor table. set the MMU table */

    sysLoadGdt (gdtr);

#ifdef	FAST_REBOOT

    /* 
     * save the brand new bootrom image that will be protected by MMU.
     * The last 2 bytes of ROM_SIZE are for the checksum. 
     * - compression would minimize the DRAM usage.
     * - when restore, jumping to the saved image would be faster.
     */

    memTopPhys -= ROM_SIZE;
    bcopy ((char *)ROM_BASE_ADRS, memTopPhys, ROM_SIZE);
    *(UINT16 *)(memTopPhys + ROM_SIZE - 2) = 
        checksum ((UINT16 *)memTopPhys, ROM_SIZE - 2);
    memRom = memTopPhys;		/* remember it */

#endif	/* FAST_REBOOT */

    /* set the MMU descriptor table */

    (UINT32)memTopPhys &= ~(VM_PAGE_SIZE - 1);	/* VM_PAGE_SIZE aligned */

#if	(VM_PAGE_SIZE == PAGE_SIZE_4KB)
    pMmu = &sysPhysMemDesc[4];		/* 5th entry: above 1.5MB upper memory */
    pMmu->len = (UINT32) memTopPhys - (UINT32) pMmu->physicalAddr;
#else	/* (VM_PAGE_SIZE == PAGE_SIZE_4KB) */
    pMmu = &sysPhysMemDesc[2];		/* 3rd entry: above 8MB upper memory */
    pMmu->len = (UINT32) memTopPhys - (UINT32) pMmu->physicalAddr;
#endif	/* (VM_PAGE_SIZE == PAGE_SIZE_4KB) */

    return (memTopPhys);
    }

/*******************************************************************************
*
* sysMemTop - get the address of the top of VxWorks memory
*
* This routine returns a pointer to the first byte of memory not
* controlled or used by VxWorks.
*
* The user can reserve memory space by defining the macro USER_RESERVED_MEM
* in config.h.  This routine returns the address of the reserved memory
* area.  The value of USER_RESERVED_MEM is in bytes.
*
* RETURNS: The address of the top of VxWorks memory.
*/

char * sysMemTop (void)
    {
    static char * memTop = NULL;

    if (memTop == NULL)
        {
        memTop = sysPhysMemTop () - USER_RESERVED_MEM;

        if ((UINT32)(&end) < 0x100000)		/* this is for bootrom */
            memTop = (char *)EBDA_START;	/* preserve the MP table */
        else if ((UINT32)(&end) < RAM_LOW_ADRS)	/* bootrom in upper mem */
            memTop = (char *)(RAM_LOW_ADRS & 0xfff00000);
        }

    return (memTop);
    }

/*******************************************************************************
*
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  It is usually called
* only by reboot() -- which services ^X -- and by bus errors at interrupt
* level.  However, in some circumstances, the user may wish to introduce a
* new <startType> to enable special boot ROM facilities.
*
* RETURNS: Does not return.
*/

STATUS sysToMonitor
    (
    int startType   /* passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR pEntry;
    INT16 * pDst;
    
    VM_ENABLE (FALSE);			/* disbale MMU */

#if	(CPU == PENTIUM) || (CPU == PENTIUM2) || (CPU == PENTIUM3) || \
	(CPU == PENTIUM4)

    pentiumMsrInit ();			/* initialize MSRs */

#endif	/* (CPU == PENTIUM) || (CPU == PENTIUM[234]) */

    /* decide a destination RAM address and the entry point */

    if ((UINT32)(&end) > RAM_LOW_ADRS)
	{
	pDst = (short *)RAM_HIGH_ADRS;	/* copy it in lower mem */
	pEntry = (FUNCPTR)(RAM_HIGH_ADRS + ROM_WARM_HIGH);
	}
    else
	{
	pDst = (short *)RAM_LOW_ADRS;	/* copy it in upper mem */
	pEntry = (FUNCPTR)(RAM_LOW_ADRS + ROM_WARM_LOW);
	}

#ifdef	FAST_REBOOT

    /* restore the saved brand new bootrom image, then jump */

    if ((memRom != NULL) &&
        (*(UINT16 *)(memRom + ROM_SIZE - 2) == 
         checksum ((UINT16 *)memRom, ROM_SIZE - 2)))
        {
        INT32 ix;
        INT32 * dst = (INT32 *) ROM_TEXT_ADRS;
        INT32 * src = (INT32 *) memRom;

	pEntry = (FUNCPTR)(ROM_TEXT_ADRS + ROM_WARM_HIGH);

        for (ix = 0; ix < (ROM_SIZE >> 2); ix++)
            *dst++ = *src++;

        goto sysToMonitorJump;
        }

#endif	/* FAST_REBOOT */

    /* disable 16-bit memory access */

#ifdef  INCLUDE_ELC
    sysOutByte (IO_ADRS_ELC + 5, sysInByte (IO_ADRS_ELC + 5) & ~0x80);
#endif  /* INCLUDE_ELC */

#ifdef	INCLUDE_ROMCARD
    {
    INT32 ix;
    INT32 iy;
    INT32 iz;
    char buf[ROM_SIGNATURE_SIZE];
    short *pSrc;

    /* copy EPROM to RAM and jump, if there is a VxWorks EPROM */

    for (ix = 0; ix < NELEMENTS(sysRomBase); ix++)
	{
	bcopyBytes ((char *)sysRomBase[ix], buf, ROM_SIGNATURE_SIZE);
	if (strncmp (sysRomSignature, buf, ROM_SIGNATURE_SIZE) == 0)
	    {
	    for (iy = 0; iy < 1024; iy++)
		{
		*sysRomBase[ix] = iy;	/* map the moveable window */
		pSrc = (short *)((int)sysRomBase[ix] + 0x200);
	        for (iz = 0; iz < 256; iz++)
		    *pDst++ = *pSrc++;
		}
	    goto sysToMonitorJump;	/* jump to the entry point */
	    }
	}
    }
#endif	/* INCLUDE_ROMCARD */

#ifdef  INCLUDE_IACSFL
    {
    unsigned int * romSize = (unsigned int *)0xffffffe0;
    unsigned int * dest = (unsigned int *) pDst;
    unsigned int * src = (unsigned int *) (0 - *romSize);
    int i = 0;
    
    for (i = 0; i < (*romSize >> 2); i++)
        *dest++ = *src++;

    goto sysToMonitorJump;
    }
#endif /* INCLUDE_IACSFL */

#if	(defined(INCLUDE_FD) || defined(INCLUDE_ATA) || defined(INCLUDE_TFFS))
    if ((sysWarmType == SYS_WARM_FD) || (sysWarmType == SYS_WARM_ATA) || 
	(sysWarmType == SYS_WARM_TFFS))
	{
	u_char * pChar;

        /* check to see if device exists, if so don't create it */

        if (NULL != dosFsVolDescGet(BOOTROM_DIR, &pChar))
            {
            goto bootDevExists; /* avoid attempt to recreate device */
            }
        }
#endif /* defined(INCLUDE_FD) || defined(INCLUDE_ATA) || defined(INCLUDE_TFFS) */

#ifdef	INCLUDE_FD
    if (sysWarmType == SYS_WARM_FD)
	{
	IMPORT int dosFsDrvNum;

        fdDrv (INT_NUM_GET (FD_INT_LVL), FD_INT_LVL);	/* initialize floppy */
	if (dosFsDrvNum == ERROR)
    	    dosFsInit (NUM_DOSFS_FILES);	/* initialize DOS-FS */

	if (usrFdConfig (sysWarmFdDrive, sysWarmFdType, BOOTROM_DIR) == ERROR)
	    {
	    printErr ("usrFdConfig failed.\n");
	    return (ERROR);
	    }
	}
#endif	/* INCLUDE_FD */

#ifdef	INCLUDE_ATA
    if (sysWarmType == SYS_WARM_ATA)
	{
	ATA_RESOURCE *pAtaResource  = &ataResources[sysWarmAtaCtrl];
	IMPORT int dosFsDrvNum;

        if (ataDrv (sysWarmAtaCtrl, pAtaResource->drives,
	    pAtaResource->intVector, pAtaResource->intLevel,
	    pAtaResource->configType, pAtaResource->semTimeout,
	    pAtaResource->wdgTimeout) == ERROR)	/* initialize ATA/IDE disk */
	    {
	    printErr ("Could not initialize.\n");
	    return (ERROR);
	    }
	if (dosFsDrvNum == ERROR)
    	    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

	if (ERROR == usrAtaConfig (sysWarmAtaCtrl, 
                                   sysWarmAtaDrive, BOOTROM_DIR))
	    {
	    printErr ("usrAtaConfig failed.\n");
	    return (ERROR);
	    }
	}
#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_TFFS
    if (sysWarmType == SYS_WARM_TFFS)
	{
	IMPORT int dosFsDrvNum;

        tffsDrv ();				/* initialize TFFS */
	if (dosFsDrvNum == ERROR)
    	    dosFsInit (NUM_DOSFS_FILES);	/* initialize DOS-FS */

	if (usrTffsConfig (sysWarmTffsDrive, FALSE, BOOTROM_DIR) == ERROR)
	    {
	    printErr ("usrTffsConfig failed.\n");
	    return (ERROR);
	    }
	}
#endif	/* INCLUDE_TFFS */

#if	(defined(INCLUDE_FD) || defined(INCLUDE_ATA) || defined(INCLUDE_TFFS))

bootDevExists:  /* reboot device exists */

    if ((sysWarmType == SYS_WARM_FD) || (sysWarmType == SYS_WARM_ATA) || 
        (sysWarmType == SYS_WARM_TFFS))
        {
        int  fd;
        BOOL hasAoutHdr = FALSE;

        if ((fd = open (BOOTROM_BIN, O_RDONLY, 0644)) == ERROR)
            {
            printErr ("Error opening file \"%s\", trying \"%s\" ... \n",
                      BOOTROM_BIN, BOOTROM_AOUT);

            if ((fd = open (BOOTROM_AOUT, O_RDONLY, 0644)) == ERROR)
                {
                printErr ("Error opening file \"%s\"\n", BOOTROM_AOUT);
                return (ERROR);
                }
            if (read (fd, (char *) pDst, 0x20) == ERROR) /* a.out header */
                {
                printErr ("Error reading file \"%s\"\n", BOOTROM_AOUT);
                return (ERROR);
                }

            hasAoutHdr = TRUE;
            }

        /* read text and data, write them to the memory */

        if (read (fd, (char *) pDst, 0x98000) == ERROR)
            {
            printErr ("Error reading file \"%s\"\n",
                      hasAoutHdr ?  BOOTROM_AOUT : BOOTROM_BIN);

            return (ERROR);
            }

#ifdef INCLUDE_FD
        /* explicitly release floppy disk, SPR#30280 */

        if (SYS_WARM_FD == sysWarmType)
            {
            sysOutByte(FD_REG_OUTPUT,(FD_DOR_CLEAR_RESET | FD_DOR_DMA_ENABLE));
            sysDelay ();
            }
#endif /* INCLUDE_FD */

	goto sysToMonitorJump;		/* jump to the entry point */
	}
#endif	/* (INCLUDE_FD) || (INCLUDE_ATA) || (INCLUDE_TFFS) */

    /* perform the cold boot since the warm boot is not possible */

    {
    intLock ();

#ifdef INCLUDE_ELC
    elcdetach (0);
#endif /* INCLUDE_ELC */

    sysClkDisable ();

    sysWait ();
    sysOutByte (COMMAND_8042, 0xfe);	/* assert SYSRESET */
    sysWait ();
    sysOutByte (COMMAND_8042, 0xff);	/* NULL command */

    sysReboot ();			/* crash the global descriptor table */

    return (OK);	/* in case we ever continue from ROM monitor */
    }

    /* jump to the warm start entry point */

sysToMonitorJump:		/* cleanup and jump to the entry point */

    sysClkDisable ();		/* disable the system clock interrupt */
    sysIntLock ();		/* lock the used/owned interrupts */

#if	defined (SYMMETRIC_IO_MODE) || defined (VIRTUAL_WIRE_MODE)

    intLock ();			/* LOCK INTERRUPTS */
    loApicEnable (FALSE);	/* disable the Local APIC */

#   ifdef SYMMETRIC_IO_MODE

    if (sysBp)
        ioApicEnable (FALSE);	/* disable the IO APIC */

#   endif /* SYMMETRIC_IO_MODE */
#endif	/* defined (SYMMETRIC_IO_MODE) || defined (VIRTUAL_WIRE_MODE) */

    (*pEntry) (startType);

    return (OK);	/* in case we ever continue from ROM monitor */
    }

/*******************************************************************************
*
* sysIntInitPIC - initialize the interrupt controller
*
* This routine initializes the interrupt controller.
*
* RETURNS: N/A
*
* ARGSUSED0
*/

LOCAL void sysIntInitPIC (void)
    {

#if	defined(VIRTUAL_WIRE_MODE)
    {
    UINT32 addrLo;	/* page aligned Local APIC Base Address */
    UINT32 lengthLo;	/* length of Local APIC registers */

    loApicInit ();
    i8259Init ();

    /* add an entry to the sysMmuPhysDesc[] for Local APIC */

    addrLo   = ((UINT32)loApicBase / VM_PAGE_SIZE) * VM_PAGE_SIZE;
    lengthLo = (UINT32)loApicBase - addrLo + LOAPIC_LENGTH;
    if ((lengthLo % VM_PAGE_SIZE) != 0)
	lengthLo = (lengthLo / VM_PAGE_SIZE + 1) * VM_PAGE_SIZE;
    
    sysMmuMapAdd ((void *)addrLo, lengthLo, 
		  VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_IO);
    }
#elif	defined(SYMMETRIC_IO_MODE)
    {
    UINT32 addrLo;	/* page aligned Local APIC Base Address */
    UINT32 addrIo;	/* page aligned IO APIC Base Address */
    UINT32 lengthLo;	/* length of Local APIC registers */
    UINT32 lengthIo;	/* length of IO APIC registers */

    loApicInit ();
    i8259Init ();
    ioApicInit ();

    /* add an entry to the sysMmuPhysDesc[] for Local APIC and IO APIC */

    addrLo   = ((UINT32)loApicBase / VM_PAGE_SIZE) * VM_PAGE_SIZE;
    lengthLo = (UINT32)loApicBase - addrLo + LOAPIC_LENGTH;
    if ((lengthLo % VM_PAGE_SIZE) != 0)
	lengthLo = (lengthLo / VM_PAGE_SIZE + 1) * VM_PAGE_SIZE;
    
    addrIo   = ((UINT32)ioApicBase / VM_PAGE_SIZE) * VM_PAGE_SIZE;
    lengthIo = (UINT32)ioApicBase - addrIo + IOAPIC_LENGTH;
    if ((lengthIo % VM_PAGE_SIZE) != 0)
	lengthIo = (lengthIo / VM_PAGE_SIZE + 1) * VM_PAGE_SIZE;
    
    if ((addrLo == addrIo) ||
        ((addrLo < addrIo) && ((addrLo + lengthLo) >= addrIo)) || 
        ((addrIo < addrLo) && ((addrIo + lengthIo) >= addrLo)))
	{
	UINT32 addr   = min (addrLo, addrIo);
	UINT32 length = max ((addrLo + lengthLo), (addrIo + lengthIo)) - addr;

        sysMmuMapAdd ((void *)addr, length,
		      VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_IO);
	}
    else
	{
        sysMmuMapAdd ((void *)addrLo, lengthLo, 
		      VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_IO);
        sysMmuMapAdd ((void *)addrIo, lengthIo, 
		      VM_STATE_MASK_FOR_ALL, VM_STATE_FOR_IO);
	}
    }
#else
    i8259Init ();
#endif	/* defined(VIRTUAL_WIRE_MODE) */
    }

/*******************************************************************************
*
* sysIntLock - lock out all interrupts
*
* This routine saves the mask and locks out all interrupts.
*
* SEE ALSO: sysIntUnlock()
*
* ARGSUSED0
*/

VOID sysIntLock (void)

    {
    INT32 oldLevel = intLock ();	/* LOCK INTERRUPTS */

#if	defined(VIRTUAL_WIRE_MODE)
    loApicIntLock ();
    i8259IntLock ();
#elif	defined(SYMMETRIC_IO_MODE)
    loApicIntLock ();
    ioApicIntLock ();
#else
    i8259IntLock ();
#endif	/* defined(VIRTUAL_WIRE_MODE) */

    intUnlock (oldLevel);		/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* sysIntUnlock - unlock the PIC interrupts
*
* This routine restores the mask and unlocks the PIC interrupts
*
* SEE ALSO: sysIntLock()
*
* ARGSUSED0
*/

VOID sysIntUnlock (void)

    {
    INT32 oldLevel = intLock ();	/* LOCK INTERRUPTS */

#if	defined(VIRTUAL_WIRE_MODE)
    loApicIntUnlock ();
    i8259IntUnlock ();
#elif	defined(SYMMETRIC_IO_MODE)
    loApicIntUnlock ();
    ioApicIntUnlock ();
#else
    i8259IntUnlock ();
#endif	/* defined(VIRTUAL_WIRE_MODE) */

    intUnlock (oldLevel);		/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* sysIntDisablePIC - disable a bus interrupt level
*
* This routine disables a specified bus interrupt level.
*
* RETURNS: OK, or ERROR if failed.
*
* ARGSUSED0
*/

STATUS sysIntDisablePIC
    (
    int irqNo		/* IRQ(PIC) or INTIN(APIC) number to disable */
    )
    {

#if	defined(VIRTUAL_WIRE_MODE)
    return (i8259IntDisable (irqNo));
#elif	defined(SYMMETRIC_IO_MODE)
    return (ioApicIntDisable (irqNo));
#else
    return (i8259IntDisable (irqNo));
#endif	/* defined(VIRTUAL_WIRE_MODE) */
    }

/*******************************************************************************
*
* sysIntEnablePIC - enable a bus interrupt level
*
* This routine enables a specified bus interrupt level.
*
* RETURNS: OK, or ERROR if failed.
*
* ARGSUSED0
*/

STATUS sysIntEnablePIC
    (
    int irqNo		/* IRQ(PIC) or INTIN(APIC) number to enable */
    )
    {

#if	defined(VIRTUAL_WIRE_MODE)
    return (i8259IntEnable (irqNo));
#elif	defined(SYMMETRIC_IO_MODE)
    return (ioApicIntEnable (irqNo));
#else
    return (i8259IntEnable (irqNo));
#endif	/* defined(VIRTUAL_WIRE_MODE) */
    }

/*******************************************************************************
*
* sysIntEoiGet - get EOI/BOI function and its parameter
*
* This routine gets EOI function and its parameter for the interrupt controller.
* If returned EOI/BOI function is NULL, intHandlerCreateX86() replaces 
* "call _routineBoi/Eoi" in intConnectCode[] with NOP instruction.
*
* RETURNS: N/A
*
* ARGSUSED0
*/

LOCAL void sysIntEoiGet
    (
    VOIDFUNCPTR * vector,	/* interrupt vector to attach to */
    VOIDFUNCPTR * routineBoi,	/* BOI function */
    int * parameterBoi,		/* a parameter of the BOI function */
    VOIDFUNCPTR * routineEoi,	/* EOI function */
    int * parameterEoi		/* a parameter of the EOI function */
    )
    {
    int intNum = IVEC_TO_INUM (vector);
    int irqNo;

    /* set default BOI routine & parameter */

    *routineBoi   = NULL;
    *parameterBoi = 0;

    /* find a match in sysInumTbl[] */

    for (irqNo = 0; irqNo < sysInumTblNumEnt; irqNo++)
	{
	if (sysInumTbl[irqNo] == intNum)
	    break;
	}

    *parameterEoi = irqNo;	/* irq is sysInumTblNumEnt, if no match */

#ifdef	SYMMETRIC_IO_MODE

    if (irqNo < ioApicRedEntries)	/* IRQ belongs to the IO APIC */
        *routineEoi = ioApicIntEoi;	/* set IO APIC's EOI routine */
    else				/* IRQ belongs to the Local APIC */
        {
        if (intNum == INT_NUM_LOAPIC_SPURIOUS)
            *routineEoi = NULL;		/* no EOI is necessary */
        else
            *routineEoi = loApicIntEoi;	/* set Local APIC's EOI routine */
	}

#else

#   ifdef VIRTUAL_WIRE_MODE

    if (irqNo >= N_PIC_IRQS)		/* IRQ belongs to the Local APIC */
        {
        if (intNum == INT_NUM_LOAPIC_SPURIOUS)
            *routineEoi = NULL;		/* no EOI is necessary */
        else
            *routineEoi = loApicIntEoi;	/* set Local APIC's EOI routine */
	return;
	}

#   endif /* VIRTUAL_WIRE_MODE */

    /* set the [BE]OI parameter for the master & slave PIC */

    *parameterBoi = irqNo;
    *parameterEoi = irqNo;

    /* set the right BOI routine */

    if (irqNo == 0)			/* IRQ0 BOI routine */
	{
#if	(PIC_IRQ0_MODE == PIC_AUTO_EOI)
        *routineBoi   = NULL;
#elif	(PIC_IRQ0_MODE == PIC_EARLY_EOI_IRQ0)
        *routineBoi   = i8259IntBoiEem;
#elif	(PIC_IRQ0_MODE == PIC_SPECIAL_MASK_MODE_IRQ0)
        *routineBoi   = i8259IntBoiSmm;
#else
        *routineBoi   = NULL;
#endif	/* (PIC_IRQ0_MODE == PIC_AUTO_EOI) */
	}
    else if ((irqNo == PIC_MASTER_STRAY_INT_LVL) || 
	     (irqNo == PIC_SLAVE_STRAY_INT_LVL))
	{
        *routineBoi   = i8259IntBoi;
	}

    /* set the right EOI routine */

    if (irqNo == 0)			/* IRQ0 EOI routine */
	{
#if	(PIC_IRQ0_MODE == PIC_AUTO_EOI) || \
	(PIC_IRQ0_MODE == PIC_EARLY_EOI_IRQ0)
        *routineEoi   = NULL;
#elif	(PIC_IRQ0_MODE == PIC_SPECIAL_MASK_MODE_IRQ0)
        *routineEoi   = i8259IntEoiSmm;
#else
        *routineEoi   = i8259IntEoiMaster;
#endif	/* (PIC_IRQ0_MODE == PIC_AUTO_EOI) || (PIC_EARLY_EOI_IRQ0) */
	}
    else if (irqNo < 8)			/* IRQ[1-7] EOI routine */
	{
#if	(PIC_IRQ0_MODE == PIC_AUTO_EOI)
        *routineEoi   = NULL;
#else
        *routineEoi   = i8259IntEoiMaster;
#endif	/* (PIC_IRQ0_MODE == PIC_AUTO_EOI) */
	}
    else				/* IRQ[8-15] EOI routine */
	{
#if	defined (PIC_SPECIAL_FULLY_NESTED_MODE)
        *routineEoi   = i8259IntEoiSlaveSfnm;
#else
        *routineEoi   = i8259IntEoiSlaveNfnm;
#endif	/* defined (PIC_SPECIAL_FULLY_NESTED_MODE) */
	}

#endif	/* SYMMETRIC_IO_MODE */
    }

/*******************************************************************************
*
* sysIntLevel - get an IRQ(PIC) or INTIN(APIC) number in service
*
* This routine gets an IRQ(PIC) or INTIN(APIC) number in service.  
* We assume followings:
*   - this function is called in intEnt()
*   - IRQ number of the interrupt is at intConnectCode [29]
*
* RETURNS: 0 - (sysInumTblNumEnt - 1), or sysInumTblNumEnt if we failed to get it.
*
* ARGSUSED0
*/

int sysIntLevel 
    (
    int arg		/* parameter to get the stack pointer */
    )
    {
    UINT32 * pStack;
    UCHAR * pInst;
    INT32 ix;
    INT32 irqNo = sysInumTblNumEnt; /* return sysInumTblNumEnt if we failed */

    pStack = &arg;		/* get the stack pointer */
    pStack += 3;		/* skip pushed volitile registers */

    /* 
     * we are looking for a return address on the stack which point
     * to the next instruction of "call _intEnt" in the malloced stub.
     * Then get the irqNo at intConnectCode [29].
     */

    for (ix = 0; ix < 10; ix++, pStack++)
	{
	pInst = (UCHAR *)*pStack;		/* return address */
	if ((*pInst == 0x50) && 		/* intConnectCode [5] */
	    ((*(int *)(pInst - 4) + (int)pInst) == (int)intEnt))
	    {
    	    irqNo = *(int *)(pInst + 24);	/* intConnectCode [29] */
	    break;
	    }
	}

    return (irqNo);
    }

/****************************************************************************
*
* sysProcNumGet - get the processor number
*
* This routine returns the processor number for the CPU board, which is
* set with sysProcNumSet().
*
* RETURNS: The processor number for the CPU board.
*
* SEE ALSO: sysProcNumSet()
*/

int sysProcNumGet (void)
    {
    return (sysProcNum);
    }

/****************************************************************************
*
* sysProcNumSet - set the processor number
*
* Set the processor number for the CPU board.  Processor numbers should be
* unique on a single backplane.
*
* NOTE: By convention, only Processor 0 should dual-port its memory.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*/

void sysProcNumSet
    (
    int procNum		/* processor number */
    )
    {
    sysProcNum = procNum;
    }

/*******************************************************************************
*
* sysDelay - allow recovery time for port accesses
*
* This routine provides a brief delay used between accesses to the same serial
* port chip.
* 
* RETURNS: N/A
*/

void sysDelay (void)
    {
    char ix;

    ix = sysInByte (UNUSED_ISA_IO_ADDRESS);	/* it takes 720ns */
    }

/*******************************************************************************
*
* sysStrayInt - Do nothing for stray interrupts.
*
* Do nothing for stray interrupts.
*/

LOCAL void sysStrayInt (void)
    {
    sysStrayIntCount++;
    }

/*******************************************************************************
*
* sysMmuMapAdd - insert a new MMU mapping
*
* This routine will create a new <sysPhysMemDesc> table entry for a memory
* region of specified <length> in bytes and with a specified base
* <address>.  The <initialStateMask> and <initialState> parameters specify
* a PHYS_MEM_DESC type state mask and state for the memory region.
*
* CAVEATS
* This routine must be used before the <sysPhysMemDesc> table is
* referenced for the purpose of initializing the MMU or processor address
* space (us. in usrMmuInit()).
*
* The <length> in bytes will be rounded up to a multiple of VM_PAGE_SIZE
* bytes if necessary.
*
* The current implementation assumes a one-to-one mapping of physical to
* virtual addresses.
*
* RETURNS: OK or ERROR depending on availability of free mappings.
*
* SEE ALSO: vmLib
*/  

STATUS sysMmuMapAdd 
    (
    void * address,           /* memory region base address */
    UINT   length,            /* memory region length in bytes*/
    UINT   initialStateMask,  /* PHYS_MEM_DESC state mask */
    UINT   initialState       /* PHYS_MEM_DESC state */
    )
    {
    STATUS result = OK;

    PHYS_MEM_DESC * const pMmu = &sysPhysMemDesc[sysPhysMemDescNumEnt];


    if (pMmu->virtualAddr != (void *) DUMMY_VIRT_ADDR)
        {
        result = ERROR;
        }
    else
        {
        address = (void *)(((UINT32) address / VM_PAGE_SIZE) * VM_PAGE_SIZE);

        if ((length % VM_PAGE_SIZE) != 0)
            length = (length / VM_PAGE_SIZE + 1) * VM_PAGE_SIZE;

        pMmu->virtualAddr	= address;
        pMmu->physicalAddr	= address;
        pMmu->len		= length;
        pMmu->initialStateMask	= initialStateMask;
        pMmu->initialState	= initialState;
        sysPhysMemDescNumEnt	+= 1;
        }

    return (result);
    }
