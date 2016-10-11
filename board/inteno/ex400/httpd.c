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
#include "lwip_start.h"
#include "httpserver_raw/httpd.h"

#define PRE "httpd: "

ulong load_addr,cur_load_addr;
ulong time_start;

static int data_len;
static int do_reboot;

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
//        printf(PRE "%s()\n",__func__);
//        printf(PRE "%s: uri [%s]\n",__func__, uri);
//        printf(PRE "%s: http_request [%.*s]\n",__func__, http_request_len, http_request);
//        printf(PRE "%s: response_uri [%.*s]\n",__func__, response_uri_len, response_uri);
//        printf(PRE "%s: content_len [%d]\n",__func__, content_len);

        data_len = content_len;

        if (strncmp("/firmware_upload", uri, 16) )
        {
                printf(PRE "unknown POST destination %s\n", uri);
                return ERR_VAL;
        }

        time_start = get_timer(0);

        return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
        struct pbuf *q;
//        printf(PRE "%s()\n",__func__);

        for(q = p; q != NULL; pbuf_free(q),q = q->next) {

                memcpy((void*)cur_load_addr, q->payload, q->len);
                cur_load_addr += q->len;
        }

        return ERR_OK;
}

static char *OK_uri="/firmware_ok.html";
static char *FAIL_uri="/firmware_fail.html";

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
        char *body, *file_start;

        time_start = get_timer(time_start);

        /* http header Content-Type:  should contain the boundary=xxxxx */
        /* but we do not get that header so assume the first line in the body to be the boundary */
        body = strstr((char*)load_addr,"\r\n");

        /* put a \0 at the end of boundary string */
        *body = 0;
        body+=2;

        /* find start of file data */
        file_start = strstr((char*)body,"\r\n\r\n") + 4;

        /* find end of file by searching for the boundary string. */
        {
                int l1, l2;
                char *s1,*s2;

        	l2 = strlen((char*)load_addr);
                s2 = (char*)load_addr;

                s1 = file_start;
                l1 = data_len;

                while (l1 >= l2) {
                        l1--;
                        if (!memcmp(s1,s2,l2)){
                                data_len = s1 - file_start - 2;
                                break;
                        }
                        s1++;
                }
        }

        memmove((void*)load_addr, file_start, data_len );



        printf(PRE "Data received = %u / ", data_len);
        print_size((cur_load_addr - load_addr) / (time_start ? time_start : 1) * 1000, "/s\n");
        setenv_hex("filesize", data_len);


        /* do some sanity checks on new image */
        if (run_command("ubi write ${loadaddr} rootfs_0 $filesize", 0))
                goto err;
        if (run_command("ubifsmount ubi0:rootfs_0", 0))
                goto err;
        if (run_command("ubifsls /boot/dtb", 0))
                goto err;
        if (run_command("ubifsls /boot/uImage", 0))
                goto err;

        /* ok prepare for reboot */
        if (run_command("setenv root_vol rootfs_0", 0))
                goto err;
        if (run_command("saveenv", 0))
                goto err;

        do_reboot = 1;

        strcpy(response_uri,OK_uri);

        lwip_break(40000); /* stop lwip main loop, we are done */
        return;
err:
        strcpy(response_uri,FAIL_uri);
        return;
}


static int do_httpd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        char *s;

        /* pre-set load_addr */
        s = getenv("loadaddr");
        if (s != NULL)
                load_addr = simple_strtoul(s, NULL, 16);

        cur_load_addr = load_addr;

        if (argc < 1)
                return CMD_RET_USAGE;

        lwip_start();

        httpd_init();

        lwip_loop();
        lwip_stop();

        if (do_reboot)
                run_command("reset", 0);

        return 0;
}


U_BOOT_CMD(
        httpd,   4,      1,      do_httpd,
        "A rescue http server",
        "Used to allow a user to upload a system with a http browser"
        "exit with ctrl-c"
        );
