#include "g1_hal.h"

HAL_CALC_t hal_calc;

#if 1
int hal_calc_is_num(char ch)
{
	if (ch >= '0' && ch <= '9'){
		return 1;
	}else{
		return 0;
	}
}

int hal_calc_is_point(char ch)
{
	if (ch == '.'){
		return 1;
	}else{
		return 0;
	}
}

int hal_calc_is_operator(char ch)
{
	if(ch == '+'){
		return 1;
	}else{
		return 0;
	}
}

double hal_calc_sum(double n1, double n2)
{
	return n1+n2;
}

int hal_calc_exps_is_valid(char *exps, u8 exps_len)
{
	u8 i=0;
	
	if (exps == NULL || exps_len == 0){
		goto error;
	}
	
	for (i=0; i<exps_len; i++){

		//printf("i = %d, exps[i] = %c\n",i, exps[i]);
		if ((i == exps_len-1) && (!hal_calc_is_num(exps[i]))){
			goto error;
		}
		
		
		if (hal_calc_is_point(exps[i])&& !hal_calc_is_num(exps[i+1])){
			goto error;
		}
		
		if (hal_calc_is_operator(exps[i])&& !hal_calc_is_num(exps[i+1])){
			goto error;
		}
	}

	return 1;
error:
	return 0;
}

void hal_calc_reset(void)
{
	memset(&hal_calc,0x0, sizeof(HAL_CALC_t));

	hal_calc.disp_space_num = HAL_CALC_DISP_WCHAR_BUF_SIZE;
	memcpy(hal_calc.disp_wchar_buf, HAL_CALC_8_SPACE_WCHAR, HAL_CALC_DISP_INFO_LEN);
}

void hal_calc_disp(void)
{
	u8 mode=0x01;
	u8 flash_onoff=0x00;
	u8 speed=0x0E;
	u8 show_time=0x00;
	u8 flash_time=0x00;
	u8 show_count=0x00;
	
	hal_ls_CMD_SEND_INFO(hal_calc.disp_wchar_buf, HAL_CALC_DISP_INFO_LEN,mode,
	flash_onoff,speed,show_time,flash_time,show_count);
}

void hal_calc_set_wchar_buf(wchar_t *input_wchar_buf, u8 input_wchar_index,u8 len)
{
	if (input_wchar_buf == NULL){

		log_err("%s", "Null Pointer");
		return;
	}

	if(len == 0){
		log_err("len %s", "is 0");
		hal_calc_reset();
		hal_calc_disp();
		return;
	}
	
	memcpy(hal_calc.input_wchar_buf, input_wchar_buf, len);
	hal_calc.input_wchar_index = input_wchar_index;
	hal_calc.input_info_len = len;

	
	hal_calc.disp_space_num = (hal_calc.input_wchar_index<HAL_CALC_DISP_WCHAR_BUF_SIZE)\
		?(HAL_CALC_DISP_WCHAR_BUF_SIZE-1-hal_calc.input_wchar_index):0;

	printf("hal_calc.disp_space_num is %d\n", hal_calc.disp_space_num);
	if (hal_calc.disp_space_num >0){//disp all if less than 8 wchar
		memcpy(hal_calc.disp_wchar_buf, HAL_CALC_8_SPACE_WCHAR, 2*hal_calc.disp_space_num);	//wchar, so num*2
	
		hal_calc.disp_wchar_index = (HAL_CALC_DISP_INFO_LEN-hal_calc.input_info_len)/2;
		memcpy(hal_calc.disp_wchar_buf+hal_calc.disp_wchar_index,hal_calc.input_wchar_buf,\
			hal_calc.input_info_len);
	}else{// disp the last 8 wchar if more than 8 wchar
		hal_calc.disp_wchar_index = 0;
		
		memcpy(hal_calc.disp_wchar_buf, \
			hal_calc.input_wchar_buf + hal_calc.input_wchar_index-HAL_CALC_DISP_WCHAR_BUF_SIZE+1,\
			HAL_CALC_DISP_INFO_LEN);
	}

	hal_calc_disp();	
}



