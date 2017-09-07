#include "g1_hal.h"

u32 hal_timer0 = 0;
int hal_timer0_flag = HAL_TIMER0_DISABLE;

void hal_timer0_reset(void)
{
	hal_timer0 = 0;
}

u32 hal_timer0_get(void)
{
	return hal_timer0;
}

int hal_timer0_is_timeout(u16 timeout)
{
	if (hal_timer0 >timeout){
		return 1;
	}else{
		return 0;
	}
}

void hal_timer0_set_flag(int flag)
{
	hal_timer0_flag = flag;
}
static void hal_timer_thread(void)
{
	while (1){
		hal_timer0++;
		if (hal_timer0_flag && hal_timer0_is_timeout(HAL_TIMER0_KEYBOARD_TIMEOUT)){

			hal_timer0_set_flag(HAL_TIMER0_DISABLE);
			hal_timer0_reset();

			hal_key_buf_clear();
			hal_calc_reset();

			hal_ls_clear();
			hal_ls_show_date_time();
		}
		log_debug("hal_timer0: %d S\n",hal_timer0);
		sleep(1);
	}
}

void hal_timer_init(void)
{
	TRD_t trd_id;
	hal_timer0_reset();
	trd_create(&trd_id, (void *)&hal_timer_thread,NULL);
}



