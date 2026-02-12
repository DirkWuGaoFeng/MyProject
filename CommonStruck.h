#ifndef __COMMON_STRUCK_H__
#define __COMMON_STRUCK_H__

#include <vector>
#include <string>
#include <set>
#include "BoardHandle.h"

using namespace std;

// 将字符串数组改为声明（使用extern关键字）
extern string strFiberColor[];
extern string strAlarmLevel[];
extern string strAlarmType[];

// 定义枚举类型 FiberColor
enum FiberColor
{
    FC_RED = 0,
    FC_YELLOW = 1,
    FC_GREEN = 2,
};

struct PortNode
{
    PortNode(int iBoardId, const string& strPortKey)
        : m_iBoardId(iBoardId), m_strPortKey(strPortKey) {}
    ~PortNode() {}

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

    int m_iBoardId;             //板卡ID
    std::string m_strPortKey;   //端口键值
};

struct Fiber
{
    int m_iLinkId;              //链路ID
    PortNode m_SrcPortNode;     //源端口节点
    PortNode m_DstPortNode;     //目的端口节点
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

// 定义结构体 AlarmNode，用于表示告警节点
struct AlarmNode
{
    AlarmNode(int iBoardId, const string& strPortKey, AlarmLevel eAlarmLevel, AlarmType eAlarmType)
        : m_stPortNode(iBoardId, strPortKey), m_eAlarmLevel(eAlarmLevel), m_eAlarmType(eAlarmType) {}
    ~AlarmNode() {}

    PortNode m_stPortNode;      //端口节点
    AlarmLevel m_eAlarmLevel;   //告警级别
    AlarmType m_eAlarmType;     //告警类型
};

#endif // __COMMON_STRUCK_H__