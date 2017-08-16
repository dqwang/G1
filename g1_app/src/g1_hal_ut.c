#include "g1_hal.h"

void test_thread(void)
{
	int i=0;
	
	while(1){
		sleep(1);
		log_debug("test thread: %d\n",i++);
	}
}

void hal_ut_thread(void)
{
	TRD_t trd_id;

	trd_create(&trd_id, (void*)&test_thread, NULL);
}

void hal_ut_ttyS0_send_thread(void)
{
	char buf[]={0,1,2,3,4};

	while (1){
		sleep(1);
		hal_ttyS0_send(buf,sizeof(buf));
	}
}

void hal_ut_ttyS0_echo(void)
{
	TRD_t trd_id;
	
	//open
	hal_ttyS0_open();

	//set
	hal_ttyS0_set(57600,8,3,1);
	//recv thread
	trd_create(&trd_id, (void*)&hal_ttyS0_recv_thread, NULL);
	//send
	trd_create(&trd_id, (void*)&hal_ut_ttyS0_send_thread, NULL);

	//close
	//hal_ttyS0_close();
}

void hal_ut_LED_screen(void)
{

	/*u8 buf[]={
		0xA0,
		0x3C,0x00,
		0x01,0x00,
		0x05,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x03,
		0x00,0x00,

		0x01,0x00,0x0D,0x10,0x0E,0x00,0x02,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x03,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
		
		0x81,0x1F,0xA1,0x2F,0x81,0x1F,0x26,0x19,
		0x6D,0x03,0x00,0x50};*/
	u8 buf[]={
		0xA0,0x70,0x00,0x01,0x00,0x06,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,

		0x02,0x00,0x01,0x00,0x0E,0x02,0x02,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x01,0x05,0x00,0x00,0x3C,0x00,0x00,0x00,

		0x3A,0x0C,0x99,0x16,0x9F,0x23,0x32,0x00,
		0x30,0x00,0x0B,0x00,0x0C,0x00,0xE4,0x1B,
		0x0D,0x00,0x0E,0x00,0x78,0x24,0x0F,0x00,
		0x10,0x00,0x55,0x23,0x20,0x00,0x8F,0x23,
		0x8F,0x24,0x15,0x00,0x22,0x63,0x20,0x31,
		0x98,0x14,0x66,0x23,0x64,0x53,0x11,0x00,
		0x12,0x00,0x66,0x23,0x13,0x00,0x14,0x00,
		0x76,0x0F,0x72,0x07,

		0x1B,0x0C,0x00,0x50
	};
	/*
	u8 buf[]={

	//head
	0xA0,//header
	0x40,0x00,//len: lsb, msb
	0x01,0x00,//id: lsb, msb  (00 00, broadcast)
	0x04,0x00,0x00,0x00,//seq :LSB 32bits
	0x00,0x00,0x00,0x00,//seq:MSB 32bits, reserved
	0x03,//cmd , or  error code
	0x00,0x00,//reserved , default 0000

	//param 32bytes
	0x01,0x00,//info seq
	0x01,//mode 动画效果
	0x00,//flash on/off 停留方式
	0x0E,//speed
	0x02,//show time
	0x02,//flash time
	0x00,//show count
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//start time
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//stop time
	0x01,//color
	0x00,//info type
	0x00,//info current index 
	0x00,//info max index
	0x0C,0x00,//info len
	0x00,//section
	0x00,//reserved

	//info, 1024bytes max
	0x4C,0x00,0x45,0x00,0x44,0x00,0xAE,0x23,
	0xAA,0x36,0xBF,0x19,

	//tail
	0x67,0x04,//checksum, sum of all bytes before checksum, 16bits
	0x00,//reserved
	0x50//tail
	};*/
	
	hal_ttyS0_send(buf,sizeof(buf));
}

//wchar_t c = L'A';  
//wchar_t szBuffer[100] = L"A String"; 

