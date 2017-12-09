#ifndef __G1_HAL_CALC_H__
#define __G1_HAL_CALC_H__

#include "g1_hal_type.h"

#define HAL_CALC_INPUT_WCHAR_BUF_SIZE 100
#define HAL_CALC_DISP_WCHAR_BUF_SIZE 8

#define HAL_CALC_DISP_INFO_LEN (2*HAL_CALC_DISP_WCHAR_BUF_SIZE)


#define HAL_CALC_INPUT_CHAR_BUF_SIZE HAL_CALC_INPUT_WCHAR_BUF_SIZE
#define HAL_CALC_NUM_BUF_SIZE 25

#define HAL_CALC_8_SPACE_WCHAR L"        "

#define HAL_CALC_SUM_CHAR_STR_LEN 16
#define HAL_CALC_SUM_WCHAR_STR_LEN HAL_CALC_SUM_CHAR_STR_LEN

#define HAL_CALC_SUM_MAX 999999.99

typedef struct hal_calc{
	wchar_t input_wchar_buf[HAL_CALC_INPUT_WCHAR_BUF_SIZE];
	u8 input_wchar_index;
	u8 input_info_len;

	wchar_t disp_wchar_buf[HAL_CALC_DISP_WCHAR_BUF_SIZE];
	u8 disp_wchar_index;
	
	u8 disp_space_num;

	char input_char_buf[HAL_CALC_INPUT_CHAR_BUF_SIZE];
	u8 input_char_index;
	
	double num[HAL_CALC_NUM_BUF_SIZE];
	u8 num_index;
	double sum;
	char sum_str[HAL_CALC_SUM_CHAR_STR_LEN];//xxxxxx.xx

}HAL_CALC_t;


extern HAL_CALC_t hal_calc;

extern double today_income;
extern char today_income_str[HAL_CALC_SUM_CHAR_STR_LEN];//xxxxxx.xx

void hal_calc_reset(void);
void hal_calc_disp(void);

void hal_calc_set_wchar_buf(wchar_t *input_wchar_buf, u8 input_wchar_index,u8 len);


int hal_calc_exps(char *exps, u8 exps_len, double *result, char *result_str);
void hal_calc_disp_money(char *money_str);
void hal_calc_disp_money_successfully(char *money_str);
void hal_calc_disp_balance(char *money_str);
void hal_calc_disp_today_income(char *money_str);



#endif/*__G1_HAL_CALC_H__*/
