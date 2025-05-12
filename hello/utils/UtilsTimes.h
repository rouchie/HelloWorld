#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

#define SSP(n) std::this_thread::sleep_for(std::chrono::seconds(n))
#define MSP(n) std::this_thread::sleep_for(std::chrono::milliseconds(n))
#define USP(n) std::this_thread::sleep_for(std::chrono::microseconds(n))

inline uint64_t GetUS()
{
	const auto tp = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
	return static_cast<uint64_t>(tp.time_since_epoch().count());
}

inline uint64_t GetMS()
{
	auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	return (uint64_t) tp.time_since_epoch().count();
}

inline uint64_t GetSecond()
{
	auto tp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
	return (uint64_t) tp.time_since_epoch().count();
}

class CCurrentTime {
public:
	CCurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		std::tm now_tm;
#ifdef _WIN32  
		localtime_s(&now_tm, &now_time_t);
#else  
		localtime_r(&now_time_t, &now_tm);
#endif
		
		m_year       = now_tm.tm_year + 1900;
		m_month      = now_tm.tm_mon + 1;
		m_day        = now_tm.tm_mday;
		m_hour       = now_tm.tm_hour;
		m_minute     = now_tm.tm_min;
		m_second     = now_tm.tm_sec;
		m_millSecond = (int) milliseconds.count();
	}

	std::string Now(bool bMill)
	{
		char buf[128] = {0};
		if (bMill) {
			snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%03d", m_year, m_month, m_day, m_hour, m_minute, m_second, m_millSecond);
		}
		else {
			snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", m_year, m_month, m_day, m_hour, m_minute, m_second);
		}
		return std::string(buf);
	}

private:
	int m_year       = 0;
	int m_month      = 0;
	int m_day        = 0;
	int m_hour       = 0;
	int m_minute     = 0;
	int m_second     = 0;
	int m_millSecond = 0;
};

inline std::string Now(bool bMill = false)
{
	return CCurrentTime().Now(bMill);
}

inline std::string GetFormattedTime()
{
	// 获取当前时间
	std::time_t now = std::time(nullptr);
	std::tm gmt_time;
	gmtime_s(&gmt_time, &now);

	// 创建一个字符串流
	std::ostringstream oss;

	// 使用 std::put_time 来格式化时间
	oss << std::put_time(&gmt_time, "%a, %b %d %Y %H:%M:%S GMT");

	return oss.str();
}

inline int GmTime(struct tm& tm)
{
    time_t utc;
    time(&utc);

#ifdef _WIN32
    return gmtime_s(&tm, &utc);
#else
    return gmtime_r(&utc, &tm);
#endif
}

/**
 * @brief 将 2024-01-02 03:04:05 字符串解析到 struct tm 结构体中
 * @param utc 
 * @param tm 
 * @return 
 */
inline int Str2GmTime(const std::string& utc, struct tm& tm)
{
    std::istringstream ss(utc);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    // 检查解析是否成功  
    if (ss.fail()) {
        return -1;
    }
    return 0;
}
