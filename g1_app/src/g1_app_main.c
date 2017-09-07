#include "g1_hal.h"


int main(int argc, char *argv[])
{
	log_debug("start g1_app %s\n", "demo 0.3");

	hal_ttyS0_init();
	hal_ls_init();
	hal_key_init();
	hal_timer_init();
	
#if 0
	hal_ut();//unit testing
#endif
		
	for(;;){
				
	}
	return 0;
}
