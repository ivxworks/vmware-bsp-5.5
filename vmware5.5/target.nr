'\" t
.so wrs.an
.\" pc/target.nr - Pentium/2/3/4 target-specific documentation
.\"
.\" Copyright 1984-2002 Wind River Systems, Inc.
.\"
.\" modification history
.\" --------------------
.\" 01a,17apr03,zmm  Documented that memory auto sizing is disabled for pc486
.\"                  BSP.
.\" 01a,21mar03,zmm  Remove CPU reference from Memory Auto scan section.
.\" 02t,08nov02,hdn  added doc for non Intel X86 processor (spr 76279)
.\" 02s,24sep02,hdn  added doc for Thermal Monitor and Geyserville III support
.\" 02r,18jul02,tfr  Minor doc fixes
.\" 02q,12jul02,pai  add commentary on changes to the default BSP memory map
.\" 02p,28jun02,hdn  added bootrom images built by Project (spr 71289)
.\" 02o,18jun02,hdn  added a description of the new reboot mechanism
.\" 02n,14jun02,hdn  updated the "Enhanced MMU" section
.\" 02m,20may02,rhe  Update build instructions to use .bin files
.\" 02l,07may02,pai  Removed documentation on deprecated and / or unsupported
.\"                  VxWorks and bootrom images.
.\" 02k,28apr02,hdn  added comment for newly supported PIC modes (spr 76411)
.\" 02j,24apr02,pai  Removed comments on USR_ENTRY as a result of the fix for
.\"                  (SPR 73382).
.\" 02i,09apr02,pai  Reworked ATA configuration and documentation (SPR#
.\"                  73848).  Updated aic7880Lib configuration documentation.
.\" 02h,01apr02,jkf  Added _WRS_BSP_DEBUG_NULL_ACCESS to generate exception
.\"                  for code accesses to lower page of memory, null ptr.
.\" 02g,26mar02,pai  Added comments on PIT1_FOR_AUX configuration (SPR 73396).
.\"                  Updated comments in section on TrueFFS support
.\"                  (SPR 74103).
.\" 02f,01feb01,dmh  make bootrom instructions clearer
.\" 02e,03dec01,hdn  updated APIC, Pentium4 stuff. added Upgrading... section
.\" 02d,11oct01,pai  updated for Veloce.
.\" 02c,27aug01,dgp  change manual pages to reference entries per SPR 23698
.\" 02b,12mar99,cn   added el3c90xEnd driver comment.
.\" 02a,11mar99,sbs  added ne2000End driver comment.
.\" 01z,11mar99,sbs  added ultraEnd driver comment.
.\" 01y,24feb99,hdn  added comment for Pentium/PentiumPro's data cache mode.
.\" 01x,01feb99,jkf  added 3com and AMD SENS support comment.
.\" 01w,26jan99,jkf  added INCLUDE_ADD_BOOTMEM information. (SPR#21338)
.\" 01v,04jun98,hdn  added PentiumPro's APIC support
.\" 01u,12may98,hdn  added Pentium/PentiumPro support
.\" 01t,01may98,yp   added TrueFFS support
.\" 01s,23mar98,ms-  rewritten to conform with guidelines in BSP developers kit
.\" 01r,12mar98,sbs  changed info on use of SCSI-2 driver.
.\" 01q,20nov97,map  mention no support for ISA PnP. [SPR# 9877]
.\" 01p,10jul97,dds  added info on use of the SCSI-2 driver for the AHA 2940
.\"                  PCI SCSI Adapter card.
.\" 01o,25feb97,mas  added info on use of mkboot/mkbootFd and reboot device
.\"		     selection using sysWarmType as well as info on the mapping
.\"		     of IRQ numbers to IRQ levels (SPR 7850).
.\" 01n,22nov96,dat  doc cleanup, added EtherExpress PRO100B support.
.\" 01m,01nov96,hdn  added support for PCMCIA.
.\" 01l,09sep96,hdn  added support for compression.
.\" 01k,26jul96,hdn  added support for ATA and SMC91c9x driver.
.\" 01j,19jun96,wlf  doc: cleanup.
.\" 01i,23oct95,jdi  doc: updated for Tornado
.\" 01h,28sep95,hdn  cleaned up.
.\" 01g,03aug95,myz  upgraded to 5.3 
.\" 01f,06jun94,hdn  added changes by Roland for 5.1.1 FCS.
.\" 01e,13may94,hdn  updated to 5.1.1 FCS.
.\" 01d,01dec93,hdn  cleaned up.
.\" 01c,09nov93,vin  upgraded to 5.1
.\" 01b,26mar93,hdn  added a description about bootrom image.
.\" 01a,12jan93,hdn  written.
.\"
.\" NOTE
.\" EXOS
.\"
.TH "pcPentium/2/3/4" T "PC 386/486 and Pentium/2/3/4" "Rev: 26 Oct 01" "TORNADO REFERENCE: VXWORKS"

.SH "NAME"
.aX "PC Pentium/2/3/4"

.SH "INTRODUCTION"
This reference entry provides board-specific information necessary to run
VxWorks for the pcPentium, pcPentium2, pcPentium3, and pcPentium4 BSPs. 

Before using a board with VxWorks, verify that the board 
runs in the factory configuration by using vendor-supplied 
ROMs and jumper settings and checking the RS-232 connection.

Please note that the pc386, pc486, pcPentium, pcPentium2, pcPentium3, and
pcPentium4 BSPs are generic BSPs for generic PC style motherboards.  For
best results, the board vendor's documentation should be used in
conjunction with this document.

.SS "BOOT PROCESS"

When a standard PC-AT computer is powered on, the system BIOS code 
loads and executes the bootstrap loader. The bootstrap loader is 
written in 8088 16-bit assembly language. The BIOS obtains the 
bootstrap loader from the boot sector, which may be in one of 
several locations: a diskette, a hard disk, or some other alternatives
such as a ROMCARD or TFFS card.  When the BIOS finds the bootstrap loader, 
it transfers execution to it.  The bootstrap loader finds the bootrom.sys 
file, loads it into memory, and transfers execution to romInit.

.SS "TrueFFS support"
This BSP supports the optional product TrueFFS for Tornado.  To use
TrueFFS, install the product and define INCLUDE_TFFS configuration
constant in the BSP config.h file.

TrueFFS is set up to use the M-Systems DiskOnChip 2000 and two PCMCIA
slots as a drive for use with dosFs.

If you wish to reboot from the DiskOnChip device change the definitions for
SYS_WARM_TYPE in config.h from SYS_WARM_FD to SYS_WARM_TFFS.

