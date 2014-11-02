/* pc386/config.h - PC {3,4}86/Pentium{,2,3,4} configuration header */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------

01a 2008-3-24 23:30    modify by frank(http://www.vxdev.com)
*/



#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
extern "C" {
#endif

/* BSP version/revision identification, before configAll.h */

#define BSP_VER_1_1	1	/* 1.2 is backward compatible with 1.1 */
#define BSP_VER_1_2	1
#define BSP_VERSION	"1.2"	/* Tornado 2.2 is 1.2 */
#define BSP_REV		"/3"	/* increment by whole numbers */

#include "configAll.h"
#include "pc.h"


/* BSP specific prototypes that must be in config.h */

#ifndef _ASMLANGUAGE
    IMPORT void sysHwInit0 (void);
    IMPORT UINT8 sysInumTbl[];		/* IRQ vs intNum table */
#endif

/* BSP specific initialisation (before cacheLibInit() is called) */

#define INCLUDE_SYS_HW_INIT_0
#define SYS_HW_INIT_0()         (sysHwInit0())

/* CPU auto detection with sysCpuProbe() that support Intel CPUs for now */

#define	INCLUDE_CPU_PROBE		/* define to perform sysCpuProbe() */
#ifndef	INCLUDE_CPU_PROBE
#   undef  CPU
#   define CPU		PENTIUM		/* for CPU conditionals in BSP */
#endif	/* INCLUDE_CPU_PROBE */
#define X86CPU_DEFAULT	X86CPU_PENTIUM	/* for sysProcessor set in BSP */

/* Default boot line */

#if	(CPU == I80386)
#define DEFAULT_BOOT_LINE \
	"fd=0,0(0,0)host:/fd0/vxWorks.st h=90.0.0.3 e=90.0.0.50 u=target"
#elif	(CPU == I80486)
#define DEFAULT_BOOT_LINE \
	"fd=0,0(0,0)host:/fd0/vxWorks.st h=90.0.0.3 e=90.0.0.50 u=target"
#elif	(CPU == PENTIUM)
#define DEFAULT_BOOT_LINE \
	"ata=0,0(0,0)host:vxWorks h=192.168.152.1 e=192.168.152.88 u=target pw=target o=lnPci"
#elif	(CPU == PENTIUM2)
#define DEFAULT_BOOT_LINE \
	"fd=0,0(0,0)host:/fd0/vxWorks.st h=90.0.0.3 e=90.0.0.50 u=target"
#elif	(CPU == PENTIUM3)
#define DEFAULT_BOOT_LINE \
	"fd=0,0(0,0)host:/fd0/vxWorks.st h=90.0.0.3 e=90.0.0.50 u=target"
#elif	(CPU == PENTIUM4)
#define DEFAULT_BOOT_LINE \
	"fd=0,0(0,0)host:/fd0/vxWorks.st h=90.0.0.3 e=90.0.0.50 u=target"
#endif	/* (CPU == I80386) */

/* Warm boot (reboot) devices and parameters */

#define SYS_WARM_BIOS 		0 	/* warm start from BIOS */
#define SYS_WARM_FD   		1 	/* warm start from FD */
#define SYS_WARM_ATA  		2	/* warm start from ATA */
#define SYS_WARM_TFFS  		3	/* warm start from DiskOnChip */
/*
#define SYS_WARM_TYPE		SYS_WARM_BIOS
修改为 
#define SYS_WARM_TYPE		SYS_WARM_ATA 
*/
#define SYS_WARM_TYPE		SYS_WARM_ATA 

#define SYS_WARM_FD_DRIVE       0       /* 0 = drive a:, 1 = b: */
#define SYS_WARM_FD_TYPE        0       /* 0 = 3.5" 2HD, 1 = 5.25" 2HD */
/*设置为 1*/
#define SYS_WARM_ATA_CTRL       1       /* controller 0 */
#define SYS_WARM_ATA_DRIVE      0       /* 0 = c:, 1 = d: */
#define SYS_WARM_TFFS_DRIVE     0       /* 0 = c: (DOC) */

/* Warm boot (reboot) device and filename strings */

/* 
 * BOOTROM_DIR is the device name for the device containing
 * the bootrom file. This string is used in sysToMonitor, sysLib.c 
 * in dosFsDevCreate().
 */

#define BOOTROM_DIR  "/vxboot/"

/* 
 * BOOTROM_BIN is the default path and file name to either a binary 
 * bootrom file or an A.OUT file with its 32 byte header stripped.
 * Note that the first part of this string must match BOOTROM_DIR
 * The "bootrom.sys" file name will work with VxLd 1.5.
 */

#define BOOTROM_BIN  "/vxboot/bootrom.sys"

/* 
 * BOOTROM_AOUT is that default path and file name of an A.OUT bootrom
 * _still containing_ its 32byte A.OUT header.   This is legacy code.
 * Note that the first part of this string must match BOOTROM_DIR
 * The "bootrom.dat" file name does not work with VxLd 1.5.
 */

#define BOOTROM_AOUT "/vxboot/bootrom.dat"

/* IDT entry type options */

#define SYS_INT_TRAPGATE 	0x0000ef00 	/* trap gate */
#define SYS_INT_INTGATE  	0x0000ee00 	/* int gate */

/* driver and file system options */

#define	INCLUDE_DOSFS		/* include dosFs file system */
#undef	INCLUDE_FD		/* include floppy disk driver */
#define	INCLUDE_ATA		/* include IDE/EIDE(ATA) hard disk driver */
#undef	INCLUDE_LPT		/* include parallel port driver */
#undef	INCLUDE_TIMESTAMP	/* include TIMESTAMP timer for Wind View */
#undef	INCLUDE_TFFS		/* include TrueFFS driver for Flash */
#undef	INCLUDE_PCMCIA		/* include PCMCIA driver */

/*
	设置硬盘，初始化文件系统
*/
#define DOSFS_NAMES_ATA_PRIMARY_MASTER ""		
#define DOSFS_NAMES_ATA_PRIMARY_SLAVE ""	
#define DOSFS_NAMES_ATA_SECONDARY_MASTER "/ata0a/"	
#define DOSFS_NAMES_ATA_SECONDARY_SLAVE ""

/* TFFS driver options */

#ifdef	INCLUDE_TFFS
#   define INCLUDE_SHOW_ROUTINES
#endif	/* INCLUDE_TFFS */

/* SCSI driver options */

#undef	INCLUDE_SCSI            /* include SCSI driver */
#undef	INCLUDE_AIC_7880        /* include AIC 7880 SCSI driver */
#undef	INCLUDE_SCSI_BOOT       /* include ability to boot from SCSI */
#undef	INCLUDE_CDROMFS         /* file system to be used */
#undef	INCLUDE_TAPEFS          /* file system to be used */
#undef	INCLUDE_SCSI2           /* select SCSI2 not SCSI1 */

/* Network driver options */

#define INCLUDE_END             /* Enhanced Network Driver Support */

#undef  INCLUDE_DEC21X40_END    /* (END) DEC 21x4x PCI interface */
#undef  INCLUDE_EL_3C90X_END    /* (END) 3Com Fast EtherLink XL PCI */
#undef  INCLUDE_ELT_3C509_END   /* (END) 3Com EtherLink III interface */
#undef  INCLUDE_ENE_END         /* (END) Eagle/Novell NE2000 interface */
#undef	INCLUDE_FEI_END         /* (END) Intel 8255[7/8/9] PCI interface */
#undef	INCLUDE_GEI8254X_END    /* (END) Intel 82543/82544 PCI interface */
#define  INCLUDE_LN_97X_END      /* (END) AMD 79C97x PCI interface */
#undef  INCLUDE_ULTRA_END       /* (END) SMC Elite16 Ultra interface */

#undef  INCLUDE_BSD             /* BSD / Netif Driver Support (Deprecated) */

#undef  INCLUDE_EEX             /* (BSD) Intel EtherExpress interface */
#undef  INCLUDE_EEX32           /* (BSD) Intel EtherExpress flash 32 */
#undef  INCLUDE_ELC             /* (BSD) SMC Elite16 interface */
#undef  INCLUDE_ESMC            /* (BSD) SMC 91c9x Ethernet interface */

/* PCMCIA driver options */

#ifdef  INCLUDE_PCMCIA

#   define INCLUDE_ATA          /* include ATA driver */
#   define INCLUDE_SRAM         /* include SRAM driver */
#   undef INCLUDE_TFFS          /* include TFFS driver */

#   ifdef INCLUDE_NETWORK
#       define INCLUDE_BSD      /* include BSD / Netif Driver Support */
#       define INCLUDE_ELT      /* (BSD) 3Com EtherLink III interface */
#   endif /* INCLUDE_NETWORK */

#endif  /* INCLUDE_PCMCIA */


/* Include PCI support for drivers & libraries that require it. */

#if defined (INCLUDE_LN_97X_END)   || defined (INCLUDE_EL_3C90X_END) || \
    defined (INCLUDE_FEI_END)      || defined (INCLUDE_DEC21X40_END) || \
    defined (INCLUDE_GEI8254X_END) || defined (INCLUDE_AIC_7880)     || \
    defined (INCLUDE_WINDML)       || defined (INCLUDE_USB)

#   define INCLUDE_PCI

#endif


/* default MMU options and PHYS_MEM_DESC type state constants */

#define INCLUDE_MMU_BASIC       /* bundled MMU support */
#undef  VM_PAGE_SIZE		/* page size */
#define VM_PAGE_SIZE		PAGE_SIZE_4KB	/* default page size */

#define VM_STATE_MASK_FOR_ALL \
	VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE
#define VM_STATE_FOR_IO \
	VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT
#define VM_STATE_FOR_MEM_OS \
	VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#define VM_STATE_FOR_MEM_APPLICATION \
	VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#define VM_STATE_FOR_PCI \
	VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT


/* default system and auxiliary clock constants
 *
 * Among other things, SYS_CLK_RATE_MAX depends upon the CPU and application
 * work load.  The default value, chosen in order to pass the internal test
 * suite, could go up to PIT_CLOCK.
 */

#define SYS_CLK_RATE_MIN    (19)           /* minimum system clock rate */
#define AUX_CLK_RATE_MIN    (2)            /* minimum auxiliary clock rate */
#define AUX_CLK_RATE_MAX    (8192)         /* maximum auxiliary clock rate */


/* CPU family/type-specific macros and options */

#if	(CPU == I80386) || (CPU == I80486) /* [34]86 specific macros */

/*
 * software floating point emulation support. DO NOT undefine hardware fp
 * support in configAll.h as it is required for software fp emulation.
 */

#define INCLUDE_SW_FP		/* enable emulator if there is no FPU */
#define SYS_CLK_RATE_MAX	(PIT_CLOCK/32) /* max system clock rate */

#ifdef	INCLUDE_TIMESTAMP
#   define INCLUDE_TIMESTAMP_PIT2 /* include PIT2 for timestamp */
#endif	/* INCLUDE_TIMESTAMP */

#elif	(CPU == PENTIUM)	/* P5 specific macros */

#undef	INCLUDE_SW_FP		/* Pentium has hardware FPP */
#undef	USER_D_CACHE_MODE	/* Pentium write-back data cache support */
#define	USER_D_CACHE_MODE	(CACHE_COPYBACK | CACHE_SNOOP_ENABLE)
#undef	INCLUDE_PMC		/* include PMC */
#define SYS_CLK_RATE_MAX	(PIT_CLOCK/32) /* max system clock rate */

#ifdef	INCLUDE_TIMESTAMP	/* select TSC(default) or PIT2 */
#   undef  INCLUDE_TIMESTAMP_PIT2 /* include PIT2 for timestamp */
#   define INCLUDE_TIMESTAMP_TSC  /* include TSC for timestamp */
#   define PENTIUMPRO_TSC_FREQ	0 /* TSC freq, 0 for auto detect */
#endif	/* INCLUDE_TIMESTAMP */

#elif	(CPU == PENTIUM2) || (CPU == PENTIUM3) || (CPU == PENTIUM4) /* P6,P7 */

#undef	INCLUDE_SW_FP		/* Pentium[234] has hardware FPP */
#undef	USER_D_CACHE_MODE	/* Pentium[234] write-back data cache support */
#define	USER_D_CACHE_MODE	(CACHE_COPYBACK | CACHE_SNOOP_ENABLE)
#define	INCLUDE_MTRR_GET	/* get MTRR to sysMtrr[] */
#define	INCLUDE_PMC		/* include PMC */
#undef	VIRTUAL_WIRE_MODE	/* Interrupt Mode: Virtual Wire Mode */
#undef	SYMMETRIC_IO_MODE	/* Interrupt Mode: Symmetric IO Mode */
#define SYS_CLK_RATE_MAX	(PIT_CLOCK/16) /* max system clock rate */

#ifdef	INCLUDE_TIMESTAMP         /* select TSC(default) or PIT2 */
#   undef  INCLUDE_TIMESTAMP_PIT2 /* include PIT2 for timestamp */
#   define INCLUDE_TIMESTAMP_TSC  /* include TSC for timestamp */
#   define PENTIUMPRO_TSC_FREQ	0 /* TSC freq, 0 for auto detect */
#endif	/* INCLUDE_TIMESTAMP */

#define	INCLUDE_MMU_P6_32BIT	/* include 32bit MMU for Pentium[234] */
#ifdef	INCLUDE_MMU_P6_32BIT
#   undef  VM_PAGE_SIZE		/* page size could be 4KB, 4MB */
#   define VM_PAGE_SIZE		PAGE_SIZE_4KB	/* PAGE_SIZE_4MB */
#endif	/* INCLUDE_MMU_P6_32BIT */
#ifdef	INCLUDE_MMU_P6_36BIT
#   undef  VM_PAGE_SIZE		/* page size could be 4KB, 2MB */
#   define VM_PAGE_SIZE		PAGE_SIZE_4KB	/* PAGE_SIZE_2MB */
#endif	/* INCLUDE_MMU_P6_32BIT */

#if	defined (INCLUDE_MMU_P6_32BIT) || defined (INCLUDE_MMU_P6_36BIT) 
#   undef  VM_STATE_MASK_FOR_ALL
#   undef  VM_STATE_FOR_IO
#   undef  VM_STATE_FOR_MEM_OS
#   undef  VM_STATE_FOR_MEM_APPLICATION
#   undef  VM_STATE_FOR_PCI
#   define VM_STATE_MASK_FOR_ALL \
	   VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | \
	   VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_WBACK | VM_STATE_MASK_GLOBAL
#   define VM_STATE_FOR_IO \
	   VM_STATE_VALID | VM_STATE_WRITABLE | \
	   VM_STATE_CACHEABLE_NOT | VM_STATE_WBACK_NOT | VM_STATE_GLOBAL_NOT
#   define VM_STATE_FOR_MEM_OS \
	   VM_STATE_VALID | VM_STATE_WRITABLE | \
	   VM_STATE_CACHEABLE | VM_STATE_WBACK | VM_STATE_GLOBAL_NOT
#   define VM_STATE_FOR_MEM_APPLICATION \
	   VM_STATE_VALID | VM_STATE_WRITABLE | \
	   VM_STATE_CACHEABLE | VM_STATE_WBACK | VM_STATE_GLOBAL_NOT
#   define VM_STATE_FOR_PCI \
	   VM_STATE_VALID | VM_STATE_WRITABLE | \
	   VM_STATE_CACHEABLE_NOT | VM_STATE_WBACK_NOT | VM_STATE_GLOBAL_NOT
#endif	/* defined (INCLUDE_MMU_P6_32BIT) || defined (INCLUDE_MMU_P6_36BIT) */

/* 
 * To enable the IOAPIC, define the mother board from the following list.
 * If the IOAPIC is already enabled, defining the mother board is not 
 * needed.  Related code locates in pciCfgIntStub.c.
 *   D815EEA = Pentium3 + i815e + ICH2(i82801BA)
 *   D850GB  = Pentium4 + i850  + ICH2(i82801BA)
 * The PIRQ[n] is directly handled by IOAPIC in the SYMMETRIC_IO_MODE.
 */

#undef	INCLUDE_D815EEA		/* Pentium3 + i815e + ICH2 */
#undef	INCLUDE_D850GB		/* Pentium4 + i850  + ICH2 */

#if	defined (INCLUDE_D815EEA) || defined (INCLUDE_D850GB)
#   define INCLUDE_ICH2		/* ICH2 IO controller hub */
#else
#   if	(CPU == PENTIUM4)
#       define INCLUDE_ICH3	/* set ICH3 as default */
#   endif /* (CPU == PENTIUM4) */
#endif	/* defined (INCLUDE_D815EEA) || defined (INCLUDE_D850GB) */

#if	(CPU == PENTIUM4) && \
	(defined (VIRTUAL_WIRE_MODE) || defined (SYMMETRIC_IO_MODE))

#   define INCLUDE_THERM_MONITOR	/* Thermal Monitor and GV 3 */
#   ifdef  INCLUDE_THERM_MONITOR
#       define TM_MODE       GV3_AUTO	/* automatic Thermal Management */
#       define TM_AC_CHK_RTN NULL	/* AC power check routine */
#   endif /* INCLUDE_THERM_MONITOR */

#   undef  INCLUDE_DEBUG_STORE		/* Debug Store (BTS/PEBS) */
#   ifdef  INCLUDE_DEBUG_STORE
#       define DS_SYS_MODE   FALSE	/* TRUE system mode, FALSE task mode */
#       define BTS_ENABLED   TRUE	/* BTS TRUE enable, FALSE disable */
#       define BTS_INT_MODE  TRUE	/* BTS TRUE int mode, FALSE circular */
#       define BTS_BUF_MODE  TRUE	/* BTS TRUE buffer mode, FALSE bus */
#       define PEBS_ENABLED  TRUE	/* PEBS TRUE enable, FALSE disable */
#       define PEBS_EVENT    PEBS_REPLAY		/* PEBS event */
#       define PEBS_METRIC   PEBS_2NDL_CACHE_LOAD_MISS	/* PEBS metric */
#       define PEBS_OS       TRUE	/* PEBS TRUE supervisor, FALSE usr */
#       define PEBS_RESET    -1LL	/* PEBS default reset counter value */
#   endif /* INCLUDE_DEBUG_STORE */

#endif	/* (CPU == PENTIUM4) */

#endif	/* (CPU == I80386) || (CPU == I80486) */


#define IO_ADRS_ELC	0x240
#define INT_LVL_ELC	0x0b
#define MEM_ADRS_ELC	0xc8000
#define MEM_SIZE_ELC	0x4000
#define CONFIG_ELC	0	/* 0=EEPROM 1=RJ45+AUI 2=RJ45+BNC */

#define IO_ADRS_ULTRA	0x240
#define INT_LVL_ULTRA	0x0b
#define MEM_ADRS_ULTRA	0xc8000
#define MEM_SIZE_ULTRA	0x4000
#define CONFIG_ULTRA	0	/* 0=EEPROM 1=RJ45+AUI 2=RJ45+BNC */

#define IO_ADRS_EEX	0x240
#define INT_LVL_EEX	0x0b
#define NTFDS_EEX	0x00
#define CONFIG_EEX	0	/* 0=EEPROM  1=AUI  2=BNC  3=RJ45 */
				/* Auto-detect is not supported, so choose */
				/* the right one you're going to use */

#define IO_ADRS_ELT	0x240
#define INT_LVL_ELT	0x0b
#define NRF_ELT		0x00
#define CONFIG_ELT	0	/* 0=EEPROM 1=AUI  2=BNC  3=RJ45 */

#define IO_ADRS_ENE	0x300
#define INT_LVL_ENE	0x05
				/* Hardware jumper is used to set */
				/* RJ45(Twisted Pair) AUI(Thick) BNC(Thin) */

#define IO_ADRS_ESMC	0x300
#define INT_LVL_ESMC	0x0b
#define CONFIG_ESMC	0	/* 0=EEPROM 1=AUI  2=BNC 3=RJ45 */
#define RX_MODE_ESMC	0	/* 0=interrupt level 1=task level */

#ifdef	INCLUDE_EEX32
#   define INCLUDE_EI		/* include 82596 driver */
#   define INT_LVL_EI	0x0b
#   define EI_SYSBUS	0x44	/* 82596 SYSBUS value */
#   define EI_POOL_ADRS	NONE	/* memory allocated from system memory */
#endif	/* INCLUDE_EEX32 */


/*
 * ATA_TYPE <ataTypes[][]> ATA_GEO_FORCE parameters 
 *
 * ATA_TYPE is defined in h/drv/hdisk/ataDrv.h.  The <ataTypes[][]> table
 * is declared in sysLib.c.
 */

/* controller zero device zero */

#define ATA_CTRL0_DRV0_CYL  (761)   /* ATA 0, device 0 cylinders */
#define ATA_CTRL0_DRV0_HDS  (8)     /* ATA 0, device 0 heads */
#define ATA_CTRL0_DRV0_SPT  (39)    /* ATA 0, device 0 sectors per track */
#define ATA_CTRL0_DRV0_BPS  (512)   /* ATA 0, device 0 bytes per sector */
#define ATA_CTRL0_DRV0_WPC  (0xff)  /* ATA 0, device 0 write pre-compensation */

/* controller zero device one */

#define ATA_CTRL0_DRV1_CYL  (761)   /* ATA 0, device 1 cylinders */
#define ATA_CTRL0_DRV1_HDS  (8)     /* ATA 0, device 1 heads */
#define ATA_CTRL0_DRV1_SPT  (39)    /* ATA 0, device 1 sectors per track */
#define ATA_CTRL0_DRV1_BPS  (512)   /* ATA 0, device 1 bytes per sector */
#define ATA_CTRL0_DRV1_WPC  (0xff)  /* ATA 0, device 1 write pre-compensation */

/* controller one device zero */

#define ATA_CTRL1_DRV0_CYL  (761)   /* ATA 1, device 0 cylinders */
#define ATA_CTRL1_DRV0_HDS  (8)     /* ATA 1, device 0 heads */
#define ATA_CTRL1_DRV0_SPT  (39)    /* ATA 1, device 0 sectors per track */
#define ATA_CTRL1_DRV0_BPS  (512)   /* ATA 1, device 0 bytes per sector */
#define ATA_CTRL1_DRV0_WPC  (0xff)  /* ATA 1, device 0 write pre-compensation */

/* controller one device one */

#define ATA_CTRL1_DRV1_CYL  (761)   /* ATA 1, device 1 cylinders */
#define ATA_CTRL1_DRV1_HDS  (8)     /* ATA 1, device 1 heads */
#define ATA_CTRL1_DRV1_SPT  (39)    /* ATA 1, device 1 sectors per track */
#define ATA_CTRL1_DRV1_BPS  (512)   /* ATA 1, device 1 bytes per sector */
#define ATA_CTRL1_DRV1_WPC  (0xff)  /* ATA 1, device 1 write pre-compensation */

/*
 * ATA_RESOURCE <ataResources[]> parameters 
 *
 * ATA_RESOURCES is defined in h/drv/pcmcia/pccardLib.h.  The <ataResources[]>
 * table is declared in sysLib.c.  Defaults are based on the pcPentium BSP.
 */

/* ATA controller zero ataResources[] parameters */

#define ATA0_VCC          (5)          /* ATA 0 Vcc (3 or 5 volts) */
#define ATA0_VPP          (0)          /* ATA 0 Vpp (5 or 12 volts or 0) */
#define ATA0_IO_START0    (0x1f0)      /* Start I/O Address 0 for ATA 0 */
#define ATA0_IO_START1    (0x3f6)      /* Start I/O Address 1 for ATA 0 */
#define ATA0_IO_STOP0     (0x1f7)      /* Stop I/O Address for ATA 0 */
#define ATA0_IO_STOP1     (0x3f7)      /* Stop I/O Address for ATA 0 */
#define ATA0_EXTRA_WAITS  (0)          /* ATA 0 extra wait states (0-2) */
#define ATA0_MEM_START    (0)          /* ATA 0 memory start address */
#define ATA0_MEM_STOP     (0)          /* ATA 0 memory start address */
#define ATA0_MEM_WAITS    (0)          /* ATA 0 memory extra wait states */
#define ATA0_MEM_OFFSET   (0)          /* ATA 0 memory offset */
#define ATA0_MEM_LENGTH   (0)          /* ATA 0 memory offset */
#define ATA0_CTRL_TYPE    (IDE_LOCAL)  /* ATA 0 logical type */
#define ATA0_NUM_DRIVES   (1)          /* ATA 0 number drives present */
#define ATA0_INT_LVL      (0x0e)       /* ATA 0 interrupt level */

#define ATA0_CONFIG       (ATA_GEO_CURRENT | ATA_PIO_AUTO | \
                           ATA_BITS_16     | ATA_PIO_MULTI)

#define ATA0_SEM_TIMEOUT  (5)          /* ATA 0 sync. semaphore timeout */
#define ATA0_WDG_TIMEOUT  (5)          /* ATA 0 watchdog timer timeout */
#define ATA0_SOCKET_TWIN  (0)          /* Socket number (TWIN PCMCIA Card) */
#define ATA0_POWER_DOWN   (0)          /* ATA power down mode */

/* ATA controller one ataResources[] parameters */

#define ATA1_VCC          (5)          /* ATA 1 Vcc (3 or 5 volts) */
#define ATA1_VPP          (0)          /* ATA 1 Vpp (5 or 12 volts or 0) */
#define ATA1_IO_START0    (0x170)      /* Start I/O Address 0 for ATA 1 */
#define ATA1_IO_START1    (0x376)      /* Start I/O Address 1 for ATA 1 */
#define ATA1_IO_STOP0     (0x177)      /* Stop I/O Address 0 for ATA 1 */
#define ATA1_IO_STOP1     (0x377)      /* Stop I/O Address 1 for ATA 1 */
#define ATA1_EXTRA_WAITS  (0)          /* ATA 1 extra wait states (0-2) */
#define ATA1_MEM_START    (0)          /* ATA 1 memory start address */
#define ATA1_MEM_STOP     (0)          /* ATA 1 memory start address */
#define ATA1_MEM_WAITS    (0)          /* ATA 1 memory extra wait states */
#define ATA1_MEM_OFFSET   (0)          /* ATA 1 memory offset */
#define ATA1_MEM_LENGTH   (0)          /* ATA 1 memory offset */
/*
修改一下三个宏定义
#define ATA1_CTRL_TYPE    (ATA_PCMCIA)
#define ATA1_NUM_DRIVES   (1)
#define ATA1_INT_LVL      (0x09)
*/
#define ATA1_CTRL_TYPE    (IDE_LOCAL)  /* ATA 1 logical type */
#define ATA1_NUM_DRIVES   (1)          /* ATA 1 number drives present */
#define ATA1_INT_LVL      (0x0f)       /* ATA 1 interrupt level */


#define ATA1_CONFIG       (ATA_GEO_CURRENT | ATA_PIO_AUTO | \
                           ATA_BITS_16     | ATA_PIO_MULTI)

