#include <common.h>
#include "led.h"
#include "rt_mmap.h"

#define RT2880_PRGIO_ADDR       (RALINK_SYSCTL_BASE + 0x600) // Programmable I/O
#define RT2880_REG_PIODIR       (RT2880_PRGIO_ADDR + 0x00)
#define RT2880_REG_PIODATA      (RT2880_PRGIO_ADDR + 0x20)


void ex400_init_leds( void )
{
        /* set led gpio to output */
        RALINK_REG(RT2880_REG_PIODIR)|= (1<<LED_STATUS_G);
        RALINK_REG(RT2880_REG_PIODIR)|= (1<<LED_STATUS_R);
        RALINK_REG(RT2880_REG_PIODIR)|= (1<<LED_WPS);

        /* set led status_g on and rest to off */
        set_led(LED_STATUS_G, LED_STATE_ON);
        set_led(LED_STATUS_R, LED_STATE_ON);
        set_led(LED_WPS, LED_STATE_OFF);
}

/* fucked up red status has inverted logic for some reason */
void set_led(int led, int state)
{

        if (led == LED_STATUS_R) {
                if (state) {
                        RALINK_REG(RT2880_REG_PIODATA) |= (1<<led);
                }else {
                        RALINK_REG(RT2880_REG_PIODATA) &= ~(1<<led);
                }
        }else {
                if (state) {
                        RALINK_REG(RT2880_REG_PIODATA) &= ~(1<<led);
                }else {
                        RALINK_REG(RT2880_REG_PIODATA) |= (1<<led);
                }
        }
}

static int do_led(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        int led=0,state=0;

        if (argc != 3)
                return 1;

        if (!strcmp("status_g",argv[1]))
                led = LED_STATUS_G;
        else if (!strcmp("status_r",argv[1]))
                led = LED_STATUS_R;
        else if (!strcmp("wps",argv[1]))
                led = LED_WPS;

        if (!strcmp("on",argv[2]))
                state = LED_STATE_ON;

        if (led) {
                printf("led = %x state %d\n",led, state);
                set_led(led,state);
        }else
                printf("no or wrong led name was used\n");

        return 0;
}

U_BOOT_CMD(
        led,   4,      1,      do_led,
        "Set led to on off.",
        "\n"
        "led [name] <on|off>\n"
        "name = status_g,status_r,wps\n"
        );
