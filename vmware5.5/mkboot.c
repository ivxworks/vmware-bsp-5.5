/* mkboot.c - program to make a boot diskette */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01u,18jul02,jkf  SPR#79650, honor newer partition types, now will
                 avoid corrupting FAT32 partitions, added comments,
                 support big sectors and use malloc to use less stack
01t,26jun02,hdn/dmh  updated revision no. in bootStrap[]
01s,25jun02,hdn  replaced bootrom_uncmp with bootrom.bin
01r,04dec01,jkf  using dosFsVolDescGet to see if device exists
01q,24may00,jkf  new bootStrap 1.5, fixed SPR#31310,31445,30280.
01p,12nov99,jkf  Added a bootStrap that has support for VxWorks long 
                 filename (VXEXT 1.0/1.1) matches vxsys.asm 1.4 version. 
                 Added comments.  Removed boot disk value update, since 
                 BIOS hands us the boot disk in dl, when the bootstrap is 
                 called. by convention.  SPR#7452
01o,01oct99,hdn  added support for binary file.
01n,08apr99,jkf  corrected warning, SPR#26453.
01m,27aug98,dat  added config.h, #ifdef INCLUDE_TFFS.
01l,04mar98,hdn	 fixed a problem in mkbootTffs().
01k,03mar98,kbw	 made changes to man page text for mkbootTffs
01j,15dec97,hdn	 added support for TFFS.
01i,15feb96,hdn	 renamed to mkboot.c
01h,12jun95,hdn	 added support for ATA. added mkbootFd and mkbootAta.
01g,24jan95,jdi	 doc cleanup.
01f,25oct94,hdn	 added fdRawio() instead of using raw file system.
		 swapped 1st and 2nd parameter of vxsys().
01e,28may94,hdn	 updated the boot sector to DOS version.
01d,27jan94,hdn	 changed the entry point 0x10000 to 0x8000.
01c,16nov93,hdn	 changed name of the boot image "bootrom.dat" to "bootrom.sys".
01b,07nov93,hdn	 adopted Dave Fraser's idea.
01a,12oct93,hdn	 written
*/

/*
DESCRIPTION
This library creates a boot diskette or disk.  mkbootFd() creates a boot
floppy disk, mkbootAta() creates a boot IDE/ATA disk.

NOMANUAL
*/

#include "vxWorks.h"
#include "config.h"
#include "a_out.h"
#include "errnoLib.h"
#include "ioLib.h"
#include "stdio.h"
#include "string.h"
#include "usrLib.h"
#include "dosFsLib.h"
#include "drv/fdisk/nec765Fd.h"
#include "drv/hdisk/ataDrv.h"
#include "usrFdiskPartLib.h"
#ifdef INCLUDE_TFFS
#include "tffs/tffsDrv.h"
#endif /*INCLUDE_TFFS */


#define VXSYS_FD	0		/* floppy disk */
#define VXSYS_ATA	1		/* ATA disk */
#define VXSYS_TFFS	2		/* flash memory: DOC */
#define VXSYS_DOSDEV	"/vxsysdos/"

/* 
 * VXLONGNAMES will store the "." confusing the bootstrap, FAT/VFAT wont.
 * so we remove the "." for the bootStrap code on longnames detection.
 */
/* MUST MATCH string in bootStrap array.. */
#define VXSYS_FILE     "/vxsysdos/BOOTROM.SYS"
#define VXSYS_FILE_VXEXT "/vxsysdos/BOOTROM SYS"

#define AOUT_MAGIC_MASK	0x0000ffff	/* a.out magic number mask */
#define AOUT_MAGIC	0x00000107	/* a.out magic number */                        


#define VXDOS		"VXDOS"
#define VXEXT		"VXEXT"

#define MAX_SECTOR_SIZE (8 * 512)

/* 448 bytes for bootstrap, + 2 bytes (55aa) signature */
#define BOOTSTRAP_AND_SIGNATURE 450  

/* global */

