#ifndef __G1_HAL_TTYS_H__
#define __G1_HAL_TTYS_H__

#define TTY_DEV "/dev/ttyS0"
#define TTYS0_RBUF_MAX (1024)


void hal_ttyS0_open(void);
void hal_ttyS0_close(void);

int hal_ttyS0_set(int baud,int databits, int parity, int stopbits);

void hal_ttyS0_recv_thread(void);
int hal_ttyS0_send(void *ptr, int nbytes);

void hal_ttyS0_init(void);




#endif/*__G1_HAL_TTYS_H__*/

