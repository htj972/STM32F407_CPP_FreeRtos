#include "Tcp_Server.h"
#include <cstdarg>

/* ================= 单例 ================= */

TcpServer& TcpServer::instance(uint16_t port)
{
    static TcpServer s(port);
    return s;
}

TcpServer::TcpServer(uint16_t port)
        : m_listen(nullptr)
{
    mport = port;
    std::memset(m_conns, 0, sizeof(m_conns));
}

/* ================= 初始化 ================= */

bool TcpServer::init()
{
    if (m_listen)
        return true;

    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return false;

    if (tcp_bind(pcb, IP_ADDR_ANY, mport) != ERR_OK)
    {
        tcp_abort(pcb);
        return false;
    }

    m_listen = tcp_listen(pcb);
    if (!m_listen)
    {
        tcp_abort(pcb);
        return false;
    }

    tcp_accept(m_listen, &TcpServer::onAccept);
    return true;
}

/* ================= Conn 管理 ================= */

TcpServer::Conn* TcpServer::allocConn()
{
    for (auto& c : m_conns)
    {
        if (!c.used)
        {
            std::memset(&c, 0, sizeof(c));
            c.used  = true;
            c.state = State::ACTIVE;
            return &c;
        }
    }
    return nullptr;
}

void TcpServer::freeConn(Conn* c)
{
    if (!c) return;

    if (c->tx)
    {
        pbuf_free(c->tx);
        c->tx = nullptr;
    }

    c->pcb = nullptr;
    c->used = false;
    c->state = State::FREE;
    c->rx_ready = false;
}

/* ================= lwIP callbacks ================= */

err_t TcpServer::onAccept(void*, tcp_pcb* newpcb, err_t)
{
    TcpServer& s = TcpServer::instance();
    Conn* c = s.allocConn();
    if (!c)
    {
        /* 超过最大连接数，拒绝 */
        tcp_abort(newpcb);
        return ERR_ABRT;
    }

    c->pcb = newpcb;

    tcp_arg(newpcb, c);
    tcp_recv(newpcb, &TcpServer::onRecv);
    tcp_err (newpcb, &TcpServer::onError);
    tcp_poll(newpcb, &TcpServer::onPoll, 2);
    tcp_sent(newpcb, &TcpServer::onSent);

    return ERR_OK;
}

err_t TcpServer::onRecv(void* arg, tcp_pcb* pcb, pbuf* p, err_t err)
{
    Conn* c = static_cast<Conn*>(arg);

    if (!c)
    {
        if (p) pbuf_free(p);
        return ERR_VAL;
    }

    if (!p)
    {
        c->state = State::CLOSING;
        if (!c->tx)
            instance().closeConn(pcb, c);
        return ERR_OK;
    }

    if (err != ERR_OK)
    {
        pbuf_free(p);
        return err;
    }

    /* 若上一次数据未被读取，丢弃新数据 */
    if (c->rx_ready)
    {
        tcp_recved(pcb, p->tot_len);
        pbuf_free(p);
        return ERR_OK;
    }

    uint16_t copied = 0;
    for (pbuf* q = p; q && copied < TCP_SERVER_RX_BUFSIZE; q = q->next)
    {
        uint16_t n = (q->len > TCP_SERVER_RX_BUFSIZE - copied)
                     ? (TCP_SERVER_RX_BUFSIZE - copied)
                     : q->len;
        std::memcpy(&c->rx[copied], q->payload, n);
        copied += n;
    }

    c->rx_len   = copied;
    c->rx[c->rx_len++ ]= 0;  // Null-terminate for convenience
    c->rx_ready = true;

    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}

void TcpServer::onError(void* arg, err_t)
{
    Conn* c = static_cast<Conn*>(arg);
    instance().freeConn(c);
}

err_t TcpServer::onPoll(void* arg, tcp_pcb* pcb)
{
    Conn* c = static_cast<Conn*>(arg);
    if (!c)
    {
        tcp_abort(pcb);
        return ERR_ABRT;
    }

    if (c->tx)
        instance().flushSend(pcb, c);

    if (c->state == State::CLOSING && !c->tx)
        instance().closeConn(pcb, c);

    return ERR_OK;
}

