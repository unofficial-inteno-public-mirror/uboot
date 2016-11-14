#ifndef __LED__
#define __LED__

#define LED_STATUS_G 8
#define LED_STATUS_R 11
#define LED_WPS      12

#define LED_STATE_ON  1
#define LED_STATE_OFF 0

void ex400_init_leds( void );
void set_led(int led, int state);

#endif
