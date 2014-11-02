/* ln97xEnd.h - END based AMD LANCE Ethernet header */

/* Copyright 1998 Wind River Systems, Inc. */
/* Copyright 1998 CETIA Inc. */
/*

modification history
--------------------
01a,07dec98,snk	added support 7997x[012] across architectures.
		written from 01b of lnPciEnd.h
*/

#ifndef __INCln97xEndh
#define __INCln97xEndh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "end.h"
#include "cacheLib.h"

#if FALSE
#define DRV_DEBUG	/* temporary should be taken out */
#endif    

/* define the various levels of debugging if the DRV_DEBUG is defined */    

#ifdef	DRV_DEBUG
#include "logLib.h"
#define DRV_DEBUG_OFF		0x0000
#define DRV_DEBUG_RX		0x0001
#define	DRV_DEBUG_TX		0x0002
#define DRV_DEBUG_INT		0x0004
#define	DRV_DEBUG_POLL		(DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define	DRV_DEBUG_POLL_RX	0x0008
#define	DRV_DEBUG_POLL_TX	0x0010
#define	DRV_DEBUG_LOAD		0x0020
#define	DRV_DEBUG_LOAD2		0x0040
#define	DRV_DEBUG_IOCTL		0x0080
#define	DRV_DEBUG_RESET		0x0100
#define	DRV_DEBUG_MCAST		0x0200
#define	DRV_DEBUG_CSR		0x0400
#define DRV_DEBUG_RX_PKT        0x0800
#define DRV_DEBUG_POLL_REDIR	0x10000
#define	DRV_DEBUG_LOG_NVRAM	0x20000
#define DRV_DEBUG_ALL           0xfffff
#endif /* DRV_DEBUG */

/* device IO or memory mapped accessa and data registers */
    
#define LN_97X_RDP	((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x10))
#define LN_97X_RAP	((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x14))
#define LN_97X_RST	((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x18))
#define LN_97X_BDP	((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x1C))

/* csr and bcr addresses */

#define CSR(x)                  (x)
#define BCR(x)                  (x)

/* Definitions for fields and bits in the LN_DEVICE */

#define CSR0_ERR		0x8000	/* (RO) err flg (BABL|CERR|MISS|MERR) */
#define CSR0_BABL		0x4000	/* (RC) babble transmitter timeout */
#define CSR0_CERR		0x2000	/* (RC) collision error */
#define CSR0_MISS		0x1000	/* (RC) missed packet */
#define CSR0_MERR		0x0800	/* (RC) memory error */
#define CSR0_RINT		0x0400	/* (RC) receiver interrupt */
#define CSR0_TINT		0x0200	/* (RC) transmitter interrupt */
#define CSR0_IDON		0x0100	/* (RC) initialization done */
#define CSR0_INTR		0x0080	/* (RO) interrupt flag */
#define CSR0_INEA		0x0040	/* (RW) interrupt enable */
#define CSR0_RXON		0x0020	/* (RO) receiver on */
#define CSR0_TXON		0x0010	/* (RO) transmitter on */
#define CSR0_TDMD		0x0008	/* (WOO)transmit demand */
#define CSR0_STOP		0x0004	/* (WOO)stop (& reset) chip */
#define CSR0_STRT		0x0002	/* (RW) start chip */
#define CSR0_INIT		0x0001	/* (RW) initialize (acces init block) */

#define CSR0_INTMASK 		(CSR0_BABL | CSR0_CERR | CSR0_MISS | \
			         CSR0_MERR | CSR0_RINT | CSR0_TINT | \
                                 CSR0_IDON | CSR0_INEA)

#define	CSR3_BSWP		0x0004  	/* Byte Swap */

/* BCR 2 */

#define BCR2_AUTO_SELECT	0x0002	/* auto select port type 10BT/AUI */
    
/* BCR 20 software style register */
    
#define BCR20_SSIZE32		0x0100
#define BCR20_SWSTYLE_LANCE	0x0000
#define BCR20_SWSTYLE_ILACC	0x0001
#define BCR20_SWSTYLE_PCNET	0x0002
    
/* Control block definitions for AMD LANCE (Ethernet) chip. */

typedef struct
    {
    ULONG       rdp;            /* register data Port */
    ULONG       rap;            /* Register Address Port */
    ULONG       rst;            /* Reset Register */
    ULONG       bdp;            /* Bus Configuration Register Data Port */
    } LN_DEVICE;