.SS "BOOT ROMS"
The PC-AT generic boards typically use a boot diskette instead of boot ROMs. 
The boot diskette includes the boot sector (sector 0) and a DOS file system
containing a boot ROM image named `bootrom.sys'.  The boot ROM image could 
be one of following.  Note, that the Project builds vxWorks_romCompress.bin 
and vxWorks_romResident.bin.  VxWorks_rom.bin can be built by either Project
or the command line.  Other images are built by the command line.

.TS E
expand;
c c c c
c c c c
l l l l.
		Image is	Image Loads
Image Name	Description	Compressed	Into
=
vxWorks_rom.bin	bootable vxWorks	No	High Memory
vxWorks.st_rom.bin	bootable vxWorks.st	Yes	High Memory
bootrom.bin	bootrom	Yes	High Memory
bootrom_uncmp.bin	bootrom	No	High Memory
vxWorks_romCompress.bin	bootable vxWorks	Yes	High Memory
vxWorks_romResident.bin	bootable vxWorks	No	Low Memory
_
.TE

VxWorks.st is a fully linked stand alone image which 
includes a target based shell, symbol table, and network 
interface. Note that the network interface is not initialized.

The boot ROM image must be copied into the floppy disk (a:) or 
the IDE disk (c:) using the DOS boot utility "mkboot.bat" or the 
VxWorks boot utilities "mkbootFd" for floppy disks or 
"mkbootAta" for hard disks. 

Note to avoid rebooting issues, adjust the SYS_WARM_TYPE parameter 
appropriately in workspace and propagate the change to your
project. The default is to use the floppy disk for rebooting.

Making boot disks on the host side involves two steps.  Creating
the bootrom image file and preparing the boot disk.

To load VxWorks, and for more information, follow the instructions in the
.I "Tornado User's Guide: Getting Started."

.SH "Creating a bootrom image"

1. Begin by choosing one of the 6 supported boot image types with a .bin
extension name, vxWorks_rom.bin, vxWorks.st_rom.bin, bootrom.bin, or
bootrom_uncmp.bin based on your application needs.  Keep in mind that Low
Memory boot images are limited to approximately 640KB in size.    

2. Choose an appropriate boot parameters and define them in
DEFAULT_BOOT_LINE via Tornado or directly in config.h.  
Some examples values for DEFAULT_BOOT_LINE are...

.CS
"fei(0,0)host:/path/name/vxWorks h=90.0.0.3 e=90.0.0.50 u=bob"

"fd=0,0(0,0)host:/fd0/vxWorks e=90.0.0.50 u=jane o=fei"

"ata=0,0(0,0)host:/ata0disk0/vxWorks e=90.0.0.50 u=steve o=fei"

"ata=0,1(0,0)host:/ata0disk1/vxWorks.st"
.CE

3. Compile the boot image by running "make image_name" in the BSP
directory or see 
.I "Tornado User's Guide" 
for instructions on how to compile a bootable image from the Tornado IDE.

Preparing a Boot Disk/Diskette.

4. The boot loader searches the floppy disk for the file bootrom.sys. Boot
files with the .bin extension may be renamed to bootrom.sys. All other
boot images require a conversion tool create bootrom.sys. 

At this point, these instructions fork into three separate sets of
instructions that apply to Solaris, Windows and vxWorks itself.

.SS "Creating bootable diskettes from Solaris"

Use /usr/bin/fdformat that comes with Solaris. 
It requires a bootstrap loader file called vxld.bin located in
your Tornado directory tree at $WIND_BASE/host/sun4-solaris2/bin/vxld.bin

Insert a 1.44MB diskette into the Sun diskette drive, and issue the 
fdformat command to format the diskette and install the boot block.

.CS
fdformat -U -d -B $WIND_BASE/host/sun4-solaris2/bin/vxld.bin
Formatting 1.44 MB in /vol/dev/rdiskette0/no_name#0
Press return to start formatting floppy.
....................................................................
fdformat: using "vxld.bin" for MS-DOS boot loader
.CE

Eject and re-insert the diskette. On many systems the eject command is
required.

.CS
> eject
/vol/dev/rdiskette0/no_name can now be manually ejected
> volcheck
.CE

Copy the bootrom image to the diskette. Use cp for bin images or
objcopypentium for all other images.

.CS
> cp bootrom.bin /floppy/floppy0/bootrom.sys
...
> objcopypentium -O binary bootrom /floppy/floppy0/bootrom.sys
.CE


.SS "Creating bootable diskettes from Windows"

.CS

C:\Tornado\target\config\pcPentium\> format a: /v /q
Insert new disk for drive A:
and press ENTER when ready...
The type of the file system is FAT.
Verifying 1.44M
Format complete.
Volume label (11 characters, ENTER for none)?

  1457664 bytes total disk space.
  1457664 bytes available on disk.

      512 bytes in each allocation unit.
     2847 allocation units available on disk.

Volume Serial Number is 307A-4ACB

Format another (Y/N)? n


C:\Tornado\target\config\pcPentium> mkboot a: bootrom.bin
VxSys 1.6 (c) Wind River 1993-2002
Boot sector installed OK.
        1 file(s) copied.
System transferred.  Checking a:BOOTROM.SYS is contiguous
chkdsk a:bootrom.sys
The type of the file system is FAT.
Volume Serial Number is D4CF-F52B
Windows is verifying files and folders...
File and folder verification is complete.
Windows has checked the file system and found no problem.

    1,457,664 bytes total disk space.
      271,360 bytes in 1 files.
    1,186,304 bytes available on disk.

          512 bytes in each allocation unit.
        2,847 total allocation units on disk.
        2,317 allocation units available on disk.
All specified file(s) are contiguous.

.CE

mkboot.bat writes the boot sector containing the boot loader onto the 
floppy disk.  Then mkboot copies the boot file to the floppy disk and
checks to ensure the boot file is contiguous.  The user must pay attention 
to the output and ensure the boot file is reported as contiguous.  The boot 
loader will not properly handle a non-contiguous boot files.   

An alternative to mkboot.bat is to manually invoke: 

format

use .\host\x86-win32\bin\vxsys  to create a boot sector 

use copy for boot images with a .bin extension

use objcopypentium for all other boot images

For example:

.CS
C:\Tornado\target\config\pcPentium\> format a: /v /q
Insert new disk for drive A:
and press ENTER when ready...
The type of the file system is FAT.
QuickFormatting 1.44M
Format complete.
Volume label (11 characters, ENTER for none)?

  1457664 bytes total disk space.
  1457664 bytes available on disk.

      512 bytes in each allocation unit.
     2847 allocation units available on disk.

Volume Serial Number is DC31-1143

QuickFormat another (Y/N)? n

C:\Tornado\host\x86-win32\bin> vxsys a:
VxSys 1.6 (c) Wind River 1993-2002
Boot sector installed OK.

.CE
Now copy the boot image to the floppy. For .bin images use copy
.CS
C:\Tornado\target\config\pcPentium> copy bootrom.bin a:bootrom.sys

.CE
For all other images use objcopypentium 
.CS
C:\Tornado\host\x86-win32\bin\objcopypentium -O binary --gap-fill=0
bootrom a:bootrom.sys

.CE
Now verify that the file is contiguous. Non contiguous images will not
boot.
.CS
c:\Tornado\target\config\pcPentium> chkdsk a:bootrom.sys
The type of the file system is FAT.
Volume Serial Number is DC31-1143
CHKDSK is verifying files and directories...
File and directory verification completed.

  1457664 bytes total disk space.
   493056 bytes in 1 user files.
   964608 bytes available on disk.

      512 bytes in each allocation unit.
     2847 total allocation units on disk.
     1884 allocation units available on disk.
All specified file(s) are contiguous.


.CE

You may also use a hard disk to boot VxWorks.   You must boot from a 
primary bootable partition on the primary disk as seen by the PC BIOS.  

It is recommended that you use FDISK or a similar utility to create 
the primary bootable partition.   The reason is that FDISK uses 
PC BIOS calls to create the disks master boot record.  The BIOS is 
responsible for loading the boot loader during the boot process.   
Therefore, a utility that uses the BIOS to write partitions, such as 
FDISK, is recommended.  

The partition and file system on the disk may be either FAT12, or FAT16, 
or VxWorks proprietary VXLONGNAMES file system.  FAT32 is not supported 
by the boot loader.  If you need FAT32, make a small primary boot partition 
that is FAT16, and a second larger partition that is FAT32 atop the 
remainder of the disk.

To create a bootable hard disk, replace "c:" for "a:" in the 
above example.  When you use c:, vxsys.com will ask you the following:

.CS
That's a hard disk!  Are you sure (y/n)? 
.CE

You should enter a "y" to indicate approval of the operation.
Be aware that this will prevent other operating systems from 
booting on the disk.

.SS "Creating bootable diskettes from vxWorks"

The usage of the three VxWorks boot utilities is as follows:
.ne 15
.CS
STATUS mkbootFd
    (
    int drive,    /* destination drive number: (0 - 3)  */
    int fdType,   /* type of floppy disk: (0 - 1)       */
    char *in      /* source file name                   */
    )

STATUS mkbootAta 
    (
    int ctrl,     /* dest. controller number: (0 - 1)   */
    int drive,    /* dest. drive number: (0 - 1)        */
    char *in      /* source file name                   */
    )

STATUS mkbootTffs
    (
    int drive,       /* drive number: (0 - TFFS_MAX_DRIVES - 1) */
    int removable,   /* removable or not: (TRUE - FALSE)        */
    char *in         /* source file name                        */
    )
.CE
All routines return OK on success and ERROR if there is an error while
copying the image from the source onto the disk.  The source code for
these routines is in the BSP file mkboot.c

EXAMPLES:

Example 1: Creating a boot floppy disk using mkbootFd:

The floppy disk is in drive 0 (or a:), the diskette is of type 0,
and the image file name is `bootrom.sys':
.CS
    -> mkbootFd 0, 0, "bootrom.sys"
.CE
Example 2: Creating a bootable hard disk using mkbootFd:

The hard disk is on ATA controller 0 and is drive number 0
(or c:). The image file name is `bootrom.sys':
.CS
    -> mkbootAta 0, 0, "bootrom.sys"
.CE
Example 3: Creating a bootable TrueFFS disk using mkbootTffs:

The TrueFFS disk is drive 0 (or c:), that is a non removable drive.
The image file name is `bootrom.sys':
.CS
    -> mkbootTffs 0, 0, "bootrom.sys"
.CE

These boards do not have non-volatile RAM; thus, boot parameters are not
preserved whenever the system is powered off.  However, static boot
parameters can be set in the boot disk by setting the boot parameter line
DEFAULT_BOOT_LINE in config.h.

The Bootrom Utilities

vxsys.com drive:

This command installs a VxWorks bootstrap loader in a drive boot sector. 
The drive can be either a diskette (drive A:), or a hard disk that is 
searched by the BIOS bootstrap.  The VxWorks bootstrap loader searches for
the file bootrom.sys in the root directory and loads it directly into
memory at linear address 0x8000.  Execution then jumps to romInit() at 0x8000.

NOTE: After a bootstrap loader is installed in the disk boot sector, you do not
need to repeat the vxsys operation for new ROM images.  Just copy a new boot
image to the disk that has already had a boot sector installed.

vxld.bin

vxld.bin is neither a command nor a program, rather it is a copy of the
boot sector installed onto a disk or diskette by vxsys.com.  It is
separately included to facilitate creating boot diskettes from Solaris or
Linux as well as allowing support for some alternative third-party boot
loaders.

mkboot drive: source_file

This command is an MS-DOS batch file that uses vxsys.com to install the VxWorks
bootstrap loader in the drive boot sector, and then uses copy to transfer
source_file to drive:bootrom.sys. It also runs the MS-DOS utility chkdsk to
check whether bootrom.sys is contiguous.

vxload.com [image_file]

This command is used during an MS-DOS session to load and execute the
VxWorks image, typically the bootrom image. It can be more
convenient or quicker than loading the image via the PC boot cycle. 
vxload takes a parameter, the image file name.  vxload.com is not
compatible with any version of Microsoft Windows, it is only supported
under MS-DOS. 


