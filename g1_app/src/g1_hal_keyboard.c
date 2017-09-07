#include "g1_hal.h"

wchar_t input_wchar_buf[HAL_CALC_INPUT_WCHAR_BUF_SIZE]={0};
u8 input_wchar_index=0;
u8 input_info_len=0;

int g1_hal_key_open(void)
{
	int fd = -1;
	fd = open(KEY_DEVICE,O_RDONLY);
	if (fd<0){
		log_err("%s\n", "open failed");
		return -1;
	}
	log_msg("open %s\n", KEY_DEVICE);

	return fd;
}

void g1_hal_key_close(int fd)
{
	if (fd)
		close(fd);
}

void hal_key_buf_clear(void)
{
	memset(input_wchar_buf, 0x00, sizeof(input_wchar_buf));
	input_wchar_index = 0;
	input_info_len = 0;
}

void hal_key_get_wchar_code(wchar_t *key)
{
	if (key == NULL){
		log_err("key pointer %s\n", "NULL");
		return;
	}
	
	memcpy(input_wchar_buf + input_wchar_index, key, HAL_WCHAR_SIZE);

	if (input_info_len>=2*HAL_CALC_INPUT_WCHAR_BUF_SIZE){//XXX :if out of memory
		log_err(" %s\n", "Outof Memory");
		return;	
	}	

	input_info_len += HAL_WCHAR_SIZE;
	
	hal_calc_set_wchar_buf(input_wchar_buf, input_wchar_index, input_info_len);

	input_wchar_index += HAL_WCHAR_INDEX;	
}


void hal_key_get_wchar_code_backspace(void)
{
	if ( input_wchar_index>0){
		input_wchar_index -= HAL_WCHAR_INDEX;
		input_info_len -= HAL_WCHAR_SIZE;
		
		memset(input_wchar_buf + input_wchar_index, 0x00, HAL_WCHAR_SIZE);//set the last wchar 0

		if (input_wchar_index != 0){
			hal_calc_set_wchar_buf(input_wchar_buf, input_wchar_index-1, input_info_len);
		}else{
			hal_calc_reset();
			hal_calc_disp();
		}		
	}
}


void hal_key_get_char_code(char *key)
{
	if (key == NULL){
		log_err("key pointer %s\n", "NULL");
		return;
	}

	if (hal_calc.input_char_index>=HAL_CALC_INPUT_CHAR_BUF_SIZE){//XXX :if out of memory
		log_err(" %s\n", "Outof Memory");
		return;	
	}	
	
	strcat(hal_calc.input_char_buf, key);
	hal_calc.input_char_index++;

	printf("hal_calc.input_char_buf is %s hal_calc.input_char_index is %d\n", hal_calc.input_char_buf, hal_calc.input_char_index);
}

void hal_key_get_char_code_backspace(void)
{
	if (hal_calc.input_char_index>0){
		hal_calc.input_char_index--;
		hal_calc.input_char_buf[hal_calc.input_char_index] = '\0';
	}

	printf("hal_calc.input_char_buf is %s hal_calc.input_char_index is %d\n", hal_calc.input_char_buf, hal_calc.input_char_index);
}


void hal_key_input_error(void)
{
	wchar_t input_error[]={0x8f93, 0x5165, 0x9519,0x8bef, 0x0000};//ÊäÈë´íÎó

	u8 mode=0x01;
	u8 flash_onoff=0x00;
	u8 speed=0x0E;
	u8 show_time=0x00;
	u8 flash_time=0x00;
	u8 show_count=0x00;
	
	hal_ls_CMD_SEND_INFO(input_error,hal_wstr_char_len(input_error),mode,
		flash_onoff,speed,show_time,flash_time,show_count);
}

int hal_key_enter_count_is_3(long ev_time)
{
	static u8 enter_count = 0;
	static long time=0;

	if (enter_count == 0){
		time= ev_time;
	}
	enter_count++;
		
	printf("Event: time %ld time %ld , enter_count is %d\n\n", ev_time, time, enter_count);
	
	if ((enter_count>=3)){// 3 times per second
		enter_count = 0;
		if((ev_time - time)==0 ||(ev_time - time)==1){
			time = 0;
			return 1;
		}else{
			time = 0;
			return 0;
		}		
	}else{
		return 0;
	}
}