/* Initialization Block */

typedef struct lnIB
    {
    ULONG       lnIBMode;	/* mode register */
    UCHAR       lnIBPadr [8];   /* PADR: bswapd ethnt phys address */
    UCHAR       lnIBLadrf [8];  /* logical address filter */
    ULONG       lnIBRdra;       /* RDRA: read ring address, long word */
    ULONG       lnIBTdra;       /* TDRA: transmit ring address long word */
    } LN_IB;

/* Receive Message Descriptor Entry.*/

typedef struct lnRMD
    {
    ULONG       lnRMD0;         /* bits 31:00 of receive buffer address */
    ULONG       lnRMD1;         /* status & buffer byte count (negative) */
    ULONG       lnRMD2;         /* message byte count */
    ULONG       lnRMD3;         /* reserved */
    } LN_RMD;

/* Transmit Message Descriptor Entry. */

typedef struct lnTMD
    {
    ULONG       lnTMD0;         /* bits 31:00 of transmit buffer address */
    ULONG       lnTMD1;         /* message byte count */
    ULONG       lnTMD2;         /* errors */
    ULONG       lnTMD3;         /* reserved */
    } LN_TMD;

/* initialization block */

#define IB_MODE_TLEN_MSK	0xf0000000
#define IB_MODE_RLEN_MSK	0x00f00000

/* receive descriptor */

#define RMD1_OWN		0x80000000	/* Own */
#define RMD1_ERR		0x40000000	/* Error */
#define RMD1_FRAM		0x20000000	/* Framming Error */
#define RMD1_OFLO		0x10000000	/* Overflow */
#define RMD1_CRC		0x08000000	/* CRC */
#define RMD1_BUFF		0x04000000	/* Buffer Error */
#define RMD1_STP		0x02000000	/* Start of Packet */
#define RMD1_ENP		0x01000000	/* End of Packet */
#define RMD1_RES		0x00ff0000	/* reserved */
#define RMD1_CNST		0x0000f000	/* rmd1 constant value */

#define RMD1_BCNT_MSK		0x00000fff	/* buffer cnt mask */
#define RMD2_MCNT_MSK		0x00000fff	/* message buffer cnt mask */

/* transmit descriptor */

#define TMD1_OWN		0x80000000	/* Own */
#define TMD1_ERR		0x40000000	/* Error */
#define TMD1_MORE		0x10000000	/* More than One Retry */
#define TMD1_ONE		0x08000000	/* One Retry */
#define TMD1_DEF		0x04000000	/* Deferred */
#define TMD1_STP		0x02000000	/* Start of Packet */
#define TMD1_ENP		0x01000000	/* End of Packet */
#define TMD1_RES		0x00FF0000	/* High Address */
#define TMD1_CNST		0x0000f000	/* tmd1 constant value */

#define TMD2_BUFF		0x80000000	/* Buffer Error */
#define TMD2_UFLO		0x40000000	/* Underflow Error */
#define TMD2_LCOL		0x10000000	/* Late Collision */
#define TMD2_LCAR		0x08000000	/* Lost Carrier */
#define TMD2_RTRY		0x04000000	/* Retry Error */
#define TMD2_TDR		0x03FF0000	/* Time Domain Reflectometry */

#define TMD1_BCNT_MSK		0x00000fff	/* buffer cnt mask */

#define	rBufAddr		lnRMD0
#define	rBufRmd1		lnRMD1
#define	rBufMskCnt		lnRMD2
#define rBufRmd3		lnRMD3

#define	tBufAddr		lnTMD0
#define tBufTmd1		lnTMD1
#define tBufTmd2		lnTMD2
#define tBufTmd3		lnTMD3

/* CRC for logical address filter */

#define LN_CRC_POLYNOMIAL	0xedb88320	/* CRC polynomial */
#define LN_CRC_TO_LAF_IX(crc)	((crc) >> 26)	/* get 6 MSBits */
#define LN_LAF_LEN		8    		/* logical addr filter legth */
#define LN_LA_LEN		6		/* logical address length */

/* Definitions for the drvCtrl specific flags field */

