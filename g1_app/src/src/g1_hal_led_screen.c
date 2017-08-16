#include "g1_hal.h"

HAL_LS_t hal_ls;

u8 hal_packet_sbuf[HAL_LS_PACKET_MAX_LEN];

static u32 packet_seq1 = 0;

HAL_LS_PARAM_CMD_SET_TIME_t hal_param_cmd_set_time;

void hal_ls_init_struct(void)
{
	memset(&hal_ls, 0x0, sizeof(HAL_LS_t));
	memset(hal_packet_sbuf, 0x0, sizeof(hal_packet_sbuf));
}

/*wide-character string example*/

//wchar_t c = L'A';  
//wchar_t szBuffer[100] = L"A String"; 

void hal_ls_info_set_cmd_send_info(const wchar_t *unicode_in,u16 info_len, wchar_t *info_out)
{
	size_t nCount;
	int i=0;
	wchar_t ch;
	
	nCount = info_len/2;//wchar

	#if 1
	{
		int i=0;
		printf("unicode_in %s:\n",__FUNCTION__);
		for (i=0;i<info_len;i++){
			printf("%02x ", ((u8*)unicode_in)[i]);
			if ((i+1)%16 == 0 )
				printf("\n");
		}
		printf("\n");
	}
	#endif
	
	for(i=0; i<nCount; i++)
	{
		ch = unicode_in[i];
		if(ch >= 0x80)
		{
			if(ch <= 0xFF)
				ch -= 0x80;
			else if(ch >= 0x2000 && ch <= 0x266F)
				ch = ch - 0x2000 + 128;
			else if(ch >= 0x3000 && ch <= 0x33FF)
				ch = ch - 0x3000 + 1648 + 128;
			else if(ch >= 0x4E00 && ch <= 0x9FA5)
				ch = ch - 0x4E00 + 1648 + 1024 + 128; 
			else if(ch >= 0xF900 && ch <= 0xFFFF)
				ch = ch - 0xF900 + 1648 + 1024 + 20902 + 128;
			ch += 128;	
		}
		info_out[i] = ch;
	}

	#if 1
	{
		int i=0;
		printf("info_out %s:\n",__FUNCTION__);
		for (i=0;i<info_len;i++){
			printf("%02x ", ((u8*)info_out)[i]);
			if ((i+1)%16 == 0 )
				printf("\n");
		}
		printf("\n");
	}
	#endif
}



//1st
int hal_ls_param_set_cmd_send_info(u8 mode,u8 flash_onoff,u8 speed,
	u8 show_time,u8 flash_time,u8 show_count,u16 info_len,
	HAL_LS_PARAM_CMD_SEND_INFO_t *param_out)
{
	if (param_out == NULL){
		return -1;
	}
	param_out->info_seq = 0x0001;
	param_out->mode = mode;
	param_out->flash_onoff = flash_onoff;
	param_out->speed = speed;
	param_out->show_time = show_time;
	param_out->flash_time = flash_time;
	param_out->show_count = show_count;

	memset(param_out->start_time, 0x0, sizeof(param_out->start_time));
	memset(param_out->stop_time, 0x0, sizeof(param_out->stop_time));

	param_out->color = 0x01;
	param_out->info_type = 0x00;
	param_out->info_current_index = 0x00;
	param_out->info_max_index = 0x00;

	param_out->info_len = info_len;
	param_out->section = 0x00;
	param_out->reserved = 0x00;

	return 0;
}

static u32 hal_ls_get_packet_seq1(void)
{
	packet_seq1++;
	return packet_seq1;
}

static u16 hal_ls_checksum(u8 *buf, u16 buf_len)
{
	u32 sum = 0;
	u16 checksum = 0;
	u16 i = 0;
	
	for (i=0; i<buf_len; i++){
		sum += buf[i];
	}
	checksum = (u16)(sum%0x10000);
	return checksum;
}
//2nd
int hal_ls_packet_set_cmd_send_info(u16 id, u8 cmd, HAL_LS_PARAM_CMD_SEND_INFO_t *param_in, wchar_t *info,
	HAL_LS_t *ls_out)
{
	if(param_in == NULL || info == NULL || ls_out == NULL){
		return -1;
	}

	ls_out->header = HAL_LS_HEAD;
	ls_out->len = HAL_LS_PACKET_MIN_LEN + param_in->info_len;
	ls_out->id = id;
	ls_out->seq1 = hal_ls_get_packet_seq1();
	ls_out->seq2 = 0;
	
	ls_out->cmd = cmd;
	ls_out->reserved1 = 0;

	memcpy(&ls_out->param, param_in, sizeof(HAL_LS_PARAM_CMD_SEND_INFO_t));

	#if 1
	{
		int i=0;
		printf("info %s:\n",__FUNCTION__);
		for (i=0;i<param_in->info_len;i++){
			printf("%02x ", ((u8*)info)[i]);
			if ((i+1)%16 == 0 )
				printf("\n");
		}
		printf("\n");
	}
	#endif
	memcpy(ls_out->info, (u8*)info, param_in->info_len);
	

	//ls_out->checksum = hal_ls_checksum();
	ls_out->checksum = 0;
	ls_out->reserved2 = 0;
	ls_out->tail = HAL_LS_TAIL;
	return 0;
}

