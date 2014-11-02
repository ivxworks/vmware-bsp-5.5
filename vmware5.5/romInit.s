/* romInit.s - PC-386 ROM initialization module */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01t,12nov02,hdn  made CR4 initialization only for P5 or later (spr 83992)
01s,19jun02,hdn  updated the copyright year
01r,22may02,hdn  saved/restored value at 0x0 and 0x100000
01q,03apr02,hdn  added MP_N_CPU increment with bus locked
01p,26mar02,pai  Legacy ROM-resident builds require a symbol named '_sdata'
                 (SPR 74537).
01o,19nov01,hdn  added WINDML support
01n,30aug01,hdn  added FUNC/FUNC_LABEL GTEXT/GDATA macro.
		 removed ROM_XXX macro(spr 69891).  added romEaxShow.
01m,03may00,ms   do not call romA20on if booted though SFL.
01l,03sep96,hdn  added the compression support.
01k,21oct94,hdn  cleaned up.
01j,23sep94,hdn  deleted _sysBootType and uses stack.
01i,06apr94,hdn  moved a processor checking routine to sysALib.s.
		 created the system GDT at GDT_BASE_OFFSET.
01h,17feb94,hdn  deleted a piece of code which copy itself to upper memory.
01g,27oct93,hdn  added _sysBootType.
01f,25aug93,hdn  changed a way to enable A20.
01e,12aug93,hdn  added codes to load a user defined global descriptor table.
01d,09aug93,hdn  added codes to recognize a type of cpu.
01c,17jun93,hdn  updated to 5.1.
01b,26mar93,hdn  added some codes to switch to the protected mode
01a,19mar92,hdn  written by modifying v01c of h32/romInit.s
*/

/*
DESCRIPTION
This module contains the entry code for the VxWorks bootrom.

The routine sysToMonitor(2) jumps to the location XXX bytes
passed the beginning of romInit, to perform a "warm boot".

This code is intended to be generic accross i80x86 boards.
Hardware that requires special register setting or memory
mapping to be done immediately, may do so here.
*/

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"


	.data
	.globl	FUNC(copyright_wind_river)
	.long	FUNC(copyright_wind_river)


	/* internals */

	.globl	romInit			/* start of system code */
	.globl	_romInit		/* start of system code */
	.globl	GTEXT(romWait)		/* wait routine */
	.globl	GTEXT(romA20on)		/* turn on A20 */

	.globl	sdata			/* start of data */
	.globl	_sdata			/* start of data */


 sdata:
_sdata:
	.asciz	"start of data"


	.text
	.balign 16

/*******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*

* romInit (startType)
*     int startType;	/@ only used by 2nd entry point @/

*/

	/* cold start entry point in REAL MODE(16 bits) */

romInit:
_romInit:
	cli				/* LOCK INTERRUPT */
	jmp	cold			/* offset must be less than 128 */


	/* warm start entry point in PROTECTED MODE(32 bits) */

	.balign 16,0x90
romWarmHigh:				/* ROM_WARM_HIGH(0x10) is offset */
	cli				/* LOCK INTERRUPT */
	movl    SP_ARG1(%esp),%ebx	/* %ebx has the startType */
	jmp	warm

	/* warm start entry point in PROTECTED MODE(32 bits) */
	
	.balign 16,0x90
romWarmLow:				/* ROM_WARM_LOW(0x20) is offset */
	cli				/* LOCK INTERRUPT */
	cld				/* copy itself to ROM_TEXT_ADRS */
	movl	$ RAM_LOW_ADRS,%esi	/* get src addr (RAM_LOW_ADRS) */
	movl	$ ROM_TEXT_ADRS,%edi	/* get dst addr (ROM_TEXT_ADRS) */
	movl	$ ROM_SIZE,%ecx		/* get nBytes to copy */
	shrl	$2,%ecx			/* get nLongs to copy */
	rep				/* repeat next inst ECX time */
	movsl				/* copy nLongs from src to dst */
	movl    SP_ARG1(%esp),%ebx	/* %ebx has the startType */
	jmp	warm			/* jump to warm */

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1984-2002 Wind River Systems, Inc."


	/* cold start code in REAL MODE(16 bits) */

	.balign 16,0x90
cold:
	.byte	0x67, 0x66		/* next inst has 32bit operand */
	lidt	%cs:(romIdtr - romInit)	/* load temporary IDT */

	.byte	0x67, 0x66		/* next inst has 32bit operand */
	lgdt	%cs:(romGdtr - romInit)	/* load temporary GDT */

	/* switch to protected mode */

	mov	%cr0,%eax		/* move CR0 to EAX */
	.byte	0x66			/* next inst has 32bit operand */
	or	$0x00000001,%eax	/* set the PE bit */
	mov	%eax,%cr0		/* move EAX to CR0 */
	jmp	romInit1		/* near jump to flush a inst queue */

