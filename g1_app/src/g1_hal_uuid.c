#include "g1_hal.h"

hal_uuid_t hal_uuid;

double hal_current_balance = 10000;
char hal_current_balance_str[HAL_CALC_SUM_CHAR_STR_LEN]={0};

u8 hal_uuid_count = 0;
u8 hal_uuid_valid_flag = 0;

u8 hal_uuid_thread_start_flag = 0;

u32 hal_uuid_money_sum =0;
char hal_uuid_sum_str[HAL_CALC_SUM_CHAR_STR_LEN]={0};
u8 hal_uuid_qr_flag = 0;

int hal_uuid_is_valid(hal_uuid_t * uuid)
{
	//TODO
	//return 1;
	//return 0;

	if (hal_uuid_valid_flag){
		return 1;
	}else{
		return 0;
	}	
}

int hal_uuid_get_from_QR(hal_uuid_t * uuid)
{
	hal_uuid_t hal_uuid;
	
	
	//TODO 调用赖工API,获取UUID
	//block API?
	//nonblock API?

	do{
		if (hal_uuid_qr_flag){
			hal_uuid_qr_flag =0;
			break;
		}
	}while (1);
	
	if (hal_uuid_is_valid(&hal_uuid)){
		//memcpy(uuid, &hal_uuid, sizeof(hal_uuid_t));
		
		return 0;
	}

	return 1;
}

void hal_uuid_thread(void)
{
	int ret = -1;

	while (1){
		
		ret=hal_uuid_get_from_QR(&hal_db_current.uuid);

		//send the uuid to server, get balance?
		
		
		if (ret != 0){

			if (hal_uuid_count<HAL_UUID_READ_COUNT_MAX){
				hal_ls_show_consume_failed_retry();//消费失败，请重试

			}	

			if (hal_uuid_count>=HAL_UUID_READ_COUNT_MAX){
				hal_ls_show_consume_failed();//show "消费失败，已取消"
				hal_uuid_thread_start_flag = 0;
				
				sleep(6);
				hal_ls_clear();
				hal_ls_show_date_time();
				break;
			}
		}else{//余额判断

			if (hal_current_balance - hal_uuid_money_sum>0){//消费成功账户余额

				hal_current_balance -= hal_uuid_money_sum;
				
				sprintf(hal_current_balance_str, "%.2f", hal_current_balance);
				hal_calc_disp_money_successfully(hal_current_balance_str);


				//TODO record to database

				//hal_db_current.uuid
				//memcpy(hal_db_current.money_str, hal_uuid_sum_str,sizeof(HAL_CALC_SUM_CHAR_STR_LEN));
				//time
				//device name

				#if 1
				{
					hal_date_t ntp_date;

					memset(&ntp_date, 0x00, sizeof(hal_date_t));
					
					hal_get_time_from_mt7688(&ntp_date);
					
					hal_db_set_current(&hal_uuid,&ntp_date,HAL_DB_DEVICE_NAME, hal_uuid_sum_str, &hal_db_current);

					//printf("uuid: %s\n", hal_db_current.uuid.uuid_str);
					//printf("time: %s\n",hal_db_current.date.time);
					//printf("name: %s\n",hal_db_current.name);
					//printf("money: %s\n", hal_db_current.money_str);
					hal_db_current.index++;
					hal_db_enqueue_record(&hal_db_current, hal_db_queue);
					hal_db_print_records();
				}
				#endif
				

				sleep(6);
				hal_ls_clear();
				hal_ls_show_date_time();
				
				
				
				hal_uuid_thread_start_flag = 0;
				break;
			}else{//余额不足
				hal_ls_show_insufficient_balance();
			}
		}
		sleep(2);
	}
}

void hal_uuid_init(void)
{
	TRD_t trd_id;
	
	hal_uuid_thread_start_flag = 1;

	//debug only
	#if 1
	u8 uuid_str[HAL_UUID_LEN+1]="0123456789abcdef0123456789abcdef";
	#endif

	memcpy(hal_uuid.uuid_str, uuid_str, sizeof(hal_uuid.uuid_str));
	
	trd_create(&trd_id,(void*)&hal_uuid_thread, NULL);
}

