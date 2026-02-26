/**
* @Author kokirika
* @Name TCP_SERVER_APP
* @Date 2026-02-25
*/

#include "Tcp_Server_App.h"

/* ================= 单例 ================= */

TcpServer_APP& TcpServer_APP::instance(uint16_t port)
{
    static TcpServer_APP s(port);
    return s;
}

/* ================= 构造 ================= */

TcpServer_APP::TcpServer_APP(uint16_t port)
        : TcpServer(port)
{

    for (auto& c : m_ctx)
        c.reset();

    for (auto& b : m_lastConn)
        b = false;
}

/* ================= 状态查询 ================= */

TcpServer_APP::ChanMode TcpServer_APP::mode(uint8_t id) const
{
    if (id >= TCP_SERVER_MAX_CLIENTS)
        return ChanMode::MODBUS;

    return m_ctx[id].mode;
}

bool TcpServer_APP::marked(uint8_t id) const
{
    if (id >= TCP_SERVER_MAX_CLIENTS)
        return false;

    return m_ctx[id].marked;
}

void TcpServer_APP::resetChan(uint8_t id)
{
    if (id < TCP_SERVER_MAX_CLIENTS)
        m_ctx[id].reset();
}

void TcpServer_APP::resetAllChan()
{
    for (auto& c : m_ctx)
        c.reset();
}

/* ================= 断线处理 ================= */

void TcpServer_APP::onClientDisconnected(uint8_t id)
{
    if (id < TCP_SERVER_MAX_CLIENTS)
        m_ctx[id].reset();
}

/* ================= CMD 模式处理 ================= */

void TcpServer_APP::handleCmd(uint8_t id,
                              const std::string& rx,
                              std::string& out)
{
    std::string s = rx;

    /* 去掉换行 */
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n'))
        s.pop_back();

    /* 转大写 */
    for (auto& c : s)
        if (c >= 'a' && c <= 'z')
            c = char(c - 'a' + 'A');

    if (s == "EXIT" || s == "ATO")
    {
        m_ctx[id].mode = ChanMode::MODBUS;
        out = "OK\r\nMODBUS\r\n";
        return;
    }

    if (s == "MODE?")
    {
        out = (m_ctx[id].mode == ChanMode::CMD) ?
              "CMD\r\n" : "MODBUS\r\n";
        return;
    }

    if (s == "MARK?")
    {
        out = m_ctx[id].marked ?
              "MARK=1\r\n" : "MARK=0\r\n";
        return;
    }

    out = "ERR\r\nCMD>\r\n";
}

/* ================= +++ 判断 ================= */

bool TcpServer_APP::isTrimPlusOnly(const std::string& s)
{
    size_t a = 0;
    size_t b = s.size();

    while (a < b && (s[a]==' '||s[a]=='\r'||s[a]=='\n'||s[a]=='\t'))
        ++a;

    while (b > a && (s[b-1]==' '||s[b-1]=='\r'||s[b-1]=='\n'||s[b-1]=='\t'))
        --b;

    return (b - a == 3 &&
            s[a]   == '+' &&
            s[a+1] == '+' &&
            s[a+2] == '+');
}