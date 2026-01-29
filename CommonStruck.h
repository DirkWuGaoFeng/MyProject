#ifndef __COMMON_STRUCK_H__
#define __COMMON_STRUCK_H__

#include <vector>
#include <string>
#include <set>

using namespace std;

// 将字符串数组改为声明（使用extern关键字）
extern string strFiberColor[];
extern string strAlarmLevel[];
extern string strAlarmType[];

enum FiberColor
{
    FC_RED = 0,
    FC_YELLOW = 1,
    FC_GREEN = 2,
};

struct PortNode
{
    bool operator<(const PortNode& other) const
    {
        if (m_iBoardId != other.m_iBoardId)
            return m_iBoardId < other.m_iBoardId;
        return m_strPortKey < other.m_strPortKey;
    }

    bool operator != (const PortNode& other) const
    {
        return m_iBoardId != other.m_iBoardId || m_strPortKey != other.m_strPortKey;
    }

    int m_iBoardId;
    std::string m_strPortKey;
};

struct Fiber
{
    int m_iLinkId;
    std::vector<PortNode> m_vPortNode;
};

enum AlarmLevel
{
    AT_RED_ALARM = 0,
    AT_YELLOW_ALARM = 1,
};

enum AlarmType
{
    AT_SEND_ALARM = 0,
    AT_CLEAR_ALARM = 1,
};

struct AlarmNode
{
    PortNode m_stPortNode;
    AlarmLevel m_eAlarmLevel;
    AlarmType m_eAlarmType;
};

#endif // __COMMON_STRUCK_H__