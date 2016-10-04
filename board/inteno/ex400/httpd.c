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



u32_t sys_now(void);
void lwip_periodic_handle( void );

/* from net.h but its hard to include that one. */
void net_init(void);
int eth_init(void);
void net_init_loop(void);
int eth_rx(void);
void eth_halt(void);

void httpd_init(void);

struct netif netif;

u32_t sys_now(void)
{
        printf("%s not implemented\n",__func__);
        return 0;
}


void lwip_new_packet( void * bufptr, int len)
{
        struct pbuf *p, *q;
        struct eth_hdr *ethhdr;

        /* We allocate a pbuf chain of pbufs from the pool. */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

        if(p != NULL) {
                /* We iterate over the pbuf chain until we have read the entire
                   packet into the pbuf. */
                for(q = p; q != NULL; q = q->next) {
                        /* Read enough bytes to fill this pbuf in the chain. The
                           available data in the pbuf is given by the q->len
                           variable. */
                        /* read data into(q->payload, q->len); */
                        memcpy(q->payload, bufptr, q->len);
                        bufptr += q->len;
                }
                /* acknowledge that packet has been read(); */

                ethhdr = (struct eth_hdr *)p->payload;

                switch(htons(ethhdr->type)) {
                        /* IP or ARP packet? */
                case ETHTYPE_IP:
                case ETHTYPE_ARP:
                        /* full packet send to tcpip_thread to process */
                        if (netif.input(p, &netif) != ERR_OK) {
                                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                                p = NULL;
                        }
                        break;
                default:
                        pbuf_free(p);
                        break;
                }

        } else {
                /* drop packet(); */
                pbuf_free(p);
        }
        return;
}

int eth_send(void *packet, int length);
char tx_buf[2000];

/* due to resuing the uboot driver we cant send the pbuf directly to the driver,
   the data needs to be in one buffer so cpy to a tempporary buffer.
*/
static err_t uboot_net_output(struct netif * netif, struct pbuf *p)
{
        struct pbuf *q;
        char *p_buf;
        int len;

        p_buf = tx_buf;
        len = 0;

        for(q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
                memcpy(p_buf,q->payload, q->len);
                p_buf += q->len;
                len += q->len;
        }

        eth_send(tx_buf, len);

        return 0;
}

void hexdump(char *buf, int len){
        int i;

        printf("\n");
        for (i=0;i<len;i++){
                printf("%02x:", (unsigned char)buf[i]);
        }
        printf("\n");
}

#define ETHERNET_MTU 1500

extern u8 net_ethaddr[6];
err_t
uboot_net_init(struct netif *netif)
{

        netif->output = etharp_output;
        netif->linkoutput = uboot_net_output;
        netif->mtu = 1500;
        /* hardware address length */
        netif->hwaddr_len = 6;

//        printf("dst %x, src [%x] [%x]\n",(int)&netif->hwaddr, (int)net_ethaddr, (int)&net_ethaddr[0]);
//        hexdump((char*)&netif->hwaddr,6);
//        hexdump((char*)&net_ethaddr[0],6);
        memcpy(&netif->hwaddr, net_ethaddr, 6);

        netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
        return 0;
}

err_t
uboot_net_input(struct pbuf *p,struct netif *netif)
{
        printf("%s\n",__func__);

        return ERR_OK;
}

void lwip_periodic_handle( void )
{
	static ulong arp_start = 0;
        static ulong tcp_start = 0;

        /* tcp timer */
        if (get_timer(tcp_start) >= TCP_TMR_INTERVAL)
        {
//                printf("tcp_tmr()\n");
                tcp_tmr();
                tcp_start = get_timer(0);
        }

        /* arp timer */
        if (get_timer(arp_start) >= ARP_TMR_INTERVAL)
        {
//                printf("etharp_tmr()\n");
                etharp_tmr();
                arp_start = get_timer(0);
        }
}

extern int lwip_redirect;

static int do_httpd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
        struct netif *netif_ret;

        static int init_done=0;

        if (argc < 1)
                return CMD_RET_USAGE;

	net_init();
        eth_init();
	net_init_loop();

        if (!init_done){

                IP4_ADDR(&ipaddr, 192, 168, 1, 3);
                IP4_ADDR(&gw, 192, 168, 1, 1);
                IP4_ADDR(&netmask, 255, 255, 255, 0);
                lwip_init();

                netif.name[0] = 'e';
                netif.name[1] = '0';
                netif.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

                netif_ret = netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &uboot_net_init, ethernet_input);

                if (netif_ret == 0){
                        printf("Error running netif_add() \n");
                        return 1;
                }

                netif_set_default(&netif);
                netif_set_up(&netif);

                init_done = 1;
//                printf("netif_add() OK!\n");
        }

        lwip_redirect = 1;
        /* KEN: TODO: init uboot net so eth works */
        httpd_init();
        
        while(1)
	{
                /* if something has been received process it */
		if(eth_rx() > 0)
		{
//                        static int loop;
//                        printf("-------------------------------------------------------------------------------\n");
//                        printf("loop %d\n",loop++);

//                        printf("\nPacket in rx buffer from eth\n");
                        lwip_periodic_handle();
//                        printf("\nafter lwip_periodic_handle()\n");
                }
                /* exit on ctrl-c */
		if(ctrlc()){
			printf("\nhttpd server aborted!\n\n");
                        eth_halt();
                        break;
		}
	}

        lwip_redirect = 0;
        return 0;
}


U_BOOT_CMD(
        httpd,   4,      1,      do_httpd,
        "A rescue http server",
        "Used to allow a user to upload a system with a http browser"
        "exit with ctrl-c"
        );