VxWorks low memory images run within a memory range from 0x8000 to 0xa0000.
This restricts the size of the memory pool available to drivers in such
images.  The INCLUDE_ADD_BOOTMEM configuration option in config.h enables
runtime code which will add a specified amount of upper memory (memory above
physical address 0x100000) to the memory pool of an image in lower memory.
This option cannot be used on systems with less than 4MB of memory.

The default value for ADDED_BOOTMEM_SIZE, the amount of memory to add
to a lower memory image's memory pool, is 2MB.  This value may be increased,
but one must ensure that the pool does not overlap with the downloaded
vxWorks image when the INCLUDE_ADD_BOOTMEM option is used for lower memory
boot images.  If there is an overlap, then loading the vxWorks runtime
image will corrupt the added memory pool.

The calculation for determining the ADDED_BOOTMEM_SIZE value is:

.CS
    (RAM_LOW_ADRS + vxWorks image size) < (memTopPhys + ADDED_BOOTMEM_SIZE)
.CE

Where <memTopPhys> is calculated in the BSP sysLib.c file.


.SS "New Reboot Mechanism"
The new mechanism saves the brand new bootrom image to the top of
the physical memory in sysPhysMemTop(), and restores it when reboot
in sysToMonitor().  The saved image is protected by the MMU during
runtime of the VxWorks.  This mechanism is independent of the BIOS
boot devices, is faster and simpler than the current method.  It 
also preserves the boot line.  This reboot mechanism assumes that 
the brand new bootrom image is kept in the lower memory when it is 
saved.  That means the bootrom image need to be located in the 
upper memory.  To do this, the RAM_LOW_ADRS and RAM_HIGH_ADRS need 
to be changed in both config.h and Makefile in the BSP.  Then build 
the bootrom and VxWorks image with defining FAST_REBOOT macro.  The 
recommended RAM_LOW_ADRS is 0x00608000, RAM_HIGH_ADRS is 0x00408000.
The FAST_REBOOT macro can be defined statically in config.h or 
dynamically in the command line as "make EXTRA_DEFINE=-DFAST_REBOOT".  
This macro is defined automatically for the BSPs configured for the
Asymmetric Multi Processing, such as ones suffixed with _bp or _ap.
The memory size LOCAL_MEM_SIZE may need to be increased for larger 
page sizes.  Note that the bootrom image in your boot device needs 
to be updated before downloading the new VxWorks image that contains 
this mechanism.


.SS "Jumpers"
Refer to the board vendor's documentation.

.SH "FEATURES"
Since this is a generic BSP for generic PC style motherboards,
your board may or may not have all of these features, and your
board may have some features not discussed here.
Your board vendor's documentation should be used in conjunction 
with this section.

.SS "Supported Features"
Supported features are discussed below in the "Devices" section
under the "HARDWARE DETAILS" heading.

.SS "Unsupported Features"

This BSP does not support ISA PnP (Plug and Play). Such devices can
be supported if PnP is disabled and the device parameters (IO address,
Memory address, IRQ, DMA channel etc) are set to match those in the
BSP driver configuration. If the device uses soft-configuration
instead of jumpers, an appropriate utility program, usually available
from the device manufacturer, should be used to set the device parameters.

.SS "Feature Interactions"
Refer to the board vendor's documentation.

.SH "HARDWARE DETAILS"
This section documents the details of the device drivers and board
hardware elements.

.SS "Devices"
Drivers included with this BSP are for both on-board chips
and for separate adaptor cards that can be used with the
motherboard.
Refer to the vendor's documentation for both the motherboard and
any adaptor cards used. Vendor documentation for on-board
chips may be necessary also.

Note that for all ISA drivers, the I/O base address,
memory address, and interrupt level must match those in config.h
or pc.h.

In the table below, drivers ending in the .c extension are
delivered in source form; the other drivers are delivered
in object form only.
    
.TS
center;
l l.
i8250Sio.c	Intel UART tty driver
i8237Dma.c	8237 DMA driver
pcConsole.c	console driver
i8042Kbd.c	Intel keyboard controller
i8048Kbd.c	Intel keyboard controller
m6845Vga.c	Motorola M6845 VGA controller
nec765Fd.c	nec765 floppy disk controller
ataDrv.c	IDE/ATA hard disk controller
ataShow.c	IDE/ATA hard disk controller show routines
aic7880Lib.o	AHA-2940 PCI SCSI Adapter card
i8253Timer.c	Intel 8253 timer driver
loApicTimer.c	Intel Pentium/2/3/4 Local APIC/xAPIC Timer library
i8259Intr.c	Intel 8259 PIC
loApicIntr.c	Intel Pentium/2/3/4 Local APIC/xAPIC driver
loApicIntrShow.c	Intel Pentium/2/3/4 Local APIC/xAPIC driver show routine
ioApicIntr.c	Intel IO APIC/xAPIC driver
ioApicIntrShow.c	Intel IO APIC/xAPIC driver show routine
nullNvRam.c	null NVRAM library
nullVme.c	null VMEbus library
pciConfigLib.c	PCI configuration space access support for PCI drivers
pciIntLib.c	PCI shared interrupt support
pcmciaLib.c	PCMCIA driver
pcmciaShow.c	PCMCIA driver show routine
fei82557End.o	Intel EtherExpress PRO100B PCI END driver
gei82543End.o	Intel PRO1000X/F/XT/XF PCI END driver
ln97xEnd.o	AMD Am79C97X PCI END driver
elt3c509End.o	3COM 3C509 END driver
el3c90xEnd.o	3COM 3C90x PCI END driver
ultraEnd.o	SMC Elite Ultra END driver
dec21x40End.o	DEC 21x4x PCI END driver
ne2000End.o	Novell/Eagle 2000 END driver
lptDrv.c	Parallel port driver
.TE

Below are some brief notes on each driver.
For more details refer to the
entry for each driver in the Libraries section of the 
.I "VxWorks Reference Manual".

.IP "i8250Sio"
Intel 8250 Universal Asynchronous 
Receiver Transmitter (UART) tty driver.
Used for the serial ports

.IP "i8237Dma"
Driver for the ISA DMA controller.
This is used in nec765Fd.c, which also
serves as a good application example.

.IP "pcConsole, i8042Kbd and i8048Kbd"
Driver for the on-board Intel 8042 and 8048 keyboard controllers
To use these controllers
the INCLUDE_PC_CONSOLE directive must be enabled in config.h.  
The macro
PC_KBD_TYPE should be defined in config.h as PC_PS2_101_KBD to include
i8042Kbd.c, and as PC_XT_83_KBD to include i8048Kbd.c.

.IP "m6845Vga"
Driver for the Motorola M6845 VGA controller.
To use this controller, define INCLUDE_PC_CONSOLE in config.h.

.IP "nec765Fd"
Driver for the nec765 floppy disk controller.
To use this driver, the INCLUDE_FD directive must be enabled in
config.h.

.IP "ataDrv and ataShow"
Driver for the IDE/ATA hard disk controller.  
To use this driver, the INCLUDE_ATA directive must be enabled in config.h. 
Note that the old INCLUDE_IDE directive is replaced by INCLUDE_ATA,
and that vxsys() is replaced by mkbootFd() and mkbootAta().

By default, with INCLUDE_ATA, vxWorks is set up for one ATA hard disk
device on the primary ATA controller and one drive on a secondary
controller.  If a system has more than two controllers or more than one
drive per controller, then configuration constants in config.h, and
possibly the <ataResources> table in sysLib.c, must be modified to support
additional drives and controllers.

For example, suppose that the system's ATA controller zero has two
physical drives.  Modify the ATA0_NUM_DRIVES constant in config.h such
that the defined value is 2, instead of the default value of 1:

.CS
    /@ config.h @/
    ...
    #define ATA0_NUM_DRIVES (2)
    ...
.CE

Note that the BSP config.h and sysLib.c files predefine ATA configuration
values and <ataResources> table entries for at most two controllers.  Such
a limited configuration will not be representative of every target system.

Consider the case of a system with a hard disk on the primary controller,
a CD-ROM device on a secondary controller, and a PCMCIA device on a third
controller.  The default <ataResources> table must be modified such that
ataDrv can initialize and use all of the controllers on such system.
Specifically, one should define additional configuration constants which
are then used to initialize a third controller entry in the <ataResources>
table in a manner similar to the following:

.ne 34
.CS
...
ATA_RESOURCE ataResources[ATA_MAX_CTRLS] =
    {
    /@ ATA controller zero resources @/
    {
    /@ ATA 0 initializers ... @/
    },

    /@ ATA controller one resources @/
    {
    /@ ATA 1 initializers ... @/
    },

    /@ ATA controller two resources @/
    {
        /@  PCCARD_RESOURCE @/
        { 
        ATA2_VCC,           /@ 3-5 volts Vcc @/
        ATA2_VPP,           /@ 5-12 volts Vpp @/
            {
            ATA2_IO_START0, /@ start I/O address 0 @/
            ATA2_IO_START1  /@ start I/O address 1 @/
            },  

            {
            ATA2_IO_STOP0,  /@ end I/0 address 0 @/
            ATA2_IO_STOP1   /@ end I/0 address 1 @/
            }, 
        ATA2_EXTRA_WAITS,   /@ extra wait states 0-2 @/
        ATA2_MEM_START,     /@ start host mem address @/
        ATA2_MEM_STOP,      /@ stop host mem address @/
        ATA2_MEM_WAITS,     /@ mem extra wait states 0-2 @/
        ATA2_MEM_OFFSET,    /@ mem offset of card address @/
        ATA2_MEM_LENGTH     /@ length of memory @/
        },
    ATA2_CTRL_TYPE,         /@ IDE_LOCAL or ATA_PCMCIA @/
    ATA2_NUM_DRIVES,        /@ number of drives on controller @/ 
    INT_NUM_ATA2,           /@ interrupt number of controller @/
    ATA2_INT_LVL,           /@ interrupt level of controller @/
    ATA2_CONFIG,            /@ device configuration settings @/
    ATA2_SEM_TIMEOUT,       /@ semaphore timeout for controller @/
    ATA2_WDG_TIMEOUT,       /@ watchdog timeout for controller @/
    ATA2_SOCKET_TWIN,       /@ socket number for twin card @/
    ATA2_POWER_DOWN         /@ power down mode for this controller @/
    }
    };