err_t TcpServer::onSent(void* arg, tcp_pcb* pcb, uint16_t)
{
    Conn* c = static_cast<Conn*>(arg);
    if (c && c->tx)
        instance().flushSend(pcb, c);
    return ERR_OK;
}

/* ================= 发送 ================= */

void TcpServer::queueSend(Conn* c, const void* data, uint16_t len)
{
    if (!c || !data || !len) return;

    pbuf* pb = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
    if (!pb) return;

    pbuf_take(pb, data, len);

    if (!c->tx)
        c->tx = pb;
    else
        pbuf_cat(c->tx, pb);
}

void TcpServer::flushSend(tcp_pcb* pcb, Conn* c)
{
    while (c->tx)
    {
        pbuf* pb = c->tx;
        if (pb->len > tcp_sndbuf(pcb))
            return;

        if (tcp_write(pcb, pb->payload, pb->len, TCP_WRITE_FLAG_COPY) != ERR_OK)
            return;

        c->tx = pb->next;
        pb->next = nullptr;
        pbuf_free(pb);
        tcp_output(pcb);
    }
}

void TcpServer::closeConn(tcp_pcb* pcb, Conn* c)
{
    tcp_arg(pcb, nullptr);
    tcp_recv(pcb, nullptr);
    tcp_sent(pcb, nullptr);
    tcp_err (pcb, nullptr);
    tcp_poll(pcb, nullptr, 0);

    if (tcp_close(pcb) != ERR_OK)
        tcp_abort(pcb);

    freeConn(c);
}

/* ================= 对外接口 ================= */

bool TcpServer::hasData(uint8_t clientId) const
{
    if (clientId >= TCP_SERVER_MAX_CLIENTS)
        return false;

    const Conn& c = m_conns[clientId];
    return c.used && c.rx_ready;
}

bool TcpServer::send(uint8_t clientId, const void* data, uint16_t len)
{
    if (clientId >= TCP_SERVER_MAX_CLIENTS)
        return false;

    Conn& c = m_conns[clientId];
    if (!c.used || c.state != State::ACTIVE)
        return false;

    queueSend(&c, data, len);
    flushSend(c.pcb, &c);
    return true;
}

bool TcpServer::send(uint8_t clientId, const string &str) {
    return send(clientId, str.data(), str.size());
}

void TcpServer::broadcast(const void* data, uint16_t len)
{
    for (uint8_t i = 0; i < TCP_SERVER_MAX_CLIENTS; ++i)
        send(i, data, len);
}

uint16_t TcpServer::read(uint8_t clientId, void* out, uint16_t maxLen)
{
    if (clientId >= TCP_SERVER_MAX_CLIENTS || !out)
        return 0;

    Conn& c = m_conns[clientId];
    if (!c.used || !c.rx_ready)
        return 0;

    uint16_t n = (c.rx_len > maxLen-1) ? maxLen-1 : c.rx_len;

    std::memcpy(out, c.rx, n);

    c.rx_len = 0;
    c.rx_ready = false;
    return n;
}

string TcpServer::read(uint8_t clientId) {
    if (clientId >= TCP_SERVER_MAX_CLIENTS)
        return "";

    Conn& c = m_conns[clientId];
    if (!c.used || !c.rx_ready)
        return "";

    string result(reinterpret_cast<char*>(c.rx), c.rx_len - 1); // Exclude null terminator

    c.rx_len = 0;
    c.rx_ready = false;
    return result;
}

uint8_t TcpServer::clientCount() const {
    uint8_t count = 0;
    for (const auto& c : m_conns)
    {
        if (c.used && c.state == State::ACTIVE)
            ++count;
    }
    return count;
}

TcpServer::Client &TcpServer::clientRef(uint8_t clientId) {
    static Client clients[TCP_SERVER_MAX_CLIENTS] = {
            Client(this, 0),
            Client(this, 1),
            Client(this, 2)
    };

    return clients[clientId];

//    return Client(this, clientId);
}

TcpServer::Client* TcpServer::client(uint8_t clientId)
{
    if (clientId >= TCP_SERVER_MAX_CLIENTS)
        return nullptr;

    if (!m_conns[clientId].used)
        return nullptr;

    static Client clients[TCP_SERVER_MAX_CLIENTS] = {
            Client(this, 0),
            Client(this, 1),
            Client(this, 2)
    };

    return &clients[clientId];

//    return Client(this, clientId);
}