int vxsysDebug = 0;


/* local */

LOCAL BOOL vxsysBootsec = TRUE;
LOCAL BOOL vxsysBootrom = TRUE; 

LOCAL unsigned char bootStrap[BOOTSTRAP_AND_SIGNATURE] =
    {
    0xFA, 0x88, 0x16, 0x24, 0x7C, 0xB4, 0x08, 0xCD, 
    0x13, 0x72, 0x11, 0x83, 0xE1, 0x3F, 0x89, 0x0E, 
    0x18, 0x7C, 0xFE, 0xC6, 0x32, 0xD2, 0x86, 0xF2, 
    0x89, 0x16, 0x1A, 0x7C, 0xFA, 0x8C, 0xC8, 0x8E, 
    0xD8, 0x8E, 0xC0, 0xFC, 0x8E, 0xD0, 0xBC, 0xFE, 
    0x7B, 0xFB, 0xBE, 0xE1, 0x7D, 0xE8, 0x14, 0x01, 
    0x33, 0xDB, 0x8B, 0xC3, 0x8B, 0xD3, 0xA0, 0x10, 
    0x7C, 0xF7, 0x26, 0x16, 0x7C, 0x8B, 0x0E, 0x0E, 
    0x7C, 0x03, 0x0E, 0x1C, 0x7C, 0x13, 0x16, 0x1E, 
    0x7C, 0x03, 0xC8, 0x13, 0xD3, 0x89, 0x0E, 0x00, 
    0x7B, 0x89, 0x16, 0x02, 0x7B, 0xBE, 0x03, 0x7C, 
    0xBF, 0xF9, 0x7D, 0xC7, 0x06, 0x36, 0x7C, 0x20, 
    0x00, 0xB9, 0x05, 0x00, 0xF3, 0xA6, 0x75, 0x06, 
    0xC7, 0x06, 0x36, 0x7C, 0x40, 0x00, 0xA1, 0x36, 
    0x7C, 0xF7, 0x26, 0x11, 0x7C, 0x8B, 0x36, 0x0B, 
    0x7C, 0x03, 0xC6, 0x48, 0xF7, 0xF6, 0x8B, 0xC8, 
    0x51, 0xA1, 0x00, 0x7B, 0x8B, 0x16, 0x02, 0x7B, 
    0xBB, 0x00, 0x7E, 0xE8, 0xD0, 0x00, 0x73, 0x03, 
    0xE9, 0xA5, 0x00, 0x8B, 0x16, 0x0B, 0x7C, 0xBF, 
    0x00, 0x7E, 0xBE, 0xEB, 0x7D, 0x57, 0xB9, 0x0B, 
    0x00, 0xF3, 0xA6, 0x5F, 0x74, 0x1C, 0x03, 0x3E, 
    0x36, 0x7C, 0x2B, 0x16, 0x36, 0x7C, 0x75, 0xEA, 
    0x83, 0x06, 0x00, 0x7B, 0x01, 0x83, 0x16, 0x02, 
    0x7B, 0x00, 0x59, 0xE2, 0xC3, 0xBE, 0xEA, 0x7D, 
    0xEB, 0x79, 0x59, 0x01, 0x0E, 0x00, 0x7B, 0x83, 
    0x16, 0x02, 0x7B, 0x00, 0x33, 0xC9, 0xBB, 0x1C, 
    0x00, 0x83, 0x3E, 0x36, 0x7C, 0x40, 0x75, 0x03, 
    0x83, 0xC3, 0x20, 0x8B, 0x01, 0x43, 0x43, 0x8B, 
    0x11, 0xF7, 0x36, 0x0B, 0x7C, 0x40, 0xA3, 0x04, 
    0x7B, 0x83, 0xEB, 0x04, 0x8B, 0x01, 0x48, 0x48, 
    0x8A, 0x0E, 0x0D, 0x7C, 0xF7, 0xE1, 0x03, 0x06, 
    0x00, 0x7B, 0x13, 0x16, 0x02, 0x7B, 0xBB, 0x00, 
    0x08, 0x8E, 0xC3, 0x33, 0xDB, 0x50, 0x52, 0xE8, 
    0x54, 0x00, 0x5A, 0x58, 0x72, 0x2A, 0xBE, 0xF7, 
    0x7D, 0xE8, 0x30, 0x00, 0xFF, 0x0E, 0x04, 0x7B, 
    0x74, 0x0D, 0x83, 0xC0, 0x01, 0x83, 0xD2, 0x00, 
    0x8C, 0xC3, 0x83, 0xC3, 0x20, 0xEB, 0xDA, 0xA0, 
    0x24, 0x7C, 0x24, 0x80, 0x75, 0x06, 0xBA, 0xF2, 
    0x03, 0x32, 0xC0, 0xEE, 0xFF, 0x2E, 0x7E, 0x7D, 
    0xBE, 0xE6, 0x7D, 0xE8, 0x06, 0x00, 0xEB, 0xFE, 
    0x00, 0x00, 0x00, 0x08, 0x53, 0x50, 0x80, 0x3E, 
    0xE0, 0x7D, 0x00, 0x75, 0x0E, 0xAC, 0x0A, 0xC0, 
    0x74, 0x09, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 
    0x10, 0xEB, 0xF2, 0x58, 0x5B, 0xC3, 0x89, 0x1E, 
    0x06, 0x7B, 0x8B, 0x1E, 0x18, 0x7C, 0xF7, 0xF3, 
    0x42, 0x8B, 0xDA, 0x33, 0xD2, 0xF7, 0x36, 0x1A, 
    0x7C, 0x86, 0xE0, 0xB1, 0x06, 0xD2, 0xE0, 0x91, 
    0x0A, 0xCB, 0x8A, 0xF2, 0xBB, 0x05, 0x00, 0x53, 
    0x8B, 0x1E, 0x06, 0x7B, 0x8A, 0x16, 0x24, 0x7C, 
    0xB8, 0x01, 0x02, 0x51, 0x52, 0xCD, 0x13, 0x5A, 
    0x59, 0x72, 0x03, 0x5B, 0xF8, 0xC3, 0x33, 0xC0, 
    0xCD, 0x13, 0x5B, 0xFE, 0xCF, 0x75, 0xE0, 0xF9, 
    0xEB, 0xF3, 0x00, 0x56, 0x31, 0x2E, 0x36, 0x00, 
    0x21, 0x52, 0x64, 0x00, 0x21, 0x42, 0x4F, 0x4F, 
    0x54, 0x52, 0x4F, 0x4D, 0x20, 0x53, 0x59, 0x53, 
    0x00, 0x2B, 0x00, 0x56, 0x58, 0x45, 0x58, 0x54, 
    0x55, 0xAA, 
    };  /* VxSYS 1.6 */