...
.CE

where the table initialization elements are constants defined in the BSP
config.h file.

The size of the <ataResources> table, and the number of ATA controllers
supported by ataDrv, are specified by the ATA_MAX_CTRLS constant which
is defined in the $WIND_BASE/target/h/drv/hdisk/ataDrv.h file.  By
default, ATA_MAX_CTRLS is set to value 2 under the assumption that
ataDrv will support at most 2 controllers.  When the <ataResources> table
is modified to specify more than two controllers, as in the example
above, the ATA_MAX_CTRLS constant should be redefined and the
$WIND_BASE/target/src/drv/hdisk/ataDrv.c file should be rebuilt prior
to building a vxWorks image with the new configuration.

.IP "aic7880Lib"
A device driver for the AHA-2940 PCI SCSI Adapter card is provided.  This
card houses the AIC-7880 Adaptec SCSI Host Adapter.  Configure a system to
use the AIC-7880 driver by defining the INCLUDE_AIC_7880 configuration
constant in the BSP config.h file.

Note that the AIC-7880 PCI SCSI Adapter card driver for x86 requires SCSI-2.
The configuration constants, INCLUDE_SCSI and INCLUDE_SCSI2, must be defined
in config.h in order to configure SCSI-2 support into a system.  Also note
that, as the AIC-7880 is a PCI card, the INCLUDE_PCI configuration
constant will be defined in the config.h file when INCLUDE_AIC_7880 is
defined.

The default Interrupt Request (IRQ) Channel number used by x86 BSPs for
the SCSI interrupt may be assigned by a system BIOS.  The IRQ number
assigned by a BIOS should not conflict with other interrupts in the
system (Eg. Ethernet).  If a VxWorks system is configured to use "forced"
PCI configuration by setting the PCI_CFG_TYPE configuration to the value
PCI_CFG_FORCE, then the IRQ number will default to the value specified by
AIC7880_INT_LVL in the BSP pc.h file.  The AIC7880_INT_LVL is set to 0x0a
by default.  Modify this value if necessary to prevent conflicts with
other devices installed on the system built to use forced PCI configuration.
It is recommended that one use the CMOS setup menu to set the IRQ number
for a SCSI Host Adapter when appropriate to the particular hardware and
system being configured.

If SCSI configuration fails, it can be the result of improper SCSI bus 
termination.  Check your hardware setup.

For information regarding installation and configuration of the AHA-2940,
see the "Adaptec 7800 Family Manager Set User's Guide".

.IP "i8253Timer"
This library contains a board-independent interface for manipulating the
timer functions on Intel 8253 and compatible timer chip devices.  This
library provides system clock and the auxiliary clock functions.

Configuration macros in the BSP can be used to modify how the system clock
and auxiliary clock facilities are implemented via i8253-compatible devices.
However, it is strongly recommended that users consult the documentation
for the target hardware platform prior to using certain configuration
macros related to the i8253Timer driver configuration.  In particular,
users should know how the outputs of the timer channels are connected on
the target hardware platform.

As an example of why it is important to consider how an 8253-compatible
device is integrated into the system, consider how such devices were often
implemented in legacy consumer desktop applications.  The 8253-compatible
chips usually contain three timers.  Typically, all three timers are driven
by a 14.31818 MHz crystal input from the system board, divided by 12, to
yield a 1.19318 MHz input clock to the timers.  The outputs from each
timer channel were, and are, often connected as follows in desktop
systems:

.bS
                       8253
                 +---------------+
                 |    Timer 2    |
 from bit 0      |         output+------> to speaker circuitry
 of port 61h ----+->gate         |
                 |               |
 1.19318 MHz ----+->clk 2        |
                 |               |
                 +---------------+
                 |    Timer 1    |
 +5 V            |         output+------> DRAM refresh
 (logic 1)--+----+->gate         |
            |    |               |
 1.19318 MHz ----+->clk 1        |
            |    |               |
            |    +---------------+
            |    |    Timer 0    |
            |    |         output+------> to IRQ0 (timer interrupt)
            +----+->gate         |
                 |               |
 1.19318 MHz ----+->clk 0        |
                 |               |
                 +---------------+
.bE

As indicated in the diagram, the output of timer channel 2 is connected to
nothing other than the speaker.  The output of timer 2 is not connected to
the 8259 PIC or other type of interrupt controller.

The output from timer channel 1 is dedicated to providing DRAM refresh.
As a result, this timer should not be manipulated once it is programmed
appropriately for the system DRAM.

Because the output from timer channel 0 in the example above is connected
to an interrupt controller and is not used as a time base for a system
critical function (i.e., DRAM refresh), timer 0 is a good candidate for use
as a programmable system or auxiliary clock device.

The example above is but one possible way 8253-compatible timer devices
might be integrated into a target system.  Some system boards may connect
all timer channel outputs to an interrupt controller.  Not every system
will connect timer channel outputs to DRAM refresh or to a speaker.
Again, users are encouraged to consult the target hardware documentation
in order to understand the requirements for a particular system.  The
default i8253Timer driver configuration will be appropriate for most
systems, but the optional configuration macros may not be appropriate for
every system.

.IP
The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

.IP
The macro PIT_CLOCK must also be defined to indicate the clock frequency
of the i8253.

.IP
The default configuration implements the system clock through the
programmable timer channel 0.  The real time clock (based on the Motorola
MC146818) is used as the auxiliary clock.

The i8253Timer driver can be configured to use the programmable timer
channel 1 (instead of the real time clock) for the auxiliary clock device.
The BSP does not predefine such a configuration.  However, the driver can
be easily configured to use channel 1 for the auxiliary timer, provided the
user defines the appropriate items in the BSP.  For reasons noted above,
please consult the target system documentation prior to implementing
the auxiliary clock via timer channel 1.

In order to configure i8253Timer to use timer 1 as the auxiliary clock,
the following items must be defined in the BSP:

    (1) Define the manifest constant PIT1_FOR_AUX in config.h.  This constant
        has no associated value.  By defining it, the driver will attempt to
        use timer channel 1 as the auxiliary clock.

    (2) Define the manifest constant PIT1_INT_LVL.  This constant should
        specify the interrupt level (specifically, the IRQ number) associated
        with the output from timer channel 1.  See the definitions of
        PIT0_INT_LVL in the BSP config.h and pc.h files for an example of how
        to do this.

    (3) Connect an interrupt handler to service the auxiliary clock interrupt
        in sysHwInit2().  Existing code in sysHwInit2() will illustrate how to
        install the ISR.  The following code fragment should be sufficient:

.CS
    ...
    #ifdef PIT1_FOR_AUX
    intConnect (INUM_TO_IVEC (INT_NUM_GET (PIT1_INT_LVL)), sysAuxClkInt, 0);
    #endif
    ...
.CE

.IP
This driver includes a timestamp driver; to use this
feature, the macro INCLUDE_TIMESTAMP must be defined in config.h.

.IP "loApicTimer"
This library contains routines to manipulate the timer functions on the
Intel P6 (PentiumPro, II, and III) and P7 (Pentium4) family processor's 
Local APIC/xAPIC Timer with a board-independent interface.
This library handles both the system clock and the auxiliary clock 
functions.  The auxiliary clock is either the RTC (real time clock) or
PIT (programmable interrupt timer) channel 0 (define PIT0_FOR_AUX in 
config.h).

.IP
The macro APIC_TIMER_CLOCK_HZ must also be defined to indicate the clock 
frequency of the Local APIC/xAPIC Timer.

.IP "i8259Intr"
Driver for the Intel 8259A Programmable Interrupt Controller (PIC).

.IP "loApicIntr" and "loApicIntrShow"
Driver for the Intel P6 (PentiumPro, II, III) and P7 (Pentium4) 
family processor's Local APIC/xAPIC.
This driver is used in either Virtual Wire Mode (define VIRTUAL_WIRE_MODE
in config.h) or Symmetric IO Mode (define SYMMETRIC_IO_MODE in config.h).
loApicInit () initializes the Local APIC/xAPIC and scans certain memory
regions as specified in the specification to determine the base
addresses.  It uses LOAPIC_BASE and IOAPIC_BASE defined in the BSP, if
it is not able to find the addresses in the MP configuration table.
Scanned memory regions are defined by two pairs of macro in pc.h,
BIOS_ROM_START and BIOS_ROM_END, EBDA_START and EBDA_END.

