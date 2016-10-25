#ifndef __LWIP_START__
#define __LWIP_START__

err_t lwip_loop( int);
err_t lwip_start( void );
void lwip_stop( void );
void lwip_break( int );

#endif
