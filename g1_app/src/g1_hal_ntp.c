#include "g1_hal.h"

#if 0
#define NTP_SERVER      "clock.via.net"  
#define NTP_PORT            123  
  
//  
//rfc1305 defined from 1900 so also  2208988800 (1900 - 1970 ) seconds left   
//  
//timeval.tv_sec + JAN_1970 = timestamp.coarse  
#define JAN_1970            0x83aa7e80        
  
//timeval.tv_usec=>timestamp.fine  
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x))>>11))  
//timeval.tv_usec<=timestamp.fine  
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))  
 
#define Data(i) ntohl(((unsigned int *)data)[i])  
 
#define LI 0  
#define VN 3  
#define MODE 3  
#define STRATUM 0  
#define POLL 4   
#define PREC -6  
  
struct ntptime   
{  
    unsigned int coarse;  
    unsigned int fine;  
};  
  
void send_packet(int fd)  
{  
        unsigned int data[12];  
        struct timeval now;  
        int ret;  
  
        if (sizeof(data) != 48)   
        {  
            fprintf(stderr,"size error/n");  
            return;  
        }  
  
        memset((char*)data, 0, sizeof(data));  
    data[0] = htonl((LI << 30) | (VN << 27) | (MODE << 24)   
                  | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));  
    data[1] = htonl(1<<16);  /* Root Delay (seconds) */  
    data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */  
    gettimeofday(&now, NULL);  
    data[10] = htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */  
    data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */  
    send(fd, data, 48, 0);  
}  
  
void get_udp_arrival_local_timestamp(struct ntptime *udp_arrival_local_timestamp)  
{  
    struct timeval udp_arrival_local_time;  
      
    gettimeofday(&udp_arrival_local_time, NULL);  
  
    //print just precise to second  
    printf("local time=>%s/n",ctime(&(udp_arrival_local_time.tv_sec)));  
}  
  
void get_new_time(unsigned int *data,struct timeval* new_time)  
{  
    //int li, vn, mode, stratum, poll, prec;  
    //int delay, disp, refid;  
    //struct ntptime reftime, orgtime, rectime;  
    struct ntptime trantime;  
    /*li      = Data(0) >> 30 & 0x03; 
    vn      = Data(0) >> 27 & 0x07; 
    mode    = Data(0) >> 24 & 0x07; 
    stratum = Data(0) >> 16 & 0xff; 
    poll    = Data(0) >>  8 & 0xff; 
    prec    = Data(0)       & 0xff; 
    if (prec & 0x80) prec|=0xffffff00; 
    delay   = Data(1); 
    disp    = Data(2); 
    refid   = Data(3); 
    reftime.coarse = Data(4); 
    reftime.fine   = Data(5); 
    orgtime.coarse = Data(6); 
    orgtime.fine   = Data(7); 
    rectime.coarse = Data(8); 
    rectime.fine   = Data(9);*/  
    trantime.coarse = Data(10);  
    trantime.fine   = Data(11);  
      
    new_time->tv_sec = trantime.coarse - JAN_1970;  
    new_time->tv_usec = USEC(trantime.fine);  
  
    //print just precise to second  
    printf("server time=>%s/n",ctime(&(new_time->tv_sec)));  
}  
  
void set_local_time(struct timeval new_time)  
{  
    /* need root user. */  
    if (0 != getuid() && 0 != geteuid())  
    {  
        printf("must be root user!:0)/n");  
        return;  
    }  
  
    settimeofday(&new_time, NULL);  
}  
  
int ntp_thread(void)  
{  
    int sockfd;  
    struct sockaddr_in addr_src,addr_dst;  
    fd_set fds_read;  
    int ret;  
    int receivebytes;  
    unsigned int buf[12];  
    int addr_len;  
      
    struct timeval timeout,new_time;  
    struct ntptime udp_arrival_local_timestamp;  
      
    struct hostent* host;   
  
    addr_len = sizeof(struct sockaddr_in);  
  
    memset(&addr_src, 0, addr_len);  
    addr_src.sin_family = AF_INET;  
    addr_src.sin_addr.s_addr = htonl(INADDR_ANY);  
    addr_src.sin_port = htons(0);  
  
    memset(&addr_dst, 0, addr_len);  
    addr_dst.sin_family = AF_INET;  
    host = gethostbyname(NTP_SERVER); 
	if (host == NULL){
		log_err("start %s\n", "ntp thread");
		return;
	}
	
    memcpy(&(addr_dst.sin_addr.s_addr), host->h_addr_list[0], 4);  
    addr_dst.sin_port = htons(NTP_PORT);  
  
    /* create socket. */  
    if(-1==(sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)))  
    {         
        printf("create socket error!/n");  
        exit(1);      
    }  
  
    /* bind local address. */  
    ret = bind(sockfd, (struct sockaddr*)&addr_src, addr_len);  
    if(-1==ret)   
    {         
        printf("bind error!/n");          
        close(sockfd);    
    }  
      
    /* connect to ntp server. */  
    ret = connect(sockfd, (struct sockaddr*)&addr_dst, addr_len);  
    if(-1==ret)   
    {         
        printf("connect error!/n");       
        close(sockfd);    
    }  
    /*get and set time*/  
    while (1)  
    {  
        FD_ZERO(&fds_read);  
        FD_SET(sockfd, &fds_read);  
  
        timeout.tv_sec = 6;  
        timeout.tv_usec = 0;  
        ret = select(sockfd + 1, &fds_read, NULL, NULL, &timeout);  
        if (0 == ret || !FD_ISSET(sockfd, &fds_read))  
        {  
            /* send ntp protocol packet. */  
            send_packet(sockfd);  
            continue;  
        }  
        /* recv ntp server's response. */  
        receivebytes = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&addr_dst, &addr_len);  
        if(-1==receivebytes)          
        {             
            printf("recvfrom error/n");           
            close(sockfd);            
            exit(1);          
        }  
        /* get local timestamp. */  
        get_udp_arrival_local_timestamp(&udp_arrival_local_timestamp);  
        /* get server's time and print it. */  
        get_new_time(buf, &new_time);     
        /* set local time to the server's time, if you're a root user. */  
        set_local_time(new_time);  
    }  
  
    close(sockfd);  
    return 0;  
}  


#endif
int hal_get_time_from_ntp_server(void)
{
	//TODO 

	return 0;
}

int hal_get_time_from_mt7688(hal_date_t *hal_date)
{
	struct timeval tv;
    struct tm stm;

	if (hal_date == NULL){
		log_err("%s\n", "NULL Pointer");
		return -1;
	}
		
	gettimeofday ( &tv, NULL );
    stm = * ( localtime ( &tv.tv_sec ) );
   
    sprintf ( hal_date->time, "%04d-%02d-%02d %02d:%02d:%02d",
                stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, 
                stm.tm_min, stm.tm_sec);
        
    log_debug("hal_date->time %s\n", hal_date->time);
	//TODO
	return 0;
}


int hal_set_time_to_LEDScreen(HAL_LS_PARAM_CMD_SET_TIME_t*param)
{
	hal_ls_CMD_SET_TIME(param);

	return 0;
}


