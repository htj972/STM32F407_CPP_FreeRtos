#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "lan8720.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LWIP_MAX_DHCP_TRIES		4   //DHCP服务器最大重试次数
   
//lwip控制结构体
typedef struct  
{
	u8 mac[6];      //MAC地址
	u8 remoteip[4];	//远端主机IP地址 
	u8 ip[4];       //本机IP地址
	u8 netmask[4]; 	//子网掩码
	u8 gateway[4]; 	//默认网关的IP地址
	
	vu8 dhcpstatus;	//dhcp状态 
					//0,未获取DHCP地址;
					//1,进入DHCP获取状态
					//2,成功获取DHCP地址
					//0XFF,获取失败.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip控制结构体

extern char DHCP_str[5][128];

void lwip_pkt_handle(void);
void lwip_periodic_handle(void);
	
void lwip_comm_default_ip_set(__lwip_dev *lwipx);
uint8_t lwip_comm_mem_malloc(void);
void lwip_comm_mem_free(void);
uint8_t lwip_comm_init(void);
void lwip_dhcp_process_handle(void);
void  lwip_setup();
void DNS_init();
ip_addr_t DNS_get_ip(const char *name);
uint8_t *get_dns_ip();

#ifdef __cplusplus
}
#endif

#endif













