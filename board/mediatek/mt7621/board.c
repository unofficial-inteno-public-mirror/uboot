
#include <common.h>
#include "serial.h"
#include "rt_mmap.h"

#define RALINK_REG(x)		(*((volatile u32 *)(x)))

/* running in DRAM(flash) not relocated */
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

#define RT2880_PRGIO_ADDR       (RALINK_SYSCTL_BASE + 0x600) // Programmable I/O
#define RT2880_REG_PIOINT       (RT2880_PRGIO_ADDR + 0x90)
#define RT2880_REG_PIOEDGE      (RT2880_PRGIO_ADDR + 0xA0)
#define RT2880_REG_PIORENA      (RT2880_PRGIO_ADDR + 0x50)
#define RT2880_REG_PIOFENA      (RT2880_PRGIO_ADDR + 0x60)
#define RT2880_REG_PIODATA      (RT2880_PRGIO_ADDR + 0x20)
#define RT2880_REG_PIODIR       (RT2880_PRGIO_ADDR + 0x00)
#define RT2880_REG_PIOSET       (RT2880_PRGIO_ADDR + 0x30)
#define RT2880_REG_PIORESET     (RT2880_PRGIO_ADDR + 0x40)


void trigger_hw_reset(void)
{
#ifdef GPIO14_RESET_MODE
        //set GPIO14 as output to trigger hw reset circuit
        RALINK_REG(RT2880_REG_PIODIR)|=1<<14; //output mode

        RALINK_REG(RT2880_REG_PIODATA)|=1<<14; //pull high
	udelay(100);
        RALINK_REG(RT2880_REG_PIODATA)&=~(1<<14); //pull low
#endif
}

/* just for eth driver now. */
unsigned long mips_bus_feq;
unsigned long mips_cpu_feq;

/* running in DRAM and relocated */
int board_early_init_r( void )
{
	unsigned int reg;

	reg = RALINK_REG(RALINK_SYSCTL_BASE + 0x2C);
	if( reg & ((0x1UL) << 30)) {
		reg = RALINK_REG(RALINK_MEMCTRL_BASE + 0x648);
		mips_cpu_feq = (((reg >> 4) & 0x7F) + 1) * 1000 * 1000;
		reg = RALINK_REG(RALINK_SYSCTL_BASE + 0x10);
		reg = (reg >> 6) & 0x7;
		if(reg >= 6) { //25Mhz Xtal
			mips_cpu_feq = mips_cpu_feq * 25;
		} else if (reg >=3) { //40Mhz Xtal
			mips_cpu_feq = mips_cpu_feq * 20;
		} else { //20Mhz Xtal
			/* TODO */
		}
	}else {
		reg = RALINK_REG(RALINK_SYSCTL_BASE + 0x44);
		mips_cpu_feq = (500 * (reg & 0x1F) / ((reg >> 8) & 0x1F)) * 1000 * 1000;
	}
	mips_bus_feq = mips_cpu_feq/4;

#define RT2880_SYS_CNTL_BASE			(RALINK_SYSCTL_BASE)
#define RT2880_RSTSTAT_REG			(RT2880_SYS_CNTL_BASE+0x38)
#define RT2880_WDRST            (1<<1)
#define RT2880_SWSYSRST         (1<<2)
#define RT2880_SWCPURST         (1<<3)


	reg = RALINK_REG(RT2880_RSTSTAT_REG);
	if(reg & RT2880_WDRST ){
		printf("***********************\n");
		printf("Watchdog Reset Occurred\n");
		printf("***********************\n");
		RALINK_REG(RT2880_RSTSTAT_REG)|=RT2880_WDRST;
		RALINK_REG(RT2880_RSTSTAT_REG)&=~RT2880_WDRST;
		trigger_hw_reset();
	}else if(reg & RT2880_SWSYSRST){
		printf("******************************\n");
		printf("Software System Reset Occurred\n");
		printf("******************************\n");
		RALINK_REG(RT2880_RSTSTAT_REG)|=RT2880_SWSYSRST;
		RALINK_REG(RT2880_RSTSTAT_REG)&=~RT2880_SWSYSRST;
		trigger_hw_reset();
	}else if (reg & RT2880_SWCPURST){
		printf("***************************\n");
		printf("Software CPU Reset Occurred\n");
		printf("***************************\n");
		RALINK_REG(RT2880_RSTSTAT_REG)|=RT2880_SWCPURST;
		RALINK_REG(RT2880_RSTSTAT_REG)&=~RT2880_SWCPURST;
		trigger_hw_reset();
	}


	return 0;

}

phys_size_t initdram(int board_type)
{
	return CONFIG_SYS_MEM_SIZE;
}

#ifdef  CONFIG_CMD_NET

int rt2880_eth_initialize(bd_t *bis);
void setup_internal_gsw( void );
void LANWANPartition(void);

int board_eth_init(bd_t *bis)
{
	//enable MDIO
	RALINK_REG(0xbe000060) &= ~(1 << 12); //set MDIO to Normal mode
	RALINK_REG(0xbe000060) &= ~(1 << 14); //set RGMII1 to Normal mode
	RALINK_REG(0xbe000060) &= ~(1 << 15); //set RGMII2 to Normal mode
	setup_internal_gsw();
	LANWANPartition();

	return rt2880_eth_initialize(bis);
}
#endif
