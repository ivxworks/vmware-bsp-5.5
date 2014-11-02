/* sysScsi.c - x86 SCSI-2 initialization for sysLib.c */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,19nov01,hdn replaced "irq + INT_NUM_IRQ0" with INT_NUM_GET (irq)
01h,19mar98,dat made #include cdromFsLib, conditional on INCLUDE_CDROMFS
	        (cdromFsLib will be productized later).
01g,17mar98,sbs moved AIC7880 specific declarations to aic7880.h header file.
01f,12mar98,sbs added sysAic7880PciInit().
                removed device specific PCI initialization from sysScsiInit().
                added AIC7880 PCI host adapter definitions.
01e,03mar98,sbs removed update of sysPhysMemDesc[].
                using sysMmuMapAdd() for adding a Mmu entry.
01d,22aug97,dds sysScsiInit returns error if SCSI device not found.
01c,05aug97,dds added modifications to routine sysScsiInit ().
01d,22aug97,dds sysScsiInit returns error if SCSI device not found.
01c,05aug97,dds added modifications to routine sysScsiInit ().
01b,11jul97,dds added aic7880.h header file.
01a,10jul97,dds written (from template68k/sysScsi.c, ver 01e).
*/

/* 
DESCRIPTION:

This file contains the sysScsiInit() and related routines necessary for
initializing the SCSI subsystem. The routine "sysScsiInit" is called during
system startup, the routine scans the PCI bus to check if any SCSI Host
Adapter is present. If it finds a SCSI Host Adapter it creates the SCSI
Controller Structure with a default bus Id (SCSI_DEF_CTRL_BUS_ID), 
Initializes the Host Adapter, connects the Interupt Service Routine (ISR) 
and enables the SCSI interupt. The SCSI interupt level and interupt vector
are defined by the macros SCSI_INT_LVL and SCSI_INT_VEC. This routine
essentialy initializes the SCSI system and the SCSI manager task which is 
then ready to execute SCSI transactions. 
*/

#ifdef  INCLUDE_SCSI

/* includes */

#include "sysLib.h"
#include "config.h"
#include "vxWorks.h"

#ifdef	INCLUDE_SCSI2
#include "drv/scsi/aic7880.h"
#include "tapeFsLib.h"
#endif

#ifdef INCLUDE_CDROMFS
#include "cdromFsLib.h"
STATUS cdromFsInit (void);
#endif

#define UNKNOWN -1

/* typedefs */

typedef struct aic7880Info
    {
    UINT32 	pciBus;
    UINT32	pciDevice;
    UINT32	pciFunc;
    char	irq; 	
    } AIC7880_INFO;

/* locals */

LOCAL AIC7880_INFO aic7880InfoTable =
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN};


#ifdef INCLUDE_AIC_7880
/*******************************************************************************
*
* sysAic7880PciInit - initialize PCI specific configuration
*
* This routine does PCI specific device initialization. 
*
*/