/* forward declaration */

LOCAL STATUS vxsys (int device, int ctrl, int drive, int fdType, char *in);


/*******************************************************************************
*
* mkbootFd - create a boot floppy disk from a specified file
*
* This command creates a boot floppy disk from a specified file
*
* EXAMPLES:
* The following example makes a bootable floppy disk from the file named
* `bootrom.bin' to the floppy drive 0: "a:", which contains a type 0 diskette.
* .CS
*     -> mkbootFd 0, 0, "bootrom.bin"
* .CE
*
* RETURNS:
* OK, or ERROR if there is an error copying from <in> to the disk.
*
*/

STATUS mkbootFd 
    (
    int drive,			/* drive number:	(0 - 3)	       */
    int fdType,			/* type of floppy disk: (0 - 1)	       */
    char *in			/* name of file to read: "bootrom.bin" */
    )
    {
    return (vxsys (VXSYS_FD, 0, drive, fdType, in));
    }

/*******************************************************************************
*
* mkbootAta - create a boot ATA disk from a specified file
*
* This command creates a ATA boot disk from a specified file
* Note that the disk should have a FAT16 or a FAT12 volume.
* FAT32 volumes are not supported.  
*
* EXAMPLES:
* The following example makes a bootable hard disk from the file named
* `bootrom.bin' to the ATA ctrl 0, drive 0: "c:" drive.
* .CS
*     -> mkbootAta 0, 0, "bootrom.bin"
* .CE
*
* RETURNS:
* OK, or ERROR if there is an error copying from <in> to the disk.
*
*/