.IP "ioApicIntr" and "ioApicIntrShow"
Driver for the Intel P6 (PentiumPro, II, III) and P7 (Pentium4) 
family processor's IO APIC/xAPIC.
This driver is used in Symmetric IO Mode (define SYMMETRIC_IO_MODE in
config.h).  ioApicInit() initializes the IO APIC/xAPIC with information 
stored in redTable[].  redTable[] has three entries - lsw, vectorNo, mask.  
First entry, lsw, stores the least significant word of the IO APIC/xAPIC's 
redirection table.  That includes Trigger Mode, Interrupt Input Pin 
Polarity, Destination Mode, Delivery Mode.  Second entry, vectorNo, is
the vectorNo of the redirection table.  Third entry, mask, should be 0 
and used by ioApicIntLock() and ioApicIntUnlock() to hold the interrupt
mask status.

.IP "nullNvRam"
This library contains dummy non-volatile RAM manipulation routines for targets
lacking non-volatile RAM.  Read and write routines that return ERROR
are included.

The macro NV_RAM_SIZE should be defined as NONE for targets lacking
non-volatile RAM.

.IP "nullVme"
This library contains null routines for boards which do not include any
common bus routines.

.IP "pcmciaLib and pcmciaShow"
Drivers for PCMCIA. In order to use any PCMCIA card 
the INCLUDE_PCMCIA directive
must be enabled in config.h. These drivers currently
support three cards. To use an ATA PC card, enable
INCLUDE_ATA; to use an SRAM PC card, enable INCLUDE_SRAM;
to use a 3COM Etherlink III PC card, enable INCLUDE_ELT.
By default, all three cards are enabled when INCLUDE_PCMCIA is enabled.

.SS "Memory Maps"
.TS
center allbox;
c c c
l l l.
Start Address	Size	Use
0x0	0xa0000	lower memory
0xa0000	0x60000	video ram, etc
0x100000	sysPhysMemTop() - 0x100000	upper memory
.TE

.SS "Shared Memory"
Not applicable to this BSP

.SS "Interrupts"
All ISA interrupts are external to the CPU and are routed
through the ISA interrupt prioritization hardware.  This hardware is
comprised of two 82C59 PICs.  There are 16 ISA interrupts and
interrupt priority levels numbered 0 through 15. The mapping between
interrupt numbers and priority levels is not necessarily one to one.
The motherboard hardware determines the mapping of interrupt
request lines (IRQ) to priority levels. The hardware should
adhere to the standard ISA assignments:

.ne 18 
            IRQ         Priority
            ---         --------
             0              0
             1              1
             2              2
             3             11
             4             12
             5             13
             6             14
             7             15
             8              3
             9              4
            10              5
            11              6
            12              7
            13              8
            14              9
            15             10

IRQs 0 - 7 are handled by PIC1 and IRQs 8 - 15 by PIC2.  PIC2 interrupts are
cascaded into PIC1 at IRQ2 which is reflected in the above table.
The Fully Nested Mode is used in the default configuration of this BSP.

Fully Nested Mode.
In this mode, interrupt requests are ordered in priority
from 0 through 7 (0 is the highest).  When an interrupt is acknowledged the
highest priority request is determined and its vector is placed on the bus.
Additionally, a bit of the Interrupt Service (IS) register is set.  This bit
remains set until the microprocessor issues an EOI command immediately before
returning from the service routine.  While the IS bit is set, all further
interrupts of the same or lower priority are inhibited, while higher level
interrupts are allowed.  The PICs in a PC typically operate in this mode
(normal nested mode).  In this mode, while the slave PIC is being
serviced by the master PIC, the slave PIC blocks all higher priority
interrupt requests.  Alternatively, to allow interrupts of a higher priority,
enable the Special Fully Nested Mode.

Special Fully Nested Mode: define PIC_SPECIAL_FULLY_NESTED_MODE.
This mode is similar to the Fully Nested Mode with the following exceptions:
1) When an interrupt request from a slave PIC is in service, the slave is
not locked out from the master's priority logic and further interrupt
requests from higher priority IRs within the slave will be recognized by
the master and will initiate interrupts to the processor.  2) When exiting
the interrupt service routine, the software must check whether or not the
interrupt serviced was the only interrupt request from the slave.  If it
was the only interrupt request, a non-specific EOI is sent to the master.
If not, no EOI is sent.

The PIC(8259A) IRQ0 is hard wired to the PIT(8253) channel 0 in a PC
motherboard.  IRQ0 is the highest priority in the 8259A interrupt
controller.  Thus, the system clock interrupt handler blocks all lower
level interrupts.  This may cause a delay of the lower level interrupts in
some situations even though the system clock interrupt handler finishes its
job without any delay.  This is quite natural from the hardware point
of view, but may not be ideal from the application software standpoint.
The following modes are supplied to mitigate this situation by providing the
corresponding configuration macros in the BSP.  The three modes are
mutually exclusive.

Early EOI Issue in IRQ0 ISR: define PIC_EARLY_EOI_IRQ0.
In this mode, the EOI is issued before the IRQ0 system clock interrupt
service routine starts the kernel work.  This lowers the IRQ0 ISR blocking
level to the next lower level.  If no IRQs are in service, the next lower
level is the lowest level.  If IRQn is in service, the next lower level
corresponds to the next lower priority.  As a result, the kernel work in
the system clock interrupt service routine can be interrupted by an
interrupt with a higher priority than the blocking level.

Special Mask Mode in IRQ0 ISR: define PIC_SPECIAL_MASK_MODE_IRQ0.
In this mode, the Special Mask Mode is used in the IRQ0 system clock
service routine.  This lowers the blocking level to the specified level
(currently hard coded to the lowest level in i8259Intr.c).

Automatic EOI Mode: define PIC_AUTO_EOI.
This mode provides no nested multi-level interrupt structure in PIC1.
The EOI command is automatically sent to the master PIC at the end of the
interrupt acknowledge cycle.  Thus, no software intervention is needed.

P6 (PentiumPro, II, III) and P7 (Pentium4) family processor has new interrupt 
controller APIC/xAPIC (Advanced Programmable Interrupt Controller) which 
consists of Local APIC/xAPIC (on-chip) and IO APIC/xAPIC (on chipset).
They are used in two additional interrupt modes that are configurable in BSP.  

Virtual Wire Mode: One of three interrupt modes defined by the MP
specification.  In this mode, interrupts are generated by the 8259A 
equivalent PICs, but delivered to the BSP by an APIC that is programmed to 
act as a "virtual wire"; that is, the APIC is logically indistinguishable 
from a hardware connection.  This is a uniprocessor compatibility mode.
If the Local APIC exist in the processor indicated by APIC feature flag
in the CPUID, this mode can be configured and used.
To use this mode, define VIRTUAL_WIRE_MODE in config.h

Symmetric IO Mode: One of three interrupt modes defined by the MP
specification.  In this mode, the APICs are fully functional, and 
interrupts are generated and delivered to the processors by the APICs.  
Any interrupt can be delivered to any processor.  This is the only 
multiprocessor interrupt mode.  If the Local APIC exist in the processor
indicated by APIC feature flag in the CPUID and the IO APIC in the chipset
is available, this mode can be configured and used.  The PIRQ[n] is 
directly handled by the IO APIC in this mode.
To use this mode, define SYMMETRIC_IO_MODE in config.h

.SS "Serial Configuration"
Refer to the board vendor's documentation.

.SS "SCSI Configuration"
Refer to the board vendor's documentation.

.SS "Network Configuration"
Please note that the following END driver configuration mechanism,
particularly with respect to the use of <endDevTbl>, will be obsolete in
a future Tornado 2.x release.  Wind River Systems' Intel Architecture BSPs
continue to use the following END driver configuration mechanism as of
Tornado 2.2 FCS.

The BSP configNet.h file contains a static table, <endDevTbl>, that
specifies END driver instances which must be loaded to the MUX via
muxDevLoad() when the network is initialized.  Each table entry of type
END_TBL_ENTRY records information the muxDevLoad() routine requires.   The
END_TBL_ENTRY is defined as follows:

.CS
typedef struct end_tbl_entry
    {
    int          unit;                           /@ device unit number @/
    END_OBJ * (* endLoadFunc) (char *, void *);  /@ the load function @/
    char *       endLoadString;                  /@ the load string @/
    BOOL         endLoan;                        /@ buffer loaning flag @/
    void *       pBSP;                           /@ BSP private @/
    BOOL         processed;                      /@ processed flag @/

    } END_TBL_ENTRY;
.CE

The specified value and use of the END device <unit> number is significant
to the BSP END driver configuration modules.  Among other things, the END
unit number facilitates associating an END driver instance with a specific
physical device in the case of PCI network interface cards.  The END unit
number is not used this way in the case of ISA devices, as explained below.

The BSP configuration modules for PCI network interface cards (NIC)s will
iterate the PCI bus in search of supported NICs.  Whenever a supported
PCI NIC is located on the bus, information on the device is stored in an
internal table.  These internal tables are indexed via END unit numbers
during END device initialization.

The END unit numbering for each entry of a kind in the <endDevTbl> table
should start with 0 and proceed up through positive integers for each
successive entry.  For example, assuming that the system will have two
physical Am79c97x Ethernet devices and an END driver instance
associated with each device, an additional entry should be added to the
<endDevTbl> table as follows:

.CS
END_TBL_ENTRY endDevTbl [] =
    {
    ...
    #ifdef INCLUDE_LN_97X_END
        {0, LN_97X_LOAD_FUNC, LN_97X_LOAD_STR, LN_97X_BUFF_LOAN,
        NULL, FALSE},

        {1, LN_97X_LOAD_FUNC, LN_97X_LOAD_STR, LN_97X_BUFF_LOAN,
        NULL, FALSE},
    #endif /@ INCLUDE_LN_97X_END @/
    ...
    };
