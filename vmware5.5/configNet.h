/* configNet.h - Network configuration header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01l,25apr02,rhe  Added C++ Protection
01k,23apr02,pai  Made DEC and GEI END driver config names consistent with
                 other END driver config names.
01j,22oct01,pai  Cleaned up formatting and set XXX_BUFF_LOAN to TRUE, as
                 this is formally a kind of Boolean value.
01i,18oct01,jln  added support for GEI8254X END driver
01h,26sep01,pai  Added support for dec21x40End driver.
01g,31mar99,dat  SPR 25958, added #ifndef IP_MAX_UNITS
01f,12mar99,cn   added support for SMC el3c90xEnd driver (SPR# 25327).
01e,08mar99,sbs  added support for SMC Elite Ultra card.(SPR #25234)
                 changed elt3c509 end support to use sysElt3c509End routine
                 added support for ne2000End driver (SPR #25398)
01d,01feb99,jkf  added support for AMD 7997x PCI card.
01c,26nov98,ms_  add support for end enabled elt3c509
01b,12nov98,dat  added INCLUDE_FEI_END around fei unit 0 entry
01a,31mar98,cn   written.
*/

#ifndef INCconfigNeth
#define INCconfigNeth

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"


/* Intel 8255x PCI (fei) driver defines */

#ifdef INCLUDE_FEI_END

#define FEI82557_LOAD_FUNC    fei82557EndLoad
#define FEI82557_BUFF_LOAN    TRUE

/*
 * The fei82557End initialization string format is:
 *
 * <memBase>:<memSize>:<nCFDs>:<nRFDs>:<userFlags>
 */

#define FEI82557_LOAD_STRING  "-1:0x00:0x20:0x20:0x00"

IMPORT END_OBJ * FEI82557_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_FEI_END */


/* DEC 21x40 PCI (dc) driver defines */

#ifdef INCLUDE_DEC21X40_END

#define END_DC_LOAD_FUNC      sysDec21x40EndLoad
#define END_DC_BUFF_LOAN      TRUE
#define END_DC_LOAD_STRING    ""

IMPORT END_OBJ * END_DC_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_DEC21X40_END */


/* 3Com EtherLink III (elt) driver defines */

#ifdef INCLUDE_ELT_3C509_END

#define END_3C509_LOAD_FUNC   sysElt3c509EndLoad
#define END_3C509_BUFF_LOAN   TRUE
#define END_3C509_LOAD_STRING ""

IMPORT END_OBJ * END_3C509_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_ELT_3C509_END */


/* SMC Elite Ultra (ultra) driver definitions */

#ifdef INCLUDE_ULTRA_END

#define END_ULTRA_LOAD_FUNC   sysUltraEndLoad
#define END_ULTRA_BUFF_LOAN   TRUE
#define END_ULTRA_LOAD_STRING ""

IMPORT END_OBJ * END_ULTRA_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_ULTRA_END */


/* Ne2000 (ene) driver definitions */

#ifdef INCLUDE_ENE_END

#define END_ENE_LOAD_FUNC     sysNe2000EndLoad
#define END_ENE_BUFF_LOAN     TRUE
#define END_ENE_LOAD_STRING   ""

IMPORT END_OBJ * END_ENE_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_ENE_END */


/* Am79C97x (lnPci) driver defines */

#ifdef INCLUDE_LN_97X_END

#define LN_97X_LOAD_FUNC      sysLn97xEndLoad
#define LN_97X_BUFF_LOAN      TRUE
#define LN_97X_LOAD_STR       ""

IMPORT END_OBJ * LN_97X_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_LN_97X_END */


/* 3Com EtherLink PCI (elPci) driver defines */

#ifdef INCLUDE_EL_3C90X_END

#define EL_3C90X_LOAD_FUNC    sysEl3c90xEndLoad
#define EL_3C90X_BUFF_LOAN    TRUE
#define EL_3C90X_LOAD_STR     ""

IMPORT END_OBJ * EL_3C90X_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_EL_3C90X_END */


/* Intel 82543/82544 PCI (gei) driver defines */

#ifdef INCLUDE_GEI8254X_END

#define GEI8254X_LOAD_FUNC    sysGei8254xEndLoad
#define GEI8254X_BUFF_LOAN    TRUE
#define GEI8254X_LOAD_STR     ""

IMPORT END_OBJ * GEI8254X_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_GEI8254X_END */


/* max number of END ipAttachments we can have */

#ifndef IP_MAX_UNITS
#   define IP_MAX_UNITS (NELEMENTS (endDevTbl) - 1)
#endif



/******************************************************************************
*
* END DEVICE TABLE
* ----------------
* Specifies END device instances that will be loaded to the MUX at startup.
*/

END_TBL_ENTRY endDevTbl [] =
    {
#ifdef INCLUDE_EL_3C90X_END
    {0, EL_3C90X_LOAD_FUNC, EL_3C90X_LOAD_STR, EL_3C90X_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_EL_3C90X_END */

#ifdef INCLUDE_LN_97X_END
    {0, LN_97X_LOAD_FUNC, LN_97X_LOAD_STR, LN_97X_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_LN_97X_END */

#ifdef INCLUDE_FEI_END
    {0, FEI82557_LOAD_FUNC, FEI82557_LOAD_STRING, FEI82557_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_FEI_END */

#ifdef INCLUDE_DEC21X40_END
    {0, END_DC_LOAD_FUNC, END_DC_LOAD_STRING, END_DC_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_DEC21X40_END */

#ifdef INCLUDE_ELT_3C509_END
    {0, END_3C509_LOAD_FUNC, END_3C509_LOAD_STRING, END_3C509_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_ELT_3C509_END */

#ifdef INCLUDE_ULTRA_END
    {0, END_ULTRA_LOAD_FUNC, END_ULTRA_LOAD_STRING, END_ULTRA_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_ULTRA_END */

#ifdef INCLUDE_ENE_END
    {0, END_ENE_LOAD_FUNC, END_ENE_LOAD_STRING, END_ENE_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_ENE_END */

#ifdef INCLUDE_GEI8254X_END
    {0, GEI8254X_LOAD_FUNC, GEI8254X_LOAD_STR, GEI8254X_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_GEI8254X_END */

    {0, END_TBL_END, NULL, 0, NULL, FALSE}
    };

#ifdef __cplusplus
}
#endif

#endif /* INCconfigNeth */

