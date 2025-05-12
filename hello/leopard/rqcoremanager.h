#pragma once

#include "rqmsg.h"
#include "rqevent.h"

#include "workflow/RWLock.h"
#include "utils/UtilsIndex.h"

#include <mutex>
#include <unordered_map>
#include <functional>

using RQCommunication = std::function<void(RQMsg::PTR req)>;

class ObjectInfo : public std::enable_shared_from_this<ObjectInfo>
{
public:
    using PTR = std::shared_ptr<ObjectInfo>;

public:
    ObjectInfo(RQCommunication communication, bool signal) : m_communication(communication), m_signal(signal) {}

public:
    void PushMsg(RQMsg::PTR msg);

private:
    RWLock m_rwlocktask;
    bool m_running = false;

    RWLock m_rwlockmsg;
    std::list<RQMsg::PTR> m_listMsg;

    RQCommunication m_communication;
    bool m_signal;
};

class RQCoreManager : public std::enable_shared_from_this<RQCoreManager>
{
public:
    using PTR = std::shared_ptr<RQCoreManager>;

public:
    static PTR Inst();

public:
    bool Start(sid_t sid);
    bool Stop();

public:
    bool AddModule(mid_t& mid, RQCommunication done, bool signal = false);
    bool DelModule(mid_t mid);

public:
    bool Send(mid_t recver, RQMsg::PTR msg);

public:
    bool SendBack(RQMsg::PTR msg, int64_t result)
    {
		auto m = std::make_shared<RQMsg>(msg->Recver(), msg->Sender(), msg->Command(), result, msg->Count(), msg->Message(), msg->Binary());
		return Send(msg->Snd(), m);
    }

    bool SendBack(RQMsg::PTR msg, int64_t result, const std::string& message)
    {
		auto m = std::make_shared<RQMsg>(msg->Recver(), msg->Sender(), msg->Command(), result, msg->Count(), message, msg->Binary());
		return Send(msg->Snd(), m);
    }

private:
    std::mutex m_mutex;
    std::unordered_map<mid_t, std::list<RQMsg::PTR>> m_mapTask;

    RWLock m_rwlock;
	std::unordered_map<mid_t, std::unordered_map<mid_t, ObjectInfo::PTR>> m_mapModule2Func;

    CIndex<mid_t> m_index;
};

#define REQ(RCV, MMM) RQCoreManager::Inst()->Send(RCV, MMM) 
#define RSP(MMM, ...) RQCoreManager::Inst()->SendBack(MMM, __VA_ARGS__) 

//#define SEND(RCV, MMM) RQCoreManager::Inst()->Send(RCV, MMM) 
//#define SENDBACK(MMM, ...) RQCoreManager::Inst()->SendBack(MMM, __VA_ARGS__) 

