#ifndef __G1_HAL_DATABASE_H__
#define __G1_HAL_DATABASE_H__

#include "g1_hal_type.h"
#include "g1_hal_timer.h"

//Name
#define HAL_DB_DEVICE_NAME_LEN 32
#define HAL_DB_DEVICE_NAME "G1-Demo"

#define HAL_DB_INDEX_MAX 1000

typedef struct hal_database_{
	u32 index;
	u8 upload_to_server_flag;
	hal_uuid_t uuid;
	u32 money_int;
	u8 money_str[HAL_CALC_SUM_CHAR_STR_LEN];
	hal_date_t date;//XXXX-XX-XX XX:XX
	char name[HAL_DB_DEVICE_NAME_LEN];
	
}hal_db_t;

extern hal_db_t hal_db_current;

extern hal_db_t hal_db_queue[HAL_DB_INDEX_MAX];


void hal_db_set_time(hal_db_t *hal_db, hal_date_t *ntp_date);
void hal_db_set_uuid(hal_db_t *hal_db, hal_uuid_t *uuid);


void hal_db_set_device_name(hal_db_t *hal_db, char *name);
void hal_db_set_consumption(hal_db_t *hal_db, char *money_str);


void hal_db_set_current(hal_uuid_t *uuid, hal_date_t *ntp_date, char *name, char *money_str, hal_db_t *hal_db);

//void hal_db_enqueue_record(hal_db_t *current, hal_db_t *queue);
void hal_db_enqueue_record(hal_db_t *current, hal_db_t queue[]);

void hal_db_print_records(void);

void hal_db_init(void);

#endif/*__G1_HAL_DATABASE_H__*/
