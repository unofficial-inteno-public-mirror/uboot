
# include $(TOPDIR)/config.mk

# # CFLAGS += -DET_DEBUG -DDEBUG
# LIB	= liblwip.a

# OBJS= api_lib.o api_msg.o err.o netbuf.o netdb.o netifapi.o sockets.o tcpip.o def.o dhcp.o dns.o init.o mem.o memp.o netif.o pbuf.o raw.o stats.o sys.o tcp.o tcp_in.o tcp_out.o timers.o udp.o autoip.o icmp.o igmp.o inet.o inet_chksum.o ip.o ip_addr.o ip_frag.o etharp.o ethernetif.o slipif.o
# vpath %.c lwip/src/api/ lwip/src/core/ lwip/src/netif/ lwip/src/core/ipv4/
# all:	$(LIB)

# $(LIB):	$(START) $(OBJS)
# 	$(AR) crv $@ $(OBJS)
# CFLAGS += -I../extra_include/
# #########################################################################

# .depend:	Makefile $(OBJS:.o=.c)
# 		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

# sinclude .depend

# #########################################################################


ifndef CONFIG_SPL_BUILD


API_PATH       := lwip/src/api
CORE_PATH      := lwip/src/core
CORE_IPV4_PATH := lwip/src/core/ipv4
CORE_IPV6_PATH := lwip/src/core/ipv6
CORE_NETIF_PATH := lwip/src/netif

ccflags-y += -I$(srctree)/include/lwip
ccflags-y += -I$(srctree)/include/lwip/netif
ccflags-y += -I$(srctree)/include/lwip/ipv4
ccflags-y += -I$(srctree)/include/lwip/ipv6
ccflags-y += -I$(srctree)/include/linux

obj-y += $(API_PATH)/api_lib.o
obj-y += $(API_PATH)/api_msg.o
obj-y += $(API_PATH)/err.o
obj-y += $(API_PATH)/netbuf.o
obj-y += $(API_PATH)/netdb.o
obj-y += $(API_PATH)/netifapi.o
obj-y += $(API_PATH)/sockets.o
obj-y += $(API_PATH)/tcpip.o

obj-y += $(CORE_PATH)/def.o
obj-y += $(CORE_PATH)/dhcp.o
obj-y += $(CORE_PATH)/dns.o
obj-y += $(CORE_PATH)/init.o
obj-y += $(CORE_PATH)/mem.o
obj-y += $(CORE_PATH)/memp.o
obj-y += $(CORE_PATH)/netif.o
obj-y += $(CORE_PATH)/pbuf.o
obj-y += $(CORE_PATH)/raw.o
obj-y += $(CORE_PATH)/stats.o
obj-y += $(CORE_PATH)/sys.o
obj-y += $(CORE_PATH)/tcp.o
obj-y += $(CORE_PATH)/tcp_in.o
obj-y += $(CORE_PATH)/tcp_out.o
obj-y += $(CORE_PATH)/timers.o
obj-y += $(CORE_PATH)/udp.o

obj-y += $(CORE_IPV4_PATH)/autoip.o
obj-y += $(CORE_IPV4_PATH)/icmp.o
obj-y += $(CORE_IPV4_PATH)/igmp.o
obj-y += $(CORE_IPV4_PATH)/inet.o
obj-y += $(CORE_IPV4_PATH)/inet_chksum.o
obj-y += $(CORE_IPV4_PATH)/ip.o
obj-y += $(CORE_IPV4_PATH)/ip_addr.o
obj-y += $(CORE_IPV4_PATH)/ip_frag.o

obj-y += $(CORE_NETIF_PATH)/etharp.o
#obj-y += $(CORE_NETIF_PATH)/ethernetif.o
obj-y += $(CORE_NETIF_PATH)/slipif.o


# ifndef CONFIG_SPL_BUILD
endif 
