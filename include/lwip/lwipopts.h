#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  1

/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#define MEM_LIBC_MALLOC         1

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT           4

/* KEN: FOR DEBUG, set to 0 normally */
#define MEMP_OVERFLOW_CHECK             1
#define MEMP_SANITY_CHECK               1


#define TCP_MSS                 (1500 - 40)
//#define LWIP_DHCP               1
#define LWIP_ICMP       1

#define LWIP_STATS              0
#define LWIP_PROVIDE_ERRNO      1

#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_COMPAT_MUTEX               1
#define LWIP_SO_RCVTIMEO                1

#if 0
#define LWIP_DEBUG                     1

#define LWIP_DBG_MIN_LEVEL 0
#define LWIP_COMPAT_SOCKETS 1
#define TAPIF_DEBUG LWIP_DBG_ON
#define TUNIF_DEBUG LWIP_DBG_OFF
#define UNIXIF_DEBUG LWIP_DBG_OFF
#define DELIF_DEBUG LWIP_DBG_OFF
#define SIO_FIFO_DEBUG LWIP_DBG_OFF
#define TCPDUMP_DEBUG LWIP_DBG_ON

#define PPP_DEBUG        LWIP_DBG_ON
#define MEM_DEBUG        LWIP_DBG_ON
#define MEMP_DEBUG       LWIP_DBG_ON
#define PBUF_DEBUG       LWIP_DBG_ON
#define API_LIB_DEBUG    LWIP_DBG_ON
#define API_MSG_DEBUG    LWIP_DBG_ON
#define TCPIP_DEBUG      LWIP_DBG_ON
#define NETIF_DEBUG      LWIP_DBG_ON
#define SOCKETS_DEBUG    LWIP_DBG_ON
#define DEMO_DEBUG       LWIP_DBG_ON
#define IP_DEBUG         LWIP_DBG_ON
#define IP_REASS_DEBUG   LWIP_DBG_ON
#define RAW_DEBUG        LWIP_DBG_ON
#define ICMP_DEBUG       LWIP_DBG_ON
#define UDP_DEBUG        LWIP_DBG_ON
#define TCP_DEBUG        LWIP_DBG_ON
#define TCP_INPUT_DEBUG  LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
#define TCP_RTO_DEBUG    LWIP_DBG_ON
#define TCP_CWND_DEBUG   LWIP_DBG_ON
#define TCP_WND_DEBUG    LWIP_DBG_ON
#define TCP_FR_DEBUG     LWIP_DBG_ON
#define TCP_QLEN_DEBUG   LWIP_DBG_ON
#define TCP_RST_DEBUG    LWIP_DBG_ON
#define ETHARP_DEBUG     LWIP_DBG_ON

#endif

//#define LWIP_DBG_TYPES_ON (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)

#endif /* __LWIPOPTS_H__ */