STATUS mkbootAta 
    (
    int ctrl,			/* controller number:	(0 - 1)		 */
    int drive,			/* drive number:	(0 - 1)		 */
    char *in			/* name of file to read: "bootrom.bin" */
    )
    {
    return (vxsys (VXSYS_ATA, ctrl, drive, 0, in));
    }

#ifdef INCLUDE_TFFS
/*******************************************************************************
*
* mkbootTffs - make the specified flash device a boot device
*
* This command sets up the flash device, <drive>, as a boot device.  
* The <removable> parameter expects a 1 if the flash device is removable 
* or a 0 if it is not. The <in> parameter specifies the name of the boot file.
*
* Note that the disk should have a FAT16 or a FAT12 volume.
* FAT32 volumes are not supported.  
*
* EXAMPLES:
* In the following example, the first zero identifies drive zero as the flash
* device to be made into a boot device.  The second zero indicates that the 
* flash is not removable.  The 'bootrom.bin' parameter specifies the name 
* of the boot file.
* .CS
*     -> mkbootTffs 0, 0, "bootrom.bin"
* .CE
*
* RETURNS:
* OK, or ERROR if there is an error copying from <in> to the disk.
*
*/

STATUS mkbootTffs 
    (
    int drive,			/* drive number: (0 - TFFS_MAX_DRIVES - 1) */
    int removable,		/* removable or not: (TRUE - FALSE)	 */
    char *in			/* name of file to read: "bootrom.bin" */
    )
    {
    return (vxsys (VXSYS_TFFS, 0, drive, removable, in));
    }
#endif /*INCLUDE_TFFS */

/*******************************************************************************
*
* vxsys - create a boot device from a specified file
*
* This command changes a boot sector and copies the text and data of 
* a specified file to the output device.
*
* RETURNS:
* OK, or
* ERROR if <in> or <out> cannot be opened/created, or if there is an
* error copying from <in> to <out>.
*
*/

