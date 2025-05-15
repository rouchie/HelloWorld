#pragma once

#include "leopard/rqtcpsession.h"

class HelloSession final : public RQTcpSession
{
public:
    explicit HelloSession(int64_t uuid);
    ~HelloSession() override = default;

public:
    void Recved(const char *data, size_t size) override;

private:
    size_t m_sizeRecved = 0;
};