.CE

The muxDevLoad() routine will attempt to load two instances of the
ln97xEnd driver.  One instance will have END unit number 0, and the
other will have END unit number 1.  END unit number 0 will be associated
with the first Am79x97x device instance found on the PCI bus, while END
unit 1 will be associated with the subsequent Am79c97x device instance
found on the PCI bus.

Extending the example a bit, suppose that the system will have two
physical Am79x97x Ethernet devices and one Intel 82558 Ethernet device.
The entries defined above could be left intact, while an entry for the
82558 device would be included by virtue of defining the INCLUDE_FEI_END
configuration constant.  The preprocessed table would define the following
entries:

.CS
END_TBL_ENTRY endDevTbl [] =
    {
    ...
        {0, FEI82557_LOAD_FUNC, FEI82557_LOAD_STRING, FEI82557_BUFF_LOAN,
        NULL, FALSE},

        {0, LN_97X_LOAD_FUNC, LN_97X_LOAD_STR, LN_97X_BUFF_LOAN,
        NULL, FALSE},

        {1, LN_97X_LOAD_FUNC, LN_97X_LOAD_STR, LN_97X_BUFF_LOAN,
        NULL, FALSE},
    ...
    };
.CE

In short, for a particular kind of driver and physical device on the PCI
bus, END unit number <n> associates the END driver instance with the <n>th
physical device instance beginning with instance number 0.

Note that the number of physical PCI devices configured for the system
is finite.  The system cannot load more END instances than physical devices.
A constant in each sysXXXEnd.c configuration file specifies how many
physical devices will be configured for a particular driver.  This value
and the associated data structures can be customized.  Read the sysXXXEnd.c
file associated with a particular driver for more information.

In the case of ISA network interface cards, configuration software cannot
dynamically determine memory, IO, and interrupt resources required for such
devices.  As noted previously, the BSP does not support ISA PnP.  Moreover,
the BSP predefines these values for, at most, one physical device instance
of each kind.  As a result, the configuration modules for ISA network
interface cards assume that only one instance of the device will be loaded
to the MUX.  Physical device information is not stored in a table indexed
via the END unit number.  Hence, END unit numbers for ISA network controllers
are arbitrary.  The sysXXXEnd.c configuration modules for ISA NICs can be
customized to store information for multiple physical devices in a table and
index the table via END unit number in a manner similar to the configuration
modules for PCI NICs.  The tables for multiple ISA devices must be defined
and constructed statically when a VxWorks system is compiled.

The BSP sysXXXEnd.c driver configuration modules may not initialize an END
driver for all possible PCI vendor IDs, PCI device IDs, and PCI revision IDs
associated with compatible devices.  In many cases, this is because the
driver has not been tested on the particular device.  The PCI vendor,
device, and revision IDs the driver will be configured for are specified
directly in the sysXXXEnd.c file associated with the driver.  In some
cases, the header file associated with a driver may define PCI ID values.
Refer to the board vendor's documentation and the appropriate sysXXXEnd.c
and END driver header files in cases where the configuration module fails
to initialize a PCI END driver for a specific physical device on the bus.

.SS "VME Access"
This BSP has no VME access

.SS "PCI Access"
In order to use PCI the INCLUDE_PCI
directive must be enabled in config.h.
Furthermore, PCI_CFG_TYPE in pc.h
must be set to the correct configuration type.
Values for PCI_CFG_TYPE must be one of:

.TS
center;
c c
l l.
Macro	meaning
_
PCI_CFG_FORCE	force user specified configuration
PCI_CFG_AUTO	VxWorks does auto configuration. Currently unavailable.
PCI_CFG_NONE	external agent does configuration
.TE

PCI defines two mechanisms. The newer method, and the default vxWorks
method, is PCI_MECHANISM_1. Some older PCs might work better with
the old method, PCI_MECHANISM_2. If PCI is not working with
your older PC, change the first argument to pciConfigLibInit
to PCI_MECHANISM_2 in sysLib.c:
.ne 7
.CS
#ifdef  INCLUDE_PCI
    pciConfigLibInit (PCI_MECHANISM_1, PCI_CONFIG_ADDR, PCI_CONFIG_DATA,
NONE);
#if     FALSE
    pciConfigLibInit (PCI_MECHANISM_2, PCI_CONFIG_CSE, PCI_CONFIG_FORWARD,
                     PCI_CONFIG_BASE);
#endif  /* FALSE */
.CE
.ne 22
.SS "Boot Devices"
The supported boot devices are:
	
.CS
    \f3xx\f1 - Ethernet (any one of the many adaptors described above)
.CE
.CS
    \f3pcmcia=<slot number>\f1 - PCMCIA ATA device
        slot number is one of:
            0 = PCMCIA slot 0
            1 = PCMCIA slot 1
	Note: Supported PCMCIA boot cards are the 3COM Etherlink III PC card
	and a PCMCIA ATA card
.CE
.CS
    \f3fd=<drive number>,<diskette type>\f1 - Diskette
        drive number is one of:
            0 = default; the first diskette drive (drive A:)
            1 = the second diskette drive (drive B:)
        diskette type is one of:
            0 = default; 3.5" diskette
            1 = 5.25" diskette
.CE
.CS
    \f3ata=<controller number>,<drive number>\f1 - ATA/IDE drive
        controller number is one of:
            0 = controller described in the first entry of 
                the ataResources table
            1 = controller described in the second entry of 
                the ataResources table
        drive number is one of:
            0 = first drive on the controller
            1 = second drive on the controller
.CE

A boot EPROM (type 27020 or 27040) is supported 
with Blunk Microsystems' ROM Card 1.0. 
For information on booting from these devices, 
see the Blunk Microsystems documentation.
The program romcard.s, a loader for code programmed in to the EPROM,
is provided to support 
VxWorks with the ROM Card.
In addition, the following configurations are defined in 
the Makefile to generate Motorola S-record format from 
bootrom_uncmp or from vxWorks_boot.st :
.TS
center;
l l.
romcard_bootrom_512.hex	boot	ROM image for 27040 (512 KB)
romcard_bootrom_256.hex	boot	ROM image for 27020 (256 KB)
romcard_vxWorks_st_512.hex	bootable VxWorks image for 27040 (512 KB)
.TE
Neither the ROM Card nor the EPROM is distributed with VxWorks.
For more information visit http://www.blunkmicro.com/

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols apply only to Ethernet devices

.SS "ROM Considerations"
Not applicable to this BSP

.SH "SPECIAL CONSIDERATIONS"
.SS "Delivered Objects"
.ne 20
.TS
center;
c c
l l.
Object Name	Description
_
vxWorks	T{
An image with no target shell or target symbol table. Network support
is included and initialized.
T}
VxWorks.st	T{
A fully linked standalone image, including a target based
shell, symbol table, and network interface.  Note that the
network interface is not initialized.  There is no WDB agent.
T}
bootrom_uncmp.bin	T{
An uncompressed bootrom image which will run in upper memory by default.
T}
bootrom.bin	T{
A compressed bootrom image which will run in upper memory by default.
T}
mkboot.o	A VxWorks utility for creating boot disks.
.TE

.SS "Make Targets"
The BSP supports the following make targets:

.TS E
expand;
cw(1i) cw(2i) c
lw(1i) lw(2i) l.
Image Name	Description	Comments
=
vxWorks_rom.bin	T{
A high memory uncompressed bootable vxWorks image.
T}
vxWorks.st_rom.bin	T{
A high memory compressed bootable vxWorks.st image.
T}
bootrom.bin	T{
A high memory compressed bootrom.
T}
bootrom_uncmp.bin	T{
A high memory uncompressed bootrom.
T}
vxWorks	The standard "Tornado-style" VxWorks image.
vxWorks.st	T{
A fully linked standalone vxWorks, including target based shell, symbol
table, and network interface.  The network interface is not initialized.
There is no WDB agent.
T}
vxWorks.res_rom	T{
A standalone VxWorks image that can be put in ROM.
Only the data segment of this ROM image is copied into RAM.
T}
vxWorks.res_rom_nosym	T{
A standalone VxWorks image that can be put in ROM.
Only the data segment of this ROM image is copied into RAM.
There is no symbol table. 
T}
_
.TE

.SS "Special Routines"
The following routines are specific to this BSP and are available
to the user. The are written in assembly code in sysALib.s. For 
further details see the reference entries:
.TS
expand;
sysInByte() | input one byte from I/O space
sysInWord() | input one word from I/O space
sysInLong() | input one long-word from I/O space
sysOutByte() | output one byte to I/O space
sysOutWord() | output one word to I/O space
sysOutLong() | output one long-word to I/O space
sysInWordString() | input word string from I/O space
sysInLongString() | input long string from I/O space
sysOutWordString() | output word string to I/O space
sysOutLongString() | output long string to I/O space
.TE


.SH "KNOWN PROBLEMS"

.SS "Tornado 2.2 Problems"
As of 1 April, 2002 (Tornado 2.2 Beta), the Intel Architecture stack
trace library, trcLib, has not been updated to handle subroutine
prologue code which might be inserted for SSE instruction support.
If the Tornado 2.2 compiler is used to build code implementing
Streaming SIMD Extension (SSE or SSE2) instructions, then the -msse
compiler flag may be used so that SSE stack operands are aligned on
the proper boundaries.  The compiler currently supports the alignment
of stack variables by inserting subroutine prologue code that the
stack trace library does not recognize.  An immediate consequence of
this is that routines such as tt() may generate exceptions when one
attempts to trace code that has been built with -msse.


