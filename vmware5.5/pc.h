/* pc.h - PC common header */

/* Copyright 1992-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
04o,27jun02,hdn  added comment for FD_DMA_BUF_ADDR (spr 34045)
		 changed EBDA for the GS BIOS to preserve the MP table
04n,09may02,hdn  added Pentium4 Asymmetric Multi Processor configuration
02m,28apr02,hdn  added optional new i8259 PIC specific features (spr 76411)
02l,25apr02,rhe  Added C++ Protection
02k,09apr02,pai  Reworked ATA configuration and documentation (SPR# 73848).
02j,12mar02,hdn  added macros for PIC and IO/Local APIC for HTT (spr 73738)
02i,02nov01,hdn  removed "#include ideDrv.h"
02h,23oct01,pai  Removed #include pciConfigLib.h.  Removed FEI specific
                 definitions.  Added VESA BIOS macros for WindML (spr 69874).
02g,23oct01,dmh  document steps for additional serial ports. (spr 5704)
02f,02oct01,pai  Added MSE_INT_LVL mouse interrupt level value for WindML.
02e,31aug01,hdn  removed ROM_XXX offset as GAS improved (spr 69891)
		 updated [LI]OAPIC support for Pentium4
		 discontinued ideDrv in favor of ataDrv
		 changed BUS to BUS_TYPE_PCI. (spr 32598)
		 added LPT[012]_xxx macros (spr 30067) on behalf of pai
02d,08feb99,jkf  removed PCI_CFG_* definitions, now set in configAll.h
02c,28may98,hdn  added support for APIC.
02b,13may98,hdn  moved PAGE_SIZE_XXX macro to mmuI86Lib.h and include it.
		 added UNUSED_ISA_IO_ADDRESS for IO address 0x84.
02a,16apr98,hdn  added PAGE_SIZE_XXX macro for PentiumPro.
01z,17mar98,sbs  added definitions for dynamic mmu entries.
01y,12mar98,sbs  changed SCSI-2 definitions to device specific definitions. 
                 Added PCI configuration definitions and FEI specific 
                 definitions. 
01x,10jul97,dds  added SCSI-2 support.
01w,16jan97,hdn  added PCMCIA_SOCKS, PCMCIA_MEMBASE.
01v,03dec96,hdn  renamed PCI_FEIxxx to FEIxxx.
01u,20nov96,hdn  added support for PRO100B.
01t,01nov96,hdn  added support for PCMCIA.
01s,21oct96,hdn  added new configuration macros for LPT: LPT_xxx.
01r,12oct96,hdn  added support for ATA-2.  changed ATA[01]_CONFIG.
01q,03sep96,hdn  added the compression support.
01p,09aug96,hdn  renamed INT_VEC_IRQ0 to INT_NUM_IRQ0.
01o,19jul96,hdn  added support for ATA driver.
01n,25jun96,hdn  added support for TIMESTAMP timer.
01m,28may96,hdn  renamed PIT_INT_xxx to PIT0_INT_xxx.
01l,14jun95,myz  removed #include tyLib.h
01k,21oct94,hdn  deleted ENABLE_A20 macro.
01j,15oct94,hdn  added macros for LPT parallel driver.
01i,25apr94,hdn  moved a macro PC_KBD_TYPE to config.h.
01h,08nov93,vin  added support for pc console drivers.
01g,12oct93,hdn  added interrupt level macros.
01f,16aug93,hdn  added RTC related macros.
01e,03aug93,hdn  changed vectors for serial and timer.
01d,17jun93,hdn  updated to 5.1.
01c,07apr93,hdn  renamed compaq to pc.
01b,26mar93,hdn  deleted a macro CPU because it supports 386 and 486.
01a,15may92,hdn  written based on frc386 version.
*/

/*
This file contains IO address, memory address, and related constants
for the PC [3|4]86 and PC Pentium[2|3|4] BSPs.
*/

#ifndef	INCpch
#define	INCpch