#define ATA1_SEM_TIMEOUT  (5)          /* ATA 1 sync. semaphore timeout */
#define ATA1_WDG_TIMEOUT  (5)          /* ATA 1 watchdog timer timeout */
#define ATA1_SOCKET_TWIN  (0)          /* Socket number (TWIN PCMCIA Card) */
#define ATA1_POWER_DOWN   (0)          /* ATA 1 power down mode */


/* console definitions  */

#undef	NUM_TTY
#define NUM_TTY       (N_UART_CHANNELS)  /* number of tty channels */

#define INCLUDE_PC_CONSOLE                /* PC keyboard and VGA console */

#ifdef INCLUDE_PC_CONSOLE
#   define PC_CONSOLE           (0)      /* console number */
#   define N_VIRTUAL_CONSOLES   (2)      /* shell / application */
#endif /* INCLUDE_PC_CONSOLE */

/* PS/2 101-key default keyboard type (use PC_XT_83_KBD for 83-key) */

#define PC_KBD_TYPE   (PC_PS2_101_KBD)


/* memory addresses, offsets, and size constants */

#if (SYS_WARM_TYPE == SYS_WARM_BIOS)            /* non-volatile RAM size */
#   define NV_RAM_SIZE          (NONE)
#else
#   define NV_RAM_SIZE          (0x1000)
#endif

