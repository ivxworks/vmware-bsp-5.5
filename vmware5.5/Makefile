# MakeSkel - makefile skeleton for vw/config/pc
#
# Copyright 1984-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01l,25jun02,pai  Changed RAM_HIGH_ADRS, RAM_LOW_ADRS, and
#                  LOCAL_MEM_LOCAL_ADRS.
# 01k,05jul02,hdn  added EXTRA_DEFINE
# 01j,20may02,rhe  Add bootrom.bin to RELEASE
# 01i,08may02,pai  Removed build rules for deprecated bootrom_high,
#                  vxWorks_rom_low, and vxWorks_low images.
# 01h,19nov01,hdn  removed .bin rule as it is defined in the upper level
# 01g,14nov01,dat  RELEASE update
# 01f,02nov01,hdn  replaced bootrom_uncmp with bootrom_uncmp.bin
# 01e,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 01d,25sep01,dat  Update for T2.2
# 01c,13aug01,hdn  removed macro CPU_VARIANT.
# 01b,12may98,hdn  added macro CPU_VARIANT.
# 01a,09jul97,hdn  written based on pc386/Makefile.
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# PC Pentium.
#
# NOTE
# The constants ROM_TEXT_ADRS and RAM_HIGH_ADRS
# must be identical to those in config.h.
# 
# INCLUDES
#     makeTarget
#*/

CPU		= PENTIUM
TOOL		= gnu

TGT_DIR = $(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.bsp
#include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
#include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.


TARGET_DIR = vmWare
VENDOR     =
BOARD      = PC Pentium

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, MakeSkel, Makefile, and Makefile.*
# All definitions for these constants must be identical.
#

# ifdef BOOTCODE_IN_RAM
ROM_TEXT_ADRS      = 00008000	# ROM entry address - A: or C:
ROM_SIZE           = 00190000	# number of bytes of ROM space
# else
# ROM_TEXT_ADRS      = fff20000	# ROM entry address - EPROM
# ROM_SIZE           = 0007fe00	# number of bytes of ROM space
# endif

RAM_LOW_ADRS       = 00308000	# VxWorks image entry point
RAM_HIGH_ADRS      = 00108000	# Boot image entry point

EXTRA_DEFINE       = 		# -DFAST_REBOOT

MACH_EXTRA	   = ln97xEnd.o


RELEASE += bootrom_uncmp.bin bootrom.bin mkboot.o


## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)


# Following bootable vxWorks should be placed in bootable diskette
# by "mkboot" or "mkbootFd, mkbootAta" utility.
#
#   vxWorks.st_rom	- bootable vxWorks.st(compressed): upper mem
#   bootrom		- bootrom(compressed):		   upper mem
#   bootrom_uncmp	- bootrom:			   upper mem
#
# The boot image is either bootrom_uncmp or vxWorks.st_rom.
# If the EPROM is 27020 (256 Kbytes)
#	$(BINHEX) -a 3fe00 -b romcard >> $@
# If the EPROM is 27040 (512 Kbytes)
#	$(BINHEX) -a 7fe00 -b romcard >> $@

SED	= sed

romcard_bootrom_512.hex:	depend.$(BSP_NAME) romcard.o bootrom_uncmp
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 7fe00 bootrom_uncmp
	$(BINHEX) -a 0 bootrom_uncmp | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 7fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard

romcard_bootrom_256.hex:	depend.$(BSP_NAME) romcard.o bootrom_uncmp
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 3fe00 bootrom_uncmp
	$(BINHEX) -a 0 bootrom_uncmp | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 3fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard

romcard_vxWorks_st_512.hex:	depend.$(BSP_NAME) romcard.o vxWorks.st_rom
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 7fe00 vxWorks.st_rom
	$(BINHEX) -a 0 vxWorks.st_rom | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 7fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard

