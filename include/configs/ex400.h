/*
 *
 */

#ifndef _MT7621_CONFIG_H
#define _MT7621_CONFIG_H
/*#define DEBUG
#define CONFIG_MTD_DEBUG
#define CONFIG_MTD_DEBUG_VERBOSE 1

#define TRACE32 */

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
/*#define CONFIG_RT2880_ETH
  #define CONFIG_DM_ETH */
#define MT7621_USE_GE1
#define MT7621_ASIC_BOARD
#define MAC_TO_MT7530_MODE
#define PDMA_NEW
#define RALINK_MDIO_ACCESS_FUN
#define RALINK_EPHY_INIT
#define RX_SCATTER_GATTER_DMA
#define CONFIG_SYS_RX_ETH_BUFFER 60 /* code assumes 24+24 plus some extra I guess. */

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_EARLY_INIT_R

#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_LZMA

#define CONFIG_SYS_ISA_IO_BASE_ADDRESS	0

/*
 * CPU Configuration
 */
#define CONFIG_SYS_MHZ			880/2
#define CONFIG_SYS_MIPS_TIMER_FREQ	(CONFIG_SYS_MHZ * 1000000)

/*
 * Memory map
 */
/*#define CONFIG_SYS_TEXT_BASE		0xbfc00000 */ /* Rom version */
#define CONFIG_SYS_TEXT_BASE		0xA0200000 /* DRAM version */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_SDRAM_BASE		0x80000000 /* Cached addr */
/*#define CONFIG_SYS_MEM_SIZE		(256 * 1024 * 1024) */
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


/*
 * Serial driver
 */
#define CONFIG_BAUDRATE			57600
#define SERIAL_CLOCK_DIVISOR 16


/* #include "../rt_mmap.h" */
/* KEN:BUG  can not be included here but we need the addresses... what to do ??? */
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

   u-boot	0  -> 1M
   UBI		1M -> reest of memory
*/

/* #include "../rt_mmap.h" */
/* KEN:BUG  can not be included here but we need the addresses... what to do ??? */
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

#else
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE			(1024*10) /*can actually be 128kB */
#endif

/*#define CONFIG_BOOTDELAY      3      */ /* autoboot after 3 seconds     */
#define CONFIG_CMD_UBI
#define CONFIG_UBI_SILENCE_MSG
#define CONFIG_RBTREE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE

#define NANDID                  "MT7621-NAND"
#define MTDIDS_DEFAULT          "nand0=" NANDID ""
#define MTDPARTS_DEFAULT        "mtdparts=" NANDID ":1M(uboot),-(ubi)"

#define CONFIG_CMD_UBIFS
#define CONFIG_LZO
#define CONFIG_BOOTCOMMAND "run boot_ubi"

#define CONFIG_EXTRA_ENV_SETTINGS               \
        "ethaddr=00:AA:BB:CC:DD:10\0"           \
        "ipaddr=192.168.1.1\0"                  \
        "serverip=192.168.1.2\0"                \
        "mtdparts=" MTDPARTS_DEFAULT "\0"       \
        "loadaddr=0x85000000\0"                 \
        "fdtaddr=0x84000000\0" \
        "root_vol=rootfs_0\0"   \
        "update_uboot=" \
                "if tftpboot ${loadaddr} uboot.img; then " \
                        "nand erase.part uboot ;" \
                        "nand write ${loadaddr} uboot $filesize;" \
                "fi;\0" \
        "update_root0=" \
                "if tftpboot ${loadaddr} root.ubifs; then " \
                        "ubi write ${loadaddr} rootfs_0 $filesize;" \
                        "setenv root_vol rootfs_0;" \
                "fi;\0" \
        "boot_ram="\
                "run bootargs_ram;" \
                "tftpboot ${loadaddr} initramfs-kernel.bin;"\
                "tftpboot ${fdtaddr}  dtb;"        \
                "bootm ${loadaddr} - ${fdtaddr}\0" \
        "boot_ubi="\
                "run bootargs_ubi;" \
                "ubifsmount ubi0:${root_vol};"\
                "ubifsload ${loadaddr} /boot/uImage;"\
                "ubifsload ${fdtaddr} /boot/dtb;"\
                "bootm ${loadaddr} - ${fdtaddr}\0" \
        "bootargs_ubi=setenv bootargs " \
                "${extra} console=ttyS0,${baudrate} root=ubi0:${root_vol} ubi.mtd=ubi rootfstype=ubifs ${mtdparts}\0" \
        "bootargs_ram=setenv bootargs " \
                "${extra} console=ttyS0,${baudrate} ${mtdparts}\0" \
        "format_nand1="\
                "nand erase.part ubi; reset\0" \
        "format_nand2="\
	"ubi create env1 19000;" \
	"ubi create env2 19000;" \
	"ubi create rootfs_0 2800000;" \
	"saveenv\0"




/*
 * Commands
 */

#define CONFIG_SYS_LONGHELP		/* verbose help, undef to save memory */

#endif /* */
