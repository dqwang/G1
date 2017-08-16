#include "g1_hal.h"

int g1_hal_key_fd = -1;

void g1_hal_key_open(void)
{
	g1_hal_key_fd = open(KEY_DEVICE,O_RDONLY);
	if (g1_hal_key_fd<0){
		log_err("%s\n", "open failed");
		return;
	}
	log_msg("open %s\n", KEY_DEVICE);
}

void g1_hal_key_close(void)
{
	if (g1_hal_key_fd)
		close(g1_hal_key_fd);
}



void hal_key_thread(void)
{
	struct input_event ev[64];
	int i, rd, fd;
	fd_set rdfs;
	static volatile sig_atomic_t stop = 0;

	u16 index = 0;
	u16 len=0;	
	wchar_t wstr[100] = L"";
	u16 sum=0;
	
	log_msg("start %s\n", "key thread");
	
	fd = g1_hal_key_fd;

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
			return;
		}

		for (i = 0; i < rd / sizeof(struct input_event); i++) {
			unsigned int type, code;

			type = ev[i].type;
			code = ev[i].code;

			//printf("Event: time %ld.%06ld, ", ev[i].time.tv_sec, ev[i].time.tv_usec);

			if (type == EV_KEY) {
				

				if (ev[i].value != 1){
					continue;
				}

				printf("type %d, code %d ",	type,code);
				if (type == EV_MSC && (code == MSC_RAW || code == MSC_SCAN))
					printf("value %02x\n", ev[i].value);
				else
					printf("value %d\n", ev[i].value);

				
				switch(code){
					case 82:{//0
						wchar_t w[]= L"0";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;

					case 79:{// 1
						wchar_t w[]= L"1";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 80:{// 2
						wchar_t w[]= L"2";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 81:{// 3
						wchar_t w[]= L"3";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 75:{// 4
						wchar_t w[]= L"4";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 76:{// 5
						wchar_t w[]= L"5";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 77:{// 6
						wchar_t w[]= L"6";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 71:{// 7
						wchar_t w[]= L"7";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 72:{// 8
						wchar_t w[]= L"8";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 73:{// 9
						wchar_t w[]= L"9";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 83:{// .
						wchar_t w[]= L".";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;

					case 96:{// enter
						wchar_t w[]= L"=";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);


						/*TODO*/

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;

					case 14:{// backspace
						wchar_t w[]= L"";
						
						if (index){
							index-=1;
							len-=2;

						
							memcpy(wstr+index, w, sizeof(w));
							memset(wstr+index, 0x0, sizeof(wstr)-len);
							/*TODO*/
							if (len != 0)
								hal_ls_CMD_SEND_INFO(wstr,len);
							else
								//hal_ls_CMD_CLEAR_INFO();
								hal_ls_CMD_SEND_INFO(wstr,sizeof(wstr));
						}					
						
					}break;
					case 78:{// +
						wchar_t w[]= L"+";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 74:{// -
						wchar_t w[]= L"-";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 55:{// *
						wchar_t w[]= L"*";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					case 98:{// /
						wchar_t w[]= L"/";
						memcpy(wstr+index,w, hal_wstr_char_len(w));
						index += hal_wstr_wchar_len(w);
						len += hal_wstr_char_len(w);

						hal_ls_CMD_SEND_INFO(wstr,len);
					}break;
					default :{// 
						log_err("%s\n", "invalid key");
					}break;
				};

				sys_log(FUNC, LOG_DBG,"wstr index is %d, len is %d\n",index, len);
				
			} 
		}

	}

	ioctl(fd, EVIOCGRAB, (void*)0);
	
}


void hal_key_init(void)
{
	TRD_t trd_id;
	g1_hal_key_open();
	trd_create(&trd_id,(void*)&hal_key_thread, NULL);
}