#define LS_PROMISCUOUS_FLAG     0x1	/* set the promiscuous mode */
#define LS_MEM_ALLOC_FLAG       0x2	/* allocating memory flag */
#define LS_PAD_USED_FLAG        0x4	/* padding used flag */
#define LS_RCV_HANDLING_FLAG    0x8	/* handling recv packet */
#define LS_START_OUTPUT_FLAG    0x10	/* trigger output flag */
#define LS_POLLING              0x20	/* polling flag */
#define LS_MODE_MEM_IO_MAP	0x100   /* device registers memory mapped */
#define LS_MODE_DWIO		0x200   /* device in 32 bit mode */

/* descriptor size */

#define RMD_SIZ 		sizeof(LN_RMD)
#define TMD_SIZ 		sizeof(LN_TMD)
#define IB_SIZ  		sizeof(LN_IB)

/* free routine hooked to the transmit descriptor */
    
typedef struct freeArgs
    {
    void * arg1;
    void * arg2;
    } FREE_ARGS;

/* The definition of the driver control structure */

typedef struct ln97xDevice
    {
    END_OBJ	endObj;			/* The class we inherit from */
    LN_IB *	ib;			/* ptr to Initialization Block */
    int 	unit;			/* unit number of the device */
    int         rmdIndex;               /* current RMD index */
    int         rringSize;              /* RMD ring size */
    int         rringLen;               /* RMD ring length (bytes) */
    LN_RMD *    pRring;                 /* RMD ring start */
    int         tmdIndex;               /* current TMD index */
    int         tmdIndexC;              /* current TMD index */
    int         tringSize;              /* TMD ring size */
    int         tringLen;               /* TMD ring length (bytes) */
    LN_TMD *    pTring;                 /* TMD ring start */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    UINT32 *	pRdp;                   /* device register CSR */
    UINT32 *	pRap;                   /* device register RAP */
    UINT32 *	pReset;                 /* device register Reset */
    UINT32 *	pBdp;                   /* device register BCR */
    UINT32      devAdrs;                /* device structure address */
    UINT16	csr3B;			/* csr3 value board specific */
    char *	pShMem;                 /* real ptr to shared memory */
    char *	memBase;                /* LANCE memory pool base */
    char *	memAdrs;                /* LANCE memory pool base */
    int         memSize;                /* LANCE memory pool size */
    int         memWidth;               /* width of data port */
    int         offset;			/* offset of data in the buffer */
    UINT32      flags;               	/* Our local flags */
    UINT32	pciMemBase;		/* memory base as seen from PCI*/
    UINT8	enetAddr[6];		/* ethernet address */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    BOOL        txBlocked;              /* transmit flow control */
    BOOL        txCleaning;		/* transmit descriptor cleaning */
    FUNCPTR     freeRtn [128];          /* Array of free routines. */
    FREE_ARGS   freeData [128];		/* Array of free arguments */
    CL_POOL_ID  pClPoolId;		/* cluster pool Id */
    M_CL_CONFIG	mClCfg;			/* mBlk & cluster config structure */
    CL_DESC	clDesc;			/* cluster descriptor table */
    END_ERR     lastError;              /* Last error passed to muxError */
    } LN_97X_DRV_CTRL;

/* Configuration items */

#define BUS_LATENCY_COUNT	0x1000	/* Max BUS timeo len in 0.1 uSeconds */
#define LN_MIN_FBUF    		100     /* Minsize of first buffer in chain */
#define LN_BUFSIZ		(ETHERMTU + ENET_HDR_REAL_SIZ + 6)
#define LN_SPEED        	10000000
#define LN_RMD_RLEN     	5       /* ring size as a ^ 2 -- 32 RMD's */
#define LN_TMD_TLEN     	5      	/* ring size as a ^ 2 -- 32 TMD's */
#define LN_RMD_MIN		2    	/* min descriptors 4 */
#define LN_TMD_MIN		2    	/* min descriptors 4 */
#define LN_RMD_MAX		7    	/* max descriptors 128 */
#define LN_TMD_MAX		7    	/* max descriptors 128 */
#define LN_97X_DEV_NAME 	"lnPci"	/* name of the device */
#define LN_97X_DEV_NAME_LEN 	6	/* length of the name string */
#define LN_97X_APROM_SIZE    	32	/* hardware address prom size */

/*
 * Generic shared memory read and write macros.
 */

#ifndef SHMEM_RD
#define SHMEM_RD(x)		*(x)
#endif

#ifndef SHMEM_WR
#define SHMEM_WR(x,y)		(*(x) = y)
#endif

