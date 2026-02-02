/**
* @Author kokirika
* @Name TCP_SERVER
* @Date 2026-01-23
*/

#ifndef KOKIRIKA_TCP_SERVER_H
#define KOKIRIKA_TCP_SERVER_H

#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm/lwip_comm.h"
#include <string>
#include "HARD_BASE.h"
#include "malloc.h"
#include <cstring>
#include "delay.h"
/* ================= 配置 ================= */
#include <string>
using namespace std;


#ifndef TCP_SERVER_MAX_CLIENTS
#define TCP_SERVER_MAX_CLIENTS  3
#endif

#ifndef TCP_SERVER_RX_BUFSIZE
#define TCP_SERVER_RX_BUFSIZE   1024
#endif

/* ================= TcpServer ================= */

class TcpServer
{
private:
    /* ===== 内部状态 ===== */
    enum class State : uint8_t {
        FREE = 0,
        ACTIVE,
        CLOSING
    };

    struct Conn {
        bool     used{};
        State    state{};
        tcp_pcb* pcb{};
        pbuf*    tx{};

        uint8_t  rx[TCP_SERVER_RX_BUFSIZE]{};
        uint16_t rx_len{};
        bool     rx_ready{};
    };
    uint16_t mport;
    tcp_pcb* m_listen{};
    Conn     m_conns[TCP_SERVER_MAX_CLIENTS]{};

protected:
    explicit TcpServer(uint16_t port);
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;
    /* ===== lwIP callbacks（static） ===== */
    static err_t onAccept(void* arg, tcp_pcb* newpcb, err_t err);
    static err_t onRecv(void* arg, tcp_pcb* pcb, pbuf* p, err_t err);
    static void  onError(void* arg, err_t err);
    static err_t onPoll(void* arg, tcp_pcb* pcb);
    static err_t onSent(void* arg, tcp_pcb* pcb, uint16_t len);

    /* ===== 内部实现 ===== */
    Conn* allocConn();
    void  freeConn(Conn* c);
    void  closeConn(tcp_pcb* pcb, Conn* c);

    void  queueSend(Conn* c, const void* data, uint16_t len);
    void  flushSend(tcp_pcb* pcb, Conn* c);
public:
    class Client
    {
    public:
        bool valid() const;

        /* 接收 */
        bool hasData() const;
        uint16_t read(void* out, uint16_t maxLen);
        string  read();
        /* 发送 */
        bool send(const void* data, uint16_t len);
        bool send(const string& str);
        uint8_t id() const;

        void write(const char *str,uint16_t len);
        void write(uint8_t *str,uint16_t len);
        void write(string String);
        uint16_t print(const char *fmt, ...);
        uint16_t print(const string& String);
        uint16_t print(const char *s);
        uint16_t print(char s);
        uint16_t print(int integer);
        uint16_t println(const char *fmt, ...);
        uint16_t println(const string& String);
        uint16_t println(const char *s);
        uint16_t println(int integer);

        Client& operator<<(const int integer)
        {
            this->print("%d", integer);
            return *this;
        }
        Client& operator<<(const char *s)
        {
            this->print("%s", s);
            return *this;
        }
        Client& operator<<(const std::string& Str)
        {
            this->print(Str);
            return *this;
        }

        Client& operator>>(std::string& Str)
        {
            Str.append(this->read());
            return *this;
        }


    private:
        friend class TcpServer;
        Client(TcpServer* srv, uint8_t idx);

        Client(){};
        TcpServer* m_srv{};
        uint8_t    m_idx{};
    };
    Client& clientRef(uint8_t clientId);  // 返回引用（要求你自己保证合法）
    Client* client(uint8_t clientId);     // 返回指针（可为 nullptr）
    /* 单例（RAW API 友好） */

    static TcpServer& instance(uint16_t port=5000);
    bool setPort(uint16_t port);
    /* 初始化（只调用一次） */
    bool init();
    /* ===== 接收接口 ===== */
    /* 是否有新数据 */
    bool hasData(uint8_t clientId) const;
    /* 读取数据（读取后自动清空） */
    uint16_t read(uint8_t clientId, void* out, uint16_t maxLen);
    string read(uint8_t clientId);
    /* ===== 发送接口 ===== */
    /* 单独发送 */
    bool send(uint8_t clientId, const void* data, uint16_t len);
    bool send(uint8_t clientId, const string& str);
    /* 广播发送 */
    void broadcast(const void* data, uint16_t len);
    /* 获取连接数量 */
    uint8_t clientCount() const;
    /* 主动断开某个客户端 */
    bool disconnect(uint8_t clientId);
    /* 主动断开所有客户端 */
    void disconnectAll();
    /* 关闭整个 TCP Server（包括 listen） */
    bool shutdown();



};

#endif //KOKIRIKA_TCP_SERVER_H