romInit1:
	.byte	0x66			/* next inst has 32bit operand */
	mov	$0x0010,%eax		/* set data segment 0x10 is 3rd one */
	mov	%ax,%ds			/* set DS */
	mov	%ax,%es			/* set ES */
	mov	%ax,%fs			/* set FS */
	mov	%ax,%gs			/* set GS */
	mov	%ax,%ss			/* set SS */
	.byte	0x66			/* next inst has 32bit operand */
	mov	$ ROM_STACK,%esp 	/* set lower mem stack pointer */
	.byte	0x67, 0x66		/* next inst has 32bit operand */
	ljmp	$0x08, $ ROM_TEXT_ADRS + romInit2 - romInit


	/* temporary IDTR stored in code segment in ROM */

romIdtr:
	.word	0x0000			/* size   : 0 */
	.long	0x00000000		/* address: 0 */


	/* temporary GDTR stored in code segment in ROM */

romGdtr:
	.word	0x0027			/* size   : 39(8 * 5 - 1) bytes */
	.long	(romGdt	- romInit + ROM_TEXT_ADRS) /* address: romGdt */


	/* temporary GDT stored in code segment in ROM */

	.balign 16,0x90
romGdt:
	/* 0(selector=0x0000): Null descriptor */
	.word	0x0000
	.word	0x0000
	.byte	0x00
	.byte	0x00
	.byte	0x00
	.byte	0x00

	/* 1(selector=0x0008): Code descriptor */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 2(selector=0x0010): Data descriptor */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x92			/* Data r/w, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 3(selector=0x0018): Code descriptor, for the nesting interrupt */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 4(selector=0x0020): Code descriptor, for the nesting interrupt */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */


	/* cold start code in PROTECTED MODE(32 bits) */

	.balign 16,0x90
romInit2:
	cli				/* LOCK INTERRUPT */
	movl	$ ROM_STACK,%esp	/* set a stack pointer */

#if	defined (TGT_CPU) && defined (SYMMETRIC_IO_MODE)

        movl    $ MP_N_CPU, %eax
        lock
        incl    (%eax)

#endif	/* defined (TGT_CPU) && defined (SYMMETRIC_IO_MODE) */

	/* WindML + VesaBIOS initialization */
	
#ifdef	INCLUDE_WINDML	
	movl	$ VESA_BIOS_DATA_PREFIX,%ebx	/* move BIOS prefix addr to EBX */
	movl	$ VESA_BIOS_KEY_1,(%ebx)	/* store "BIOS" */
	addl	$4,%ebx				/* increment EBX */
	movl	$ VESA_BIOS_KEY_2,(%ebx)	/* store "DATA" */
	movl	$ VESA_BIOS_DATA_SIZE,%ecx	/* load ECX with nBytes to copy */
	shrl    $2,%ecx				/* get nLongs to copy */
	movl	$0,%esi				/* load ESI with source addr */
	movl	$ VESA_BIOS_DATA_ADDRESS,%edi	/* load EDI with dest addr */
	rep
	movsl					/* copy BIOS data to VRAM */
#endif	/* INCLUDE_WINDML */

	/*
         * Don't call romA20on if booted through SFL. romA20on does ISA
         * I/O port accesses to turn A20 on. In case of SFL boot, ISA
         * I/O address space will not be initialized by properly by the
         * time this code gets executed. Also, A20 comes ON when booted
         * through SFL.
         */

#ifndef INCLUDE_IACSFL	
	call	FUNC(romA20on)		/* enable A20 */
	cmpl	$0, %eax		/* is A20 enabled? */
	jne	romInitHlt		/*   no: jump romInitHlt */
#endif	/* INCLUDE_IACSFL */

	movl    $ BOOT_COLD,%ebx	/* %ebx has the startType */

	/* copy bootrom image to dst addr if (romInit != ROM_TEXT_ADRS) */

warm:
	ARCH_REGS_INIT			/* initialize DR[0-7] CR0 EFLAGS */

#if	(CPU == PENTIUM) || (CPU == PENTIUM2) || (CPU == PENTIUM3) || \
	(CPU == PENTIUM4)

	/* ARCH_CR4_INIT		/@ initialize CR4 for P5,6,7 */

	xorl	%eax, %eax		/* zero EAX */
	movl	%eax, %cr4		/* initialize CR4 */