u8 hal_get_u16_lsb(u16 n)
{
	return (u8)(n%0x100);
}

u8 hal_get_u16_msb(u16 n)
{
	return (u8)(n/0x100);
}

u8 hal_get_u32_lsb1(u32 n)
{
	return (u8)(n%0x100);
}

u8 hal_get_u32_lsb2(u32 n)
{
	return (u8)((n%0x10000)/0x100);
}

u8 hal_get_u32_lsb3(u32 n)
{
	return (u8)((n/0x10000)%0x100);
}

u8 hal_get_u32_lsb4(u32 n)
{
	return (u8)((n/0x10000)/0x100);
}

int hal_ls_cmd_send_info_param_buf_set(HAL_LS_PARAM_CMD_SEND_INFO_t *param_in, u8 *param_buf_out)
{
	if (param_in == NULL || param_buf_out == NULL){
		return -1;
	}
	param_buf_out[0]=hal_get_u16_lsb(param_in->info_seq);
	param_buf_out[1]=hal_get_u16_msb(param_in->info_seq);
	param_buf_out[2]=param_in->mode;
	param_buf_out[3]=param_in->flash_onoff;
	param_buf_out[4]=param_in->speed;
	param_buf_out[5]=param_in->show_time;
	param_buf_out[6]=param_in->flash_time;
	param_buf_out[7]=param_in->show_count;

	memset(param_buf_out+8, 0x0, 16);//time

	param_buf_out[24]=param_in->color;
	param_buf_out[25]=param_in->info_type;
	param_buf_out[26]=param_in->info_current_index;
	param_buf_out[27]=param_in->info_max_index;

	param_buf_out[28]=hal_get_u16_lsb(param_in->info_len);
	param_buf_out[29]=hal_get_u16_msb(param_in->info_len);
	param_buf_out[30]=param_in->section;
	param_buf_out[31]=param_in->reserved;	

	return 0;
}


//3rd
int hal_ls_packet_encode(HAL_LS_t *ls_in,u8 *param_buf_in, u8 *packet_out)
{
	u16 index = 0;
	u16 checksum = 0;
	if (ls_in == NULL || packet_out == NULL){
		return -1;
	}

	packet_out[0]=ls_in->header;
	
	packet_out[1]=hal_get_u16_lsb(ls_in->len);
	packet_out[2]=hal_get_u16_msb(ls_in->len);
	packet_out[3]=hal_get_u16_lsb(ls_in->id);
	packet_out[4]=hal_get_u16_msb(ls_in->id);

	packet_out[5]=hal_get_u32_lsb1(ls_in->seq1);
	packet_out[6]=hal_get_u32_lsb2(ls_in->seq1);
	packet_out[7]=hal_get_u32_lsb3(ls_in->seq1);
	packet_out[8]=hal_get_u32_lsb4(ls_in->seq1);

	packet_out[9]=hal_get_u32_lsb1(ls_in->seq2);
	packet_out[10]=hal_get_u32_lsb2(ls_in->seq2);
	packet_out[11]=hal_get_u32_lsb3(ls_in->seq2);
	packet_out[12]=hal_get_u32_lsb4(ls_in->seq2);

	packet_out[13]=ls_in->cmd;
	packet_out[14]=0;
	packet_out[15]=0;

	//32
	memcpy(packet_out+HAL_LS_HEAD_LEN, param_buf_in, HAL_LS_PARAM_LEN);

	//1024

	if (ls_in->param.info_len>0){
		memcpy(packet_out+HAL_LS_HEAD_LEN+HAL_LS_PARAM_LEN,(u8*)(ls_in->info), ls_in->param.info_len);
	}
	
	index = HAL_LS_HEAD_LEN + HAL_LS_PARAM_LEN + ls_in->param.info_len;
	checksum = hal_ls_checksum(packet_out,index);
	
	packet_out[index]= hal_get_u16_lsb(checksum);
	packet_out[index+1]=hal_get_u16_msb(checksum);
	packet_out[index+2]=ls_in->reserved2;
	packet_out[index+3]=ls_in->tail;

	return 0;
	
}
//4th
int hal_ls_packet_send(u8 *packet, u16 packet_len)
{
	return hal_ttyS0_send((u8*)packet, packet_len);
}