#define LN_RMD_OWNED(rmd) 	(rmd->lnRMD1 & lnrmd1_OWN)

#define LN_PKT_LEN_GET(rmd) 	PCI_SWAP (rmd->rBufMskCnt) - 4;

/* device macros to read/write descriptor, initialization blocks, etc. */

#define LN_RMD_BUF_TO_ADDR(rmd, tmp, buf) \
	tmp = (void *)LN_CACHE_VIRT_TO_PHYS ((UINT32)buf); \
	tmp = (void *)MEM_TO_PCI_PHYS (tmp); \
	rmd->rBufAddr = (UINT32)PCI_SWAP (tmp);

#define LN_TMD_BUF_TO_ADDR(tmd, tmp, buf) \
	tmp = (void *)LN_CACHE_VIRT_TO_PHYS ((UINT32)buf); \
	tmp = (void *)MEM_TO_PCI_PHYS (tmp); \
        DRV_LOG (DRV_DEBUG_LOAD, "tBufAddr = 0x%X\n", tmp, 2, 3, 4, 5, 6); \
	tmd->tBufAddr = (ULONG)PCI_SWAP (tmp);

#define LN_TMD_TO_ADDR(tmd, addr) \
	{	\
	UINT32 pTemp = PCI_TO_MEM_PHYS (PCI_SWAP (tmd->rBufAddr)); \
        addr = LN_CACHE_PHYS_TO_VIRT(pTemp);		\
	}

#define LN_RMD_TO_ADDR(rmd, addr) \
	{	\
	UINT32 pTemp = PCI_TO_MEM_PHYS (PCI_SWAP (rmd->rBufAddr)); \
        addr = (char *)(LN_CACHE_PHYS_TO_VIRT(pTemp));		\
	}

#define LN_ADDR_TO_TMD(addr, tmd) \
	{	\
	UINT32 pTemp = LN_CACHE_VIRT_TO_PHYS (buf);/* convert to phys addr */ \
        pTemp = (void *)(MEM_TO_PCI_PHYS((UINT32)pTemp));		\
	tmd->tBufAddr = PCI_SWAP (addr); \
	}

#define LN_ADDR_TO_RMD(addr, rmd) \
	{ \
	UINT32 pTemp = LN_CACHE_VIRT_TO_PHYS (buf);/* convert to phys addr */ \
        pTemp = (void *)(MEM_TO_PCI_PHYS((UINT32)pTemp));		\
	rmd->rBufAddr = PCI_SWAP (addr); \
	}

#define LN_CLEAN_TXD(tmd) \
	{ \
	UINT32 temp = TMD1_CNST | TMD1_ENP | TMD1_STP;	\
	tmd->tBufTmd2 = 0; 				\
        tmd->rbuf_tmd1 = PCI_SWAP (temp);		\
	}

#define LN_CLEAN_RXD(rmd) \
	{ \
	UINT32 temp;							\
	rmd->rBufMskCnt = 0; 						\
	temp = (RMD1_BCNT_MSK & -(LN_BUFSIZ)) | RMD1_OWN | RMD1_CNST | 	\
                                                RMD1_STP | RMD1_ENP;   	\
        rmd->rBufRmd1 = PCI_SWAP (temp);		               	\
	}

/* Set address of receiver descriptor ring and size in init block */

#define LN_ADDR_TO_IB_RMD(addr, ib, rsize) \
	{ \
        /* convert to phys addr */ \
	UINT32 pTemp = (UINT32)LN_CACHE_VIRT_TO_PHYS (addr);	\
        pTemp = (UINT32)MEM_TO_PCI_PHYS (pTemp);		\
	ib->lnIBRdra = PCI_SWAP (pTemp);			\
	pTemp = PCI_SWAP (ib->lnIBMode);			\
	pTemp |= IB_MODE_RLEN_MSK & (rsize << 20);		\
	ib->lnIBMode = PCI_SWAP (pTemp);			\
        DRV_LOG (DRV_DEBUG_LOAD, "IBmode = 0x%X Rdra = 0x%X\n", \
                 ib->lnIBMode, ib->lnIBRdra, 3, 4, 5, 6);       \
	}
	
/* Set address of transmitter descriptor ring and size in init block */

