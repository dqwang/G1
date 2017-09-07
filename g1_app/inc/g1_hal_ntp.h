#ifndef __G1_HAL_NTP_H__
#define __G1_HAL_NTP_H__


#define HAL_NTP_SERVER ""

#define HAL_DATE_LEN 20//XXXX-XX-XX XX:XX:XX


typedef struct hal_date_{
	char time[HAL_DATE_LEN];
}hal_date_t;


int hal_get_time_from_ntp_server(void);

int hal_get_time_from_mt7688(hal_date_t *hal_date);


int hal_set_time_to_LEDScreen(HAL_LS_PARAM_CMD_SET_TIME_t*param);

#endif/*__G1_HAL_NTP_H__*/