void hal_ut_LED_screen_unicode(void)
{
	//const wchar_t * wstr = L"高锦科技";
	wchar_t wstr[] = L"1+2+34 = 37"; 

	//char xxx[]="请扫描二维码";
	u16 nCount = hal_wstr_char_len(wstr);

	#if 1
	{
		int i=0;
		printf("wstr %s: len:%d\n",__FUNCTION__,nCount);
		for (i=0;i<nCount;i++){
			printf("%02x ", ((u8*)wstr)[i]);
			if ((i+1)%16 == 0 )
				printf("\n");
		}
		printf("\n");
	}
	#endif
	
	hal_ls_CMD_SEND_INFO(wstr,hal_wstr_char_len(wstr));	
}


void hal_ut_LED_screen_set_time(void)
{
	HAL_LS_PARAM_CMD_SET_TIME_t param;

	hal_ls_CMD_CLEAR_INFO();

	
	hal_ls_param_set_time(0x17,0x08,0x16,0x15,0x10,0x00,0x03,0x00,10000,&param);


	hal_ls_CMD_SET_TIME(&param);
}


void hal_ut_wstr_test(void)
{
	//setlocale(LC_ALL,"zh_CN.utf8");
	u16 index = 0;
	u16 len=0;
	wchar_t wstr[50]=L"";// 50bytes
	
	wchar_t wstr0[] = L"2";
	wchar_t wstr1[] = L"+";
	wchar_t wstr2[] = L"7";
	wchar_t wstr3[] = L"+";
	wchar_t wstr4[] = L"8";
	
	memcpy(wstr+index,wstr0, hal_wstr_char_len(wstr0));
	index += hal_wstr_wchar_len(wstr0);
	len += hal_wstr_char_len(wstr0);
	
	memcpy(wstr+index,wstr1, hal_wstr_char_len(wstr1));
	index += hal_wstr_wchar_len(wstr1);
	len += hal_wstr_char_len(wstr1);
	
	memcpy(wstr+index,wstr2, hal_wstr_char_len(wstr2));
	index += hal_wstr_wchar_len(wstr2);
	len += hal_wstr_char_len(wstr2);
	memcpy(wstr+index,wstr3, hal_wstr_char_len(wstr3));
	index += hal_wstr_wchar_len(wstr3);
	len += hal_wstr_char_len(wstr3);
	memcpy(wstr+index,wstr4, hal_wstr_char_len(wstr4));
	index += hal_wstr_wchar_len(wstr4);
	len += hal_wstr_char_len(wstr4);

	
	#if 1
	{
		int i=0;
		//printf("wstr %s: len:%d\n",__FUNCTION__,nCount);
		for (i=0;i<len;i++){
			printf("%02x ", ((u8*)wstr)[i]);
			if ((i+1)%16 == 0 )
				printf("\n");
		}
		printf("\n");
	}
	#endif

	hal_ls_CMD_SEND_INFO(wstr,len);
#if 0
	wchar_t a[10] = L"hello";  

	wprintf(L"xxx %ls", a);
	wprintf(L"yyy %ls", a);

	printf("wcslen(a) is %d, sizeof(a) is %d\n", wcslen(a),sizeof(a));

	//wchar_t *wc = L"我是中国人"; 
	wchar_t *wc = L"GREAT WALL"; 
	//wprintf(L"%ls",wc);

	//printf("%d %d %d %d %d \N", wcslen(L"1"),wcslen(L"12"),wcslen(L"123"),wcslen(L"1234"),wcslen(L"12345"));


	wprintf(L"this is a test !\n");    
    wprintf(L"%d\n",wcslen(a));      
    wprintf(L"%ls\n",a);

	printf("%s\n", a);
	wprintf(L"%ls\n", a);

	//wcscat(wstr, wstr1);
	//wcscat(wstr, wstr2);
	
	//swprintf(wstr, 4, L"%s", L"1234");
	
#endif
	
	#if 0
	{
		int i=0;
		//printf("wstr %s: len:%d\n",__FUNCTION__,nCount);
		for (i=0;i<10;i++){
			printf("%02x ", ((u8*)wstr)[i]);
			if ((i+1)%16 == 0 )
				printf("\n");
		}
		printf("\n");
	}
	#endif

	

	//hal_ls_CMD_SEND_INFO(wstr,hal_wstr_char_len(wstr));
}