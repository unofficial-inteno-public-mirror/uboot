
#include <common.h>
#include <console.h>

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	
	
	
	return 0;
}


phys_size_t initdram(int board_type)
{
	return CONFIG_SYS_MEM_SIZE;
}

