#pragma once

class RQBaseSession
{
public:
    using PTR = std::shared_ptr<RQBaseSession>;

public:
    RQBaseSession(mid_t mid, int64_t uuid) : m_id(mid), m_uuid(uuid) {}
    RQBaseSession(int64_t uuid) : m_uuid(uuid) {}
    RQBaseSession() {}

public:
    int64_t UUID() const { return m_uuid; }
    int64_t UUID(int64_t uuid) { m_uuid = uuid; return m_uuid; }

public:
    mid_t Id() const { return m_id; }
    mid_t Id(mid_t mid) { m_id = mid; return m_id; }

protected:
    mid_t m_id = 0;
    int64_t m_uuid = 0;
};

