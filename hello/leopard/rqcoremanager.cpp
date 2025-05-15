#include "rqcoremanager.h"
#include "rqevent.h"

#include "workflow/WFTaskFactory.h"

void ObjectInfo::PushMsg(RQMsg::Ptr msg)
{
	if (m_signal == false) {
		auto* task = WFTaskFactory::create_go_task("comm", m_communication, msg);
		task->start();
		return;
	}

	{
		WriteLock lock(m_rwlockmsg);
		m_listMsg.emplace_back(msg);
	}

	{
		WriteLock lock(m_rwlocktask);
		if (m_running) {
			// 已经有线程在跑，直接结束就行
			return;
		}
		m_running = true;
	}

	auto self = this->shared_from_this();
	auto* task = WFTaskFactory::create_go_task("comm", [this, self]() {
		while (true) {
			RQMsg::Ptr msg;
			{
				WriteLock lock(m_rwlockmsg);
				if (m_listMsg.empty()) {
					WriteLock lock(m_rwlocktask);
					m_running = false;
					return;
				}
				msg = m_listMsg.front(); m_listMsg.pop_front();
			}
			m_communication(msg);
		}
		});
	task->start();
}

RQCoreManager::PTR RQCoreManager::Inst()
{
    static RQCoreManager::PTR inst = std::make_shared<RQCoreManager>();
    return inst;
}

bool RQCoreManager::Start(sid_t sid)
{
    m_index.pop();

	WFGlobalSettings setting = GLOBAL_SETTINGS_DEFAULT;
	setting.endpoint_params.max_connections = 2048;
	setting.poller_threads = 16;
	setting.handler_threads = 16;
	WORKFLOW_library_init(&setting);

    RQEvent::Inst()->Start();

    return true;
}

bool RQCoreManager::Stop()
{
    return true;
}

bool RQCoreManager::AddModule(mid_t& mid, RQCommunication done, bool signal)
{
    bool bValid = (mid & 0XFFFF0000) == 0;

    if (!bValid) {
        // 非法的 module_id
        return false;
    }

    WriteLock lock(m_rwlock);

    mid_t id = m_index.pop();
    if (id > 0XFFFF) {
        // 超出了限制
        return false;
    }

    id = (id << 16) + mid;

    m_mapModule2Func[mid][id] = std::make_shared<ObjectInfo>(done, signal);
    mid = id;

    return true;
}

bool RQCoreManager::DelModule(mid_t mid)
{
    WriteLock lock(m_rwlock);

    mid_t id = mid;
    mid = mid & 0X0000FFFF;

    auto it = m_mapModule2Func.find(mid);
    if (it == m_mapModule2Func.end()) {
        return false;
    }

    it->second.erase(id);

    return true;
}

bool RQCoreManager::Send(mid_t recver, RQMsg::Ptr msg)
{
    mid_t mid = recver & 0X0000FFFF;

	ReadLock lock(m_rwlock);

	auto it = m_mapModule2Func.find(mid);
	if (it == m_mapModule2Func.end() || it->second.size() == 0) {
		// TODO: 返回不存在模块错误码
		return false;
	}

	auto ii = it->second.find(recver);
	if (ii != it->second.end()) {
		ii->second->PushMsg(msg);
	}
	else {
		for (auto& tt : it->second) {
			tt.second->PushMsg(msg);
		}
	}

	return true;
}

