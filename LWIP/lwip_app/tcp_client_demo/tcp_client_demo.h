#ifndef __TCP_CLIENT_DEMO_H
#define __TCP_CLIENT_DEMO_H
#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm/lwip_comm.h"

#ifdef __cplusplus
extern "C" {
#endif
 
#define TCP_CLIENT_RX_BUFSIZE	200		//����tcp client���������ݳ���
#define	TCP_CLIENT_PORT			8080	//����tcp clientҪ���ӵ�Զ�˶˿�

//tcp����������״̬
enum tcp_client_states
{
	ES_TCPCLIENT_NONE = 0,		//û������
	ES_TCPCLIENT_CONNECTED,		//���ӵ��������� 
	ES_TCPCLIENT_CLOSING,		//�ر�����
}; 
//LWIP�ص�����ʹ�õĽṹ��
struct tcp_client_struct
{
	u8 state;               //��ǰ����״
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *p;         //ָ�����/�����pbuf
};  

void tcp_client_test(void);//TCP Client���Ժ���
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);//TCP���ӽ�������ûص�����
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);//tcp_recv()�����Ļص�����
void tcp_client_error(void *arg,err_t err);//tcp_err()�ص�����
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);//tcp_poll()�ص�����
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);//tcp_sent()�ص�����
void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);//TCP Client��������
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es );//�ر�TCP Client����

void tcp_send_data(u8 *data);//TCP��������

#ifdef __cplusplus
}
#endif

#endif























