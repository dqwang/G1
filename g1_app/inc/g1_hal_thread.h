#ifndef __G1_HAL_THREAD_H__
#define __G1_HAL_THREAD_H__
#include <semaphore.h>
#include <pthread.h>


typedef pthread_t		TRD_t;

int trd_create(TRD_t *trd_id, void *(*func)(void *), void *arg);

#endif/*__G1_HAL_THREAD_H__*/