void hal_key_thread(int *p_fd)
{
	struct input_event ev[64];
	int i, rd, fd;
	fd_set rdfs;
	static volatile sig_atomic_t stop = 0;

	log_msg("start %s\n", "key thread");

	if (p_fd){
		fd = *p_fd;
	}
		
	FD_ZERO(&rdfs);
	FD_SET(fd, &rdfs);

	while (!stop) {
		select(fd + 1, &rdfs, NULL, NULL, NULL);
		if (stop)
			break;
		rd = read(fd, ev, sizeof(ev));

		if (rd < (int) sizeof(struct input_event)) {
			printf("expected %d bytes, got %d\n", (int) sizeof(struct input_event), rd);
			perror("\nevtest: error reading");

			hal_key_init();
			return;
		}

		for (i = 0; i < rd / sizeof(struct input_event); i++) {
			u32 type, code;

			type = ev[i].type;
			code = ev[i].code;

			//printf("Event: time %ld.%06ld, ", ev[i].time.tv_sec, ev[i].time.tv_usec);

			if (type == EV_KEY) {
				

				if (ev[i].value != 1){
					continue;
				}
#if 0//debug
				printf("type %d, code %d ",	type,code);
				if (type == EV_MSC && (code == MSC_RAW || code == MSC_SCAN))
					printf("value %02x\n", ev[i].value);
				else
					printf("value %d\n", ev[i].value);
#endif
				hal_timer0_reset();
				hal_timer0_set_flag(HAL_TIMER0_ENABLE);

				switch(code){
					case HAL_KEY_CODE_0:{//0
						hal_key_get_wchar_code(HAL_KEY_0_WCHAR);
						hal_key_get_char_code(HAL_KEY_0_CHAR);
					}break;

					case HAL_KEY_CODE_1:{// 1
						hal_key_get_wchar_code(HAL_KEY_1_WCHAR);
						hal_key_get_char_code(HAL_KEY_1_CHAR);
					}break;
					case HAL_KEY_CODE_2:{// 2
						hal_key_get_wchar_code(HAL_KEY_2_WCHAR);
						hal_key_get_char_code(HAL_KEY_2_CHAR);
					}break;
					case HAL_KEY_CODE_3:{// 3
						hal_key_get_wchar_code(HAL_KEY_3_WCHAR);
						hal_key_get_char_code(HAL_KEY_3_CHAR);
					}break;
					case HAL_KEY_CODE_4:{// 4
						hal_key_get_wchar_code(HAL_KEY_4_WCHAR);
						hal_key_get_char_code(HAL_KEY_4_CHAR);
					}break;
					case HAL_KEY_CODE_5:{// 5
						hal_key_get_wchar_code(HAL_KEY_5_WCHAR);
						hal_key_get_char_code(HAL_KEY_5_CHAR);
					}break;
					case HAL_KEY_CODE_6:{// 6
						hal_key_get_wchar_code(HAL_KEY_6_WCHAR);
						hal_key_get_char_code(HAL_KEY_6_CHAR);
					}break;
					case HAL_KEY_CODE_7:{// 7
						hal_key_get_wchar_code(HAL_KEY_7_WCHAR);
						hal_key_get_char_code(HAL_KEY_7_CHAR);
					}break;
					case HAL_KEY_CODE_8:{// 8
						hal_key_get_wchar_code(HAL_KEY_8_WCHAR);
						hal_key_get_char_code(HAL_KEY_8_CHAR);
					}break;
					case HAL_KEY_CODE_9:{// 9
						hal_key_get_wchar_code(HAL_KEY_9_WCHAR);
						hal_key_get_char_code(HAL_KEY_9_CHAR);
					}break;
					case HAL_KEY_CODE_POINT:{// .
						hal_key_get_wchar_code(HAL_KEY_POINT_WCHAR);
						hal_key_get_char_code(HAL_KEY_POINT_CHAR);
					}break;
					

					case HAL_KEY_CODE_ENTER:{// enter
						double sum_double=0;
						char sum_str[HAL_CALC_SUM_CHAR_STR_LEN]={'\0'};
						int ret = -1;


						if (hal_key_enter_count_is_3(ev[i].time.tv_sec)){
							hal_ls_clear();
							hal_ls_show_date_time();
							break;
						}
						
						if (hal_calc.input_char_index == 0){
							/*do nothing*/
							break;
						}
						
						ret = hal_calc_exps(hal_calc.input_char_buf, hal_calc.input_char_index,\
								&sum_double,sum_str);

						hal_ls_clear();

						if (ret == 0 && sum_double<=HAL_CALC_SUM_MAX){
							//display result
							hal_calc_disp_money(sum_str);


							//test
							memset(hal_uuid_sum_str, 0x0, sizeof(hal_uuid_sum_str));
							hal_uuid_money_sum = sum_double;
							memcpy(hal_uuid_sum_str, sum_str, strlen(sum_str));
							
							hal_uuid_init();
						}else{
							//display error
							hal_key_input_error();						
						}

						hal_key_buf_clear();
						hal_calc_reset();
						
					}break;

					case HAL_KEY_CODE_BACKSPACE:{// backspace
						hal_key_get_wchar_code_backspace();
						hal_key_get_char_code_backspace();
					}break;
					
					case HAL_KEY_CODE_SUBTRACT:{// -						
						hal_key_get_wchar_code_backspace();	
						hal_key_get_char_code_backspace();
					}break;
					
					case HAL_KEY_CODE_PLUS:{// +
						hal_key_get_wchar_code(HAL_KEY_PLUS_WCHAR);
						hal_key_get_char_code(HAL_KEY_PLUS_CHAR);
					}break;
					
					case HAL_KEY_CODE_MULTIPLE:{// *
						//debug code
						if (hal_uuid_thread_start_flag){							
							hal_uuid_valid_flag = 1;
							hal_uuid_count = 0;	

							hal_uuid_qr_flag = 1;
							break;
						}

						sprintf(hal_current_balance_str, "%.2f", hal_current_balance);
						hal_calc_disp_balance(hal_current_balance_str);
						
					}break;
					case HAL_KEY_CODE_DIVIDE:{// /
						//debug code
						if (hal_uuid_thread_start_flag){							
							hal_uuid_valid_flag = 0;
							hal_uuid_count++;

							hal_uuid_qr_flag = 1;
						}
						
					}break;
					default :{// debug
						//log_err("%s\n", "invalid key");
						
					}break;
				}

				//sys_log(FUNC, LOG_DBG,"hal calc input_wchar_index is %d, input_info_len is %d\n",
					//hal_calc.input_wchar_index, hal_calc.input_info_len);
				
			} 
		}

	}

	ioctl(fd, EVIOCGRAB, (void*)0);
	
}


void hal_key_init(void)
{
	TRD_t trd_id;
	int fd = -1;

	hal_key_buf_clear();
	
	fd = g1_hal_key_open();
	if (fd<0){
		hal_ls_show_usb_keyboard();
	}
	while (fd<0){
		fd = g1_hal_key_open();
		sleep(1);
	}
	trd_create(&trd_id,(void*)&hal_key_thread, &fd);
	
	hal_ls_show_welcome();
	hal_ls_show_date_time();	
}

