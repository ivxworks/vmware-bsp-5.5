/* sysNvRam.c - x86 nvram api for block devices */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include <copyright_wrs.h>

/*
modification history
--------------------
01b,18jul02,rhe  Remove possible compile time warning message. SPR 79857
01a,20jun02,dmh  created. now even without eeprom peecees have storage for the bootline
*/

/*
DESCRIPTION:

This file implements the nvram api using standard io system calls
for non-volatile media such as flash ram, floppy disks and hard disk
drives.  It is primarily intended for saving and modifying boot parameters
but could in theory be used for general purpose information.  A file of
NV_RAM_SIZE size is used for storing the "nvram" data.
*/

/* includes */

#include <vxWorks.h>
#include <logLib.h>
#include <string.h>
#include "config.h"

#define NVRAMFILE  "nvram.txt"
#define NVRAMPATH   BOOTROM_DIR""NVRAMFILE

/*******************************************************************************
*
* sysNvRam_mount - make file system accessible
*
* This routine is called from sysNvRamSet and sysNvRamGet and handles
* the storage media specific details.
*
* RETURNS: N/A
*
* NOMANUAL
*/

static void sysNvRam_mount()
    {
    int ctrl = 0;
    u_char * pVolDesc;
    

#if defined(INCLUDE_ATA) && (SYS_WARM_TYPE == SYS_WARM_ATA)
    IMPORT ATA_RESOURCE ataResources[];
    ATA_RESOURCE *pAtaResource	= &ataResources[ctrl];
#endif    
    ctrl = 0;

    /* if BOOTROM_DIR has already been mounted do not try remounting it */

    if(NULL == dosFsVolDescGet(BOOTROM_DIR, &pVolDesc))
        {
        dosFsInit (NUM_DOSFS_FILES);
#if defined(INCLUDE_FD) && (SYS_WARM_TYPE == SYS_WARM_FD)
        if (fdDrv (FD_INT_VEC, FD_INT_LVL) != OK)
            {
            return;
            }

        if(usrFdConfig(ctrl, 0, BOOTROM_DIR) != OK)
            {
            logMsg("%s: usrFdConfig failed\n", (int)__FUNCTION__,2,3,4,5,6);
            return;
            }
#endif
#if defined(INCLUDE_ATA) && (SYS_WARM_TYPE == SYS_WARM_ATA)
        if (ataDrv
            (ctrl, pAtaResource->drives, pAtaResource->intVector,
             pAtaResource->intLevel, pAtaResource->configType,
             pAtaResource->semTimeout, pAtaResource->wdgTimeout) == ERROR)
            {
            return;
            }

        if(usrAtaConfig(ctrl,0,BOOTROM_DIR) != OK)
            {
            logMsg("%s: usrAtaConfig failed\n", (int)__FUNCTION__,2,3,4,5,6);
            return;
            }
#endif
#if defined(INCLUDE_TFFS) && (SYS_WARM_TYPE == SYS_WARM_TFFS)
        if (tffsDrv () != OK)
            {
            return;
            }

        if(usrTffsConfig(ctrl,0,BOOTROM_DIR) != OK)
            {
            logMsg("%s: usrTffsConfig failed\n", (int)__FUNCTION__,2,3,4,5,6);
            return;
            }
#endif        
        }
    }

/******************************************************************************
*
* sysNvRamGet - get the contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.  The string is terminated with an EOS.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamSet()
*/

STATUS sysNvRamGet
    (
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
    )
    {
    int fd, bytes;
    
    if ((offset < 0)
     || (strLen < 0)
     || ((offset + strLen) > NV_RAM_SIZE))
        return (ERROR);

    sysNvRam_mount();
    
    fd = open(NVRAMPATH, O_RDWR, 0);
    if(fd == ERROR)
        {
        logMsg("%s: open failed\n", (int)__FUNCTION__, 2, 3, 4, 5, 6);
        return (ERROR);
        }

    bytes = read(fd, string, strLen);
    if(bytes == ERROR)
        {
        logMsg("%s: read failed\n", (int)__FUNCTION__, 2, 3, 4, 5, 6);
        close(fd);
        return (ERROR);
        }
    else
        {
        string[bytes] = EOS;
        close(fd);
        return (OK);
        }
    }


/*******************************************************************************
*
* sysNvRamSet - write to non-volatile RAM
*
* This routine copies a specified string into non-volatile RAM.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamGet()
*/

STATUS sysNvRamSet
    (
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    )
    {
    int fd;

    if ((offset < 0)
     || (strLen < 0)
     || ((offset + strLen) > NV_RAM_SIZE))
        return ERROR;

    sysNvRam_mount();

    fd = open(NVRAMPATH, O_RDWR | O_CREAT, 2);
    if(fd == ERROR)
        {
        logMsg("%s: open failed\n", (int)__FUNCTION__, 2, 3, 4, 5, 6);
        return (ERROR);
        }
  
    if(write(fd, string, strLen) != strLen)
        {
        logMsg("%s: write failed\n", (int)__FUNCTION__, 2, 3, 4, 5, 6);
        close(fd);
        return (ERROR);
        }
    else
        {
        close(fd);
        return (OK);
        }
    }