#define LN_ADDR_TO_IB_TMD(addr, ib, tsize) \
	{							\
        /* convert to phys addr */ 				\
        UINT32 pTemp = (UINT32)LN_CACHE_VIRT_TO_PHYS (addr);	\
        pTemp = (UINT32)MEM_TO_PCI_PHYS (pTemp);		\
	ib->lnIBTdra = PCI_SWAP (pTemp);			\
	pTemp = PCI_SWAP (ib->lnIBMode);			\
	pTemp |= IB_MODE_TLEN_MSK & (tsize << 28); 		\
	ib->lnIBMode = PCI_SWAP (pTemp);			\
        DRV_LOG (DRV_DEBUG_LOAD, "IBmode = 0x%X Tdra = 0x%X\n", \
                 ib->lnIBMode, ib->lnIBTdra, 3, 4, 5, 6); 	\
	}

/* clear the logical address filter */
    
#define LN_ADDRF_CLEAR(pIb) (bzero(&(pIb)->lnIBLadrf[0], 8))

/* set the logical address filter to accept a new multicast address */
    
#define LN_ADDRF_SET(pIb, crc) \
    (((pIb)->lnIBLadrf[((crc) & 0x0000003f) >> 3]) |= (1 << ((crc) & 0x7)))

#define LN_RMD_ERR(rmd) \
	(rmd->lnRMD1 & RMD1_ERR) || 		\
         ((rmd->lnRMD1 & (RMD1_STP | RMD1_ENP)) != (RMD1_STP | RMD1_ENP))

#define LN_TMD_CLR_ERR(tmd) \
        {		    \
	tmd->tBufTmd1 = 0; \
	tmd->tBufTmd2 = 0; \
        }

/* board level/bus specific and architecture specific macros */
    
#if _BYTE_ORDER==_BIG_ENDIAN
#   define PCI_SWAP(x)	LONGSWAP((int)(x))
#else
#   define PCI_SWAP(x)	(x)
#endif

#if (CPU_FAMILY==I80X86)

#ifndef SYS_OUT_LONG    
#define SYS_OUT_LONG(pDrvCtrl,addr,value) \
    { \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP) 	\
        *((ULONG *)(addr)) = (value); 		\
    else 					\
        sysOutLong((int)(addr), (value)); 	\
    }
#endif /* SYS_OUT_LONG */

#ifndef SYS_IN_LONG    
#define SYS_IN_LONG(pDrvCtrl, addr, data) \
    { \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP) 	\
        ((data) = *((ULONG *)(addr))); 		\
    else 					\
        ((data) = sysInLong((int) (addr))); 	\
    }
#endif /* SYS_IN_LONG */

#ifndef SYS_OUT_SHORT
#define SYS_OUT_SHORT(pDrvCtrl,addr,value) \
    { \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP) 	\
        *((USHORT *)(addr)) = (value); 		\
    else 					\
        sysOutWord((int)(addr), (value)); 	\
    }
#endif /* SYS_OUT_SHORT */

#ifndef SYS_IN_SHORT
#define SYS_IN_SHORT(pDrvCtrl, addr, data) \
    { \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP) 	\
        ((data) = *((USHORT *)(addr)));		\
    else 					\
        ((data) = sysInWord((int) (addr))); 	\
    }
#endif /* SYS_IN_SHORT */

#ifndef SYS_OUT_BYTE
#define SYS_OUT_BYTE(pDrvCtrl,addr,value) \
    { \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP) 	\
        *((UCHAR *)(addr)) = (value); 		\
    else 					\
        sysOutByte((int)(addr), (value)); 	\
    }
#endif /* SYS_OUT_BYTE */

#ifndef SYS_IN_BYTE
#define SYS_IN_BYTE(pDrvCtrl, addr, data) \
    { \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP) 	\
        ((data) = *((UCHAR *)(addr))); 		\
    else 					\
        ((data) = sysInByte((int) (addr))); 	\
    }
#endif /* SYS_IN_BYTE */
#endif /* CPU_FAMILY == I80x86 */

#if defined(__STDC__) || defined(__cplusplus)
IMPORT END_OBJ * ln97xEndLoad (char * initString);
IMPORT STATUS 	 ln97xInitParse (LN_97X_DRV_CTRL * pDrvCtrl,char * initString);
#else
IMPORT END_OBJ * ln97xEndLoad ();
IMPORT STATUS	 ln97xInitParse ();
#endif  /* __STDC__ */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCln97xEndh */
