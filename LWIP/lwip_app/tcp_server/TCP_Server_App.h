/**
* @Author kokirika
* @Name TCP_SERVER_APP
* @Date 2026-02-25
*/

#ifndef KOKIRIKA_TCP_SERVER_APP_H
#define KOKIRIKA_TCP_SERVER_APP_H

#include "Tcp_Server.h"

class TcpServer_APP : public TcpServer
{
public:

    enum class ChanMode : uint8_t {
        MODBUS = 0,
        CMD
    };

    struct ChanCtx {
        ChanMode mode;
        bool     marked;

        void reset()
        {
            mode   = ChanMode::MODBUS;
            marked = false;
        }
    };

    static TcpServer_APP& instance(uint16_t port = 502);

    /* 轮询处理（自动做：断线复位 + 模式分流） */
    template<typename ModbusHandler>
    void pollOnce(ModbusHandler& MDTCP);

    /* 状态查询 */
    ChanMode mode(uint8_t id) const;
    bool     marked(uint8_t id) const;

    void resetChan(uint8_t id);
    void resetAllChan();

protected:
    explicit TcpServer_APP(uint16_t port);

    virtual void onClientDisconnected(uint8_t id);
    virtual void handleCmd(uint8_t id,
                           const std::string& rx,
                           std::string& out);

private:
    static bool isTrimPlusOnly(const std::string& s);

    ChanCtx m_ctx[TCP_SERVER_MAX_CLIENTS];
    bool    m_lastConn[TCP_SERVER_MAX_CLIENTS];
};


/* ================= 模板实现（必须放头文件） ================= */

template<typename ModbusHandler>
void TcpServer_APP::pollOnce(ModbusHandler& MDTCP)
{
    for (uint8_t i = 0; i < TCP_SERVER_MAX_CLIENTS; ++i)
    {
        // Client* cl = this->client(i);
        // bool nowConn = (cl != nullptr);
        //
        // /* 断线边沿检测 */
        // if (m_lastConn[i] && !nowConn)
        // {
        //     onClientDisconnected(i);
        // }
        //
        // m_lastConn[i] = nowConn;
        //
        // if (!nowConn)
        //     continue;

        if (!this->hasData(i))
            continue;

        std::string rx = this->read(i);
        if (rx.empty())
            continue;

        /* 整包 +++ 进入 CMD */
        if (isTrimPlusOnly(rx))
        {
            m_ctx[i].mode   = ChanMode::CMD;
            m_ctx[i].marked = true;
            this->send(i, "OK\r\nCMD>\r\n");
            continue;
        }

        if (m_ctx[i].mode == ChanMode::CMD)
        {
            std::string out;
            handleCmd(i, rx, out);
            if (!out.empty())
                this->send(i, out);
        }
        else
        {
            std::string tx;
            MDTCP.handleFrame(rx, tx);
            if (!tx.empty())
                this->send(i, tx);
        }
    }
}

#endif