#define USER_RESERVED_MEM       (0)             /* user reserved memory */
#define LOCAL_MEM_LOCAL_ADRS    (0x00100000)    /* on-board memory base */

/*
 * LOCAL_MEM_SIZE is the offset from the start of on-board memory to the
 * top of memory.  If the page size is 2MB or 4MB, write-protected pages
 * for the MMU directory tables and <globalPageBlock> array are also a
 * multiple of 2MB or 4MB.  Thus, LOCAL_MEM_SIZE should be big enough to
 * hold them.
 */

#if  (VM_PAGE_SIZE == PAGE_SIZE_4KB)            /* 4KB page */
#   define SYSTEM_RAM_SIZE      (0x00800000)    /* minimum 8MB system RAM */
#else   /* PAGE_SIZE_[2/4]MB */                 /* [2/4]MB page */
#   define SYSTEM_RAM_SIZE      (0x02000000)    /* minimum 32MB system RAM */
#endif  /* (VM_PAGE_SIZE == PAGE_SIZE_4KB) */

#define LOCAL_MEM_SIZE          (SYSTEM_RAM_SIZE - LOCAL_MEM_LOCAL_ADRS)


/*
 * Memory auto-sizing is supported when this option is defined.
 * See sysyPhysMemTop() in the BSP sysLib.c file.
 */

