#include "g1_hal.h"

hal_db_t hal_db_queue[HAL_DB_INDEX_MAX]={0};

hal_db_t hal_db_current;


void hal_db_set_time(hal_db_t *hal_db, hal_date_t *ntp_date)
{
	if (hal_db == NULL || ntp_date == NULL){
		log_err("%s\n", "NULL Pointer");
		return;
	}
	//NTP TODO

	//now get from local time. The time is not OK, because no RTC on board.
	memcpy(&hal_db->date, ntp_date, sizeof(hal_date_t));

	//printf("debug time: %s, %s\n",hal_db->date.time, ntp_date->time);
}
void hal_db_set_uuid(hal_db_t *hal_db, hal_uuid_t *uuid)
{
	if (hal_db == NULL || uuid == NULL){
		log_err("%s\n", "NULL Pointer");
		return;
	}

	memcpy(&hal_db->uuid, uuid, sizeof(hal_uuid_t));
}


void hal_db_set_device_name(hal_db_t *hal_db, char *name)
{
	if (hal_db == NULL || name == NULL){
		log_err("%s\n", "NULL Pointer");
		return;
	}
	memset(hal_db->name, 0x0, HAL_DB_DEVICE_NAME_LEN);
	memcpy(hal_db->name, name, HAL_DB_DEVICE_NAME_LEN);
	
	//printf("debug name: %s, %s\n",hal_db->name, name);
}
void hal_db_set_consumption(hal_db_t *hal_db, char *money_str)
{
	if (hal_db == NULL || money_str == NULL){
		log_err("%s\n", "NULL Pointer");
		return;
	}

	memcpy(hal_db->money_str, money_str, HAL_CALC_SUM_CHAR_STR_LEN);
	
}


void hal_db_set_current(hal_uuid_t *uuid, hal_date_t *ntp_date, char *name, char *money_str, hal_db_t *hal_db)
{
	hal_db->upload_to_server_flag=1;
	
	hal_db_set_uuid(hal_db,uuid);
	hal_db_set_time(hal_db,ntp_date);
	hal_db_set_device_name(hal_db,name);
	hal_db_set_consumption(hal_db,money_str);

}


void hal_db_enqueue_record(hal_db_t *current, hal_db_t queue[])
{
	//debug
	
	queue[current->index].index = current->index;
	
	memcpy(&queue[current->index].uuid, &current->uuid, sizeof(hal_uuid_t));
	memcpy(&queue[current->index].date, &current->date, sizeof(hal_date_t));
	memcpy(queue[current->index].name, current->name, HAL_DB_DEVICE_NAME_LEN);
	memcpy(queue[current->index].money_str, current->money_str, HAL_CALC_SUM_CHAR_STR_LEN);
}

void hal_db_print_records(void)
{
	u16 i=0;
	
	for (i=0; i<HAL_DB_INDEX_MAX; i++){

		if (hal_db_queue[i].index != 0){
			#if 1
			{	
				printf("index: %d\n", (int)hal_db_queue[i].index);
				printf("uuid: %s\n", hal_db_queue[i].uuid.uuid_str);
				printf("time: %s\n",hal_db_queue[i].date.time);
				printf("name: %s\n",hal_db_queue[i].name);
				printf("money: %s\n", hal_db_queue[i].money_str);
			}
			#endif
		}
	}
}


void hal_db_init(void)
{
	
}