.SS "Validation Test Suite (VTS) Failures"

nvRam test: The first test in this suite may fail on the first attempt 
with a new boot disk.  (NV-RAM is implemented as a file, created on the 
boot disk on the first write command to NV-RAM.)  Subsequent times 
through the test suite should succeed.


Catastrophic error test: fails as the VTS expects
an exception message but this BSP displays none;
however, the BSP correctly recovers
by rebooting the target.

bootline Test:
Bus error test for local error address fails.
Bus error test for off-board error address fails.
Boot commands test failed as the VTS incorrectly presumes a big
endian architecture.


.SH "OTHER"
The valid auxiliary clock rates are between 2 ticks per 
second and 2 to the power of 13 ticks per second (2^13 = 8192).

Warm booting (reboot) is dependent upon the following parameters (shown with
default values) in config.h:

.CS
#define SYS_WARM_BIOS       0   /* warm start from BIOS */
#define SYS_WARM_FD         1   /* warm start from FD */
#define SYS_WARM_ATA        2   /* warm start from ATA */
#define SYS_WARM_TFFS       3   /* warm start from DiskOnChip */

#define SYS_WARM_TYPE       SYS_WARM_FD /* warm start device */
#define SYS_WARM_FD_DRIVE   0   /* 0 = drive a:, 1 = b: */
#define SYS_WARM_FD_TYPE    0   /* 0 = 3.5" 2HD, 1 = 5.25" 2HD */
#define SYS_WARM_ATA_CTRL   0   /* controller 0 */
#define SYS_WARM_ATA_DRIVE  0   /* 0 = c:, 1 = d: */
.CE

If SCSI configuration fails, it may be the result of improper SCSI bus 
termination. Check termination carefully on all devices, including
the controller. Note that some devices
have built in termination that is configured via a jumper.

In order to dynamically update the MMU table entries, prior to MMU
initialization, several dummy entries have been added to the end of the
memory description table sysPhysMemDesc. This allows PCI device
configuration space, configured by the BIOS, to be properly mapped into
the VxWorks memory map. This is done by sysMmuMapAdd() in sysLib.c.

This BSP does not support ISA PnP. Such devices can be supported if
PnP is disabled and the device parameters (IO address, Memory address,
IRQ, DMA channel etc) is set to match its BSP driver configuration. If
the device uses soft-configuration instead of jumpers, an appropriate
utility program, usually available from the device manufacturer,
should be used to setup the device parameters.

DMA Buffer Alignment and cacheLib

If you write device drivers that use Intel 8237 direct memory access 
into buffers obtained from cacheLib, the buffers must be aligned on 
a 64KB boundary and be in the lower memory.

.SS "Non Intel X86 Processor"
sysCpuProbe() uses CPUID instruction to identify processor and features
for Intel processors.  Here is what said in the application note AP-485 
"Intel Processor Identification and the CPUID Instruction":

  - Use the CPUID feature flags in your applications to determine which
    processor features are supported.  By using the CPUID feature flags
    to determine processor features, your software can detect and avoid
    incompatibilities introduced by the addition or removal of processor
    features.
  - Do not assume that a given processor family or model has any specific
    feature.  For example, do not assume the family value 5 (Pentium
    processor) means there is a FPU on-chip.  Use the feature flags for
    this determination.
  - Do not assume processors with higher family or model numbers have
    all the features of a processor with a lower family or model
    number.  For example, a processor with a family value of 6 (P6 family
    processor) may not necessarily have all the features of a processor
    with a family value of 5.

For non Intel X86 processors, there are two ways to bypass this CPUID
identification mechanism.  The static configuration is one, and the 
dynamic runtime checking is another.  Former is to use the build parameter 
INCLUDE_CPU_PROBE, and to set the processor type manually with 
X86CPU_DEFAULT in config.h.  The latter is to execute CPUID and check the 
vendor ID string "GenuineIntel" in sysCpuProbe().  If there is no match, 
the processor type is set to X86CPU_DEFAULT.  In both cases, the default 
processor type X86CPU_DEFAULT(PENTIUM) is set and CPUID feature bits stay 
zero that prevent enabling PENTIUM specific features which non Intel X86 
processors may not have.  If the processor does not have CR4 register,
like Intel 80486, comment out the ARCH_CR4_INIT macro in romInit.s and 
sysALib.s.  The CR4 is introduced in the P5(Pentium) architecture.  The
API vxCr4Get() and vxCr4Set() checks sysProcessor, and do nothing if it
is pre P5(Pentium) architecture.  A targeting BSP for non Intel X86 
processor is pcPentium.  For now, non Intel X86 processor specific 
features are not supported.

.SS "P5 (Pentium), P6 (PentiumPro, II, III), and P7 (Pentium4) family processor"
Following features are supported for P5 (Pentium), P6 (PentiumPro, II, III), 
and P7 (Pentium4) family processor, and they are enabled in sysHwInit() depending 
upon the feature flags obtained by the CPUID instruction.  
See pentiumLib for more details of these features.

.IP "Memory Type Range Registers (MTRRs)" 
If INCLUDE_MTRR_GET is defined, contents of the MTRRS are copied to the 
sysMtrr[] table.  Otherwise it sets the contents of sysMtrr[] to the MTRRs.

.IP "Performance Monitoring Counter (PMC)"
This is an optional feature configured by INCLUDE_PMC macro.  In this release,
Pentium4's PMC is not supported yet.

.IP "Machine Check Architecture (MCA)"
This is enabled in pentiumMcaEnable() in sysHwInit() if the MCA is supported
by the processor.

.IP "Time Stamp Counter (TSC)"
If INCLUDE_TIMESTAMP_TSC is defined, on-chip TSC is used for the 
time stamp driver.  PENTIUM_TSC_FREQ specifies its frequency.
If it is defined to zero, the frequency is automatically detected.

.IP "Enhanced MMU"
The enhanced MMU is included by defining INCLUDE_MMU_P6_32BIT or
INCLUDE_MMU_P6_36BIT macro.  With INCLUDE_MMU_P6_32BIT, 4KB-page 
and 4MB-page are supported.  With INCLUDE_MMU_P6_36BIT, 4KB-page 
and 2MB-page are supported.  The page size is configurable by 
VM_PAGE_SIZE macro.  Two architecture specific VM library APIs
are provided and linked in automatically.  For bundled VM library,
they are vmBaseArch32Map() and vmBaseArch32Translate().
For unbundled VM library VxVMI, they are vmArch32Map() and 
vmArch32Translate().  Their 36 bit version are also available.
Two new memory attribute macros, VM_STATE_WBACK and 
VM_STATE_GLOBAL, are added.  
VM_STATE_WBACK (clear PWT bit) and VM_STATE_WBACK_NOT (set PWT bit)
represents the cache mode of a page.
VM_STATE_GLOBAL (set GLOBAL bit) and VM_STATE_GLOBAL_NOT (clear
GLOBAL bit) represents the global characteristics of a page.

.IP "Advanced Programmable Interrupt Controller (APIC)"
Intel P6 (PentiumPro, II, and III) and P7 (Pentium4) family processor's 
APIC/xAPIC is supported in either Virtual Wire Mode (define
VIRTUAL_WIRE_MODE in config.h) or Symmetric IO Mode (define 
SYMMETRIC_IO_MODE in config.h).  If neither of them is defined,
VxWorks uses a mode that is set up by BIOS, which could be Virtual
Wire Mode or PIC Mode.  Only Local APIC/xAPIC is used in Virtual Wire 
Mode, both Local APIC/xAPIC and IO APIC/xAPIC are used in Symmetric IO 
Mode.

.IP "Data Cache Mode"
CACHE_COPYBACK data cache mode is default for Pentium. 
It uses Write Back data cache mode with the generic MMU library
for X86 architecture.
CACHE_COPYBACK and CACHE_SNOOP_ENABLE is default for P6 (PentiumPro,
II, III) and P7 (Pentium4) family processors.
CACHE_COPYBACK has no effect to the MMU library with 
INCLUDE_MMU_PENTIUMPRO defined, that support page basis 
Write Back/Write Through cache mode.
CACHE_SNOOP_ENABLE respects MESI cache protocol and doesn't invoke
the WBINVD (write back and invalidate cache) instruction in the
flush routine in the cache library.

.SS "MTRR"
This table shows effective memory type depending on MTRR, PCD, and
PWT setting.

.TS E
expand;
c c c c
c c c c
l l l l.
MTRR mem type	PCD value	PWT value	Effective mem type
_
UC	X	X	UC
WC	0	0	WC
	0	1	WC
	1	0	WC
	1	1	UC
WT	0	X	WT
	1	X	UC
WP	0	0	WP
	0	1	WP
	1	0	UC
	1	1	UC
WB	0	0	WB
	0	1	WT
	1	X	UC
_
.TE

This table shows MTRR memory types and their properties.

.TS E
expand;
c c c c c
c c c c c
l l l l l.
	Cacheable in		Allows	Memory
	L1 and L2	Writeback	Speculative	Ordering
Mnemonic	Caches	Cacheable	Reads	Model
_
UC	No	No	No	Strong Ordering
WC	No	No	Yes	Weak Ordering
WT	Yes	No	Yes	Speculative
				Processor Ordering
