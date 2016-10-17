#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "http-parser/http_parser.h"
#include <malloc.h>
#include "lwip_start.h"

//#define PRE "!!!!!!!!!!!!!!!!!!!!"
#define PRE "wget: "

static err_t  wget_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void   wget_tcp_err(void *arg, err_t err);
static err_t  wget_tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

static void handle_new_header( void);

static int request_url_cb(http_parser* parser, const char *at, size_t length);
static int request_status_cb(http_parser* parser,const char *at, size_t length);
static int header_field_cb(http_parser* parser,const char *at, size_t length);
static int header_value_cb(http_parser* parser,const char *at, size_t length);
static int body_cb(http_parser* parser,const char *at, size_t length);

static int message_begin_cb(http_parser* parser);
static int headers_complete_cb(http_parser* parser);
static int message_complete_cb(http_parser* parser);
static int chunk_header_cb(http_parser* parser);
static int chunk_complete_cb(http_parser* parser);

enum header_state {
          PS_NONE
        , PS_HEAD
        , PS_VALUE
};

enum header_state pstate = PS_NONE;
static char *cur_header;
static char *cur_value;

static http_parser parser;

static http_parser_settings settings;

static struct tcp_pcb *wget_pcb;

int wget_port;
char *wget_hostname;
char *wget_file;

ulong load_addr,cur_load_addr;
ulong time_start;
int body_ok;

void handle_new_header( void)
{
//        printf(PRE "%s() ------------- header[%s] value[%s]\n", __func__, cur_header, cur_value);

        free(cur_header);
        free( cur_value);
        cur_header = NULL;
        cur_value  = NULL;
}

static int message_begin_cb(http_parser* parser) {
//        printf(PRE "%s()\n",__func__);
        return 0;
}
static int headers_complete_cb(http_parser* parser) {
        //printf(PRE "%s()\n",__func__);

        if (cur_header && cur_value) /* full header and value found */
                handle_new_header();
        return 0;
}
static int message_complete_cb(http_parser* parser) {
//        printf(PRE "%s()\n",__func__);

        if (body_ok){
                /* get time since start */
                time_start = get_timer(time_start);
                printf(PRE "Data received = %lu / ", cur_load_addr - load_addr);
                print_size((cur_load_addr - load_addr) / (time_start ? time_start : 1) * 1000, "/s\n");
                setenv_hex("filesize", cur_load_addr - load_addr);
        }

        lwip_break(0);
        return 0;
}
static int chunk_header_cb(http_parser* parser) {
        printf(PRE "%s()\n",__func__);
        return 0;
}
static int chunk_complete_cb(http_parser* parser) {
        printf(PRE "%s()\n",__func__);
        return 0;
}

static int request_url_cb(http_parser* parser, const char *at, size_t length) {
        printf(PRE "%s(%d)[%.*s]\n",__func__,length,length,at);
        return 0;
}
static int request_status_cb(http_parser* parser, const char *at, size_t length) {
//        printf(PRE "%s(%d)[%.*s]\n",__func__,length,length,at);

        if (! strncmp("OK",at,2)){
                body_ok = 1;
        }else
                printf(PRE "Respose from server = %.*s\n",length,at);

        return 0;
}
static int header_field_cb(http_parser* parser,const char *at, size_t length)
{
        //printf(PRE "%s(%d)[%.*s]\n",__func__,length,length,at);

        if (pstate == PS_HEAD ) {
                /* continue on previous header */
                int len = strlen(cur_header);

                cur_header = realloc(cur_header, len + length + 1);

                strncpy(cur_header + len, at, length);
                cur_header[len + length] = 0;

        }else{ /* new header found */
                if (cur_header && cur_value) /* full header and value found */
                        handle_new_header();

                cur_header = malloc(length + 1);
                strncpy(cur_header, at, length);
                cur_header[length] = 0;
                pstate = PS_HEAD;
        }

        return 0;
}
static int header_value_cb(http_parser* parser,const char *at, size_t length)
{
        //printf(PRE "%s(%d)[%.*s]\n",__func__,length,length,at);

        if (pstate == PS_HEAD ) {
                /* new value found */
                free(cur_value);
                cur_value = malloc(length + 1);
                strncpy(cur_value, at, length);
                cur_value[length] = 0;
                pstate = PS_VALUE;
        }else{
                /* continue on previous value */
                int len = strlen(cur_value);

                cur_value = realloc(cur_value, len + length + 1);

                strncpy(cur_value + len, at, length);
                cur_value[len + length] = 0;
        }

        return 0;
}
static int body_cb(http_parser* parser,const char *at, size_t length)
{
//        printf(PRE "%s(%d)[%.*s]\n", __func__, length, length, at);
//        printf(PRE "%s(%d)[%lx]\n", __func__, length, cur_load_addr);
        if (body_ok){
                memcpy((void*)cur_load_addr, at, length);
                cur_load_addr += length;
        }
        return 0;
}