#ifdef __cplusplus
extern "C" {
#endif

#include "drv/intrCtl/i8259.h"
#include "drv/timer/i8253.h"
#include "drv/timer/mc146818.h"
#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"
#include "drv/fdisk/nec765Fd.h"
#include "drv/serial/pcConsole.h"
#include "drv/parallel/lptDrv.h"
#include "drv/pcmcia/pcmciaLib.h"
#include "drv/hdisk/ataDrv.h"
#include "arch/i86/mmuI86Lib.h"
#include "arch/i86/ipiI86Lib.h"
#include "drv/intrCtl/loApic.h"
#include "drv/intrCtl/ioApic.h" 


#undef	CAST
#define	CAST

#define TARGET_PC386

#define BUS		BUS_TYPE_PCI	/* PCI Bus + ISA/VME */

#define	BOOTCODE_IN_RAM			/* for ../all/bootInit.c */

/* constant values in romInit.s */

#define ROM_STACK	0x7000		/* initial stack pointer */
#define ROM_WARM_HIGH	0x10		/* warm start entry p */
#define ROM_WARM_LOW	0x20		/* warm start entry p */

/* programmable interrupt controller (PIC) */

#define	PIC1_BASE_ADR		0x20
#define	PIC2_BASE_ADR		0xa0
#define	PIC_REG_ADDR_INTERVAL	1	/* address diff of adjacent regs. */
#define	PIC_MASTER_STRAY_INT_LVL 0x07	/* master PIC stray IRQ */
#define	PIC_SLAVE_STRAY_INT_LVL	0x0f	/* slave PIC stray IRQ */
#define	PIC_MAX_INT_LVL		0x0f	/* max interrupt level in PIC */
#define N_PIC_IRQS		16	/* number of PIC IRQs */
#undef	PIC_SPECIAL_FULLY_NESTED_MODE	/* non blocking PIC2 interrupt */
#define	PIC_NORMAL_EOI_IRQ0	   1	/* IRQ0 ISR is the highest lvl */
#define	PIC_EARLY_EOI_IRQ0	   2	/* lower IRQ0 ISR to next lower lvl */
#define	PIC_SPECIAL_MASK_MODE_IRQ0 3	/* lower IRQ0 ISR to specified lvl */
#define	PIC_AUTO_EOI		   4	/* no multilevel int struct in PIC1 */
#define	PIC_IRQ0_MODE	PIC_NORMAL_EOI_IRQ0 /* is one of above four modes */

#if	defined (PIC_SPECIAL_FULLY_NESTED_MODE)
#   define I8259_SPECIAL_FULLY_NESTED_MODE	/* for the i8259Intr.c */
#endif	/* defined (PIC_SPECIAL_FULLY_NESTED_MODE) */

#if	(PIC_IRQ0_MODE == PIC_AUTO_EOI)
#   define I8259_AUTO_EOI			/* for the i8259Intr.c */
#endif	/* (PIC_IRQ0_MODE == PIC_AUTO_EOI) */

/* serial ports (COM1,COM2,COM3,COM4) */

/* Only two UART Channels are supported out-of-the-box using defacto standard
 * PC com port resources because the serial driver does not support shared
 * interrupts. More than 2 serial channels can be supported if the user is
 * willing customize this BSP by finding unused IRQs. For example,
 * IRQ5 and IRQ2(9) are often unused.  N_UART_CHANNELS must be changed to
 * reflect the actual number of serial ports supported.  See the
 * I8250_CHAN_PARAS devParas structure in sysSerial.c for additional
 * changes necessary for supporting 3 or more serial ports.  Sometimes
 * the BIOS also needs adjustment.
 * SPR# 5704
 */

#define COM1_BASE_ADR		0x3f8
#define COM2_BASE_ADR		0x2f8
#define COM3_BASE_ADR		0x3e8
#define COM4_BASE_ADR		0x2e8
#define COM1_INT_LVL		0x04
#define COM2_INT_LVL		0x03
#define COM3_INT_LVL		0x05
#define COM4_INT_LVL		0x09
#define UART_REG_ADDR_INTERVAL	1	/* address diff of adjacent regs. */
#define	N_UART_CHANNELS		2       

/* timer (PIT) */

#define	PIT_BASE_ADR		0x40
#define	PIT0_INT_LVL		0x00
#define	PIT_REG_ADDR_INTERVAL	1	/* address diff of adjacent regs. */
#define	PIT_CLOCK		1193180

/* real time clock (RTC) */

#define	RTC_INDEX		0x70
#define	RTC_DATA		0x71
#define	RTC_INT_LVL		0x08

/* floppy disk (FD) (The buffer must be 64KB aligned and in the lower mem) */

#define FD_INT_LVL              0x06
#define FD_DMA_BUF_ADDR		0x2000	/* floppy disk DMA buffer addr */
#define FD_DMA_BUF_SIZE		0x3000	/* floppy disk DMA buffer size */

/* pcmcia (PCMCIA) */

#define PCMCIA_SOCKS		0x0	/* number of sockets. 0=auto detect */
#define PCMCIA_MEMBASE		0x0	/* mapping base address */
#define PCIC_BASE_ADR           0x3e0	/* Intel 82365SL */
#define PCIC_INT_LVL            0x0a
#define TCIC_BASE_ADR           0x240	/* Databook DB86082A */
#define TCIC_INT_LVL            0x0a
#define CIS_MEM_START           0xd0000	/* mapping addr for CIS tuple */
#define CIS_MEM_STOP            0xd3fff
#define CIS_REG_START           0xd4000	/* mapping addr for config reg */
#define CIS_REG_STOP            0xd4fff
#define SRAM0_MEM_START		0xd8000	/* mem for SRAM0 */
#define SRAM0_MEM_STOP		0xd8fff
#define SRAM0_MEM_LENGTH	0x100000
#define SRAM1_MEM_START		0xd9000	/* mem for SRAM1 */
#define SRAM1_MEM_STOP		0xd9fff
#define SRAM1_MEM_LENGTH	0x100000
#define SRAM2_MEM_START		0xda000	/* mem for SRAM2 */
#define SRAM2_MEM_STOP		0xdafff
#define SRAM2_MEM_LENGTH	0x100000
#define SRAM3_MEM_START		0xdb000	/* mem for SRAM3 */
#define SRAM3_MEM_STOP		0xdbfff
#define SRAM3_MEM_LENGTH	0x100000
#define ELT0_IO_START		0x260	/* io for ELT0 */
#define ELT0_IO_STOP		0x26f
#define ELT0_INT_LVL		0x05
#define ELT0_NRF		0x00
#define ELT0_CONFIG		0	/* 0=EEPROM 1=AUI  2=BNC  3=RJ45 */
#define ELT1_IO_START		0x280	/* io for ELT1 */
#define ELT1_IO_STOP		0x28f
#define ELT1_INT_LVL		0x09
#define ELT1_NRF		0x00
#define ELT1_CONFIG		0	/* 0=EEPROM 1=AUI  2=BNC  3=RJ45 */

/* parallel port (LPT) */

#define LPT0_BASE_ADRS		0x3bc
#define LPT1_BASE_ADRS		0x378
#define LPT2_BASE_ADRS		0x278
#define LPT0_INT_LVL            0x07
#define LPT1_INT_LVL            0x05
#define LPT2_INT_LVL            0x09
#define LPT_INT_LVL             0x07
#define LPT_CHANNELS		1

/* ISA IO address for sysDelay () */

#define UNUSED_ISA_IO_ADDRESS	0x84

/* AIC7880 PCI bus resources */

#define AIC7880_MEMBASE  	0xf5200000	
#define AIC7880_MEMSIZE		0x00001000	/* memory size for CSR, 4KB */
#define AIC7880_IOBASE		0xf800	
#define AIC7880_INT_LVL		0x0a
#define AIC7880_INIT_STATE_MASK (VM_STATE_MASK_FOR_ALL)
#define AIC7880_INIT_STATE      (VM_STATE_FOR_IO)

/* Mouse (MSE) */

#define MSE_INT_LVL		(0x0c)	/* IRQ 12 assuming PS/2 mouse */

/* key board (KBD) */

#define PC_XT_83_KBD		0	/* 83 KEY PC/PCXT/PORTABLE 	*/
#define PC_PS2_101_KBD		1	/* 101 KEY PS/2 		*/
#define KBD_INT_LVL		0x01

#define	COMMAND_8042		0x64
#define	DATA_8042		0x60
#define	STATUS_8042		COMMAND_8042
#define COMMAND_8048		0x61	/* out Port PC 61H in the 8255 PPI */
#define	DATA_8048		0x60	/* input port */
#define	STATUS_8048		COMMAND_8048

#define JAPANES_KBD             0
#define ENGLISH_KBD             1

/* beep generator */

#define DIAG_CTRL	0x61
#define BEEP_PITCH_L	1280 /* 932 Hz */
#define BEEP_PITCH_S	1208 /* 987 Hz */
#define BEEP_TIME_L	(sysClkRateGet () / 3) /* 0.66 sec */
#define BEEP_TIME_S	(sysClkRateGet () / 8) /* 0.15 sec */

/* Monitor definitions */

#define MONOCHROME              0
#define VGA                     1
#define MONO			0
#define COLOR			1
#define	VGA_MEM_BASE		(UCHAR *) 0xb8000
#define	VGA_SEL_REG		(UCHAR *) 0x3d4
#define VGA_VAL_REG             (UCHAR *) 0x3d5
#define MONO_MEM_BASE           (UCHAR *) 0xb0000
#define MONO_SEL_REG            (UCHAR *) 0x3b4
#define MONO_VAL_REG            (UCHAR *) 0x3b5
#define	CHR			2

#define VESA_BIOS_DATA_ADDRESS  (0xbfb00)       /* BIOS data storage */
#define VESA_BIOS_DATA_PREFIX   (VESA_BIOS_DATA_ADDRESS - 8)
#define VESA_BIOS_DATA_SIZE     (0x500)         /* Vesa BIOS data size */
#define VESA_BIOS_KEY_1         (0x534F4942)    /* "BIOS" */
#define VESA_BIOS_KEY_2         (0x41544144)    /* "DATA" */

/* change this to JAPANES_KBD if Japanese enhanced mode wanted */

#define KEYBRD_MODE             ENGLISH_KBD 

/* undefine this if ansi escape sequence not wanted */

#define INCLUDE_ANSI_ESC_SEQUENCE 

#define GRAPH_ADAPTER   VGA

#if (GRAPH_ADAPTER == MONOCHROME)

#define DEFAULT_FG  	ATRB_FG_WHITE
#define DEFAULT_BG 	ATRB_BG_BLACK
#define DEFAULT_ATR     DEFAULT_FG | DEFAULT_BG
#define CTRL_SEL_REG	MONO_SEL_REG		/* controller select reg */
#define CTRL_VAL_REG	MONO_VAL_REG		/* controller value reg */
#define CTRL_MEM_BASE	MONO_MEM_BASE		/* controller memory base */
#define COLOR_MODE	MONO			/* color mode */

#else /* GRAPH_ADAPTER = VGA */

#define DEFAULT_FG	ATRB_FG_BRIGHTWHITE
#define DEFAULT_BG	ATRB_BG_BLUE
#define DEFAULT_ATR	DEFAULT_FG | DEFAULT_BG
#define CTRL_SEL_REG	VGA_SEL_REG		/* controller select reg */
#define CTRL_VAL_REG	VGA_VAL_REG		/* controller value reg */
#define CTRL_MEM_BASE	VGA_MEM_BASE		/* controller memory base */
#define COLOR_MODE	COLOR			/* color mode */

#endif /* (ADAPTER == MONOCHROME) */

/*
 * sysPhysMemDesc[] dummy entries:
 * these create space for updating sysPhysMemDesc table at a later stage
 * mainly to provide plug and play
 */

#define DUMMY_PHYS_ADDR         -1
#define DUMMY_VIRT_ADDR         -1
#define DUMMY_LENGTH            -1
#define DUMMY_INIT_STATE_MASK   -1
#define DUMMY_INIT_STATE        -1

#define DUMMY_MMU_ENTRY		{ (void *) DUMMY_PHYS_ADDR, \
                                  (void *) DUMMY_VIRT_ADDR, \
                                  DUMMY_LENGTH, \
                                  DUMMY_INIT_STATE_MASK, \
                                  DUMMY_INIT_STATE \
                                }