int hal_calc_exps(char *exps, u8 exps_len, double *result, char *result_str)
{
	u8 i=0;

	sys_log(FUNC, LOG_DBG, "exps is: %s exps_len :%d\n", exps, exps_len);

	//exps is valid?
	if (!hal_calc_exps_is_valid(exps, exps_len)){
		log_err("exps is invalid: %s\n", exps);
		return -1;
	}	

	//split
	//double atof(const char *nptr)
	hal_calc.num_index = 0;
	hal_calc.sum = 0;
	memset(hal_calc.num, 0x0, sizeof(hal_calc.num));
	
	char* p = strtok(exps, "+");
	if (p){
			log_debug("%s\n",p);
			hal_calc.num[hal_calc.num_index++] = atof(p);//string to double
	}else{
		log_err("NULL %p\n", p);
	}

	while (1){
		p = strtok(NULL, "+");
		if(p){
				log_debug("%s\n",p);
				hal_calc.num[hal_calc.num_index++] = atof(p);//string to double
		}else{
			log_err("NULL %p\n", p);
			break;
		}
	}

	//calc

	for (i=0;i<hal_calc.num_index; i++){
		printf("%f\n", hal_calc.num[i]);
		hal_calc.sum += hal_calc.num[i];
	}
	
	sys_log(FUNC, LOG_DBG,"before floor: hal_calc.sum is %f\n",hal_calc.sum);
	
	//return result,  四舍五入,保留2位小数
	hal_calc.sum *= 100 ;
    hal_calc.sum += 0.5 ;
    hal_calc.sum =floor(hal_calc.sum);
    hal_calc.sum /= 100;

	sprintf(hal_calc.sum_str, "%.2f", hal_calc.sum);
	sys_log(FUNC, LOG_DBG,"after floor: hal_calc.sum=%f hal_calc.sum_str %s\n", hal_calc.sum, hal_calc.sum_str);

	*result = hal_calc.sum;
	memcpy(result_str, hal_calc.sum_str, sizeof(hal_calc.sum_str));
	return 0;
}

#endif


void hal_calc_disp_money(char *money_str)
{
	wchar_t money_wchar[HAL_CALC_SUM_WCHAR_STR_LEN]={0x0000};
	u16 info_len = 0;

	u8 mode;
	u8 flash_onoff=0x00;
	u8 speed;
	u8 show_time;
	u8 flash_time=0x00;
	u8 show_count=0x00;

	//money_wchar[0]=L'$';
	money_wchar[0]=0x00a5;//￥

	hal_ls_char2wchar(money_str,strlen(money_str) ,money_wchar+1);

	info_len = strlen(money_str)*2+2;
	if (info_len<HAL_CALC_DISP_INFO_LEN){
		mode=0x01;//不足一行,立刻显示
		speed=0x0E;
	}else{
		mode=0x0D;//超过一行,左移显示
		speed=0x0F;
		show_time=0x02;
	}
	
	hal_ls_CMD_SEND_INFO(money_wchar, info_len,mode,
		flash_onoff,speed,show_time,flash_time,show_count);//￥+ money_str
}


void hal_calc_disp_money_successfully(char *money_str)//消费成功账户余额xxxxxx.xx元
{
	wchar_t money_wchar[HAL_CALC_SUM_WCHAR_STR_LEN]={0x00};
	u16 info_len = 0;

	u8 mode=0x0d;
	u8 flash_onoff=0x00;
	u8 speed=0x0f;
	u8 show_time=0x02;
	u8 flash_time=0x00;
	u8 show_count=0x00;

	
	money_wchar[0]=0x6d88;//消费成功
	money_wchar[1]=0x8d39;
	money_wchar[2]=0x6210;
	money_wchar[3]=0x529f;

	money_wchar[4]=0x8d26;//账户余额
	money_wchar[5]=0x6237;
	money_wchar[6]=0x4f59;
	money_wchar[7]=0x989d;
	
	hal_ls_char2wchar(money_str,strlen(money_str) ,money_wchar+8);

	money_wchar[8+strlen(money_str)]=0x5143;//元

	info_len = strlen(money_str)*2+9*2;
		
	hal_ls_CMD_SEND_INFO(money_wchar, info_len,mode,
		flash_onoff,speed,show_time,flash_time,show_count);//￥+ money_str

	
}


void hal_calc_disp_balance(char *money_str)//账户余额xxxxxx.xx元
{
	wchar_t money_wchar[HAL_CALC_SUM_WCHAR_STR_LEN]={0x00};
	u16 info_len = 0;

	u8 mode=0x0d;
	u8 flash_onoff=0x00;
	u8 speed=0x0f;
	u8 show_time=0x02;
	u8 flash_time=0x00;
	u8 show_count=0x00;

	
	money_wchar[0]=0x8d26;//账户余额
	money_wchar[1]=0x6237;
	money_wchar[2]=0x4f59;
	money_wchar[3]=0x989d;
	
	hal_ls_char2wchar(money_str,strlen(money_str) ,money_wchar+4);

	money_wchar[4+strlen(money_str)]=0x5143;//元

	info_len = strlen(money_str)*2+5*2;
		
	hal_ls_CMD_SEND_INFO(money_wchar, info_len,mode,
		flash_onoff,speed,show_time,flash_time,show_count);//￥+ money_str

	
}


