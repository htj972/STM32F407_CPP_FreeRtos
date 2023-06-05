#include "tcp_client_demo.h" 
#include "delay.h"
#include "stdio.h"
#include "string.h" 

 
//TCP Client�������ݻ�����
uint8_t tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];
//TCP������������������
const uint8_t *tcp_client_sendbuf="Explorer STM32F407 TCP Client send data\r\n";
uint8_t tcp_client_data[128];//TCP Client�������ݻ�����
uint8_t tcp_client_sendlen;//TCP Client�������ݳ���

//TCP Client ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�������Ϸ�����;1,�����Ϸ�������.
//bit4~0:����
uint8_t tcp_client_flag;

 

//TCP Client ����
void tcp_client_test(void)
{
 	struct tcp_pcb *tcppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ

 	uint8_t key=0;
	uint8_t res=0;
	uint8_t t=0;
	uint8_t connflag=0;		//���ӱ��

	tcppcb=tcp_new();	//����һ���µ�pcb
	if(tcppcb)			//�����ɹ�
	{
        IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
//		IP4_ADDR(&rmtipaddr,10,40,12,14);
		tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
 	}else res=1;
	while(res==0)
	{
//		if(key==WKUP_PRES)break;
		if((key==0)&&(connflag==1))//KEY0������,��������
		{
			tcp_client_flag|=1<<7;//���Ҫ��������
            key=1;
		}
		if(tcp_client_flag&1<<6)//�Ƿ��յ�����?
		{
//			LCD_ShowString(30,230,lcddev.width-30,lcddev.height-230,16,tcp_client_recvbuf);//��ʾ���յ�������
			tcp_client_flag&=~(1<<6);//��������Ѿ���������.

            tcp_client_flag|=1<<7;//���Ҫ��������
		}
		if(tcp_client_flag&1<<5)//�Ƿ�������?
		{
			if(connflag==0)
			{
				connflag=1;//���������
			} 
		}else if(connflag)
		{
			connflag=0;	//������ӶϿ���
		} 
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if(t==200)
		{
			if(connflag==0&&(tcp_client_flag&1<<5)==0)//δ������,��������
			{ 
				tcp_client_connection_close(tcppcb,0);//�ر�����
				tcppcb=tcp_new();	//����һ���µ�pcb
				if(tcppcb)			//�����ɹ�
				{ 
					tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);//���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
				}
			}
			t=0;
		}		
	}
	tcp_client_connection_close(tcppcb,0);//�ر�TCP Client����
}
//lwIP TCP���ӽ�������ûص�����
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	struct tcp_client_struct *es=NULL;  
	if(err==ERR_OK)   
	{
		es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //�����ڴ�
		if(es) //�ڴ�����ɹ�
		{
 			es->state=ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
			es->pcb=tpcb;  
			es->p=NULL; 
			tcp_arg(tpcb,es);        			//ʹ��es����tpcb��callback_arg
			tcp_recv(tpcb,tcp_client_recv);  	//��ʼ��LwIP��tcp_recv�ص�����   
			tcp_err(tpcb,tcp_client_error); 	//��ʼ��tcp_err()�ص�����
			tcp_sent(tpcb,tcp_client_sent);		//��ʼ��LwIP��tcp_sent�ص�����
			tcp_poll(tpcb,tcp_client_poll,1); 	//��ʼ��LwIP��tcp_poll�ص����� 
 			tcp_client_flag|=1<<5; 				//������ӵ���������
			err=ERR_OK;
		}else
		{ 
			tcp_client_connection_close(tpcb,es);//�ر�����
			err=ERR_MEM;	//�����ڴ�������
		}
	}else
	{
		tcp_client_connection_close(tpcb,0);//�ر�����
	}
	return err;
}
//lwIP tcp_recv()�����Ļص�����
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
{ 
	u32 data_len=0;
	struct pbuf *q;
	struct tcp_client_struct *es;
	err_t ret_err; 
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_client_struct *)arg; 
	if(p==NULL)//����ӷ��������յ��յ�����֡�͹ر�����
	{
		es->state=ES_TCPCLIENT_CLOSING;//��Ҫ�ر�TCP ������ 
 		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!= ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
	{ 
		if(p)pbuf_free(p);//�ͷŽ���pbuf
		ret_err=err;
	}else if(es->state==ES_TCPCLIENT_CONNECTED)	//����������״̬ʱ
	{
        memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
        for(q=p;q!=NULL;q=q->next)  //����������pbuf����
        {
            //�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
            //�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
            if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
            else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
            data_len += q->len;
            if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����
        }
        tcp_client_flag|=1<<6;		//��ǽ��յ�������
        tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
        pbuf_free(p);  	//�ͷ��ڴ�
        ret_err=ERR_OK;
	}else  //���յ����ݵ��������Ѿ��ر�,
	{ 
		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
		es->p=NULL;
		pbuf_free(p); //�ͷ��ڴ�
		ret_err=ERR_OK;
	}
	return ret_err;
} 
//lwIP tcp_err�����Ļص�����
void tcp_client_error(void *arg,err_t err)
{  
	//�������ǲ����κδ���
} 
//lwIP tcp_poll�Ļص�����
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es; 
	es=(struct tcp_client_struct*)arg;
	if(es!=NULL)  //���Ӵ��ڿ��п��Է�������
	{
		if(tcp_client_flag&(1<<7))	//�ж��Ƿ�������Ҫ���� 
		{
            if(tcp_client_sendlen!=0){
                es->p=pbuf_alloc(PBUF_TRANSPORT, tcp_client_sendlen,PBUF_POOL);	//�����ڴ�
                //tcp_client_data[]�е����ݿ�����es->p_tx��
                pbuf_take(es->p,(char*)tcp_client_data,tcp_client_sendlen);
            }
            else{
                es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_client_sendbuf),PBUF_POOL);	//�����ڴ�
                //��tcp_client_sentbuf[]�е����ݿ�����es->p_tx��
                pbuf_take(es->p,(char*)tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf));
            }


			tcp_client_senddata(tpcb,es);//��tcp_client_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
			tcp_client_flag&=~(1<<7);	//������ݷ��ͱ�־
			if(es->p)pbuf_free(es->p);	//�ͷ��ڴ�
		}else if(es->state==ES_TCPCLIENT_CLOSING)
		{ 
 			tcp_client_connection_close(tpcb,es);//�ر�TCP����
		} 
		ret_err=ERR_OK;
	}else
	{ 
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT;
	}
	return ret_err;
} 
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_client_struct *es;
	LWIP_UNUSED_ARG(len);
	es=(struct tcp_client_struct*)arg;
	if(es->p)tcp_client_senddata(tpcb,es);//��������
	return ERR_OK;
}
//�˺���������������
void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	struct pbuf *ptr; 
 	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb))) //��Ҫ���͵����ݼ��뵽���ͻ��������
	{
		ptr=es->p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{  
			es->p=ptr->next;			//ָ����һ��pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
			pbuf_free(ptr);				//�ͷ�ptr 
		}else if(wr_err==ERR_MEM)es->p=ptr;
		tcp_output(tpcb);		//�����ͻ�������е������������ͳ�ȥ
	} 
} 
//�ر��������������
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	//�Ƴ��ص�
	tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  
	if(es)mem_free(es); 
	tcp_client_flag&=~(1<<5);//������ӶϿ���
}

void tcp_send_data(uint8_t *data) {
    memcpy(tcp_client_data,data,strlen((char*)data));
    tcp_client_sendlen = strlen((char*)data);
    tcp_client_flag|=1<<7;		//���������Ҫ����

}




















