
#include <common.h>
#include <version.h>

#include "serial.h"
#include "rt_mmap.h"
#include "led.h"

#define RALINK_REG(x)		(*((volatile u32 *)(x)))

/* this needs to be included in the image            */
/* needs to be formatted as one string one line      */
/* so we can use grep on the uuid and get everything */
char Inteno_version_string[]={"938f0820-2ffb-11e7-bbc9-2f21351ee6fb: {\"Major\":" CONFIG_INTENO_MAJOR ",\"Minor\":" CONFIG_INTENO_MINOR "}"};

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

	/* make sure DMA has access to dram */
	value = RALINK_REG(RALINK_DMA_ARB_BASE + 0xc);
	value &= ~(0x1);
	RALINK_REG(RALINK_DMA_ARB_BASE + 0xc) = value;


	return 0;
}

/* setup gpio pinmux */
static void gpiomode(void)
{
	//enable MDIO
	RALINK_REG(0xbe000060) &= ~(1 << 12); //set MDIO to Normal mode
	RALINK_REG(0xbe000060) &= ~(1 << 14); //set RGMII1 to Normal mode
	RALINK_REG(0xbe000060) &= ~(1 << 15); //set RGMII2 to Normal mode

	//enable gpio 18, factory default button
	RALINK_REG(0xbe000060) |= (1 << 8); //set WDT_MODE bit 8
	RALINK_REG(0xbe000060) &= ~(1 << 9); //set WDT_MODE bit 9

	// led status green, gpio 8 as gpio function
	RALINK_REG(0xbe000060) &= ~(3 << 3); //clear bit 3 & 4
	RALINK_REG(0xbe000060) |= (1 << 3); //set bit 3

	// led status red, gpio 11 as gpio function
	// led wps (green), gpio 12 as gpio function
	RALINK_REG(0xbe000060) &= ~(3 << 5); //clear bit 5 & 6
	RALINK_REG(0xbe000060) |= (1 << 5); //set bit 5



}

void version_check(void)
{
	char *s;
	s = getenv("uboot_version");
	if (s) {
		if (!strcmp(s, U_BOOT_VERSION_STRING))
			return;
	}
	setenv("uboot_version", U_BOOT_VERSION_STRING);
	setenv("uboot_inteno_version", Inteno_version_string);
	run_command("saveenv", 0);
}

void _machine_restart(void)
{
	void __iomem *reset_base;

	reset_base = (void __iomem *)CKSEG1ADDR(RT2880_RSTCTRL_REG);
	__raw_writel(0x1, reset_base);
	mdelay(1000);
}