LOCAL STATUS vxsys 
    (
    int device,			/* device type:		(0 - 2)	*/
    int ctrl,			/* controller number:	(0 - 1)	*/
    int drive,			/* drive number:	(0 - 3)	*/
    int fdType,			/* type of floppy disk: (0 - 1)	*/
    char *in			/* name of file to read: "xxx"	*/
    )
    {
    BLK_DEV *pBlkDev;
    int inFd;
    int outFd;
    int ix;
    int bytes;
    struct exec hdr;
    char * lbaSectorOne;
    char * dosVolBootSec;
    char * pSys;
    FD_RAW fdRaw;
    ATA_RAW ataRaw;
    DOS_PART_TBL *pPart;
    BOOL found		= FALSE;
    UINT32 offset	= 0;
#ifdef INCLUDE_TFFS
    int removable	= fdType;
#endif /* INCLUDE_TFFS */

    BOOL useLongNames = FALSE;


    /* allocate buffers */

    lbaSectorOne  = malloc (MAX_SECTOR_SIZE);
    dosVolBootSec = malloc (MAX_SECTOR_SIZE);

    if ((NULL == lbaSectorOne) || (NULL == dosVolBootSec))
        {
        printErr ("mkboot couldn't allocate memory\n");

        if (NULL != lbaSectorOne)
            free (lbaSectorOne);

        if (NULL != dosVolBootSec)
            free (dosVolBootSec);

        return (ERROR);
        }
    else
        {
        bzero (lbaSectorOne, MAX_SECTOR_SIZE);
        bzero (dosVolBootSec, MAX_SECTOR_SIZE);
        }

    switch (device)
	{
	case VXSYS_FD:
            if ((UINT)drive >= FD_MAX_DRIVES)
	        {
	        printErr ("drive is out of range(0-%d).\n", FD_MAX_DRIVES - 1);
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }

            /*                                                         
             * Per MSDOS convention, DOSFS floppy disks never use FDISK
             * partitions.  So presuming raw LBA sector 1 (chs 0,0,1) 
	     * is a DOSFS volumes boot sector is appropriate for nec765 Fd.
	     * Here we setup to do a raw read of sector one.
             * We may also presume a floppy is FAT12.
             */                                                        

            fdRaw.cylinder   = 0;		/* read the DOS boot sector */
            fdRaw.head       = 0;
            fdRaw.sector     = 1;
            fdRaw.pBuf       = dosVolBootSec;
            fdRaw.nSecs      = 1;
            fdRaw.direction  = 0;		/* 0 = read */
            fdRawio (drive, fdType, &fdRaw);

	    /* check to see if vxworks long file names are in use */

	    if (strncmp((char *)&dosVolBootSec[3], VXEXT, strlen(VXEXT)) == 0)
		{
		useLongNames = TRUE;
		}

        if (vxsysDebug)
            printErr ("UseLongNames is %s\n", (useLongNames) ? "TRUE":"FALSE");

            if (vxsysBootsec)
	        {
		/* modify the dos volumes boot sector */

	        dosVolBootSec[0] = 0xeb;	/* x86 relative jump inst */
	        dosVolBootSec[1] = 0x3c;	/* rel address for jmp */
	        dosVolBootSec[2] = 0x90;	/* x86 nop */

                bcopy (bootStrap, (char *)&dosVolBootSec[0x3e], 
                       sizeof (bootStrap));

                fdRaw.direction  = 1;		/* write the boot sector */
                fdRawio (drive, fdType, &fdRaw);
	        }

	    if ((pBlkDev = fdDevCreate (drive, fdType, 0, 0)) == NULL)
		{
		printErr ("Error during fdDevCreate: %x\n", errnoGet ());
                free (dosVolBootSec);
                free (lbaSectorOne);
		return (ERROR);
		}

	    break;

	case VXSYS_ATA:
            if ((UINT)ctrl >= ATA_MAX_CTRLS)
	        {
	        printErr ("ctrl is out of range (0-%d).\n", ATA_MAX_CTRLS -1);
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }
            if ((UINT)drive >= ATA_MAX_DRIVES)
	        {
	        printErr ("drive is out of range(0-%d).\n", ATA_MAX_DRIVES -1);
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }
	

            /*                                                        
             * ATA disks formatted via Windows always have partitions.
             * ATA disks formatted via vxWorks may have partitions or 
             * they may have the dosFs volume start on LBA sector one
             * without any partition.
             *
             * If the dosFs volume starts on LBA sector one, then     
             * the VXDOS, or VXEXT strings will be present there.           
	     *
             * If the partition table (Master Boot Record) resides    
             * on LBA sector one, these strings will not be present.
	     *
	     * MSDOS compatible hard discs always have FDISK style 
             * partitions. For those we must find the bootable partition 
             * sector and modify the dosFs volumes boot sector therein.
	     * The master boot record, the initial partition table,
	     * is always found on LBA sector one on MSDOS hard discs.
	     *
	     * Note that the partition should have a FAT12 or FAT16
	     * dosFs volume formatted upon it.  FAT32 will not work. 
	     */

            ataRaw.cylinder   = 0;
            ataRaw.head       = 0;
            ataRaw.sector     = 1;
            ataRaw.pBuf       = lbaSectorOne;
            ataRaw.nSecs      = 1;
            ataRaw.direction  = 0;
            ataRawio (ctrl, drive, &ataRaw);

	    pSys	= &lbaSectorOne[DOS_BOOT_SYS_ID];
	    pPart	= (DOS_PART_TBL *)&lbaSectorOne[DOS_BOOT_PART_TBL];

    	    if ((strncmp(pSys, VXDOS, strlen(VXDOS)) != 0) &&
        	(strncmp(pSys, VXEXT, strlen(VXEXT)) != 0))
        	{
	        for (ix = 0; ix < 4; ix++)
		    {
	            /* 
		     * Note that we are supporting partitions
		     * that could have FAT32 filesystems upon them.
		     * vxLd 1.x will ONLY work on FAT12 and FAT16
		     * filesystem, it will NOT work on FAT32,
		     * but we should still accept the partition types.
		     * Note that dosFsVolFormat can force FAT type.
		     */
		    if (pPart->dospt_status == PART_IS_BOOTABLE)
		        if ((pPart->dospt_type == PART_TYPE_DOS12 ) ||
		            (pPart->dospt_type == PART_TYPE_DOS3  ) ||
		            (pPart->dospt_type == PART_TYPE_DOS4  ) ||
		            (pPart->dospt_type == PART_TYPE_DOS32 ) ||
		            (pPart->dospt_type == PART_TYPE_DOS32X) ||
		            (pPart->dospt_type == PART_TYPE_WIN95_D4))
			    {
			    found = TRUE;
			    break;
			    }
		    pPart++;
		    }
	    
	        if (!found)
		    {
                    printErr ("Can't find the primary DOS partition.\n");
                    free (dosVolBootSec);
                    free (lbaSectorOne);
	            return (ERROR);
		    }

	        ataRaw.cylinder	= (pPart->dospt_startSec & 0xf0) >> 4;
	        ataRaw.head    	= pPart->dospt_startHead;
                ataRaw.sector  	= pPart->dospt_startSec & 0x0f;
                ataRaw.pBuf	= dosVolBootSec;

                ataRawio (ctrl, drive, &ataRaw); /* read the boot sector */

		offset = pPart->dospt_absSec;

	        if (strncmp((char *)&dosVolBootSec[3], VXEXT, 
					 strlen(VXEXT)) == 0)
		    {
		    useLongNames = TRUE;
		    }

                if (vxsysDebug)
            	    printErr ("UseLongNames is %s\n", 
			      (useLongNames) ? "TRUE":"FALSE");
		}
	    else  /* disk is formatted with vxWorks without partition */
		{
	        if (strncmp(pSys, VXEXT, strlen(VXEXT)) == 0)
		    {
		    useLongNames = TRUE;
		    }

                if (vxsysDebug)
            	    printErr ("UseLongNames is %s\n", 
			      (useLongNames) ? "TRUE":"FALSE");

                ataRaw.cylinder   = 0;	/* read the dosFs volume boot sector */
                ataRaw.head       = 0;
                ataRaw.sector     = 1;
                ataRaw.pBuf       = dosVolBootSec;
                ataRaw.nSecs      = 1;
                ataRaw.direction  = 0;
                ataRawio (ctrl, drive, &ataRaw);
		}
	    

            /*
             * Below, we attempt to avoid corrupting a FAT32 image.
             * For FAT32, the 16bit sectors per FAT field (at offset
             * 0x16 in a MSDOS FAT volumes boot record) will allways
             * be zero.  For FAT12, FAT16 it is always non-zero.
             * We will not continue if this offset is zero.
             */

            if ((UINT16) (0x0000) == (UINT16)(dosVolBootSec [0x16])) 
                {
                printErr ("\nThis appears to be a FAT32 volume.\n");
                printErr ("Sectors per FAT, offset 0x16 in the volumes "
                          "boot record is zero.\n");
                printErr ("mkbootAta supports only FAT16 or FAT12.\n");
                printErr ("dosFsVolFormat can force a FAT type.\n");
                free (dosVolBootSec);
                free (lbaSectorOne);
                return (ERROR);
                }

            if (vxsysBootsec)
	        {
	        dosVolBootSec[0] = 0xeb;	/* modify the boot sector */
	        dosVolBootSec[1] = 0x3c;
	        dosVolBootSec[2] = 0x90;

                bcopy (bootStrap, (char *)&dosVolBootSec[0x3e], 
				sizeof (bootStrap));
                ataRaw.direction  = 1;		/* write the boot sector */
                ataRawio (ctrl, drive, &ataRaw);
	        }

	    if ((pBlkDev = ataDevCreate (ctrl, drive, 0, offset)) == NULL)
		{
		printErr ("Error during fdDevCreate: %x\n", errnoGet ());
                free (dosVolBootSec);
                free (lbaSectorOne);
		return (ERROR);
		}

	    break;


#ifdef INCLUDE_TFFS
	case VXSYS_TFFS:
            if ((UINT)drive >= TFFS_MAX_DRIVES)
	        {
	        printErr ("drive is out of range (0-%d).\n", 
				TFFS_MAX_DRIVES-1);
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }

	    /* mount the TFFS */

	    if ((pBlkDev = tffsDevCreate (drive, removable)) == NULL)
		{
		printErr ("Error during fdDevCreate: %x\n", errnoGet ());
                free (dosVolBootSec);
                free (lbaSectorOne);
		return (ERROR);
		}

            /* read the partition table */

            tffsRawio (drive, TFFS_ABS_READ, 0, 1, (int)&lbaSectorOne);

	    offset	= 0;

	    pSys	= &lbaSectorOne[DOS_BOOT_SYS_ID];
	    pPart	= (DOS_PART_TBL *)&lbaSectorOne[DOS_BOOT_PART_TBL];

    	    if ((strncmp(pSys, VXDOS, strlen(VXDOS)) != 0) &&
        	(strncmp(pSys, VXEXT, strlen(VXEXT)) != 0))
        	{
	        for (ix = 0; ix < 4; ix++)
		    {
		    if (pPart->dospt_status == PART_IS_BOOTABLE)
		        if ((pPart->dospt_type == PART_TYPE_DOS12 ) ||
		            (pPart->dospt_type == PART_TYPE_DOS3  ) ||
		            (pPart->dospt_type == PART_TYPE_DOS4  ) ||
		            (pPart->dospt_type == PART_TYPE_DOS32 ) ||
		            (pPart->dospt_type == PART_TYPE_DOS32X) ||
		            (pPart->dospt_type == PART_TYPE_WIN95_D4))
			    {
			    found = TRUE;
			    break;
			    }
		    pPart++;
		    }
	    
	        if (found)
		    offset = pPart->dospt_absSec;
		}

            /* read the boot sector */

            tffsRawio (drive, TFFS_ABS_READ, offset, 1, (int)&dosVolBootSec);

	    if (strncmp((char *)&dosVolBootSec[3], VXEXT, strlen(VXEXT)) == 0)
		{
		useLongNames = TRUE;
		}

            if (vxsysDebug)
                printErr ("UseLongNames is %s\n", 
			  (useLongNames) ? "TRUE":"FALSE");

            /*
             * Below, we attempt to avoid corrupting a FAT32 image.
             * For FAT32, the 16bit sectors per FAT field (at offset
             * 0x16 in a MSDOS FAT volumes boot record) will allways
             * be zero.  For FAT12, FAT16 it is always non-zero.
             * We will not continue if this offset is zero.
             */

            if ((UINT16)(0x0000) == (UINT16)(dosVolBootSec [0x16])) 
                {
                printErr ("\nThis appears to be a FAT32 volume.\n");
                printErr ("Sectors per FAT, offset 0x16 in the volumes "
                          "boot record is zero.\n");
                printErr ("mkbootTffs supports only FAT16 or FAT12.\n");
                printErr ("dosFsVolFormat can force a FAT type.\n");
                free (dosVolBootSec);
                free (lbaSectorOne);
                return (ERROR);
                }

            if (vxsysBootsec)
	        {
	        dosVolBootSec[0] = 0xeb;	/* modify the boot sector */
	        dosVolBootSec[1] = 0x3c;
	        dosVolBootSec[2] = 0x90;
                bcopy (bootStrap, (char *)&dosVolBootSec[0x3e], 
				sizeof (bootStrap));

                /* write the boot sector */

                tffsRawio (drive, TFFS_ABS_WRITE, offset, 1, 
				(int)&dosVolBootSec);
	        }

	    break;
#endif /*INCLUDE_TFFS */

	default:
	    printErr ("unknown device (0-1).\n");
            free (dosVolBootSec);
            free (lbaSectorOne);
	    return (ERROR);
	}


    if (vxsysBootrom)
	{
        /* read the header to get a text-size and a data-size */

        if ((inFd = open (in, O_RDONLY, 0644)) == ERROR)
            {
            printErr ("Can't open \"%s\"\n", in);
            free (dosVolBootSec);
            free (lbaSectorOne);
	    return (ERROR);
	    }

        if ((read (inFd, (char *)&hdr, sizeof(hdr))) == ERROR)
	    {
	    printErr ("Error during read header: %x\n", errnoGet ());
            free (dosVolBootSec);
            free (lbaSectorOne);
	    return (ERROR);
	    }

        if (vxsysDebug)
            printErr ("text=0x%x data=0x%x\n", hdr.a_text, hdr.a_data);

	/* check the magic number to find out if it is a.out or binary */

	if ((hdr.a_magic & AOUT_MAGIC_MASK) == AOUT_MAGIC)	/* a.out */
	    {
            bytes = hdr.a_text + hdr.a_data;
	    }
	else							/* binary */
	    {
            bytes = ROM_SIZE;
	    close (inFd);
	    if ((inFd = open (in, O_RDONLY, 0644)) == ERROR)
		{
		printErr ("Can't open \"%s\"\n", in);
                free (dosVolBootSec);
                free (lbaSectorOne);
		return (ERROR);
		}
	    }                                                                       

        if (NULL == dosFsVolDescGet(VXSYS_DOSDEV, (u_char **)&pSys))
            {
            if ((dosFsDevInit (VXSYS_DOSDEV, pBlkDev, NULL)) == NULL)
	        {
	        printErr ("Error during dosFsDevInit: %x\n", errnoGet ());
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }
            }

	if (useLongNames == FALSE)
	    {
            if ((outFd = open (VXSYS_FILE, O_CREAT | O_RDWR, 0644)) == ERROR)
	        {
                close (inFd);
                printErr ("Can't open \"%s\"\n", VXSYS_FILE);
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }
	    }
	else /* useLongNames file name */
	    {
            if ((outFd = open (VXSYS_FILE_VXEXT, O_CREAT | O_RDWR, 0644)) 
		== ERROR)
	        {
                close (inFd);
                printErr ("Can't open \"%s\"\n", VXSYS_FILE);
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }
	    }

        if (ioctl (outFd, FIOCONTIG, bytes) == ERROR)
	    {
	    printErr ("Error during ioctl FIOCONTIG: %x\n", errnoGet ());
            free (dosVolBootSec);
            free (lbaSectorOne);
	    return (ERROR);
	    }

        /* read text and data, write them to the diskette */

        for (ix = 0; ix < bytes; ix += MAX_SECTOR_SIZE)
	    {
	    if (read (inFd, dosVolBootSec, MAX_SECTOR_SIZE) == ERROR)
	        {
	        printErr ("Error during read file: %x\n", errnoGet ());
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }

	    if (write (outFd, dosVolBootSec, MAX_SECTOR_SIZE) == ERROR)
	        {
	        printErr ("Error during write fd: %x\n", errnoGet ());
                free (dosVolBootSec);
                free (lbaSectorOne);
	        return (ERROR);
	        }
	    }

        free (dosVolBootSec);
        free (lbaSectorOne);
	iosDevDelete (iosDevFind (VXSYS_DOSDEV,&pSys));
        close (inFd);
        close (outFd);
        }
    return (OK);
    }
