#ifndef __G1_HAL_TIMER_H__
#define __G1_HAL_TIMER_H__

#define HAL_TIMER0_KEYBOARD_TIMEOUT 120

#define HAL_TIMER0_ENABLE 1
#define HAL_TIMER0_DISABLE 0

void hal_timer0_set_flag(int flag);

void hal_timer_init(void);

void hal_timer0_reset(void);

void hal_date_get(hal_date_t *date);

#endif/*__G1_HAL_TIMER_H__*/