#ifdef	INCLUDE_MMU_P6_36BIT
#   undef  LOCAL_MEM_AUTOSIZE
#else
#   if (CPU == I80486)
#      undef  LOCAL_MEM_AUTOSIZE
#   else 
#      define LOCAL_MEM_AUTOSIZE
#   endif /* (CPU == I80486) */
#endif	/* INCLUDE_MMU_P6_36BIT */

/*
 * The following parameters are defined here and in the BSP Makefile.
 * They must be kept synchronized.  Any changes made here must be made
 * in the Makefile and vice versa.
 */

#ifdef	BOOTCODE_IN_RAM
#   undef  ROMSTART_BOOT_CLEAR
#   define ROM_BASE_ADRS        (0x00008000)    /* base address of ROM */
#   define ROM_TEXT_ADRS        (ROM_BASE_ADRS) /* booting from A: or C: */
#   define ROM_SIZE             (0x00190000)    /* size of ROM */        /* modify by frank */
#else
#   define ROM_BASE_ADRS        (0xfff20000)    /* base address of ROM */
#   define ROM_TEXT_ADRS        (ROM_BASE_ADRS) /* booting from EPROM */
#   define ROM_SIZE             (0x0007fe00)    /* size of ROM */
#endif

#define RAM_LOW_ADRS            (0x00308000)    /* VxWorks image entry point */
#define RAM_HIGH_ADRS           (0x00108000)    /* Boot image entry point */