bool TcpServer::setPort(uint16_t port)
{
    /* 端口只能在 init 前修改 */
    if (m_listen != nullptr)
        return false;

    mport = port;
    return true;
}


TcpServer::Client::Client(TcpServer* srv, uint8_t idx)
        : m_srv(srv), m_idx(idx)
{
}

bool TcpServer::Client::valid() const
{
    if (!m_srv) return false;
    if (m_idx >= TCP_SERVER_MAX_CLIENTS) return false;
    return m_srv->m_conns[m_idx].used;
}

bool TcpServer::Client::hasData() const
{
    return valid() && m_srv->hasData(m_idx);
}

uint16_t TcpServer::Client::read(void* out, uint16_t maxLen)
{
    if (!valid()) return 0;
    return m_srv->read(m_idx, out, maxLen);
}

string TcpServer::Client::read() {
    if (!valid()) return "";
    return m_srv->read(m_idx);
}

bool TcpServer::Client::send(const void* data, uint16_t len)
{
    if (!valid()) return false;
    return m_srv->send(m_idx, data, len);
}

bool TcpServer::Client::send(const string &str) {
    return this->send(str.data(), str.size());
}

uint8_t TcpServer::Client::id() const {
    return m_idx;
}

void TcpServer::Client::write(const char *str, uint16_t len) {
    this->send(str, len);
}

void TcpServer::Client::write(uint8_t *str, uint16_t len) {
    this->send(str, len);
}

void TcpServer::Client::write(string String) {
    this->send(String);
}

uint16_t TcpServer::Client::print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    this->write(r);
    return r.length();
}

uint16_t TcpServer::Client::print(const string &String) {
    this->write(String);
    return String.length();
}

uint16_t TcpServer::Client::print(const char *s) {
    return this->print("%s",s);
}

uint16_t TcpServer::Client::print(char s) {
    return this->print("%c",s);
}

uint16_t TcpServer::Client::print(int integer) {
    return this->print("%d",integer);
}

uint16_t TcpServer::Client::println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    string r;
    r.resize(static_cast<size_t>(len) + 3);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    r += "\r\n";
    this->write(r);
    return r.length();
}

uint16_t TcpServer::Client::println(const string &String) {
    return this->print(String+"\r\n");
}

uint16_t TcpServer::Client::println(const char *s) {
    return this->print("%s\r\n",s);
}

uint16_t TcpServer::Client::println(int integer) {
    return this->print("%d\r\n",integer);
}

bool TcpServer::disconnect(uint8_t clientId)
{
    if (clientId >= TCP_SERVER_MAX_CLIENTS)
        return false;

    Conn& c = m_conns[clientId];
    if (!c.used || !c.pcb)
        return false;

    /* 解除回调，防止 race */
    tcp_arg(c.pcb, nullptr);
    tcp_recv(c.pcb, nullptr);
    tcp_sent(c.pcb, nullptr);
    tcp_err (c.pcb, nullptr);
    tcp_poll(c.pcb, nullptr, 0);

    /* 释放待发送数据 */
    if (c.tx)
    {
        pbuf_free(c.tx);
        c.tx = nullptr;
    }

    /* 优先尝试优雅关闭 */
    err_t e = tcp_close(c.pcb);
    if (e != ERR_OK)
    {
        /* 1.4.1 下 close 失败很常见，兜底 abort */
        tcp_abort(c.pcb);
    }

    freeConn(&c);
    return true;
}

void TcpServer::disconnectAll()
{
    for (uint8_t i = 0; i < TCP_SERVER_MAX_CLIENTS; ++i)
    {
        if (m_conns[i].used)
        {
            disconnect(i);
        }
    }
}

bool TcpServer::shutdown()
{
    /* 先踢掉所有客户端 */
    disconnectAll();

    if (!m_listen)
        return true;

    /* listen pcb 没有回调，但也要 close */
    err_t e = tcp_close(m_listen);
    if (e != ERR_OK)
    {
        tcp_abort(m_listen);
    }

    m_listen = nullptr;
    return true;
}












