#include <common.h>
#include <malloc.h>

static int   hdr_version;
static int   hdr_integrity;
static char *hdr_board;
static char *hdr_chip;
static char *hdr_model;
static char *hdr_release;
static char *hdr_customer;
static int   hdr_ubisize;

enum HDR_INTEGRITY{
        UNKNOWN,
        MD5SUM
};

#define MAXSTRING 1024
static char line[MAXSTRING];


static void getline(char *s)
{
                char *end;
                end = strstr(s,"\n");
                memset(line, 0, MAXSTRING);
                memcpy(line, s, end-s);
}

static void print_header(void)
{
        if (!hdr_version)
                return;

        printf("  Header version: %d\n", hdr_version);
        hdr_integrity ?
        printf("Integrity method: md5sum\n") :
        printf("Integrity method: unknown\n");
        printf("           Board: %s\n", hdr_board);
        printf("            Chip: %s\n", hdr_chip);
        printf("           Model: %s\n", hdr_model);
        printf("         Release: %s\n", hdr_release);
        printf("        Customer: %s\n", hdr_customer);
        printf("      UBIFS size: %d\n", hdr_ubisize);
}

static void parse_header(char *data)
{
        char *s;

        /* to avoid any issue with finding the key in the data \n is always added to the string */

        /* Version */
        if ((s = strstr(data,"\nversion" ))){
                getline(s+1);
                hdr_version = simple_strtoul( line+8, NULL, 10 );
        }

        /* Integrity */
        if ((s = strstr(data,"\nintegrity" ))){
                getline(s+1);
                if ( strstr(line + 11, "MD5SUM")){
                        hdr_integrity = UNKNOWN;
                }else{
                        hdr_integrity = MD5SUM;
                }
        }

        /* Board */
        if ((s = strstr(data,"\nboard" ))){
                if (hdr_board)
                        free(hdr_board);
                getline(s+1);

                hdr_board = strdup(line+6);   /* skip over "board " */
        }

        /* Chip */
        if ((s = strstr(data,"\nchip" ))){
                if (hdr_chip)
                        free(hdr_chip);
                getline(s+1);

                hdr_chip = strdup(line+5);  /* skip over "chip " */
        }

        /* Model */
        if ((s = strstr(data,"\nmodel" ))){
                if (hdr_model)
                        free(hdr_model);
                getline(s+1);

                hdr_model = strdup(line+6);  /* skip over "model " */
        }

        /* Release */
        if ((s = strstr(data,"\nrelease" ))){
                if (hdr_release)
                        free(hdr_release);
                getline(s+1);

                hdr_release = strdup(line+8); /* skip over "release " */
        }

        /* Customer */
        if ((s = strstr(data,"\ncustomer" ))){
                if (hdr_customer)
                        free(hdr_customer);
                getline(s+1);

                hdr_customer = strdup(line+9); /* skip over "customer " */
        }

        /* ubifs size */
        if ((s = strstr(data,"\nubifs" ))){
                getline(s+1);

                hdr_ubisize = simple_strtoul( line + 6, NULL, 10);/* skip over "\nubifs " */
        }
}

static int do_y2image(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        unsigned int addr=0;
        int len;
        int do_burn=0;
        char *volume_name;

        if (argc == 1 ){
                /* no arguments use env loadaddr and assume info */
                addr = simple_strtoul(getenv("loadaddr"), NULL, 16);
        }

        if (argc == 2 ){
                if ( ! strncmp(argv[1], "info", 4)){
                        /* no addr given use env loadaddr */
                        addr = simple_strtoul(getenv("loadaddr"), NULL, 16);
                }else if (! strncmp(argv[1], "burn", 4)){
                                printf("burn require at least the volume name to work \n");
                                return 1;
                }else{
                        printf("only info or burn as sub commands, not [%s]\n",argv[1]);
                        return 1;
                }
        }

        if (argc == 3 ){
                if ( ! strncmp(argv[1], "info", 4)){
                        addr = simple_strtoul(argv[2], NULL, 16);
                        if (!addr){
                                printf("Address given was not a number \n");
                                return 1;
                        }
                }else if (! strncmp(argv[1], "burn", 4)){
                        do_burn=1;
                        addr = simple_strtoul(argv[2], NULL, 16);
                        if (!addr){
                                volume_name = argv[2];
                                addr = simple_strtoul(getenv("loadaddr"), NULL, 16);
                        }else{
                                printf("No ubi volume name given\n");
                                return 1;
                        }
                }
        }

        if (argc == 4 ){
                if ( ! strncmp(argv[1], "info", 4)){
                        printf("info mode only takes the address as argument\n");
                        return 1;
                }else if (! strncmp(argv[1], "burn", 4)){
                        do_burn=1;
                        addr = simple_strtoul(argv[2], NULL, 16);
                        if (!addr){
                                printf("Not a valid address given \n");
                                return 1;
                        }
                        volume_name = argv[3];
                }
        }


        if (strncmp("IntenoBlob", (char*)addr, 10)){
                printf("There is no inteno y2 image att address %p\n",(void*)addr);
                return 1;
        }

        /* find header, end has to be before 1024 or header invalid */
        len = strnlen((char*)addr,1024);

        if (len >= 1024 ){
                printf("There is no inteno y2 image att address %p\n",(void*)addr);
                return 1;
        }

        printf("Found an inteno y2 image at addr %p\n",(void*)addr);
        parse_header((char *)addr);
        print_header();

        if (do_burn){
                sprintf(line,"ubi write %x %s %x", addr+1024, volume_name, hdr_ubisize);
                return run_command(line,0);
        }
        return 0;
}


U_BOOT_CMD(
        y2image,   4,      1,      do_y2image,
        "test and burn y2 image to flash",
        "\n"
        "y2image info [addr] - show info for y2 image at address\n"
        "y2iamge burn [addr] <volume> - burn y2 image to ubi volume\n"
        "\n"
        "[legends]\n"
        "  addr: the addres in ram where the y image is stored defalt = $loadaddr\n"
        "  volume: the name of theUBI volume to store to. Must be given no default\n"
        "\n"
        );
