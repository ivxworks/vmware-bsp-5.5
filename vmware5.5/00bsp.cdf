/* 00bsp.cdf - BSP configuration file */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,05nov02,hdn  made sysCpuProbe() execution conditional (spr 76279)
01a,13Feb02,pai  Written.
*/

/*
DESCRIPTION
This file overrides generic BSP components in comps/vxWorks/00bsp.cdf with
pcPentium BSP-specific versions of components and parameters defined in
the generic CDF file.
*/


/*******************************************************************************
*
* Timestamp Component and Parameters
*
*/
Component INCLUDE_TIMESTAMP  {
    NAME              Pentium high resolution timestamp driver
    SYNOPSIS          high resolution timestamping
    HDR_FILES         drv/timer/timerDev.h
    CFG_PARAMS        INCLUDE_TIMESTAMP_PIT2 \
                      INCLUDE_TIMESTAMP_TSC  \
                      PENTIUMPRO_TSC_FREQ
    INCLUDE_WHEN      INCLUDE_SYS_TIMESTAMP
}

Parameter INCLUDE_TIMESTAMP_PIT2  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          use i8253-compatible channel 2 for timestamping
    TYPE              exists
    DEFAULT           FALSE
}

Parameter INCLUDE_TIMESTAMP_TSC  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          use 64-bit Timestamp Counter for timestamping
    TYPE              exists
    DEFAULT           TRUE
}

Parameter PENTIUMPRO_TSC_FREQ  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          Timestamp Counter frequency (0 => auto detect)
    TYPE              uint
    DEFAULT           (0)
}

/*******************************************************************************
*
* System Clock and Auxiliary Clock Component and Parameters
*
*/
Component INCLUDE_SYSCLK_INIT {
    NAME              System clock
    SYNOPSIS          System clock component
    CONFIGLETTES      sysClkInit.c
    HDR_FILES         tickLib.h
    INIT_RTN          sysClkInit ();
    CFG_PARAMS        SYS_CLK_RATE      \
                      SYS_CLK_RATE_MIN  \
                      SYS_CLK_RATE_MAX
}

Parameter SYS_CLK_RATE_MAX  {
    NAME              system clock configuration parameter
    SYNOPSIS          maximum system clock rate
    TYPE              uint
    DEFAULT           (PIT_CLOCK/32)
}

Parameter SYS_CLK_RATE_MIN  {
    NAME              system clock configuration parameter
    SYNOPSIS          minimum system clock rate
    TYPE              uint
    DEFAULT           (19)
}

Parameter SYS_CLK_RATE {
    NAME              system clock configuration parameter
    SYNOPSIS          number of ticks per second
    TYPE              uint
    DEFAULT           (60)
}

Parameter AUX_CLK_RATE_MAX  {
    NAME              auxiliary clock configuration parameter
    SYNOPSIS          maximum auxiliary clock rate
    TYPE              uint
    DEFAULT           (8192)
}

Parameter AUX_CLK_RATE_MIN  {
    NAME              auxiliary clock configuration parameter
    SYNOPSIS          minimum auxiliary clock rate
    TYPE              uint
    DEFAULT           (2)
}

/*******************************************************************************
*
* Cache Configuration Parameters
*
*/
Parameter USER_D_CACHE_MODE  {
    NAME              pcPentium write-back data cache mode
    SYNOPSIS          write-back data cache mode
    TYPE              uint
    DEFAULT           (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)
}

/*******************************************************************************
*
* Additional Intel Architecture show routines
*
*/
Component INCLUDE_INTEL_CPU_SHOW {
    NAME              Intel Architecture processor show routines
    SYNOPSIS          IA-32 processor show routines
    HDR_FILES         vxLib.h
    MODULES           vxShow.o
    INIT_RTN          vxShowInit ();
    _INIT_ORDER       usrShowInit
    _CHILDREN         FOLDER_SHOW_ROUTINES
    _DEFAULTS         += FOLDER_SHOW_ROUTINES
}

/*******************************************************************************
*
* pcPentium BSP-specific configuration folder
*
*/
Folder FOLDER_BSP_CONFIG  {
    NAME              pcPentium BSP configuration options
    SYNOPSIS          BSP-specific configuration
    CHILDREN          INCLUDE_PCPENTIUM_PARAMS \
                      INCLUDE_POWER_MANAGEMENT
    DEFAULTS          INCLUDE_PCPENTIUM_PARAMS \
                      INCLUDE_POWER_MANAGEMENT
    _CHILDREN         FOLDER_HARDWARE
    _DEFAULTS         += FOLDER_HARDWARE
}

/*******************************************************************************
*
* BSP parameters Component
*
*/
Component INCLUDE_PCPENTIUM_PARAMS  {
    NAME              BSP build parameters
    SYNOPSIS          expose BSP configurable parameters
    CFG_PARAMS        INCLUDE_PMC         \
                      INCLUDE_ADD_BOOTMEM \
                      ADDED_BOOTMEM_SIZE \
		      INCLUDE_CPU_PROBE
    HELP              pcPentium
}

Parameter INCLUDE_PMC  {
    NAME              pcPentium PMC configuration parameter
    SYNOPSIS          Performance Monitoring Counter library support
    TYPE              exists
    DEFAULT           FALSE
}

Parameter INCLUDE_ADD_BOOTMEM  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          add upper memory to low memory bootrom
    TYPE              exists
    DEFAULT           TRUE
}

Parameter ADDED_BOOTMEM_SIZE  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          amount of memory added to bootrom memory pool
    TYPE              uint
    DEFAULT           (0x00200000)
}

Parameter INCLUDE_CPU_PROBE  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          auto CPU detection mechanism by sysCpuProbe()
    TYPE              exists
    DEFAULT           TRUE
}

/*******************************************************************************
*
* Power Management Component and Parameters
*
*/
Component INCLUDE_POWER_MANAGEMENT  {
    NAME              Pentium power management
    SYNOPSIS          initialize Pentium power management support
    INIT_RTN          vxPowerModeSet (VX_POWER_MODE_DEFAULT);
    CFG_PARAMS        VX_POWER_MODE_DEFAULT
    HDR_FILES         vxLib.h
    _INIT_ORDER       usrInit
    INIT_BEFORE       INCLUDE_CACHE_ENABLE
    HELP              vxLib
}

Parameter VX_POWER_MODE_DEFAULT  {
    NAME              pcPentium configuration parameter
    SYNOPSIS          set the power management mode
    TYPE              uint
    DEFAULT           VX_POWER_MODE_AUTOHALT
}