/*
 * The INCLUDE_ADD_BOOTMEM configuration option enables runtime code which
 * will add a specified amount of upper memory (memory above physical address
 * 0x100000) to the memory pool of an image in lower memory.  This option
 * cannot be used on systems with less than 4MB of memory.
 *
 * The default value for ADDED_BOOTMEM_SIZE, the amount of memory to add
 * to a lower memory image's memory pool, is 2MB.  This value may be increased,
 * but one must ensure that the pool does not overlap with the downloaded
 * vxWorks image.  If there is an overlap, then loading the vxWorks runtime
 * image will corrupt the added memory pool.   The calculation for determining
 * the ADDED_BOOTMEM_SIZE value is:
 *
 *     (RAM_LOW_ADRS + vxWorks image size) < (memTopPhys - ADDED_BOOTMEM_SIZE)
 *
 * Where <memTopPhys> is calculated in the BSP sysLib.c file.  This
 * configuration option corrects SPR 21338.
 */

#define INCLUDE_ADD_BOOTMEM     /* Add upper memory to low memory bootrom */
#define ADDED_BOOTMEM_SIZE      (0x00200000)     /* 2MB additional memory */

/* power management definitions */

#define	VX_POWER_MANAGEMENT	/* define to enable */
#define VX_POWER_MODE_DEFAULT   VX_POWER_MODE_AUTOHALT  /* set mode */

