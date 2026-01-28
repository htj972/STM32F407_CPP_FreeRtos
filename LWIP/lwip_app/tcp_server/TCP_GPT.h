////
//// Created by 53105 on 2026/1/23.
////
//
//#ifndef STM32F407_CPP_FREERTOS_TCP_GPT_H
//#define STM32F407_CPP_FREERTOS_TCP_GPT_H
//
///* ================= 标准库 ================= */
//#include <string.h>
//#include <stdio.h>
//
///* ================= lwIP ================= */
//#include "lwip/err.h"
//#include "lwip/tcp.h"
//#include "lwip/pbuf.h"
//#include "lwip/debug.h"
//#include "lwip/stats.h"
//#include "lwip/memp.h"
//#include "lwip/mem.h"
//
///* ================= 工程 ================= */
//#include "delay.h"
//#include "sys.h"
//#include "lwip_comm/lwip_comm.h"
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
///* ================= 用户可配置参数 ================= */
//
//#ifndef TCP_SERVER_PORT
//#define TCP_SERVER_PORT             5000
//#endif
//
//#ifndef TCP_SERVER_RX_BUFSIZE
//#define TCP_SERVER_RX_BUFSIZE       1024
//#endif
//
//#ifndef TCP_SERVER_GREETING
//#define TCP_SERVER_GREETING         "lwIP 1.4.1 TCP Server Ready\r\n"
//#endif
//
///* 最大客户端数量：改这里即可（你要求宏定义设置） */
//#ifndef TCP_SERVER_MAX_CLIENTS
//#define TCP_SERVER_MAX_CLIENTS      3
//#endif
//
///* ================= TCP Server 状态定义 ================= */
//
//typedef enum
//{
//    ES_NONE = 0,
//    ES_ACCEPTED,
//    ES_CLOSING
//} tcp_es_t;
//
///* TCP Server 连接控制块（每个客户端一个） */
//typedef struct
//{
//    u8_t            used;                       /* 0:空闲 1:占用 */
//    tcp_es_t        state;
//    struct tcp_pcb *pcb;
//    struct pbuf    *tx_pbuf;                    /* 待发送链 */
//    u8_t            rxbuf[TCP_SERVER_RX_BUFSIZE]; /* 每连接独立 RX 缓冲 */
//} tcp_server_conn_t;
//
///* ================= 对外 API ================= */
//
//int  tcp_server_init(void);
//void tcp_server_polling(void);
//
///* ================= 回调/内部函数（你要求声明放 h） ================= */
//
//err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
//err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
//void  tcp_server_err(void *arg, err_t err);
//err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
//err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//
//void  tcp_server_close(struct tcp_pcb *tpcb, tcp_server_conn_t *conn);
//
//err_t tcp_server_send_from_pbuf(struct tcp_pcb *tpcb, tcp_server_conn_t *conn);
//void  tcp_server_queue_send_copy(tcp_server_conn_t *conn, const void *data, u16_t len);
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif //STM32F407_CPP_FREERTOS_TCP_GPT_H
