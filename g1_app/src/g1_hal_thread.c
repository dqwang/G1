#include "g1_hal.h"

int trd_create(TRD_t *trd_id, void *(*func)(void *), void *arg)
{
    int ret = -1;

    ret = pthread_create(trd_id, NULL, func, arg);
	

    if (ret != 0)
    {
        perror("trd_create: pthread_create");
        return -1;
    }

    ret = pthread_detach(*trd_id);

    if (ret != 0)
    {
        perror("trd_create: pthread_detach");
        return -2;
    }

    return 0;
}

