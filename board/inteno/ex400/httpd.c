/* we use the LWIP headers and then we need to prevent some defines from uboot net.h to take effect.
   but we still need lots of stuff from uboot env.
*/
#define LWIP

/* uboot headers */
#include <common.h>
#include <console.h>

/* lwip headers */
#include <lwip/ip_addr.h>
#include <lwip/init.h>
#include <lwip/err.h>
#include <lwip/netif.h>
#include <lwip/tcp_impl.h>
#include <lwip/netif/etharp.h>
#include <lwip/tcpip.h>


void lwip_loop( void);
err_t lwip_start( void );
void lwip_stop( void );
void lwip_break( void );

void httpd_init(void);

static int do_httpd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
        struct netif *netif_ret;

        static int init_done=0;

        if (argc < 1)
                return CMD_RET_USAGE;

        lwip_start();

        httpd_init();

        lwip_loop();
        lwip_stop();

        return 0;
}


U_BOOT_CMD(
        httpd,   4,      1,      do_httpd,
        "A rescue http server",
        "Used to allow a user to upload a system with a http browser"
        "exit with ctrl-c"
        );
