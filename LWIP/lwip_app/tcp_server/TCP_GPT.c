//#include "tcp_gpt.h"
//
///* listen pcb 永久在线 */
//static struct tcp_pcb *g_listen_pcb = NULL;
//
///* 连接池：最多 TCP_SERVER_MAX_CLIENTS 个连接 */
//static tcp_server_conn_t g_conns[TCP_SERVER_MAX_CLIENTS];
//
///* ===== 内部辅助：找空闲 slot ===== */
//static tcp_server_conn_t* tcp_server_alloc_conn(void)
//{
//    int i;
//    for (i = 0; i < TCP_SERVER_MAX_CLIENTS; i++)
//    {
//        if (g_conns[i].used == 0)
//        {
//            memset(&g_conns[i], 0, sizeof(g_conns[i]));
//            g_conns[i].used  = 1;
//            g_conns[i].state = ES_ACCEPTED;
//            return &g_conns[i];
//        }
//    }
//    return NULL;
//}
//
///* ===== 内部辅助：释放 slot（不关 pcb 的话不要调用它） ===== */
//static void tcp_server_free_conn(tcp_server_conn_t *conn)
//{
//    if (!conn) return;
//
//    if (conn->tx_pbuf)
//    {
//        pbuf_free(conn->tx_pbuf);
//        conn->tx_pbuf = NULL;
//    }
//    conn->pcb = NULL;
//    conn->state = ES_NONE;
//    conn->used = 0;
//}
//
///* =====================================================
// * 初始化 TCP Server（只需一次）
// * ===================================================== */
//int tcp_server_init(void)
//{
//    err_t err;
//    struct tcp_pcb *pcb;
//
//    if (g_listen_pcb)
//        return 0;
//
//    memset(g_conns, 0, sizeof(g_conns));
//
//    pcb = tcp_new();
//    if (!pcb)
//        return -1;
//
//    err = tcp_bind(pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
//    if (err != ERR_OK)
//    {
//        tcp_abort(pcb);
//        return -2;
//    }
//
//    /* lwIP 1.4.1：必须用 tcp_listen 返回值覆盖 */
//    g_listen_pcb = tcp_listen(pcb);
//    if (!g_listen_pcb)
//    {
//        tcp_abort(pcb);
//        return -3;
//    }
//
//    tcp_accept(g_listen_pcb, tcp_server_accept);
//    return 0;
//}
//
///* =====================================================
// * 轮询（你的工程已有 lwip_periodic_handle）
// * ===================================================== */
//void tcp_server_polling(void)
//{
//    lwip_periodic_handle();
//    delay_ms(1);
//}
//
///* =====================================================
// * accept 回调：最多 3 个客户端
// * 第 4 个：直接 abort，新连接失败
// * ===================================================== */
//err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
//{
//    tcp_server_conn_t *conn;
//    (void)arg;
//    (void)err;
//
//    conn = tcp_server_alloc_conn();
//    if (!conn)
//    {
//        /* 无空位：拒绝第 4 个连接 */
//        tcp_abort(newpcb);
//        return ERR_ABRT;
//    }
//
//    conn->pcb = newpcb;
//
//    tcp_arg(newpcb, conn);
//    tcp_recv(newpcb, tcp_server_recv);
//    tcp_err(newpcb,  tcp_server_err);
//    tcp_poll(newpcb, tcp_server_poll, 2);
//    tcp_sent(newpcb, tcp_server_sent);
//
//    /* greeting */
//    tcp_server_queue_send_copy(conn,
//                               TCP_SERVER_GREETING,
//                               (u16_t)strlen(TCP_SERVER_GREETING));
//    tcp_server_send_from_pbuf(newpcb, conn);
//
//    return ERR_OK;
//}
//
///* =====================================================
// * recv 回调
// * ===================================================== */
//err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
//{
//    tcp_server_conn_t *conn = (tcp_server_conn_t *)arg;
//
//    if (!conn)
//    {
//        if (p) pbuf_free(p);
//        return ERR_VAL;
//    }
//
//    if (!p)
//    {
//        /* 对端关闭 */
//        conn->state = ES_CLOSING;
//        if (!conn->tx_pbuf)
//        {
//            tcp_server_close(tpcb, conn);
//        }
//        return ERR_OK;
//    }
//
//    if (err != ERR_OK)
//    {
//        pbuf_free(p);
//        return err;
//    }
//
//    /* 拷贝数据到该连接自己的 rxbuf（不越界） */
//    memset(conn->rxbuf, 0, TCP_SERVER_RX_BUFSIZE);
//
//    u16_t copied = 0;
//    struct pbuf *q;
//    for (q = p; q && copied < TCP_SERVER_RX_BUFSIZE; q = q->next)
//    {
//        u16_t remain = (u16_t)(TCP_SERVER_RX_BUFSIZE - copied);
//        u16_t len2   = (q->len > remain) ? remain : (u16_t)q->len;
//        memcpy(&conn->rxbuf[copied], q->payload, len2);
//        copied = (u16_t)(copied + len2);
//    }
//
//    /* 只在接收路径调用 tcp_recved */
//    tcp_recved(tpcb, p->tot_len);
//    pbuf_free(p);
//
//    /* demo：echo 回去（按需替换为协议处理） */
//    if (copied)
//    {
//        tcp_server_queue_send_copy(conn, conn->rxbuf, copied);
//        tcp_server_send_from_pbuf(tpcb, conn);
//    }
//
//    return ERR_OK;
//}
//
///* =====================================================
// * err 回调：连接异常中止/复位
// * 注意：这里不能 tcp_close（pcb 已被栈处理）
// * ===================================================== */
//void tcp_server_err(void *arg, err_t err)
//{
//    tcp_server_conn_t *conn = (tcp_server_conn_t *)arg;
//    (void)err;
//
//    /* 释放 slot 资源 */
//    tcp_server_free_conn(conn);
//}
//
///* =====================================================
// * poll 回调：补发 & closing
// * ===================================================== */
//err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
//{
//    tcp_server_conn_t *conn = (tcp_server_conn_t *)arg;
//
//    if (!conn)
//    {
//        tcp_abort(tpcb);
//        return ERR_ABRT;
//    }
//
//    if (conn->tx_pbuf)
//    {
//        tcp_server_send_from_pbuf(tpcb, conn);
//    }
//
//    if ((conn->state == ES_CLOSING) && (conn->tx_pbuf == NULL))
//    {
//        tcp_server_close(tpcb, conn);
//    }
//
//    return ERR_OK;
//}
//
///* =====================================================
// * sent 回调：收到 ACK 后继续发
// * ===================================================== */
//err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//    tcp_server_conn_t *conn = (tcp_server_conn_t *)arg;
//    (void)len;
//
//    if (conn && conn->tx_pbuf)
//    {
//        tcp_server_send_from_pbuf(tpcb, conn);
//    }
//
//    if (conn && (conn->state == ES_CLOSING) && (conn->tx_pbuf == NULL))
//    {
//        tcp_server_close(tpcb, conn);
//    }
//
//    return ERR_OK;
//}
//
///* =====================================================
// * 复制入 pbuf 并挂到 tx 队列末尾
// * ===================================================== */
//void tcp_server_queue_send_copy(tcp_server_conn_t *conn, const void *data, u16_t len)
//{
//    struct pbuf *pb;
//
//    if (!conn || !data || !len)
//        return;
//
//    pb = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
//    if (!pb)
//        return;
//
//    pbuf_take(pb, data, len);
//
//    if (!conn->tx_pbuf)
//        conn->tx_pbuf = pb;
//    else
//        pbuf_cat(conn->tx_pbuf, pb);
//}
//
///* =====================================================
// * 从 tx_pbuf 尽量写入 tcp sndbuf
// * 注意：发送路径不调用 tcp_recved
// * ===================================================== */
//err_t tcp_server_send_from_pbuf(struct tcp_pcb *tpcb, tcp_server_conn_t *conn)
//{
//    err_t wr_err = ERR_OK;
//
//    if (!tpcb || !conn)
//        return ERR_VAL;
//
//    while ((wr_err == ERR_OK) && conn->tx_pbuf)
//    {
//        struct pbuf *pb = conn->tx_pbuf;
//
//        if (pb->len > tcp_sndbuf(tpcb))
//            break;
//
//        wr_err = tcp_write(tpcb, pb->payload, pb->len, TCP_WRITE_FLAG_COPY);
//        if (wr_err == ERR_OK)
//        {
//            conn->tx_pbuf = pb->next;
//            pb->next = NULL;
//            pbuf_free(pb);
//            tcp_output(tpcb);
//        }
//        else if (wr_err == ERR_MEM)
//        {
//            /* 等下次 poll/sent */
//            break;
//        }
//        else
//        {
//            break;
//        }
//    }
//    return wr_err;
//}
//
///* =====================================================
// * 关闭连接：检查 tcp_close 返回值，必要时 abort
// * ===================================================== */
//void tcp_server_close(struct tcp_pcb *tpcb, tcp_server_conn_t *conn)
//{
//    err_t e;
//
//    /* 清回调，防止再次进入 */
//    tcp_arg(tpcb, NULL);
//    tcp_sent(tpcb, NULL);
//    tcp_recv(tpcb, NULL);
//    tcp_err(tpcb, NULL);
//    tcp_poll(tpcb, NULL, 0);
//
//    if (conn && conn->tx_pbuf)
//    {
//        pbuf_free(conn->tx_pbuf);
//        conn->tx_pbuf = NULL;
//    }
//
//    e = tcp_close(tpcb);
//    if (e != ERR_OK)
//    {
//        tcp_abort(tpcb);
//    }
//
//    tcp_server_free_conn(conn);
//}