void sysAic7880PciInit(void)
    {

    int   busNo;          /* PCI bus number              */
    int   devNo;          /* PCI device number           */
    int   funcNo;         /* PCI function number         */
    int   index = 0;      /* desired instance of device  */
    UINT32 membaseCsr;    /* base address 0 */
    UINT32 iobaseCsr;     /* base address 1 */
    char irq;	          /* IRQ level */	
    AIC7880_INFO *aicRes; 

    aicRes = &aic7880InfoTable;

    if ((pciFindDevice (AIC7880_PCI_VENDOR_ID, AIC7880_PCI_DEVICE_ID,
			index, &busNo, &devNo, &funcNo)) != OK)
        {
        logMsg("AIC 7880 SCSI controller not found\n", 0, 0, 0, 0, 0, 0); 
        return;
        }

    /* if PCI_CFG_FORCE is specified then set configuration parameters */ 

    if (PCI_CFG_TYPE == PCI_CFG_FORCE)
        {

        pciConfigOutLong(busNo, devNo, funcNo, PCI_CFG_BASE_ADDRESS_0,
                         AIC7880_IOBASE);
        pciConfigOutLong(busNo, devNo, funcNo, PCI_CFG_BASE_ADDRESS_1, 
                         AIC7880_MEMBASE);
        pciConfigOutByte(busNo, devNo, funcNo, PCI_CFG_DEV_INT_LINE, 
                         AIC7880_INT_LVL);
        }

    /* read the configuration parameters */

    pciConfigInLong(busNo, devNo, funcNo, PCI_CFG_BASE_ADDRESS_0, 
                    &iobaseCsr);    
    pciConfigInLong(busNo, devNo, funcNo, PCI_CFG_BASE_ADDRESS_1, 
                    &membaseCsr);  
    pciConfigInByte(busNo, devNo, funcNo, PCI_CFG_DEV_INT_LINE, 
                    &irq);
    membaseCsr &= PCI_MEMBASE_MASK;
    iobaseCsr  &= PCI_IOBASE_MASK;

    /* update the mmu table */

    if (sysMmuMapAdd((void *)membaseCsr, (UINT)AIC7880_MEMSIZE, 
                     (UINT)AIC7880_INIT_STATE_MASK, 
                     (UINT)AIC7880_INIT_STATE) == ERROR)
        {
        logMsg("Unable map requested memory\n", 0, 0, 0, 0, 0, 0);  
        return;
        }

    /* update the device specific info */

    aicRes->pciBus = busNo;
    aicRes->pciDevice = devNo;
    aicRes->pciFunc = funcNo;
    aicRes->irq = irq;

    /* enable mapped memory and IO addresses */

    pciConfigOutWord (aicRes->pciBus, aicRes->pciDevice, aicRes->pciFunc, 
                      PCI_CFG_COMMAND, PCI_CMD_IO_ENABLE | 
                      PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE);
    }

#endif /* INCLUDE_AIC_7880 */

/*******************************************************************************
*
* sysScsiInit - initialize the SCSI system
*
* This routine creates and initializes an AIC 788x SCSI Host Adapter chip.
* It connects the proper interrupt service routine to the desired vector, and
* enables the interrupt at the desired level.
*
* RETURNS: OK, or ERROR if the AIC controller structure cannot be created, the
* controller cannot be initialized, valid values cannot be set up in the
* SIOP registers, or if the interrupt service routine cannot be connected.
*/

STATUS sysScsiInit (VOID)
    {
#ifdef INCLUDE_AIC_7880

    AIC7880_INFO *aicResource;

    aicResource = &aic7880InfoTable;

    /* Create the SCSI controller */

    if ((pSysScsiCtrl = (SCSI_CTRL *) aic7880CtrlCreate 
        (aicResource->pciBus, aicResource->pciDevice, 
        SCSI_DEF_CTRL_BUS_ID)) == NULL)
        {
	logMsg ("Could not create SCSI controller\n", 
	         0, 0, 0, 0, 0, 0);
	return (ERROR);
	}
	
    /* connect the SCSI controller's interrupt service routine */
	
    if ((pciIntConnect (INUM_TO_IVEC (INT_NUM_GET (aicResource->irq)),
                        aic7880Intr, (int) pSysScsiCtrl)) == ERROR)
        {
        logMsg ("Failed to connect interrupt\n",
                 0, 0, 0, 0, 0, 0);
	return (ERROR);
        }

    sysIntEnablePIC((int) aicResource->irq) ;

#endif /* INCLUDE_AIC_7880 */

#ifdef  INCLUDE_TAPEFS
    tapeFsInit ();      /* initialize tape file system */
#endif /* INCLUDE_TAPEFS */

#ifdef INCLUDE_CDROMFS
    cdromFsInit ();     /* include CD-ROM file system */
#endif /* INCLUDE_CDROMFS */

    return (OK);
    }

/* Data for example code in sysScsiConfig, modify as needed */

SCSI_PHYS_DEV *	pSpd20;
SCSI_PHYS_DEV *	pSpd31;         /* SCSI_PHYS_DEV ptrs (suffix == ID, LUN) */
SCSI_PHYS_DEV *	pSpd40;

BLK_DEV *	pSbd0;
BLK_DEV *	pSbd1;
BLK_DEV *	pSbd2;          /* SCSI_BLK_DEV ptrs for Winchester */
BLK_DEV *	pSbdFloppy;     /* ptr to SCSI floppy block device */