#endif	/* (CPU == PENTIUM) || (CPU == PENTIUM[234]) */

	movl	$romGdtr,%eax		/* load the original GDT */
	subl	$ FUNC(romInit),%eax
	addl	$ ROM_TEXT_ADRS,%eax
	pushl	%eax	
	call	FUNC(romLoadGdt)

	movl	$ STACK_ADRS, %esp	/* initialise the stack pointer */
	movl    $ ROM_TEXT_ADRS, %esi	/* get src addr(ROM_TEXT_ADRS) */
	movl    $ romInit, %edi		/* get dst addr(romInit) */
	cmpl	%esi, %edi		/* is src and dst same? */
	je	romInit4		/*   yes: skip copying */
	movl    $ FUNC(end), %ecx	/* get "end" addr */
	subl    %edi, %ecx		/* get nBytes to copy */
	shrl    $2, %ecx		/* get nLongs to copy */
	cld 				/* clear the direction bit */
	rep				/* repeat next inst ECX time */
	movsl                           /* copy itself to the entry point */

	/* jump to romStart(absolute address, position dependent) */

romInit4:
	xorl	%ebp, %ebp		/* initialize the frame pointer */
	pushl	$0			/* initialise the EFLAGS */
	popfl
	pushl	%ebx			/* push the startType */
	movl	$ FUNC(romStart),%eax	/* jump to romStart */
	call	*%eax

	/* just in case, if there's a problem in romStart or romA20on */

romInitHlt:
	pushl	%eax
	call	FUNC(romEaxShow)	/* show EAX on your display device */
	hlt	
	jmp	romInitHlt

/*******************************************************************************
*
* romA20on - enable A20
*
* enable A20
*
* RETURNS: N/A

* void romA20on (void)
 
*/

	.balign 16,0x90
FUNC_LABEL(romA20on)
	call	FUNC(romWait)
	movl	$0xd1,%eax		/* Write command */
	outb	%al,$0x64
	call	FUNC(romWait)

	movl	$0xdf,%eax		/* Enable A20 */
	outb	%al,$0x60
	call	FUNC(romWait)

	movl	$0xff,%eax		/* NULL command */
	outb	%al,$0x64
	call	FUNC(romWait)

	movl	$0x000000,%eax		/* Check if it worked */
	movl	$0x100000,%edx
	pushl	(%eax)
	pushl	(%edx)
	movl	$0x0,(%eax)
	movl	$0x0,(%edx)
	movl	$0x01234567,(%eax)
	cmpl	$0x01234567,(%edx)
	popl	(%edx)
	popl	(%eax)
	jne	romA20on0

	/* another way to enable A20 */

	movl	$0x02,%eax
	outb	%al,$0x92

	xorl	%ecx,%ecx
romA20on1:
	inb	$0x92,%al
	andb	$0x02,%al
	loopz	romA20on1

	movl	$0x000000,%eax		/* Check if it worked */
	movl	$0x100000,%edx
	pushl	(%eax)
	pushl	(%edx)
	movl	$0x0,(%eax)
	movl	$0x0,(%edx)
	movl	$0x01234567,(%eax)
	cmpl	$0x01234567,(%edx)
	popl	(%edx)
	popl	(%eax)
	jne	romA20on0

	movl	$ 0xdeaddead,%eax	/* error, can't enable A20 */
	ret

romA20on0:
	xorl	%eax,%eax
	ret

/*******************************************************************************
*
* romLoadGdt - load the global descriptor table.
*
* RETURNS: N/A
*
* NOMANUAL

* void romLoadGdt (char *romGdtr)
 
*/

        .balign 16,0x90
FUNC_LABEL(romLoadGdt)
	movl	SP_ARG1(%esp),%eax
	lgdt	(%eax)
	movw	$0x0010,%ax		/* a selector 0x10 is 3rd one */
	movw	%ax,%ds	
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	ret

/*******************************************************************************
*
* romWait - wait until the input buffer become empty
*
* wait until the input buffer become empty
*
* RETURNS: N/A

* void romWait (void)
 
*/

	.balign 16,0x90
FUNC_LABEL(romWait)
	xorl	%ecx,%ecx
romWait0:
	movl	$0x64,%edx		/* Check if it is ready to write */
	inb	%dx,%al
	andb	$2,%al
	loopnz	romWait0
	ret

/*******************************************************************************
*
* romEaxShow - show EAX register 
*
* show EAX register in your display device 
*
* RETURNS: N/A

* void romEaxShow (void)
 
*/

	.balign 16,0x90
FUNC_LABEL(romEaxShow)

	/* show EAX register in your display device available */

	ret