/* AMP (asymmetric multi processor) definitions */

#ifdef	TGT_CPU
#   include "configAmp.h"
#endif	/* TGT_CPU */

/* interrupt mode/number definitions */

#include "configInum.h"

#ifdef	INCLUDE_IACSFL
#   include "iacsfl.h"
#endif	/* INCLUDE_IACSFL - iacsfl.h overrides some macros in config.h */

/*
 * defining _WRS_BSP_DEBUG_NULL_ACCESS will disable access to lower 
 * page in MMU, see sysPhysMemDesc [] and sysPhysMemTop() in sysLib.c 
 * for more details.  This causes the CPU to generate an exception for 
 * any NULL pointer access, or any access to lower page of memory. 
 * VxWorks will suspend the task which made the access.
 * Note that the MMU must be enabled for this to work.
 */

#define _WRS_BSP_VM_PAGE_OFFSET	(VM_PAGE_SIZE)

#undef _WRS_BSP_DEBUG_NULL_ACCESS

#ifdef _WRS_BSP_DEBUG_NULL_ACCESS  /* protect NULL access with MMU */

#   if (VM_PAGE_SIZE != PAGE_SIZE_4KB) /* works when page size is 4KB */
#       error PAGE_SIZE_4KB required to use _WRS_BSP_DEBUG_NULL_ACCESS
#   endif /* (VM_PAGE_SIZE == PAGE_SIZE_4KB) */