static void   wget_tcp_err(void *arg, err_t err)
{
        printf(PRE "wget_tcp_err(%p, %x)\n",arg, err);
}

static err_t  wget_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
        size_t nparsed;

//        printf(PRE "wget_tcp_recv(%p, %p, %p, %x)\n",arg, tpcb, p, err);

        if (p == NULL) {
//                printf(PRE "The remote host closed the connection.\n");
//                printf(PRE "Now I'm closing the connection.\n");
                tcp_close(tpcb);
                return ERR_ABRT;
        } else {
                struct pbuf *q;
//                printf(PRE "Number of pbufs %d size = %d\n", pbuf_clen(p), p->tot_len);

                for(q = p; q != NULL; q = q->next) {
                        /* Read enough bytes to fill this pbuf in the chain. The
                           available data in the pbuf is given by the q->len
                           variable. */
                        /* read data into(q->payload, q->len); */
//                        memcpy(q->payload, bufptr, q->len);
//                        bufptr += q->len;

                        nparsed = http_parser_execute(&parser, &settings, q->payload, q->len);
//                        printf(PRE "nparsed = 0X%x \n", nparsed);

                        if (nparsed != q->len){
                                printf(PRE "ERROR: http_parser_execute(%d) \n", nparsed);
                        }


                }

#if 0
                printf(PRE "Contents of pbuf \n--------------------\n");
                i = 0;
                while (i < p->len ) {
                        putc(*(char *)(p->payload + i));
                        i++;
                }
                printf("\n--------------------\n");
#endif

                /* BUG: assumtion is that we get all headers in first packet */
                /* easier code and probably correct */

                tcp_recved(tpcb, p->tot_len);
                pbuf_free(p);
        }

    return 0;        return ERR_OK;
}

static err_t  wget_tcp_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
//        printf(PRE "wget_tcp_sent(%p, %p, %x)\n",arg, pcb, len);
        return ERR_OK;
}

/* connection established callback, err is unused and only return 0 */
err_t wget_tcp_connect(void *arg, struct tcp_pcb *tpcb, err_t err)
{
        char buf[1500], *p;
        int ret;

        printf(PRE "Connection Established.\n");
//        printf(PRE "Sending http request\n");
        time_start = get_timer(0);

        p = buf;
//        p += sprintf(p, "GET /root.ubifs HTTP/1.0\r\n");
//        p += sprintf(p, "GET /mini HTTP/1.0\r\n");
        p += sprintf(p, "GET %s HTTP/1.0\r\n", wget_file);
        p += sprintf(p, "User-Agent: wget/2016.09 (uboot)\r\n");
        p += sprintf(p, "Accept: */*\r\n");
        p += sprintf(p, "Accept-Encoding: identity\r\n");
        p += sprintf(p, "Host: %s\r\n",wget_hostname);
        p += sprintf(p, "\r\n");

        ret = tcp_write(tpcb, buf, p-buf , TCP_WRITE_FLAG_MORE);
        if (ERR_OK != ret ){
                printf("%s:tcp_write() failed\n",__func__);
        }
        tcp_output(tpcb);

        /* prepare for http header parsing */
        settings.on_message_begin     = message_begin_cb;
        settings.on_headers_complete  = headers_complete_cb;
        settings.on_message_complete  = message_complete_cb;
        settings.on_chunk_header      = chunk_header_cb;
        settings.on_chunk_complete    = chunk_complete_cb;

        settings.on_url               = request_url_cb;
        settings.on_status            = request_status_cb;
        settings.on_header_field      = header_field_cb;
        settings.on_header_value      = header_value_cb;
        settings.on_body              = body_cb;

        http_parser_init(&parser, HTTP_RESPONSE);
        return 0;
}

void
wget(void)
{
        ip_addr_t ip;

        //IP4_ADDR(&ip, 192 ,168, 1, 2);

        wget_pcb = tcp_new();

        /* we only support valid ip number not hostnames. So no need to have a callback for dns answers */
        //err = dns_gethostbyname(wget_hostname, &ip, NULL , wget_pcb);
        if ( ! ipaddr_aton(wget_hostname, &ip) ){
                printf("wget: failed to get ip number for %s\n", wget_hostname);
                return;
        }

        /* set state to invalid first */
        body_ok = 0;

        /* dummy data to pass to callbacks*/
        tcp_arg(wget_pcb, (void *)0xdeadbeef);

        /* register callbacks with the pcb */

        tcp_err(wget_pcb, wget_tcp_err);
        tcp_recv(wget_pcb, wget_tcp_recv);
        tcp_sent(wget_pcb, wget_tcp_sent);

        /* now connect */
        tcp_connect(wget_pcb, &ip, wget_port, wget_tcp_connect);
}

