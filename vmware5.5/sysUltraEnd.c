/* sysUltraEnd.c - system configuration module for SMC Elite Ultra END */

/* Copyright 1999-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,22oct01,pai  Updated documentation and cleaned up format.
01b,11sep01,hdn  replaced INT_VEC_ULTRA with INT_NUM_GET (INT_LVL_ULTRA)
01a,09mar99,sbs  written from sysNE2000End.c
*/


/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks 
SMC Elite Ultra (ultra) END driver.  It has routines for initializing
device resources and provides BSP-specific ultraEnd driver routines
for SMC Ultra Elite and compatible network interface cards.

It performs the dynamic parameterization of the ultraEnd driver.
This technique of 'just-in-time' parameterization allows driver
parameter values to be declared as any other defined constants rather 
than as static strings. 

NOTE
This module and the supporting BSP files assume that, at most, one
ultraEnd driver unit will be configured, initialized, and loaded
to the MUX.  Additional entries in the END device table, <endDevTbl>,
in the BSP configNet.h file are not sufficient to load additional
driver and device instances to the MUX in the case of the ultraEnd 
driver.
*/


#if defined(INCLUDE_ULTRA_END)

/* includes */

#include "end.h"


/* defines */

#define ULTRA_OFFSET     (0)     /* memory offset for alignment */

#ifndef ULTRA_MAX_DEV
#define ULTRA_MAX_DEV    (1)     /* max number of devices configured */
#endif /* ENE_MAX_DEV */


/* imports */

IMPORT END_OBJ * ultraLoad (char *);


/******************************************************************************
*
* sysUltraEndLoad - construct a load string and load an ultraEnd device
*
* This routine will be invoked by the MUX for the purpose of loading an
* ultraEnd (ultra) device with initial parameters.  This routine is
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
* The complete ultraEnd driver load string has format:
*
*     <unit>:<ioAddr>:<memAddr>:<vecNum>:<intLvl>:<config>:<offset>
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <pParamStr> was NULL.
*
* SEE ALSO: ultraLoad()
*/
END_OBJ * sysUltraEndLoad
    (
    char *    pParamStr,   /* pointer to initialization parameter string */
    void *    unused       /* unused optional argument */
    )
    {
    END_OBJ * pEnd;
    char      paramStr [END_INIT_STR_MAX];

    static const char * const paramTemplate = "%d:%#x:%#x:%#x:%#x:%#x:%x";


    if (strlen (pParamStr) == 0)
        {
        /* PASS (1)
         * The driver load routine returns the driver name in <pParamStr>.
         */

        pEnd = ultraLoad (pParamStr);
        }
    else
        {
        /* PASS (2)
         * The END <unit> number is prepended to <pParamStr>.  Construct
         * the rest of the driver load string by appending parameters to
         * the END <unit> number.
         */

        char * holder  = NULL;
        int    endUnit = atoi (strtok_r (pParamStr, ":", &holder));


        /* finish off the initialization parameter string */

        sprintf (paramStr, paramTemplate, 
                 endUnit,
                 IO_ADRS_ULTRA,
                 MEM_ADRS_ULTRA,
                 INT_NUM_GET (INT_LVL_ULTRA),
                 INT_LVL_ULTRA,
                 CONFIG_ULTRA,
                 ULTRA_OFFSET);

        if ((pEnd = ultraLoad (paramStr)) == (END_OBJ *) NULL)
            {
            printf ("Error ultraLoad:  failed to load driver.\n");
            }
        }

    return (pEnd);
    }
 
/*******************************************************************************
*
* sysUltraIntEnable - enable SMC Ultra ethernet device interrupts
*
* This routine enables SMC Ultra interrupts.  This may involve operations
* on interrupt control hardware.
*
* RETURNS: N/A
*
* NOMANUAL
*/
void sysUltraIntEnable
    (
    int intLevel        /* irq level */
    )
    {
    sysIntEnablePIC (intLevel);
    }

/*******************************************************************************
*
* sysUltraIntDisable - disable SMC Ultra ethernet device interrupts
*
* This routine disables SMC Ultra interrupts.  This may involve operations
* on interrupt control hardware.
*
* RETURNS: N/A
*
* NOMANUAL
*/
void sysUltraIntDisable
    (
    int intLevel        /* irq level */
    )
    {
    sysIntDisablePIC (intLevel);
    }

#endif /* INCLUDE_ULTRA_END */