/* PCI device configuration type definitions */

#define PCI_CFG_TYPE		PCI_CFG_NONE	/* specify PCI config type */

/* APIC (IO APIC + Local APIC) */

#define IOAPIC_INDEX_BASE	0xfec00000      /* IO APIC Index base addr */
#define IOAPIC_DATA_BASE	(IOAPIC_INDEX_BASE + IOAPIC_DATA)
#define IOAPIC_LENGTH		0x00010000      /* IO APIC regs length */
#define LOAPIC_BASE		0xfee00000      /* Local APIC base addr */
#define LOAPIC_LENGTH		0x00001000      /* Local APIC regs length */
#define EBDA_START		0x9d000		/* Extended BIOS Data Area */ 
#define EBDA_END		0x9ffff
#define BIOS_ROM_START		0xf0000		/* BIOS ROM space */
#define BIOS_ROM_END		0xfffff
#define IOAPIC_PIRQA_INT_LVL	16		/* IOAPIC PIRQA IRQ */
#define IOAPIC_PIRQB_INT_LVL	17		/* IOAPIC PIRQB IRQ */
#define IOAPIC_PIRQC_INT_LVL	18		/* IOAPIC PIRQC IRQ */
#define IOAPIC_PIRQD_INT_LVL	19		/* IOAPIC PIRQD IRQ */
#define IOAPIC_PIRQE_INT_LVL	20		/* IOAPIC PIRQE IRQ */
#define IOAPIC_PIRQF_INT_LVL	21		/* IOAPIC PIRQF IRQ */
#define IOAPIC_PIRQG_INT_LVL	22		/* IOAPIC PIRQG IRQ */
#define IOAPIC_PIRQH_INT_LVL	23		/* IOAPIC PIRQH IRQ */
#define N_IOAPIC_PIRQS		8		/* number of IOAPIC PIRQs */

/* Multi processor (AMP/SMP) definitions */

#define MP_ANCHOR_ADRS		0x1400		/* base addr for the counters */
#define MP_N_CPU	(MP_ANCHOR_ADRS)	/* MP CPU counter */
#define MP_LOCK_SEM	(MP_ANCHOR_ADRS+4)	/* MP lock semaphore */
#define MP_N_IPI	(MP_ANCHOR_ADRS+8)	/* MP IPI counter */
#define MP_BOOT_TIMEOUT         4		/* timeout (sec) for AP booting */
#define	MP_VACANT	0x00000000		/* MP lock sem - vacant */
#define	MP_VACANT_NOT	0xffffffff		/* MP lock sem - vacant not */
#define	MP_BP			1		/* build macro for BP */
#define	MP_AP1			2		/* build macro for AP1 */
#define	MP_AP2			3		/* build macro for AP2 */
#define	MP_AP3			4		/* build macro for AP3 */
#define WARM_RESET_VECTOR       0x0467		/* BIOS: addr to hold the vector */
#define BIOS_SHUTDOWN_STATUS    0x0f		/* BIOS: shutdown status */

#ifdef __cplusplus
}
#endif

#endif	/* INCpch */