int hal_ls_CMD_SEND_INFO(const wchar_t *unicode_in, u16 info_len)
{
	wchar_t info[HAL_LS_INFO_MAX_LEN];
	HAL_LS_PARAM_CMD_SEND_INFO_t param;
	u8 param_buf[HAL_LS_PARAM_LEN];

	if (unicode_in == NULL || info_len ==0){
		return -1;
	}
	hal_ls_info_set_cmd_send_info(unicode_in, info_len, info);
	hal_ls_param_set_cmd_send_info(0x01,0x00,0x0E,0X02,0X02,0X00,info_len,&param);
	hal_ls_packet_set_cmd_send_info(0x0000,CMD_SEND_INFO, &param, info, &hal_ls);

	hal_ls_cmd_send_info_param_buf_set(&param, param_buf);

	hal_ls_packet_encode(&hal_ls,param_buf,hal_packet_sbuf);

	hal_ls_packet_send(hal_packet_sbuf, hal_ls.len);

	return 0;
}


void hal_ls_CMD_CLEAR_INFO(void)
{
	/*A0 34 00 01 00 12 00 00
	   00 00 00 00 00 06 00 00 
	   00 00 00 00 00 00 00 00 
	   00 00 00 00 00 00 00 00 
	   00 00 00 00 00 00 00 00 
	   00 00 00 00 00 00 00 00 
	   ED 00 00 50*/

	u8 buf[]={
		0xA0,0x34,0x00,0x01,0x00,0x12,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0xED,0x00,0x00,0x50	
	};
	hal_ttyS0_send(buf,sizeof(buf));
}




int hal_ls_param_set_time(u8 year, u8 month, u8 day, u8 hour, u8 min,
	u8 second, u8 week, u8 mode, u16 calibration, HAL_LS_PARAM_CMD_SET_TIME_t *param_out)
{
	
	if (param_out == NULL){
		return -1;
	}

	param_out->year = year;
	param_out->month = month;
	param_out->day = day;
	param_out->hour = hour;
	param_out->minute = min;
	param_out->second = second;
	param_out->week = week;
	param_out->mode = mode;
	param_out->calibration = calibration;

	memset(param_out->reserved, 0x0, sizeof(param_out->reserved));
	return 0;	
}

static int hal_ls_cmd_set_time_param_buf_set(HAL_LS_PARAM_CMD_SET_TIME_t *param_in, u8 *param_buf_out)
{
	if (param_in == NULL || param_buf_out == NULL){
		return -1;
	}

	param_buf_out[0] = param_in->year;
	param_buf_out[1] = param_in->month;
	param_buf_out[2] = param_in->day;
	param_buf_out[3] = param_in->hour;
	param_buf_out[3] = param_in->minute;
	param_buf_out[5] = param_in->second;
	param_buf_out[6] = param_in->week;
	param_buf_out[7] = param_in->mode;

	param_buf_out[8] = hal_get_u16_lsb(param_in->calibration);
	param_buf_out[9] = hal_get_u16_msb(param_in->calibration);

	memcpy(param_buf_out+10, param_in->reserved, sizeof(param_in->reserved));
	return 0;
}

int hal_ls_packet_set_cmd_set_time(u16 id, u8 cmd, HAL_LS_PARAM_CMD_SET_TIME_t*param_in, HAL_LS_t *ls_out)
{
	if(param_in == NULL || ls_out == NULL){
		return -1;
	}

	ls_out->header = HAL_LS_HEAD;
	ls_out->len = HAL_LS_PACKET_MIN_LEN;
	ls_out->id = id;
	ls_out->seq1 = hal_ls_get_packet_seq1();
	ls_out->seq2 = 0;
	
	ls_out->cmd = cmd;
	ls_out->reserved1 = 0;

	memcpy((HAL_LS_PARAM_CMD_SET_TIME_t *)&ls_out->param, param_in,HAL_LS_PARAM_LEN);
	//ls_out->checksum = hal_ls_checksum();
	ls_out->checksum = 0;
	ls_out->reserved2 = 0;
	ls_out->tail = HAL_LS_TAIL;
	
	return 0;
}



void hal_ls_CMD_SET_TIME(HAL_LS_PARAM_CMD_SET_TIME_t *param)
{
	u8 param_buf[HAL_LS_PARAM_LEN];
		
	hal_ls_cmd_set_time_param_buf_set(param, param_buf);
	hal_ls_packet_set_cmd_set_time(0x0000,CMD_SET_TIME,param,&hal_ls);
	

	hal_ls_packet_encode(&hal_ls,param_buf,hal_packet_sbuf);

	hal_ls_packet_send(hal_packet_sbuf, hal_ls.len);
}

void hal_ls_init(void)
{
	hal_ls_init_struct();

	//screen test
	//clear screen
	//hal_ls_CMD_CLEAR_INFO();
}


