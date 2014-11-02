/* sysElt3c509End.c - system configuration module for ELT 3C509 END */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,15jun02,dmh  add support for 3c589
01f,22oct01,pai  Updated documentation and cleaned up format.
01e,16jul01,hdn  renamed INT_VEC_ELT to INT_NUM_GET(INT_LVL_ELT)
01b,31mar99,sbs  corrected elt3c509ParamTemplate initialization.
                 (SPR #26208)
01a,09mar99,sbs  written sysNE2000End.c
*/


/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks
elt3c509End (elt) END driver.  It has routines for initializing
device resources and provides BSP-specific elt3c509End driver
routines for 3COM 3C509 EtherLink III network interface cards.

It performs the dynamic parameterization of the elt3c509End driver.
This technique of 'just-in-time' parameterization allows driver
parameter values to be declared as any other defined constants rather
than as static strings.

NOTE
This module and the supporting BSP files assume that, at most, one
elt3c509End driver unit will be configured, initialized, and loaded
to the MUX.  Additional entries in the END device table, <endDevTbl>,
in the BSP configNet.h file are not sufficient to load additional
driver and device instances to the MUX in the case of the elt3c509End 
driver.
*/


#if defined(INCLUDE_ELT_3C509_END)

/* includes */

#include <end.h>

#ifdef INCLUDE_PCMCIA
#include <drv/pcmcia/pcmciaLib.h>
#endif

/* defines */

#define ELT_ATTACHMENT_TYPE    (0)
#define ELT_RX_FRAMES         (64)

#ifndef ELT_MAX_DEV
#define ELT_MAX_DEV            (1)    /* max number of devices configured */
#endif /* ENE_MAX_DEV */


/* imports */

IMPORT END_OBJ * elt3c509Load (char *);


/******************************************************************************
*
* sysElt3c509EndLoad - construct a load string and load an elt3c509End device
*
* This routine will be invoked by the MUX for the purpose of loading an
* elt3c509End (elt) device with initial parameters.  This routine is
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
* The complete elt3c509End driver load string has format:
*
*     <unit>:<ioPort>:<intVec>:<intLevel>:<attachmentType>:<nRxFrames>
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <pParamStr> was NULL.
*
* SEE ALSO: elt3c509Load()
*/
END_OBJ * sysElt3c509EndLoad
    (
    char *    pParamStr,   /* pointer to initialization parameter string */
    void *    unused       /* unused optional argument */
    )
    {
    END_OBJ * pEnd;
    char      paramStr [END_INIT_STR_MAX];



    if (strlen (pParamStr) == 0)
        {
        /* PASS (1)
         * The driver load routine returns the driver name in <pParamStr>.
         */

        pEnd = elt3c509Load (pParamStr);
        }
    else
        {
        /* PASS (2)
         * The END <unit> number is prepended to <pParamStr>.  Construct
         * the rest of the driver load string by appending parameters to
         * the END <unit> number.
         */

        char * holder   = NULL;
        int    endUnit  = atoi (strtok_r (pParamStr, ":", &holder));
        int    port     = IO_ADRS_ELT;
        int    ilevel   = INT_NUM_GET (INT_LVL_ELT);
        int    irq      = INT_LVL_ELT;
        int    atype    = ELT_ATTACHMENT_TYPE;
        int    rxframes = ELT_RX_FRAMES;


#ifdef INCLUDE_PCMCIA
        static int nextsock = 0;
        int sock;

        const PCMCIA_CTRL * const pCtrl = &pcmciaCtrl;
        const PCMCIA_CHIP * const pChip = &(pCtrl->chip);

        /* If the number of supported NICs exceeds the number of available
         * sockets then start examining ISA cards.  This will prevent
         * overindexing of the card array.  If the socket is not a supported
         * NIC then assume it is an ISA card and use default values.
         */

        for(sock = nextsock; sock < pChip->socks; sock++)
            {
            const NETIF * const pNetIf = (pCtrl->card[sock]).pNetIf;

            if (pNetIf != NULL)     /* if this is a supported NIC */
                {
                port     = (int) pNetIf->arg1;
                ilevel   = pNetIf->arg2;
                irq      = pNetIf->arg3;
                atype    = pNetIf->arg5;
                rxframes = pNetIf->arg4;
                nextsock++;
                break;
                }
            }
#endif  /* INCLUDE_PCMCIA */


        /* finish off the initialization parameter string */

        sprintf (paramStr, "%d:%#x:%#x:%#x:%d:%d",
                 endUnit, port, ilevel, irq, atype, rxframes);

        if ((pEnd = elt3c509Load (paramStr)) == (END_OBJ *) NULL)
            {
            printf ("Error elt3c509Load:  failed to load driver.\n");
            }
        }

    return (pEnd);
    }

/*******************************************************************************
*
* sysEltIntEnable - enable ELT 3C509 ethernet device interrupts
*
* This routine enables ELT 3C509 interrupts.  This may involve operations
* on interrupt control hardware.
*
* RETURNS: N/A
*
* NOMANUAL
*/
void sysEltIntEnable
    (
    int intLevel        /* irq level */
    )
    {
    sysIntEnablePIC (intLevel);
    }

/*******************************************************************************
*
* sysEltIntDisable - disable ELT 3C509 ethernet device interrupts
*
* This routine disables ELT 3C509 interrupts.  This may involve operations
* on interrupt control hardware.
*
* RETURNS: N/A
*
* NOMANUAL
*/
void sysEltIntDisable
    (
    int intLevel        /* irq level */
    )
    {
    sysIntDisablePIC (intLevel);
    }

#endif /* INCLUDE_ELT_3C509_END */
