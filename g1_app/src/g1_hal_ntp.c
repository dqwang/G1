#include "g1_hal.h"


int hal_get_time_from_ntp_server(void)
{
	//TODO 

	return 0;
}

int hal_get_time_from_mt7688(hal_date_t *hal_date)
{
	struct timeval tv;
    struct tm stm;

	if (hal_date == NULL){
		log_err("%s\n", "NULL Pointer");
		return -1;
	}
		
	gettimeofday ( &tv, NULL );
    stm = * ( localtime ( &tv.tv_sec ) );
   
    sprintf ( hal_date->time, "%04d-%02d-%02d %02d:%02d:%02d",
                stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, 
                stm.tm_min, stm.tm_sec);
        
    log_debug("hal_date->time %s\n", hal_date->time);
	//TODO
	return 0;
}


int hal_set_time_to_LEDScreen(HAL_LS_PARAM_CMD_SET_TIME_t*param)
{
	hal_ls_CMD_SET_TIME(param);

	return 0;
}


