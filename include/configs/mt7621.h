/*
 * Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef _MT7621_CONFIG_H
#define _MT7621_CONFIG_H
//#define DEBUG
//#define CONFIG_MTD_DEBUG
//#define CONFIG_MTD_DEBUG_VERBOSE 1

//#define TRACE32

/********************************************************************************/
/* hmmm  crap  from old uboot/include/configs/rt2888.h */

#define RALINK_REG(x)          (*((volatile u32 *)(x)))

#define RT2880_REGS_BASE                        0xA0000000
#define RT2880_SYS_CNTL_BASE                    (RALINK_SYSCTL_BASE)
#define RT2880_RSTCTRL_REG                      (RT2880_SYS_CNTL_BASE+0x34)

/********************************************************************************/

/*
 * System configuration
 */
#define CONFIG_MT7621

/* net stuff */
//#define CONFIG_RT2880_ETH
//#define CONFIG_DM_ETH
#define MT7621_USE_GE1
#define MT7621_ASIC_BOARD
#define MAC_TO_MT7530_MODE
#define PDMA_NEW
#define RALINK_MDIO_ACCESS_FUN
#define RALINK_EPHY_INIT
#define RX_SCATTER_GATTER_DMA
#define CONFIG_SYS_RX_ETH_BUFFER 60 /* code assumes 24+24 plus some extra I guess. */
#define GPIO14_RESET_MODE

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_EARLY_INIT_R

#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_LZMA
//#define CONFIG_PCNET
//#define CONFIG_PCNET_79C973
//#define PCNET_HAS_PROM

#define CONFIG_SYS_ISA_IO_BASE_ADDRESS	0

/*
 * CPU Configuration
 */
// KEN: BUG: timer wrong  runs at 440
//#define CONFIG_SYS_MHZ			880
#define CONFIG_SYS_MHZ			880/2
#define CONFIG_SYS_MIPS_TIMER_FREQ	(CONFIG_SYS_MHZ * 1000000)

/*
 * Memory map
 */
//#define CONFIG_SYS_TEXT_BASE		0xbfc00000 /* Rom version */
#define CONFIG_SYS_TEXT_BASE		0xA0200000 /* DRAM version */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_SDRAM_BASE		0x80000000 /* Cached addr */
//#define CONFIG_SYS_MEM_SIZE		(256 * 1024 * 1024)
#define CONFIG_SYS_MEM_SIZE		(128 * 1024 * 1024)

#define CONFIG_SYS_INIT_SP_OFFSET	0x400000

#define CONFIG_SYS_LOAD_ADDR		0x81000000
#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

#define CONFIG_SYS_MALLOC_LEN		(1024 * 1024)
#define CONFIG_SYS_BOOTPARAMS_LEN	(128 * 1024)
#define CONFIG_SYS_BOOTM_LEN		(64 * 1024 * 1024)

#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					 sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16

#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

/* device tree */
//#define CONFIG_MIPS_BOOT_FDT
//#define CONFIG_OF_LIBFDT
//#define CONFIG_OF_BOARD_SETUP
//#define CONFIG_OF_SYSTEM_SETUP
//#define CONFIG_FIT_SIGNATURE
//#define CONFIG_CMD_FDT

/*
 * Serial driver
 */
#define CONFIG_BAUDRATE			57600
#define SERIAL_CLOCK_DIVISOR 16


//#include "../rt_mmap.h"
// KEN:BUG  can not be included here but we need the addresses... what to do ???
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	-4			/* little endian 32 bit registers, clearly we need to have -4 in reg size */
#define CONFIG_SYS_NS16550_CLK		(50 * 1000 * 1000)	/* we have a 50 MHz base clock into the UART hardware */
#define CONFIG_SYS_NS16550_COM1		(0xBE000000 + 0xc00)
#define CONFIG_SYS_NS16550_COM2		(0xBE000000 + 0xd00)
#define CONFIG_SYS_NS16550_COM3		(0xBE000000 + 0xe00)
#define CONFIG_CONS_INDEX		1

/*
 * Flash configuration
 */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_MTK_MTD_NAND
#define CONFIG_CMD_NAND


#ifdef CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_SELF_INIT

/* memory layout

   u-boot	    0  -> 512kB         0        0x07ffff
   env		512KB  -> 896KB         0x80000  0x0dffff
   env		896kB  -> 1280KB        0xe0000  0X13ffff
   UBI		1280kB -> 128MB         0X140000

*/

//#include "../rt_mmap.h"
// KEN:BUG  can not be included here but we need the addresses... what to do ???
#define RALINK_NAND_CTRL_BASE            0xBE003000
#define CONFIG_SYS_NAND_BASE RALINK_NAND_CTRL_BASE



/*
 * Environment in UBI
 */
#define CONFIG_MTD_UBI_BEB_LIMIT 30 /* this is 30 blocks per 1024 -> 3%, its also a runtime value do it's not stored on disk need to do same change in kernel */
#define CONFIG_MTD_UBI_FASTMAP

#define CONFIG_ENV_IS_IN_UBI
#define CONFIG_ENV_UBI_PART "ubi"
#define CONFIG_ENV_UBI_VOLUME "env1"
#define CONFIG_ENV_UBI_VOLUME_REDUND "env2"
#define CONFIG_ENV_SIZE 126976 /* we have a full UBI LEB and that is 126976 bytes */

/* if in raw nand uncomment this */
/* #define CONFIG_SYS_NAND_BLOCK_SIZE      (128*1024) */

/* #define CONFIG_SYS_MAX_FLASH_SECT       128 */
/* #define CONFIG_SYS_MAX_FLASH_BANKS      1 */

/* #define CONFIG_ENV_SIZE			(1024*10) /\*can actually be 128kB *\/ */
/* #define CONFIG_ENV_RANGE		(CONFIG_SYS_NAND_BLOCK_SIZE * 3) */

/* #define CONFIG_ENV_OFFSET               (CONFIG_SYS_NAND_BLOCK_SIZE * 4) */
/* #define CONFIG_ENV_OFFSET_REDUND        (CONFIG_ENV_OFFSET + CONFIG_SYS_NAND_BLOCK_SIZE * 3) */



#else
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE			(1024*10) /*can actually be 128kB */
#endif

#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define MTDIDS_DEFAULT          "nand0=mt7621.nand"

#define MTDPARTS_DEFAULT        "mtdparts=mt7621.nand:1M(uboot),"     \
                                                "-(ubi)"
#define CONFIG_CMD_UBIFS
#define CONFIG_LZO


#define CONFIG_EXTRA_ENV_SETTINGS               \
        "autoload=no\0"                         \
        "bootdelay=3"                           \
        "bootargs=console=ttyS0,57600\0"        \
        "ethaddr=00:AA:BB:CC:DD:10\0"           \
        "ipaddr=192.168.1.1\0"                  \
        "serverip=192.168.1.2\0"                \
        "mtdparts=" MTDPARTS_DEFAULT "\0"


/*
 * Commands
 */

#define CONFIG_SYS_LONGHELP		/* verbose help, undef to save memory */

#endif /* _MALTA_CONFIG_H */