WP	Yes for reads,	No	Yes	Speculative
	No for writes			Processor Ordering
WB	Yes	Yes	Yes	Speculative
				Processor Ordering
_
.TE

.SS "DEBUGGING NULL ACCESSES with the MMU"

The macro _WRS_BSP_DEBUG_NULL_ACCESS may be defined in config.h to debug 
NULL accesses in code.  Marking the lower page of RAM invalid in the MMU 
causes any code access to NULL (or any offset from NULL up to page size) 
to throw an exception, suspending the offending task.  Making it very easy 
to debug the NULL access with lkAddr(), and l() from the shell.

Consider this simple errant code which reads from a NULL pointer:

.CS
/* tmp.c */

#include "vxWorks.h"

UINT32 tmpFunc
    (
    void
    )
    {
    UINT32 * badPointer = NULL; /* force a NULL pointer */
    UINT32 tmpValue;

    tmpValue = *badPointer; /* read from the NULL pointer */

    return (tmpValue);
    }
/* end tmp.c */
.CE

Note that this code compiles without error or warning reported.
Execute the errant code on a default vxWorks image.  Note that 
errors are not reported for NULL accesses:

.CS
-> ld < ~/tmp.o
value = 67103608 = 0x3ffeb78 = tmpFunc + 0x1c8
-> tmpFunc
value = 1602816 = 0x187500 = open + 0x1f0
-> *0x0
0x0: value = 1602816 = 0x187500 = open + 0x1f0
-> sp (tmpFunc)
->
.CE

Load the same code to the shell of a target built with the macro
_WRS_BSP_DEBUG_NULL_ACCESS defined. Note that in this case, a page 
fault catches the NULL access.   It also shows which instruction 
caused the access fault, and what access address caused the fault.
Also reported is the processor specific information stating which 
kind of fault occurred.

.CS
-> ld < tmp.o
value = 67103608 = 0x3ffeb78 = tmpFunc + 0x1c8
-> sp (tmpFunc)
task spawned: id = 0x3f3b648, name = t1
value = 66303560 = 0x3f3b648
->
Page Fault
Page Dir Base   : 0x03fd6000
Esp0 0x03f3b618 : 0x00000000, 0x001265ec, 0x00000000, 0x00000000
Esp0 0x03f3b628 : 0x00000000, 0x00000000, 0x00000000, 0x00000000
Program Counter : 0x03ffe9b3
Code Selector   : 0x00000008
Eflags Register : 0x00010246
Error Code      : 0x00000000
Page Fault Addr : 0x00000000
Task: 0x3f3b648 "t1"
.CE

Use the lkAddr() shell command passing the program counter value 
to find the nearest global symbol entry point.

.CS
-> lkAddr (0x03ffe9b3)
0x03ffe9b0 tmpFunc                   text
value = 0 = 0x0
.CE

Use l() on that address to list the errant functions code.

.CS
-> l 0x03ffe9b0

tmpFunc:
0x03ffe9b0  55                      PUSH           EBP
0x03ffe9b1  89 e5                   MOV            EBP, ESP
0x03ffe9b3  a1 00 00 00 00          MOV            0x0, EAX  <-Boom!
0x03ffe9b8  89 ec                   MOV            ESP, EBP
0x03ffe9ba  5d                      POP            EBP
0x03ffe9bb  c3                      RET
= 67103168 = 0x3ffe9c0 = tmpFunc + 0x10
->
.CE

Note that "<-Boom!" shows the instruction which causes an access 
to NULL.  Using this technique, we caught a common error that otherwise
would have gone unseen in execution or in compilation.  A read from NULL 
is one bad thing, but this will also catch the other dangerous case of 
a software write to NULL, for example:

.CS
-> (*(0x0)) = 1

Page Fault
Page Dir Base   : 0x03fd6000
Esp0 0x03f3dd84 : 0x001d6820, 0x00000003, 0x00000001, 0xeeeeeeee
Esp0 0x03f3dd94 : 0x00000003, 0x001d6810, 0x001d6840, 0xeeeeeeee
Program Counter : 0x001b1d6e
Code Selector   : 0x00000008
Eflags Register : 0x00010202
Error Code      : 0x00000002
Page Fault Addr : 0x00000000
.CE

This technique will also catch any access member of an un-initialized
structure, ala : (NULL)->member, where the offset to member is less 
than one page size.

.SS "Memory Auto scan"
By default the BSP (except pc486) automatically scan for the top of memory. 
On many targets, the upper memory is reserved by the BIOS for system use.
The BSP rely on the BIOS to protect this memory region.

On some targets, the BIOS does not protect this memory region. In this
situation, automatic memory scanning may corrupt the target, interfering with
video and or destroying the caches. For these targets hardcode the upper
memory limit and disable memory auto sizing by un-defining
LOCAL_MEM_AUTOSIZE. By default memory auto sizing is disable only for pc486 BSP.
For that reason, the BSP user must set SYSTEM_RAM_SIZE macro in config.h with
appropriate value of the system RAM.
 
 
.SS "Thermal Monitor and Geyserville III support"
sysTherm.c contains routines to manipulate the Thermal Monitor, 
Clock Modulation and Geyserville III mechanism.  The Thermal Monitor 
is programmed to generate an interrupt to the processor when the 
thermal sensor is tripped.  Software Controlled Clock Modulation and
the Thermal Monitor are both enabled if the processor support them.
In this case, if the thermal status of the processor is hot, Clock 
Modulation through the Thermal Monitor takes precedence and the clock
is modulated at a 50% duty cycle, regardless of the setting of the
on-demand Clock Modulation duty cycle.  Either the Clock Modulation
or the Geyserville III technology is used to handle the performance
and power consumption state transition of the processor.  If the
processor has both capabilities, the Geyserville III is used.  The
Geyserville III support is determined by CPU feature information
returned by CPUID function 1.  Bit 7 returned in ECX denotes 
Geyserville III technology support.  Platforms supporting Geyserville
III (and the required IVMP IV VR technology) will set the GV3_EN bit
and the GV_SEL_LOCK bit in the IA32_MISC_ENABLE MSR during POST.

The initialization routine gv3LibInit() has six parameters.  The 
first parameter is how to manage the processor performance and power 
consumption state.  They are:

    - GV3_MAX_PERF (Maximum performance)
      the highest performance state is used all the time.

    - GV3_MAX_BATT (Maximum battery life)
      the lowest performance state is used all the time.

    - GV3_OPT_BATT (Optimized battery)
      the highest performance state is used if power source is AC.
      the lowest performance state is used if power source is Battery.

    - GV3_AUTO (Automatic)
      the performance state is adjusted to match the current processor
      utilization.

The next four parameters decide the state transition policy as follows.
The second and third one is X and Y, fourth and fifth one is N and M, 
respectively.

    - If CPU utilization is above X<95> % of state S for N<300> 
      millisecond, transition CPU to state S+1.

    - If CPU utilization is below Y<95> % of state S for M<1000> 
      millisecond, transition CPU to state S-1.

The last parameter is a function pointer to the routine that checks if
it is AC powered or Battery powered.  That routine returns TRUE if it 
is AC powered (default), FALSE otherwise.


.SS "Obsolete In The Tornado 2.2 Release"
The WRS Intel 80x86-based and Pentium-based BSP Makefiles formerly
defined bootrom_high, vxWorks_low, and vxWorks_rom_low build targets.
These images are deprecated and will no longer be supported.

.SS "Obsolete In The Next Release"
The following  network devices will not be supported by Intel Architecture
Pentium BSPs in the next release:

.IP
SMC 8013WC (Elite16), ISA bus Ethernet interface

.IP
SMC Elite16 Ultra, ISA bus Ethernet interface

.IP
Intel 82586 EtherExpress 16, ISA bus Ethernet interface

.IP
Intel 82596 EtherExpress flash 32, EISA bus Ethernet interface

.SS "Retained In The Next Release"
Intel Architecture Pentium BSP support for the following network devices
will be retained in the next release:

.IP
3Com 3C509 (EtherLink III), ISA/PCMCIA bus Ethernet interface

.IP
SMC/Ampro 91c9x, ISA/PC104 bus Ethernet interface

.IP
Novell/Eagle 2000 (NE2000), ISA/PC104 bus Ethernet interface

.IP
VME bus (Excellan) Ethernet interface

.IP
VME bus (CMC/Motrolla) Ethernet interface

.SS "Upgrading a BSP from VxWorks 5.4"
See Architecture Supplement "Upgrading a BSP from VxWorks 5.4"

.SH "AUTHOR"
Original port by Hdei Nunoe of Wind River Systems, Alameda, CA.
Updates to Release 1.2/2 by Hdei Nunoe, David Holloway, Jun Lin,
Muhammed Ahmed, Robert Ellis, and Paul Iannacito of Wind River Systems,
Alameda, CA.

.SH SEE ALSO
.tG "Getting Started"

.pG "Configuration"

.pG "Architecture Supplement"

.SH "BIBLIOGRAPHY"
.iB "Intel Architecture Software Developer's Manual, Volume 1: Basic Architecture"

.iB "Intel Architecture Software Developer's Manual, Volume 2: Instruction Set Reference"

.iB "Intel Architecture Software Developer's Manual, Volume 3: System Programming Guide"

.iB "AP-485, Intel Processor Identification and the CPUID Instruction"

.iB "PCI System Architecture, Fourth Edition, Addison-Wesley, 1999, ISBN 0-201-30974-2"
