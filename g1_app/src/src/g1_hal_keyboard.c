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

	wchar_t wstr[] = L"0"; 
	
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
			return 1;
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
						memcpy(wstr, L"0", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;

					case 79:{// 1
						memcpy(wstr, L"1", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 80:{// 2
						memcpy(wstr, L"2", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 81:{// 3
						memcpy(wstr, L"3", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 75:{// 4
						memcpy(wstr, L"4", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 76:{// 5
						memcpy(wstr, L"5", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 77:{// 6
						memcpy(wstr, L"6", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 71:{// 7
						memcpy(wstr, L"7", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 72:{// 8
						memcpy(wstr, L"8", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 73:{// 9
						memcpy(wstr, L"9", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 83:{// .
						memcpy(wstr, L".", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;

					case 96:{// enter

					}break;

					case 14:{// backspace

					}break;
					case 78:{// +
						memcpy(wstr, L"+", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 74:{// -
						memcpy(wstr, L"-", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 55:{// *
						memcpy(wstr, L"*", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					case 98:{// /
						memcpy(wstr, L"/", 4);
						hal_ls_CMD_SEND_INFO(wstr,hal_wstr_len(wstr));
					}break;
					default :{// 
						log_err("%s\n", "invalid key");
					}break;
				};

				
			} 
		}

	}

	ioctl(fd, EVIOCGRAB, (void*)0);
	return EXIT_SUCCESS;
}


void hal_key_init(void)
{
	TRD_t trd_id;
	g1_hal_key_open();
	trd_create(&trd_id,(void*)&hal_key_thread, NULL);
}

