#ifndef __G1_HAL_KEYBOARD_H__
#define __G1_HAL_KEYBOARD_H__

#define KEY_DEVICE	"/dev/input/event0"


void hal_key_init(void);
void hal_key_thread(void);


#endif/*__G1_HAL_KEYBOARD_H__*/
