
#include <common.h>
#include "serial.h"
#include "rt_mmap.h"

#define RALINK_REG(x)		(*((volatile u32 *)(x)))


int board_early_init_f(void)
{
	u32 value;
	u32 fdiv = 0, frac = 0, i;

	value = le32_to_cpu(*(volatile u_long *)(RALINK_SPI_BASE + 0x3c));
	value &= ~(0xFFF);
	value |= 5; //work-around 3-wire SPI issue (3 for RFB, 5 for EVB)
	*(volatile u_long *)(RALINK_SPI_BASE + 0x3c) = cpu_to_le32(value);

	value = RALINK_REG(RALINK_CUR_CLK_STS_REG);
	fdiv = ((value>>8)&0x1F);
	frac = (unsigned long)(value&0x1F);

	i = 0;

	while(frac < fdiv) {
		value = RALINK_REG(RALINK_DYN_CFG0_REG);
		fdiv = ((value>>8)&0x0F);
		fdiv--;
		value &= ~(0x0F<<8);
		value |= (fdiv<<8);
		RALINK_REG(RALINK_DYN_CFG0_REG) = value;
		udelay(500);
		i++;
		value = RALINK_REG(RALINK_CUR_CLK_STS_REG);
		fdiv = ((value>>8)&0x1F);
		frac = (unsigned long)(value&0x1F);
	}

 	//change CPLL from GPLL to MEMPLL
	value = RALINK_REG(RALINK_CLKCFG0_REG);
	value &= ~(0x3<<30);
	value |= (0x1<<30);
	RALINK_REG(RALINK_CLKCFG0_REG) = value;

	return 0;
}


phys_size_t initdram(int board_type)
{
	return CONFIG_SYS_MEM_SIZE;
}

#ifdef  CONFIG_CMD_NET

/*
MAC_TO_MT7530_MODE needs to be set

GPIOx_RESET_MODE is set in old uboot but is a nop in original code for mt7621 board. 
*/


int rt2880_eth_initialize(bd_t *bis);
/* just for eth driver now. */
unsigned long mips_bus_feq;
unsigned long mips_cpu_feq;

int board_eth_init(bd_t *bis)
{
	mips_cpu_feq = 50 * 1000 *1000;
        mips_bus_feq = mips_cpu_feq/4;
	return rt2880_eth_initialize(bis);
}
#endif