#ifdef INCLUDE_SCSI2
SEQ_DEV *	pSd0;
TAPE_CONFIG *	pTapeConfig;
#endif /* INCLUDE_SCSI2 */

/*******************************************************************************
*
* sysScsiConfig - system SCSI configuration
*
* This routine is an example SCSI configuration routine.
*
* Most of the code for this routine shows how to declare a SCSI peripheral
* configuration.  This routine must be edited to reflect the actual
* configuration of the user's SCSI bus.  This example can also be found in
* src/config/usrScsi.c.
*
* For users just getting started, hardware configurations can be tested
* by defining SCSI_AUTO_CONFIG in config.h, which probes the bus and
* displays all devices found.  No device should have the same SCSI bus ID as
* the VxWorks SCSI port (default = 7), or the same ID as any other device.
* Check for proper bus termination.
*
* This routine includes three configuration examples that demonstrate
* configuration of a SCSI hard disk (any type), of an OMTI 3500 floppy disk,
* and of a tape drive (any type).
*
* The hard disk is divided into two 32-megabyte partitions and a third
* partition with the remainder of the disk.  The first partition is
* initialized as a dosFs device.  The second and third partitions are
* initialized as rt11Fs devices, each with 256 directory entries.
*
* It is recommended that the first partition on a block device (BLK_DEV) be
* a dosFs device, if the intention is eventually to boot VxWorks from the
* device.  This will simplify the task considerably.
*
* The floppy, since it is a removable medium device, is allowed to have only
* a single partition, and dosFs is the file system of choice because it
* facilitates media compatibility with IBM PC machines.
*
* While the hard disk configuration is fairly straightforward, the floppy
* setup in this example is more intricate.  Note that the
* scsiPhysDevCreate() call is issued twice.  The first time is merely to get
* a "handle" to pass to scsiModeSelect(); the default media type is
* sometimes inappropriate (in the case of generic SCSI-to-floppy cards).
* After the hardware is correctly configured, the handle is discarded using
* scsiPhysDevDelete(), after which a second call to scsiPhysDevCreate()
* correctly configures the peripheral.  (Before the scsiModeSelect() call,
* the configuration information was incorrect.)  Also note that following
* the scsiBlkDevCreate() call, correct values for <sectorsPerTrack> and
* <nHeads> must be set using scsiBlkDevInit().  This is necessary for IBM PC
* compatibility.
*
* Similarly, the tape configuration is more complex because certain device
* parameters must be turned off within VxWorks and the tape fixed block size
* must be defined, assuming that the tape supports fixed blocks.
*
* The last parameter to the dosFsDevInit() call is a pointer to a
* DOS_VOL_CONFIG structure.  If NULL is specified, dosFsDevInit() reads this
* information off the disk in the drive.  The read may fail if no disk is
* present or if the disk has no valid dosFs directory.  Should that happen,
* use dosFsMkfs() to create a new directory on a disk.  This routine uses
* default parameters (see dosFsLib) that may not be suitable an application,
* in which case, use dosFsDevInit() with a pointer to a valid DOS_VOL_CONFIG
* structure that has been created and initialized by the user.  If
* dosFsDevInit() is used, a call to diskInit() should be made to write a new
* directory on the disk, if the disk is blank or disposable.
*
* NOTE: The variable <pSbdFloppy> is global to allow the above calls to be
* made from the VxWorks shell, for example:
* .CS
*     -> dosFsMkfs "/fd0/", pSbdFloppy
* .CE
* If a disk is new, use diskFormat() to format it.
*
* INTERNAL
* The fourth parameter passed to scsiPhysDevCreate() is now
* <reqSenseLength> (previously <selTimeout>).
*/

