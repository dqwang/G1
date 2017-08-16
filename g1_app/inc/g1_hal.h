#ifndef __G1_HAL_H__
#define __G1_HAL_H__

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

/*! standard C library */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

/*! Linux system calls */
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/select.h>
#include <sys/vfs.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>
#include <sys/types.h>
//#include <sys/io.h>
#include <sys/sendfile.h>

/*! other interfaces */
#include <time.h>
#include <string.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <termios.h>
#include <dirent.h>
#include <crypt.h>
#include <limits.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>

#include <linux/version.h>
#include <linux/input.h>

#include <errno.h>
#include <getopt.h>
#include <ctype.h>

#include "g1_hal_type.h"
#include "g1_hal_keyboard.h"
#include "g1_hal_led_screen.h"
#include "g1_hal_ttyS.h"
#include "g1_hal_log.h"
#include "g1_hal_thread.h"

#include "g1_hal_ut.h"

#endif/*__G1_HAL_H__*/

