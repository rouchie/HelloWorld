#include "rqobject.h"
#include "rqcoremanager.h"
#include "workflow/WFTaskFactory.h"

RQObject::RQObject(mid_t id, bool mutex)
    : m_mid(id), m_bMutex(mutex)
{
}

RQObject::~RQObject()
{

}

void RQObject::Start()
{
    std::weak_ptr<RQObject> weak = this->shared_from_this();

    RQCoreManager::Inst()->AddModule(m_mid, [weak](RQMsg::PTR req) {
        auto ptr = weak.lock();
        if (!ptr) {
            // TODO: resp 回复该模块已释放
            return;
        }

        ptr->Callback(req);
    }, m_bMutex);

    Started();
}

mid_t RQObject::Mid() const
{
    return m_mid;
}

int RQObject::Callback(RQMsg::PTR msg)
{
    auto command = msg->Command();

    auto it = m_mapCallback.find(command);
    if (it == m_mapCallback.end()) {
        // TODO: 需要返回给发送者错误码
        return -1;
    }

	return it->second(msg);
}

int RQObject::Connect(cid_t command, std::function<int(RQMsg::PTR)> done)
{
    m_mapCallback[command] = done;
    return 0;
}

int RQObject::Disconnect(cid_t command)
{
    m_mapCallback.erase(command);
    return 0;
}

std::string RQObject::CreateTimer(time_t seconds, long nanoseconds, RQMsg::PTR msg, uint32_t times)
{
    std::weak_ptr<RQObject> weak = shared_from_this();

    auto f = [seconds, nanoseconds, msg, times, weak](WFTimerTask* task) {
        auto ptr = weak.lock();
        if (!ptr) return;

        ptr->Callback(msg);

        uint32_t new_timer = 0;

        if (times == 1) {
            // 结束定时器
            return;
        }

        if (times == 0) {
            new_timer = 0;
        }
        else {
            new_timer = times - 1;
        }

        ptr->CreateTimer(seconds, nanoseconds, msg, new_timer);
	};

    WFTimerTask* timer = WFTaskFactory::create_timer_task(seconds, nanoseconds, f);
    timer->start();

    return std::string();
}

std::string RQObject::CreateTimer(uint32_t microseconds, RQMsg::PTR msg, uint32_t times)
{
    return CreateTimer(microseconds / 1000, microseconds % 1000 * 1000, msg, times);
}


void RQObject::CancelTimer(const std::string& timer)
{
}


class A : public std::enable_shared_from_this<A>
{
public:
    void Hello() {
        std::weak_ptr<A> weak = this->shared_from_this();
        auto f = [weak]() {};
    }
};

class B : public std::enable_shared_from_this<B>
{
public:
    void World() {
        std::weak_ptr<B> weak = this->shared_from_this();
        auto f = [weak]() {};
    }
};

class C : public A, public B
{
public:
    void Start()
    {
        Hello();
    }
};

void AAA()
{
    std::shared_ptr<C> c = std::make_shared<C>();
    c->Start();
}


