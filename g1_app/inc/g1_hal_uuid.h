#ifndef __G1_HAL_UUID_H__
#define __G1_HAL_UUID_H__

#include "g1_hal_type.h"

#define HAL_UUID_LEN 32
#define HAL_UUID_READ_COUNT_MAX 3


typedef struct {
	u8 uuid_str[HAL_UUID_LEN+1];//ascii
}hal_uuid_t;

extern u8 hal_uuid_count;//debug only
extern u8 hal_uuid_valid_flag;
extern u8 hal_uuid_thread_start_flag;

extern u32 hal_uuid_money_sum;
extern char hal_uuid_sum_str[HAL_CALC_SUM_CHAR_STR_LEN];
extern u8 hal_uuid_qr_flag;
extern char hal_current_balance_str[HAL_CALC_SUM_CHAR_STR_LEN];
extern double hal_current_balance;

int hal_uuid_get_from_QR(hal_uuid_t * uuid);

void hal_uuid_thread(void);

void hal_uuid_init(void);

#endif/*__G1_HAL_UUID_H__*/