STATUS sysScsiConfig (void)
    {

#if FALSE  /* EXAMPLE CODE IS NOT COMPILED */

    UINT which;
    int  scsiId;
    char modeData [4];		/* array for floppy MODE SELECT data */
    SCSI_OPTIONS options;

    /*
     * NOTE: Either of the following global variables may be set or reset
     * from the VxWorks shell. Under 5.0, they should NOT both be set at the
     * same time, or output will be interleaved and hard to read!! They are
     * intended as an aid to trouble-shooting SCSI problems.
     */

    scsiDebug = FALSE;		/* enable SCSI debugging output */
    scsiIntsDebug = FALSE;	/* enable SCSI interrupt debugging output */

    /*
     * The following section of code provides sample configurations within
     * VxWorks of SCSI peripheral devices and VxWorks file systems. It 
     * should however be noted that the actual parameters provided to
     * scsiPhysDevCreate(), scsiBlkDevCreate(), dosFsDevInit() etc., are
     * highly dependent upon the user environment and should therefore be 
     * modified accordingly.
     */

    /*
     * HARD DRIVE CONFIGURATION
     *
     * In order to configure a hard disk and initialise both dosFs and rt11Fs
     * file systems, the following initialisation code will serve as an
     * example.
     */

    /* configure a SCSI hard disk at busId = 2, LUN = 0 */

    if ((pSpd20 = scsiPhysDevCreate (pSysScsiCtrl, 2, 0, 0, NONE, 0, 0, 0))
        == (SCSI_PHYS_DEV *) NULL)
	{
        printErr ("usrScsiConfig: scsiPhysDevCreate failed.\n",
			0, 0, 0, 0, 0, 0);
	}
    else
	{
	/* create block devices */

        if (((pSbd0 = scsiBlkDevCreate (pSpd20, 0x10000, 0)) == NULL)       ||
            ((pSbd1 = scsiBlkDevCreate (pSpd20, 0x10000, 0x10000)) == NULL) ||
            ((pSbd2 = scsiBlkDevCreate (pSpd20, 0, 0x20000)) == NULL))
	    {
    	    return (ERROR);
	    }

        if ((dosFsDevInit  ("/sd0/", pSbd0, NULL) == NULL) )
	    {
	    return (ERROR);
	    }

#ifdef INCLUDE_RT11FS
	    if ((rt11FsDevInit ("/sd1/", pSbd1, 0, 256, TRUE) == NULL) ||
	    (rt11FsDevInit ("/sd2/", pSbd2, 0, 256, TRUE) == NULL))
	    {
	    return (ERROR);
	    }
#endif
	}


    /* 
     * FLOPPY DRIVE CONFIGURATION
     * 
     * In order to configure a removable media floppy drive with a
     * dosFs file system, the following device specific code will serve
     * as an example. Note that some arguments like mode parameters are
     * highly devcie and vendor specific. Thus, the appropriate peripheral
     * hardware manual should be consulted.
     */

    /* configure floppy at busId = 3, LUN = 1 */

    if ((pSpd31 = scsiPhysDevCreate (pSysScsiCtrl, 3, 1, 0, NONE, 0, 0, 0))
	== (SCSI_PHYS_DEV *) NULL)
	{
        printErr ("usrScsiConfig: scsiPhysDevCreate failed.\n");
	return (ERROR);
	}

    /* 
     * Zero modeData array, then set byte 1 to "medium code" (0x1b). NOTE:
     * MODE SELECT data is highly device-specific. If your device requires
     * configuration via MODE SELECT, please consult the device's Programmer's
     * Reference for the relevant data format.
     */

    bzero (modeData, sizeof (modeData));
    modeData [1] = 0x1b;

    /* 
     * issue the MODE SELECT command to correctly configure
     * floppy controller
     */

    scsiModeSelect (pSpd31, 1, 0, modeData, sizeof (modeData));

    /*
     * delete and re-create the SCSI_PHYS_DEV so that INQUIRY will return the
     * new device parameters, i.e., correct number of blocks
     */

    scsiPhysDevDelete (pSpd31);

    if ((pSpd31 = scsiPhysDevCreate (pSysScsiCtrl, 3, 1, 0, NONE, 0, 0, 0))
	== (SCSI_PHYS_DEV *) NULL)
	{
        printErr ("usrScsiConfig: scsiPhysDevCreate failed.\n");
	return (ERROR);
	}

    if ((pSbdFloppy = scsiBlkDevCreate (pSpd31, 0, 0)) == NULL)
	{
        printErr ("usrScsiConfig: scsiBlkDevCreate failed.\n");
	return (ERROR);
	}

    /*
     * Fill in the <blksPerTrack> (blocks (or sectors) per track) and <nHeads>
     * (number of heads) BLK_DEV fields, since it is impossible to ascertain
     * this information from the SCSI adapter card. This is important for
     * PC compatibility, primarily.
     */

    scsiBlkDevInit ((SCSI_BLK_DEV *) pSbdFloppy, 15, 2);

    /* Initialize as a dosFs device */

    /*
     * NOTE: pSbdFloppy is declared globally in case the following call
     * fails, in which case dosFsMkfs or dosFsDevInit can be
     * called (from the shell) with pSbdFloppy as an argument
     * (assuming pSbdFloppy != NULL)
     */

    if (dosFsDevInit ("/fd0/", pSbdFloppy, NULL) == NULL)
	{
        printErr ("usrScsiConfig: dosFsDevInit failed.\n");
	return (ERROR);
	}

    /*
     * CD-ROM DRIVE CONFIGURATION
     *
     * In order to configure a CD-ROM drive with cdromFs file system,
     * the following device specific code serves as an example.
     */

    /* configure a SCSI CDROM at busId 6, LUN = 0 */

    if ((pSpd60 = scsiPhysDevCreate (pSysScsiCtrl, 6, 0, 0, NONE, 0,
                                 0, 0)) == (SCSI_PHYS_DEV *) NULL)
        {
        SCSI_DEBUG_MSG ("sysScsiConfig: scsiPhysDevCreate failed for CDROM.\n",
        0, 0, 0, 0, 0, 0);
        }
    else if( (pSbdCd = scsiBlkDevCreate (pSpd60, 0, 0))== NULL )
        {
        SCSI_DEBUG_MSG ("sysScsiConfig: scsiBlkDevCreate failed for CDROM.\n",
        0, 0, 0, 0, 0, 0);
        }

    /*
     * Create an instance of a CDROM device in the I/O system. A block device
     * should have been created. The "cdromFsDevCreate" calls "iosDrvInstall"
     * which enters the appropriate driver routines in the I/O driver table.
     */

    if ((cdVolDesc = cdromFsDevCreate ("cdrom:", (BLK_DEV *) pBlkDev)) == NULL)
        {
        printErr (ERROR);
        }

    /* 
     * TAPE DRIVE CONFIGURATION
     * 
     * In order to configure a sequential access tape device and a tapeFs
     * file system, the following code will serve as an example. Note that
     * sequential access and tapeFs support are only available via SCSI-2.
     * To make sure that SCSI-2 is being used, check for the INCLUDE_SCSI2
     * macro definition in the BSP.
     *
     * The tape device does not support synchronous data transfers
     * or wide data transfers. Therefore, turn off the automatic configuration
     * of these features within VxWorks.
     */

    scsiId = 4;
    which = SCSI_SET_OPT_XFER_PARAMS | SCSI_SET_OPT_WIDE_PARAMS;

    options.maxOffset = SCSI_SYNC_XFER_ASYNC_OFFSET;
    options.minPeriod = SCSI_SYNC_XFER_ASYNC_PERIOD;
    options.xferWidth = SCSI_WIDE_XFER_SIZE_NARROW;

    if (scsiTargetOptionsSet (pSysScsiCtrl, scsiId, &options, which) == ERROR)
        {
        printf ("Could not set target option parameters\n");
        return (ERROR);
        }

    /* create SCSI physical device and sequential device */

    if ((pSpd40 = scsiPhysDevCreate (pSysScsiCtrl, scsiId, 0,0,NONE,0,0,0)) 
            == NULL)
        {
        printErr ("scsiPhysDevCreate failed.\n");
        return (ERROR);
        }

    if ((pSd0 = scsiSeqDevCreate (pSpd40)) == NULL)
        {
        printErr ("scsiSeqDevCreate failed.\n");
        return (ERROR);
        }

    /* configure a fixed block and rewind, tape file system */

    pTapeConfig = (TAPE_CONFIG *) calloc (sizeof(TAPE_CONFIG),1);
    pTapeConfig->rewind = TRUE;		/* rewind device */
    pTapeConfig->blkSize = 512;		/* fixed 512 byte block */ 

    if (tapeFsDevInit ("/tape1", pSd0, pTapeConfig) == NULL)
	{
	printErr ("tapeFsDevInit failed.\n");
	return (ERROR);
	}

#endif /*FALSE, END OF EXAMPLE CODE */

    return (OK);
    }

#endif /* INCLUDE_SCSI */
