#include <common.h>
#include "getopt.h"

static char buf[1000*4];

static void getopt_init(void)
{
        optarg=0;
        optopt=0;
        optind=1;
        opterr=0;
}


static void may_reboot(int reboot_counter)
{
        /* if counter is to hi there is no point in trying to reboot
           this prevents an infinite boot loop that would write to ubi
           every boot.
        */
        if (reboot_counter > 10)
                return;

        run_command("reset",0);

}

static int do_iboot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        int opt;
        char *s;
        int cnt_primary=0, cnt_alt=0;

        /* command options */

        getopt_init();
        while ((opt = getopt(argc, argv, "ad")) != -1) {
                switch (opt) {
                case 'a':
                        printf("Activating \"verify boot\"\n");
                        setenv("verify_boot", "1");
                        saveenv();
                        break;
                case 'd':
                        printf("Deactivating \"verify boot\"\n");
                        setenv("verify_boot", "");
                        saveenv();
                        break;
                default: /* '?' */
                        printf("not supported option\n");
                        return 1;
                }
        }

        /* handle verify boot */
        s = getenv("verify_boot");

        if ( s ) {
                char *root;
                char *boot_cnt_primary;
                char *boot_cnt_alt;
                int primary, alt, selected=-1;
                char *s2;

                root = getenv("root_vol");
                if(!root){
                        printf("Could not get env root_vol\n");
                        return 1;
                }

                primary = simple_strtoul(root + strlen("rootfs_"), NULL, 10);
                if (primary)
                        alt = 0;
                else
                        alt = 1;

                boot_cnt_primary = getenv("boot_cnt_primary");
                boot_cnt_alt = getenv("boot_cnt_alt");

                if (boot_cnt_primary) {
                        cnt_primary = simple_strtoul(boot_cnt_primary, NULL, 10);
                }

                if (boot_cnt_alt){
                        cnt_alt = simple_strtoul(boot_cnt_alt, NULL, 10);
                }

                if (cnt_primary < 5) {
                        cnt_primary++;
                        selected = primary;
                        cnt_alt = 0;

                } else if (cnt_alt < 5) {
                        cnt_alt++;
                        selected = alt;
                } else {
                        printf("No working system, both systems has a boot count over 5\n");
                        printf("Aborting boot\n");
                        return 1;
                }

                printf("      primary system is %d\n", primary);
                printf("primary boot counter is %d\n", cnt_primary);
                printf("          alt system is %d\n", alt);
                printf("    alt boot counter is %d\n", cnt_alt);
                printf("      seleced system is %d\n", selected);

                sprintf(buf, "%d", cnt_primary);
                setenv("boot_cnt_primary", buf);
                sprintf(buf, "%d", cnt_alt);
                setenv("boot_cnt_alt", buf);

                /* set bootargs with root_vol */
                if (run_command("run bootargs_ubi", 0)) {
                        printf("Could not run [bootargs_ubi]\n");
                        may_reboot(cnt_alt);
                        return 1;
                }

                /* now change root from root_vol to selected */
                s = getenv("bootargs");
                strcpy(buf,s);
                s2 = strstr(buf,"root=ubi0:rootfs_") + strlen("root=ubi0:rootfs_");
                sprintf(s2,"%d%s", selected, s2+1);

                printf("Kernel command line = [%s]\n",buf);
                setenv("bootargs", buf);

                saveenv();
                /* mount rootfs */
                sprintf(buf,"ubifsmount ubi0:rootfs_%d", selected);
                if (run_command(buf, 0)) {
                        printf("Could not run [%s]\n", buf);
                        may_reboot(cnt_alt);
                        return 1;
                }
        }else{
                printf("verify boot not activated\n");

                /* read in kernel device tree and boot */
                if (run_command("run bootargs_ubi", 0)) {
                        printf("Could not run [bootargs_ubi]\n");
                        return 1;
                }

                if (run_command("ubifsmount ubi0:${root_vol}", 0)) {
                        printf("Could not run [ubifsmount ubi0:${root_vol}]\n");
                        return 1;
                }
        }

        if (run_command("ubifsload ${loadaddr} /boot/uImage", 0)) {
                printf("Could not run [ubifsload ${loadaddr} /boot/uImage]\n");
                may_reboot(cnt_alt);
                return 1;
        }

        if (run_command("ubifsload ${fdtaddr} /boot/dtb", 0)) {
                printf("Could not run [ubifsload ${fdtaddr} /boot/dtb]\n");
                may_reboot(cnt_alt);
                return 1;
        }

        if (run_command("bootm ${loadaddr} - ${fdtaddr}", 0)) {
                printf("Could not run [bootm ${loadaddr} - ${fdtaddr}]\n");
                may_reboot(cnt_alt);
                return 1;
        }

        return 0;
}

U_BOOT_CMD(
        iboot,   2,      1,      do_iboot,
        "Inteno boot",
        "\n"
        "used to boot from ubifs."
        "iboot -a activated the verify boot feature\n"
        "iboot -d deactivated the verify boot feature\n"
        "\n"
        );
