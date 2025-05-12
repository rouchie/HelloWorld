#pragma once

#include "rqmsg.h"

#include <mutex>
#include <functional>
#include <unordered_map>

class RQObject : public std::enable_shared_from_this<RQObject>
{
public:
    using PTR = std::shared_ptr<RQObject>;

public:
    explicit RQObject(mid_t id, bool mutex = true);

    virtual ~RQObject();

public:
    void Start();
    virtual void Started() {}

public:
    mid_t Mid() const;

public:
    int Callback(RQMsg::PTR msg);

protected:
	int Connect(cid_t command, std::function<int(RQMsg::PTR)> done);
	int Disconnect(cid_t command);

public:
    std::string CreateTimer(time_t seconds, long nanoseconds, RQMsg::PTR msg, uint32_t times);
    std::string CreateTimer(uint32_t microseconds, RQMsg::PTR msg, uint32_t times);
    void CancelTimer(const std::string& timer);

protected:
    mid_t m_mid = 0;

private:
	std::unordered_map<cid_t, std::function<int(RQMsg::PTR req)>> m_mapCallback;

    bool m_bMutex = false;
    std::mutex m_mtx;
};

template <typename OBJECT, typename... ARGS>
RQObject::PTR RQCreateObject(ARGS&&... args) {
	RQObject::PTR ptr = std::make_shared<OBJECT>(std::forward<ARGS>(args)...);
	ptr->Start();
	return ptr;
}

