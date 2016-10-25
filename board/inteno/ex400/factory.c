#include <common.h>
#include <malloc.h>

static int do_factory(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        char *s;
        int ret;
        char *script;
        char cmd[100];
        printf("Factory command is run\n");

        s = getenv("ethaddr");
        if (s){
                if(strcmp(s,DEFAULT_ETHADDR)) { // not using default ethaddr
                                printf("factory was run but env looks to be initialized\n");
                                printf("Aborting factory with error code\n");
                                return 1;
                        }
        }

        /* if env1 volume is not existing reinit ubi partition */
        if (run_command("ubi check env1",0)){

                printf("Erasing ubi partition\n");
                /* delete entire ubi just to be sure nothing is there */
                run_command("nand erase.part ubi",0);

                printf("Initializing ubi\n");
                /* initi empty ubi */
                run_command("ubi part ubi",0);

                /* create env volumes */
                run_command("ubi create env1 1f000",0);
                run_command("ubi create env2 1f000",0);

                /* create system volumes save 12 PEBs for future use*/
//                run_command("ubi create rootfs_0 7766000",0);
//                run_command("ubi create rootfs_1 7766000",0);
                run_command("ubi create rootfs_0 76ac000",0);
                run_command("ubi create rootfs_1 76ac000",0);

                /* we need to restart to be able to save env in newly created ubi volumes env1 & env2 */
                run_command("reset",0);
        }

        script = malloc(1024*20);
        if (!script){
                printf("Factory: could not allocate 20k for script. Aborting!!!!!\n");
                return 1;
        }
        sprintf(script,"\n");

        /* wait on server answering ping */
        while (run_command("ping 192.168.1.2",0))
                ;

        printf("Factory: now we do wget from server on boot script\n");
        sprintf(cmd, "wget -s -t 5 %p ex400_setup", script);
        printf("factory: running command [%s]\n",cmd);
        while (run_command(cmd, 0)){
                printf("factory: timeout trying again [%s]\n",cmd);
        }

        printf("Factory: command done you should not get here something is wrong!!!!\n");
        free(script);
        return 1;

}

U_BOOT_CMD(
        factory,   4,      1,      do_factory,
        "Only used for factory",
        "\n"
        "This command is run first untill the factory sets the correct bootcmd"
        "\n"
        );