void config_usb_mtk_xhci(void)
{
        u32     regValue;

        regValue = RALINK_REG(RALINK_SYSCTL_BASE + 0x10);
        regValue = (regValue >> 6) & 0x7;
        if(regValue >= 6) { //25Mhz Xtal
                printf("\nConfig XHCI 25M PLL \n");
                RALINK_REG(0xbe1d0784) = 0x20201a;
                RALINK_REG(0xbe1d0c20) = 0x80004;
                RALINK_REG(0xbe1d0c1c) = 0x18181819;
                RALINK_REG(0xbe1d0c24) = 0x18000000;
                RALINK_REG(0xbe1d0c38) = 0x25004a;
                RALINK_REG(0xbe1d0c40) = 0x48004a;
                RALINK_REG(0xbe1d0b24) = 0x190;
                RALINK_REG(0xbe1d0b10) = 0x1c000000;
                RALINK_REG(0xbe1d0b04) = 0x20000004;
                RALINK_REG(0xbe1d0b08) = 0xf203200;

                RALINK_REG(0xbe1d0b2c) = 0x1400028;
                //RALINK_REG(0xbe1d0a30) =;
                RALINK_REG(0xbe1d0a40) = 0xffff0001;
                RALINK_REG(0xbe1d0a44) = 0x60001;
        } else if (regValue >=3 ) { // 40 Mhz
                printf("\nConfig XHCI 40M PLL \n");
                RALINK_REG(0xbe1d0784) = 0x20201a;
                RALINK_REG(0xbe1d0c20) = 0x80104;
                RALINK_REG(0xbe1d0c1c) = 0x1818181e;
                RALINK_REG(0xbe1d0c24) = 0x1e400000;
                RALINK_REG(0xbe1d0c38) = 0x250073;
                RALINK_REG(0xbe1d0c40) = 0x71004a;
                RALINK_REG(0xbe1d0b24) = 0x140;
                RALINK_REG(0xbe1d0b10) = 0x23800000;
                RALINK_REG(0xbe1d0b04) = 0x20000005;
                RALINK_REG(0xbe1d0b08) = 0x12203200;

                RALINK_REG(0xbe1d0b2c) = 0x1400028;
                //RALINK_REG(0xbe1d0a30) =;
                RALINK_REG(0xbe1d0a40) = 0xffff0001;
                RALINK_REG(0xbe1d0a44) = 0x60001;
        } else { //20Mhz Xtal

                /* TODO */

        }
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


/*KEN: BUG: we should find some way to tell linux what happened.*/
/* here we remove the bit and that is probably not to smart */
	reg = RALINK_REG(RT2880_RSTSTAT_REG);
	if(reg & RT2880_WDRST ){
		printf("***********************\n");
		printf("Watchdog Reset Occurred\n");
		printf("***********************\n");
		RALINK_REG(RT2880_RSTSTAT_REG)|=RT2880_WDRST;
		RALINK_REG(RT2880_RSTSTAT_REG)&=~RT2880_WDRST;

	}else if(reg & RT2880_SWSYSRST){
		printf("******************************\n");
		printf("Software System Reset Occurred\n");
		printf("******************************\n");
		RALINK_REG(RT2880_RSTSTAT_REG)|=RT2880_SWSYSRST;
		RALINK_REG(RT2880_RSTSTAT_REG)&=~RT2880_SWSYSRST;

	}else if (reg & RT2880_SWCPURST){
		printf("***************************\n");
		printf("Software CPU Reset Occurred\n");
		printf("***************************\n");
		RALINK_REG(RT2880_RSTSTAT_REG)|=RT2880_SWCPURST;
		RALINK_REG(RT2880_RSTSTAT_REG)&=~RT2880_SWCPURST;
	}

	gpiomode();
	config_usb_mtk_xhci();
	ex400_init_leds();
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
	setup_internal_gsw();
	LANWANPartition();

	return rt2880_eth_initialize(bis);
}
#endif

int board_late_init(void)
{
	version_check();
	return 0;
}

/* move this to where the rest of the string function lives */
int strcspn(const char *s,const char *r){

	int i,k,counter=0;

	for(i=0;s[i]!='\0';i++){

		for(k=0;r[k]!='\0';k++){
			if(s[i]==r[k])
				break;
			else
				counter++;
		}
	}
	return counter;
}

#define RT2880_PRGIO_ADDR       (RALINK_SYSCTL_BASE + 0x600) // Programmable I/O
#define RT2880_REG_PIODIR       (RT2880_PRGIO_ADDR + 0x00)
#define RT2880_REG_PIODATA      (RT2880_PRGIO_ADDR + 0x20)

static int do_rescue(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

        //set GPIO18 as input, this is the factory default button
        RALINK_REG(RT2880_REG_PIODIR)&= ~(1<<18); //input mode

	/* logic inverted a 1 means the button is not pressed */
        if ( ! (RALINK_REG(RT2880_REG_PIODATA) & 1<<18) ){
		set_led(LED_WPS, LED_STATE_ON);
		printf("Entering rescue mode\n");
		run_command("httpd",0);
	}

        return 0;
}


U_BOOT_CMD(
        rescue,   1,      0,      do_rescue,
        "test if we should entere resque mode",
	"If the reset pin is active start the httpd rescue mode"
        "exit with ctrl-c"
        );
