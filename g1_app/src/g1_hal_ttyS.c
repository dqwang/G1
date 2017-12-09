#include "g1_hal.h"

int hal_ttyS0_fd = -1;

void hal_ttyS0_open(void)
{  
	hal_ttyS0_fd =  open(TTY_DEV,O_RDWR|O_NONBLOCK|O_NOCTTY);
	if(hal_ttyS0_fd>0){
		fcntl(hal_ttyS0_fd,F_SETFL,0);
	}
	sys_log(FUNC, LOG_DBG,"open %s, fd is %d\n",TTY_DEV,hal_ttyS0_fd);
}

void hal_ttyS0_close(void)
{
	if(hal_ttyS0_fd>0){
		close(hal_ttyS0_fd);
	}
}

int hal_ttyS0_set(int baud,int databits, int parity, int stopbits)
{
	if (hal_ttyS0_fd <= 0)
		return -1;

	printf("(%s),hal_ttyS0_fd=%d,baud=%d,databits=%d,parity=%d,stopbits=%d\n", \
		__FUNCTION__, hal_ttyS0_fd, baud, databits, parity, stopbits);
	
	struct termios ntm;
	bzero(&ntm, sizeof(ntm));
	tcgetattr(hal_ttyS0_fd, &ntm);
	ntm.c_cflag &= ~CSIZE;

	switch (baud)
	{
		case 300:
			cfsetispeed( &ntm, B300 );
			cfsetospeed( &ntm, B300 );
		break;   
		
		case 600:
			cfsetispeed( &ntm, B600 );
			cfsetospeed( &ntm, B600 );
		break;
		
		case 1200:
			cfsetispeed( &ntm, B1200 );
			cfsetospeed( &ntm, B1200 );
		break;
		
		case 2400:
			cfsetispeed( &ntm, B2400 );
			cfsetospeed( &ntm, B2400 );
		break;
		
		case 4800:
			cfsetispeed( &ntm, B4800 );
			cfsetospeed( &ntm, B4800 );
		break;
		
		case 9600:
			cfsetispeed( &ntm, B9600 );
			cfsetospeed( &ntm, B9600 );
		break;
		
		case 19200:
			cfsetispeed( &ntm, B19200 );
			cfsetospeed( &ntm, B19200 );
		break;
		
		case 38400:
			cfsetispeed( &ntm, B38400 );
			cfsetospeed( &ntm, B38400 );
		break;
		
		case 57600:
			cfsetispeed( &ntm, B57600 );
			cfsetospeed( &ntm, B57600 );
		break;
		
		case 115200:
			cfsetispeed( &ntm, B115200 );
			cfsetospeed( &ntm, B115200 );
		break;
		
		case 230400:
			cfsetispeed( &ntm, B230400 );
			cfsetospeed( &ntm, B230400 );
		break;	
		
		case 460800:
			cfsetispeed( &ntm, B460800 );
			cfsetospeed( &ntm, B460800 );
		break;
		
		case 576000:
			cfsetispeed( &ntm, B576000 );
			cfsetospeed( &ntm, B576000 );
		break;
		
		case 921600:
			cfsetispeed( &ntm, B921600 );
			cfsetospeed( &ntm, B921600 );
		break;
		
		default:
			cfsetispeed( &ntm, B115200 );
			cfsetospeed( &ntm, B115200 );
		break;
	}
	switch (databits)
	{
		case 5:
			ntm.c_cflag |= CS5;
		break;
		
		case 6:
			ntm.c_cflag |= CS6;
		break;
		
		case 7:
			ntm.c_cflag |= CS7;
		break;
		
		case 8:
			ntm.c_cflag |= CS8;
		break;

		default:
			printf("com_params_set: data bits value[%d] is illegal!\n", databits);
		return -2;
	}

	switch (parity)
	{
		case 0://None
			ntm.c_cflag &= ~PARENB;
			ntm.c_cflag &= ~PARODD;
			ntm.c_iflag &= ~(INPCK | ISTRIP);
		break;
		
		case 1://Odd Parity Check
			ntm.c_cflag  |= PARENB;
			ntm.c_cflag  |= PARODD;
			ntm.c_iflag  |=(INPCK | ISTRIP);
		break;
		
		case 2://even parity check
			ntm.c_iflag  |=(INPCK | ISTRIP);
			ntm.c_cflag  |=PARENB;
			ntm.c_cflag  &=~ PARODD;
		break;
		
		default:
			printf("com_params_set: parity value[%d] is illegal!\n", parity);
		return -3;
	}

	//set stop bits
	switch (stopbits)
	{
		case 1:
			ntm.c_cflag &=~CSTOPB;
		break;
		
		case 2:
			ntm.c_cflag |=CSTOPB;
		break;
		
		default:
			printf("com_params_set: stop bits value[%d] is illegal!\n", stopbits);
		return -4;
	}
	ntm.c_cflag |= CLOCAL | CREAD;    
	ntm.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	ntm.c_oflag &= ~OPOST;
	ntm.c_cc[VTIME]=0;
	ntm.c_cc[VMIN]=0;
	tcflush(hal_ttyS0_fd, TCIOFLUSH);

	if (tcsetattr(hal_ttyS0_fd, TCSANOW, &ntm) != 0)
	{
		perror("com_params_set: tcsetattr");
		return -5;
	}

	return 0;
}


void hal_ttyS0_recv_thread(void)
{
	char ttyS0_rbuf[TTYS0_RBUF_MAX];
	int len;
	log_msg("start hal_ttyS0_fd=%d thread", hal_ttyS0_fd);
	
	while(1){
		len = read(hal_ttyS0_fd, ttyS0_rbuf, TTYS0_RBUF_MAX);				
		if(len > 0)	{

			#if 1
			{
				int i=0;
				printf("ttyS0 recv %s:\n",__FUNCTION__);
				for (i=0;i<len;i++){
					printf("%02x ", ttyS0_rbuf[i]);
					if ((i+1)%16 == 0 )
						printf("\n");
				}
				printf("\n");
			}
			#endif
			
			#if 0//just for testing uart
			hal_ttyS0_send(ttyS0_rbuf, len);
			#endif				
		//TODO
		
		}
	}
}


int hal_ttyS0_send(void *ptr, int nbytes)
{
    int  nleft, nwriten ;

    nleft = nbytes ;
	
#if 1
{
	int i=0;
	printf("ttyS0 send %s:\n",__FUNCTION__);
	for (i=0;i<nbytes;i++){
		printf("%02x ", ((u8*)ptr)[i]);
		if ((i+1)%16 == 0 )
			printf("\n");
	}
	printf("\n");
}
#endif
	while ( nleft > 0 )
    {
        nwriten = write(hal_ttyS0_fd, ptr, nleft) ;

        if ( nwriten < 0 )
        {
			perror("write");
            return(nwriten) ;
        }
		else if(nwriten == 0)
		{
			usleep(1000);
		}

        nleft = nleft - nwriten ;
        ptr += nwriten ;
    }

    return( nbytes - nleft) ;
}

void hal_ttyS0_init(void)
{
	TRD_t trd_id;
	
	//open
	hal_ttyS0_open();

	//set
	hal_ttyS0_set(57600,8,0,1);
	//recv thread
	trd_create(&trd_id, (void*)&hal_ttyS0_recv_thread, NULL);
}