#   if (LOCAL_MEM_LOCAL_ADRS == 0x0)
#       undef VEC_BASE_ADRS
#       define VEC_BASE_ADRS	((char *) (_WRS_BSP_VM_PAGE_OFFSET * 2))

#       undef GDT_BASE_OFFSET
#       define GDT_BASE_OFFSET	(0x1000 + (_WRS_BSP_VM_PAGE_OFFSET * 2))

#       undef SM_ANCHOR_OFFSET
#       define SM_ANCHOR_OFFSET	(0x1100 + (_WRS_BSP_VM_PAGE_OFFSET * 2))

#       undef EXC_MSG_OFFSET
#       define EXC_MSG_OFFSET	(0x1300 + (_WRS_BSP_VM_PAGE_OFFSET * 2))

#       undef FD_DMA_BUF_ADDR
#       define FD_DMA_BUF_ADDR	(0x2000 + (_WRS_BSP_VM_PAGE_OFFSET * 2))

#       undef FD_DMA_BUF_SIZE
#       define FD_DMA_BUF_SIZE	(0x1000)

#       undef BOOT_LINE_ADRS
#       define BOOT_LINE_ADRS	((char *) (0x1200))
#   endif /* (LOCAL_MEM_LOCAL_ADRS == 0x0) */

#endif /* _WRS_BSP_DEBUG_NULL_ACCESS */

#ifdef __cplusplus
}
#endif

#endif	/* INCconfigh */

#if defined(PRJ_BUILD)
#   include "prjParams.h"
#endif

