#ifndef __G1_HAL_KEYBOARD_H__
#define __G1_HAL_KEYBOARD_H__

#define KEY_DEVICE	"/dev/input/event0"

/*KEYBOARD Code when key down*/
#define HAL_KEY_CODE_0 82
#define HAL_KEY_CODE_1 79
#define HAL_KEY_CODE_2 80
#define HAL_KEY_CODE_3 81
#define HAL_KEY_CODE_4 75
#define HAL_KEY_CODE_5 76
#define HAL_KEY_CODE_6 77
#define HAL_KEY_CODE_7 71
#define HAL_KEY_CODE_8 72
#define HAL_KEY_CODE_9 73
#define HAL_KEY_CODE_PLUS 78
#define HAL_KEY_CODE_SUBTRACT 74
#define HAL_KEY_CODE_MULTIPLE 55
#define HAL_KEY_CODE_DIVIDE 98
#define HAL_KEY_CODE_ENTER 96
#define HAL_KEY_CODE_BACKSPACE 14
#define HAL_KEY_CODE_POINT 83

/*UNICODE WCHAR for LED Display*/
#define HAL_KEY_0_WCHAR L"0"
#define HAL_KEY_1_WCHAR L"1"
#define HAL_KEY_2_WCHAR L"2"
#define HAL_KEY_3_WCHAR L"3"
#define HAL_KEY_4_WCHAR L"4"
#define HAL_KEY_5_WCHAR L"5"
#define HAL_KEY_6_WCHAR L"6"
#define HAL_KEY_7_WCHAR L"7"
#define HAL_KEY_8_WCHAR L"8"
#define HAL_KEY_9_WCHAR L"9"

#define HAL_KEY_POINT_WCHAR L"."
#define HAL_KEY_PLUS_WCHAR L"+"


/*UNICODE CHAR for calculator*/
#define HAL_KEY_0_CHAR "0"
#define HAL_KEY_1_CHAR "1"
#define HAL_KEY_2_CHAR "2"
#define HAL_KEY_3_CHAR "3"
#define HAL_KEY_4_CHAR "4"
#define HAL_KEY_5_CHAR "5"
#define HAL_KEY_6_CHAR "6"
#define HAL_KEY_7_CHAR "7"
#define HAL_KEY_8_CHAR "8"
#define HAL_KEY_9_CHAR "9"

#define HAL_KEY_POINT_CHAR "."
#define HAL_KEY_PLUS_CHAR "+"

#define HAL_WCHAR_INDEX 1
#define HAL_WCHAR_SIZE 2

#define HAL_CHAR_INDEX 1
#define HAL_CHAR_SIZE 1


void hal_key_init(void);
void hal_key_thread(int *p_fd);

void hal_key_buf_clear(void);

#endif/*__G1_HAL_KEYBOARD_H__*/