static int do_wget(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        ulong addr;
        char *s, *end, *ustr = NULL;
        struct http_parser_url url;
        int ret;

        wget_file = NULL;
        wget_hostname = NULL;

        /* pre-set load_addr */
        s = getenv("loadaddr");
        if (s != NULL)
                load_addr = simple_strtoul(s, NULL, 16);

        http_parser_url_init(&url);

        switch (argc) {
        case 1:
                return CMD_RET_USAGE;
                break;

        case 2: /*
                 * Only one arg - accept two forms:
                 * Just load address, or just boot file name. The latter
                 * form must be written in a format which can not be
                 * mis-interpreted as a valid number.
                 */
                addr = simple_strtoul(argv[1], &end, 16);
                if (end == (argv[1] + strlen(argv[1])))
                        load_addr = addr;
                else
                        ustr = argv[1];
                        http_parser_parse_url(argv[1],strlen(argv[1]), 0, &url);
                break;

        case 3:
                load_addr = simple_strtoul(argv[1], NULL, 16);
                ustr = argv[2];
                http_parser_parse_url(argv[2],strlen(argv[2]), 0, &url);
                break;

        default:
                return CMD_RET_USAGE;
        }

        cur_load_addr = load_addr;

#if 0
        printf("UF_SCHEMA = [%.*s]\n",url.field_data[UF_SCHEMA].len, ustr + url.field_data[UF_SCHEMA].off );
        printf("UF_HOST = [%.*s]\n",url.field_data[UF_HOST].len, ustr + url.field_data[UF_HOST].off );
        printf("UF_PORT = [%.*s]\n",url.field_data[UF_PORT].len, ustr + url.field_data[UF_PORT].off );
        printf("UF_PATH = [%.*s]\n",url.field_data[UF_PATH].len, ustr + url.field_data[UF_PATH].off );
        printf("UF_QUERY= [%.*s]\n",url.field_data[UF_QUERY].len, ustr + url.field_data[UF_QUERY].off );
        printf("UF_FRAGMENT= [%.*s]\n",url.field_data[UF_FRAGMENT].len, ustr + url.field_data[UF_FRAGMENT].off );
        printf("UF_USERINFO= [%.*s]\n",url.field_data[UF_USERINFO].len, ustr + url.field_data[UF_USERINFO].off );


#endif
        if (!url.field_data[UF_HOST].len){
                /* we did not get any valid URL. assume we only got the filename */
                s = getenv("serverip");
                printf("wget: [%s] is not a valid url. create one [http://%s/%s]\n",ustr, s, ustr);

                wget_hostname = strdup(s);

                wget_file = malloc(strlen(strdup(ustr))+1);
                sprintf(wget_file,"/%s",ustr);

        }else {
                wget_hostname = malloc(url.field_data[UF_HOST].len+1);
                sprintf(wget_hostname,"%.*s",url.field_data[UF_HOST].len, ustr + url.field_data[UF_HOST].off);
        }

        if (url.field_data[UF_PORT].len){
                wget_port = simple_strtoul(ustr + url.field_data[UF_PORT].off, NULL, 10);
        }else
                wget_port = 80;

        if (url.field_data[UF_HOST].len && !url.field_data[UF_PATH].len){
                printf("wget: need to have a file path in the URL\n");
                free(wget_hostname);
                return CMD_RET_USAGE;
        }

        if (!wget_file){
                wget_file = malloc(url.field_data[UF_PATH].len+1);
                sprintf(wget_file,"%.*s",url.field_data[UF_PATH].len, ustr + url.field_data[UF_PATH].off);
        }

        ret = lwip_start();
        if (ret == ERR_OK)
        {
                wget();

                /* main loop ctrl-c to quit */
                ret = lwip_loop();
                tcp_close(wget_pcb);
                lwip_stop();

                /* transform lwip err to uboot command return code */
                if (ret != ERR_OK){
                        ret = 1;
                }else
                        ret = 0;

                /* the the http server respond with data ?? if not return error */
                if (!body_ok)
                        ret = 1;
        }

        free(wget_hostname);
        free(wget_file);
        return ret;
}

U_BOOT_CMD(
        wget,   4,      1,      do_wget,
        "download files from http server",
        "exit with ctrl-c"
        );
