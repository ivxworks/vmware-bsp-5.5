/* sysSerial.c - PC386/486 BSP serial device initialization */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,10apr02,pai  Implement boundary checking on channel value in
                 sysSerialChanGet() (SPR 74369).
01j,14mar02,hdn  added sysBp checking for HTT (spr 73738)
01i,23oct01,dmh  add documentation for additional serial ports. (spr 5704)
01h,12sep01,hdn  renamed COM[12]_INT_VEC to INT_NUM_COM[12].
01g,21sep98,fle  added library description
01f,19jun96,wlf  doc: cleanup.
01e,23oct95,jdi  doc: cleaned up and removed all NOMANUALs.
01d,03aug95,myz  fixed the warning message
01c,20jun95,ms   fixed comments for mangen
01b,15jun95,ms	 updated for new serial driver
01a,15mar95,myz  written based on mv162 version.
*/

/*
DESCRIPTION

This library contains routines for PC386/486 BSP serial device initialization
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "drv/sio/i8250Sio.h"

/* typedefs */

typedef struct
    {
    USHORT vector;
    ULONG  baseAdrs;
    USHORT regSpace;
    USHORT intLevel;
    } I8250_CHAN_PARAS;


/* includes */

#ifdef INCLUDE_PC_CONSOLE		/* if KBD and VGA console needed */
#   include "serial/pcConsole.c"
#   include "serial/m6845Vga.c"
#   if (PC_KBD_TYPE == PC_PS2_101_KBD)	/* 101 KEY PS/2 */
#       include "serial/i8042Kbd.c"
#   else
#       include "serial/i8048Kbd.c"	/* 83 KEY PC/PCXT/PORTABLE */
#   endif /* (PC_KBD_TYPE == PC_XT_83_KBD) */
#endif /* INCLUDE_PC_CONSOLE */


/* defines */

#define UART_REG(reg,chan) \
    (devParas[chan].baseAdrs + reg*devParas[chan].regSpace)


/* locals */

static I8250_CHAN  i8250Chan[N_UART_CHANNELS];

static I8250_CHAN_PARAS devParas[] = 
    {
      {INT_NUM_COM1,COM1_BASE_ADR,UART_REG_ADDR_INTERVAL,COM1_INT_LVL},
      {INT_NUM_COM2,COM2_BASE_ADR,UART_REG_ADDR_INTERVAL,COM2_INT_LVL}
#if FALSE
/* Only two UART Channels are supported out of the box using defacto standard
   PC com port settings because the serial driver does not support shared
   interrupts. More than 2 serial channels can be supported if the user is
   willing customize this BSP a little by finding unused IRQs. For example,
   IRQ5 and IRQ2(9) are often unused.  See the N_UART_CHANNELS macro in pc.h
   for additional changes necessary for supporting 3 or more serial ports.
   Sometimes the BIOS also needs settings changed.
   SPR# 5704 */

      ,{INT_NUM_COM3,COM3_BASE_ADR,UART_REG_ADDR_INTERVAL,COM3_INT_LVL},
       {INT_NUM_COM4,COM4_BASE_ADR,UART_REG_ADDR_INTERVAL,COM4_INT_LVL}
#endif      
    };


/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiescent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiescent state.  It is called from sysHwInit() with
* interrupts locked.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit()
*/


void sysSerialHwInit (void)
    {
    int i;

    for (i = 0; i < N_UART_CHANNELS; i++)
        {
	i8250Chan[i].int_vec = devParas[i].vector;
	i8250Chan[i].channelMode = 0;
	i8250Chan[i].lcr =  UART_REG(UART_LCR,i);
	i8250Chan[i].data =  UART_REG(UART_RDR,i);
	i8250Chan[i].brdl = UART_REG(UART_BRDL,i);
	i8250Chan[i].brdh = UART_REG(UART_BRDH,i);
	i8250Chan[i].ier =  UART_REG(UART_IER,i);
	i8250Chan[i].iid =  UART_REG(UART_IID,i);
	i8250Chan[i].mdc =  UART_REG(UART_MDC,i);
	i8250Chan[i].lst =  UART_REG(UART_LST,i);
	i8250Chan[i].msr =  UART_REG(UART_MSR,i);

	i8250Chan[i].outByte = sysOutByte;
	i8250Chan[i].inByte  = sysInByte;

	if (sysBp)
	    i8250HrdInit(&i8250Chan[i]);
        }

    }
/******************************************************************************
*
* sysSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  
* 
* Serial device interrupts cannot be connected in sysSerialHwInit() because
* the kernel memory allocator is not initialized at that point, and
* intConnect() calls malloc().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/

void sysSerialHwInit2 (void)
    {
    int i;

    /* connect serial interrupts */

    for (i = 0; i < N_UART_CHANNELS; i++)
        if (i8250Chan[i].int_vec)
	    {
            (void) intConnect (INUM_TO_IVEC (i8250Chan[i].int_vec),
                                i8250Int, (int)&i8250Chan[i] );
	    if (sysBp)
                sysIntEnablePIC (devParas[i].intLevel); 
            }

    }


/******************************************************************************
*
* sysSerialChanGet - get the SIO_CHAN device associated with a serial channel
*
* This routine gets the SIO_CHAN device associated with a specified serial
* channel.
*
* RETURNS: A pointer to the SIO_CHAN structure for the channel, or ERROR
* if the channel is invalid.
*/

SIO_CHAN * sysSerialChanGet
    (
    int channel		/* serial channel */
    )
    {
    if ((channel >= 0) && (channel < N_UART_CHANNELS))
        {
        return ((SIO_CHAN * ) &i8250Chan[channel]);
        }

    return ((SIO_CHAN *) ERROR);
    }


