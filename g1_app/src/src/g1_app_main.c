#include "g1_hal.h"


int main(int argc, char *argv[])
{
	log_debug("start g1_app %s\n", "demo 0.1");

	hal_ttyS0_init();
	hal_ls_init();
	hal_key_init();
#if 1
	//hal_ut_thread();
	//hal_ut_ttyS0_echo();
	//hal_ut_LED_screen();
	//hal_ut_LED_screen_unicode();

	//hal_ut_LED_screen_set_time();
	hal_ut_wstr_test();
#endif

	
	for(;;){
			
	}
	return 0;
}
