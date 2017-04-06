#ifndef __MDDR_CONFIG_H
#define __MDDR_CONFIG_H

/*
 * This file contains the memory configuration parameters for the cygnus board.
 */
/*--------------------------------------------------------------------------------
 * MDDR info
 */
/* Chip Select */
//#define DDR_CS1EN 1 // CSEN : whether a ddr chip exists 0 - un-used, 1 - used
//#define DDR_CS0EN 1
//#define DDR_DW32 1/* 0 - 16-bit data width, 1 - 32-bit data width */

/* MDDR paramters */
#define DDR_ROW 13 /* ROW : 12 to 14 row address */
#define DDR_ROW1 13 /* ROW : 12 to 14 row address */
#ifdef CONFIG_MEM_SIZE_64M
#define DDR_COL 10/* COL :  8 to 10 column address */
#define DDR_COL1 10/* COL :  8 to 10 column address */
#else
#define DDR_COL 9/* COL :  8 to 10 column address */
#define DDR_COL1 9/* COL :  8 to 10 column address */
#endif

#define DDR_BANK8 0 /* Banks each chip: 0-4bank, 1-8bank 0 for falcon fpga, 1 for develop board */
#define DDR_CL 4 /* CAS latency: 1 to 7 */

/*
 * MDDR controller timing1 register
 */
#define DDR_tRAS 45 /*tRAS: ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP MAX(2,0) /* 7.5ns READ to PRECHARGE command period. */
#define DDR_tRP 18 /* tRP: PRECHARGE command period to the same bank */
#define DDR_tRCD 18 /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC (DDR_tRAS + DDR_tRP) /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD 12   /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR 15 /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR MAX(2,0) /* WRITE to READ command delay. */

/*
 * MDDR controller timing2 register
 */
#define DDR_tRFC 120 /* ns,  AUTO-REFRESH command period. */
#define DDR_tMINSR DDR_GET_VALUE(DDR_tRFC, tck_g.ps) /* Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP  5 /* EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD 2 /* unit: tCK Load-Mode-Register to next valid command period: 1 to 4 tCK */

/* new add */
#define DDR_BL	4   /* MDDR Burst length: 3 - 8 burst, 2 - 4 burst , 1 - 2 burst*/
#define DDR_tAL 0		/* Additive Latency, tCK*/
#define DDR_tRL (DDR_tAL + DDR_CL)	/* MDDR: Read Latency = tAL + tCL */
#define DDR_tWL 1		/* MDDR: must 1 */
#define DDR_tRDLAT	(DDR_tRL - 2)
#define DDR_tWDLAT	(DDR_tWL - 1)
#define DDR_tCCD 4		/* CAS# to CAS# command delay , tCK, MDDR no*/
#define DDR_tRTW (DDR_tRL + DDR_tCCD + 2 - DDR_tWL)	/* Read to Write delay */
#define DDR_tFAW 50		/* Four bank activate period, ns, MDDR no */
#define DDR_tCKE 3		/* CKE minimum pulse width, tCK */
#define DDR_tXS 200		/* Exit self-refresh to next valid command delay, ns */
#define DDR_tXSRD  DDR_GET_VALUE(DDR_tXS,tck_g.ps)		/* DDR2 only: Exit self refresh to a read command, tck */
#define DDR_tCKSRE MAX(1,0) /* Valid Clock Requirement after Self Refresh Entry or Power-Down Entry */

/*
 * MDDR controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: 4096 refresh cycles/64ms */

#define DDR_CLK_DIV 1    /* Clock Divider. auto refresh
			  * cnt_clk = memclk/(16*(2^DDR_CLK_DIV))
			  */

#endif /* __MDDR_CONFIG_H */


