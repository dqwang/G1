#ifndef __G1_HAL_LED_SCREEN_H__
#define __G1_HAL_LED_SCREEN_H__

#include "g1_hal_type.h"

#define HAL_LS_PACKET_MAX_LEN 1076
#define HAL_LS_PACKET_MIN_LEN 52

#define HAL_LS_HEAD_LEN 16
#define HAL_LS_PARAM_LEN 32
#define HAL_LS_INFO_MAX_LEN 1024/2

#define HAL_LS_PARAM_TIME_LEN 8

#define HAL_LS_HEAD 0xA0
#define HAL_LS_TAIL 0x50

#define HAL_LS_ACK_HEAD 0xAF
#define HAL_LS_ACK_TAIL 0x50


typedef enum {
	CMD_SEND_EXTEND_INFO=0x02,
	CMD_SEND_INFO=0x03,
	
	CMD_DEL_INFO=0x05,
	CMD_CLEAR_INFO=0x06,

	CMD_STOP_ALARM_INFO=0x08,
	CMD_SET_ALARM_INFO=0x09,

	CMD_SET_BRIGHTNESS=0x10,
	CMD_SET_BRIGHTNESS_TIMING=0x11,
	CMD_SET_BRIGHTNESS_AUTO=0x12,

	CMD_GET_BRIGHTNESS=0x13,

	CMD_SET_TMP_STAT=0X19,
	CMD_SET_PERM_STAT=0X20,

	CMD_SET_TIME=0X21,
	CMD_GET_TIME=0X22,

	CMD_SET_HW_PARAM=0X23,
	CMD_GET_HW_PARAM=0X24,

	CMD_SET_ID=0X25,

	CMD_SET_AUTO_POWER_ONOFF=0X26,
	CMD_GET_AUTO_POWER_ONOFF=0X27,

	CMD_SET_BAUD=0X28,
	CMD_SET_SECTION=0X29,

	CMD_TEST=0X30,
	CMD_CHECKSUM_ONOFF=0X31,

	CMD_SET_HW_REBOOT=0X32,
	CMD_RESET_SOFT_PARAM=0X33,

	CMD_GET_FW_VERSION=0X35
}HAL_LS_CMD_e;


typedef struct HAL_LS_PARAM_CMD_SEND_INFO_{
	u16 info_seq;
	u8 mode;
	u8 flash_onoff;
	
	u8 speed;
	u8 show_time;
	u8 flash_time;
	u8 show_count;

	u8 start_time[HAL_LS_PARAM_TIME_LEN];
	u8 stop_time[HAL_LS_PARAM_TIME_LEN];

	u8 color;
	u8 info_type;
	u8 info_current_index;
	u8 info_max_index;

	u16 info_len;
	u8 section;
	u8 reserved;	
}HAL_LS_PARAM_CMD_SEND_INFO_t;

typedef struct HAL_LS_PARAM_CMD_SET_TIME_{
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	
	u8 minute;
	u8 second;
	u8 week;
	u8 mode;

	u16 calibration;// 0-20000

	u8 reserved[22];

}HAL_LS_PARAM_CMD_SET_TIME_t;

typedef struct HAL_LS_PARAM_CMD_GET_TIME_{
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	
	u8 minute;
	u8 second;
	u8 week;
	u8 mode;

	u16 calibration;// 0-20000

	u8 reserved[22];
}HAL_LS_PARAM_CMD_GET_TIME_t;

typedef struct hal_led_screen{
	u8 header;//0xA0
	u16 len;
	u16 id;
	u32 seq1;
	u32 seq2;//not used
	u8 cmd;
	u16 reserved1;//not used

	HAL_LS_PARAM_CMD_SEND_INFO_t param;
	wchar_t info[HAL_LS_INFO_MAX_LEN];

	u16 checksum;
	u8 reserved2;//not used
	u8 tail;//0x50
}HAL_LS_t;

/*display API*/

//0
void hal_ls_info_set_cmd_send_info(const wchar_t *unicode_in,u16 info_len, wchar_t *info_out);

//1st
int hal_ls_param_set_cmd_send_info(u8 mode,u8 flash_onoff,u8 speed,
	u8 show_time,u8 flash_time,u8 show_count,u16 info_len,
	HAL_LS_PARAM_CMD_SEND_INFO_t *param_out);
//2nd
int hal_ls_packet_set_cmd_send_info(u16 id, u8 cmd, HAL_LS_PARAM_CMD_SEND_INFO_t *param_in, wchar_t *info,
	HAL_LS_t *ls_out);



//3rd
int hal_ls_packet_encode(HAL_LS_t *ls_in,u8 *param_buf_in, u8 *packet_out);

//4th
int hal_ls_packet_send(u8 *packet, u16 packet_len);



//int hal_ls_CMD_SEND_INFO(const wchar_t *unicode_in);
int hal_ls_CMD_SEND_INFO(const wchar_t *unicode_in, u16 info_len);


/*Other API*/




int hal_ls_packet_decode(u8 *packet_in, HAL_LS_t *ls_out);

void hal_ls_CMD_CLEAR_INFO(void);



void hal_ls_init(void);

//#define hal_wstr_len(p)	(wcslen(p)/4) //not work
#define hal_wstr_char_len(p)	(sizeof(p)-2)//字节数
#define hal_wstr_wchar_len(p)	((sizeof(p)-2)/2)//宽字节数


int hal_ls_param_set_time(u8 year, u8 month, u8 day, u8 hour, u8 min,
	u8 second, u8 week, u8 mode, u16 calibration, HAL_LS_PARAM_CMD_SET_TIME_t *param_out);

void hal_ls_CMD_SET_TIME(HAL_LS_PARAM_CMD_SET_TIME_t *param);

void hal_ls_CMD_CLEAR_INFO(void);



#endif/*__G1_HAL_LED_SCREEN_H__*/
