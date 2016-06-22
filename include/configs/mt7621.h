/*
 * Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef _MT7621_CONFIG_H
#define _MT7621_CONFIG_H
#define DEBUG
/*
 * System configuration
 */
#define CONFIG_MT7621

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MEMSIZE_IN_BYTES

#define CONFIG_PCNET
#define CONFIG_PCNET_79C973
#define PCNET_HAS_PROM

#define CONFIG_SYS_ISA_IO_BASE_ADDRESS	0

#define MT7621_ASIC_BOARD


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
#define CONFIG_SYS_MEM_SIZE		(256 * 1024 * 1024)

#define CONFIG_SYS_INIT_SP_OFFSET	0x400000

#define CONFIG_SYS_LOAD_ADDR		0x81000000
#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

#define CONFIG_SYS_MALLOC_LEN		(128 * 1024)
#define CONFIG_SYS_BOOTPARAMS_LEN	(128 * 1024)
#define CONFIG_SYS_BOOTM_LEN		(64 * 1024 * 1024)

#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					 sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16

#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

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
#define CONFIG_SYS_FLASH_BASE		0xbfc00000
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	128
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE

/*
 * Environment
 */
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE		0x20000
#define CONFIG_ENV_SIZE			CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_ADDR \
	(CONFIG_SYS_FLASH_BASE + (4 << 20) - CONFIG_ENV_SIZE)


/*
 * Commands
 */

#define CONFIG_SYS_LONGHELP		/* verbose help, undef to save memory */

#endif /* _MALTA_CONFIG_H */
