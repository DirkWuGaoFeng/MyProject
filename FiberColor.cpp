#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <set>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <unordered_map>
#include "CommonStruck.h"
#include "LinkHandle.h"
#include "AlarmHandle.h"

using namespace std;

// 定义字符串数组（在头文件中已声明）
string strFiberColor[] = {"RED", "YELLOW", "GREEN"};
string strAlarmLevel[] = {"RED", "YELLOW"};
string strAlarmType[] = {"SEND_ALARM", "CLEAR_ALARM"};

// 为PortNode实现哈希函数
namespace std {
    template<> struct hash<PortNode> {
        size_t operator()(const PortNode& p) const {
            return hash<int>()(p.m_iBoardId) ^ (hash<string>()(p.m_strPortKey) << 1);
        }
    };
}

// 共享数据结构
map<int, FiberColor> m_mapLinkId2Color;
map<PortNode, Fiber> m_mapPortNode2Fiber;
map<PortNode, set<AlarmLevel>> m_mapPortNode2AlarmLevel;

// 基础互斥锁，保护共享数据
mutex mtx_link_color;
mutex mtx_port_fiber;
mutex mtx_port_alarm;

// 任务队列和线程池相关 - 基于任务分组的设计
typedef function<void()> Task;

struct TaskQueue {
    queue<Task> task_queue;
    mutex mtx;
    condition_variable cv;
    
    // 添加默认构造函数
    TaskQueue() = default;
    
    // 启用移动构造函数和移动赋值运算符
    TaskQueue(TaskQueue&& other) noexcept {
        task_queue = move(other.task_queue);
    }
    
    TaskQueue& operator=(TaskQueue&& other) noexcept {
        if (this != &other) {
            task_queue = move(other.task_queue);
        }
        return *this;
    }
    
    // 禁用复制构造函数和赋值运算符
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;
};

vector<TaskQueue> task_queues;
atomic<bool> stop_threads(false);
vector<thread> thread_pool;

// 计算任务应该分配到哪个队列
int GetQueueIndex(int linkId) {
    // 使用连纤ID的哈希值来分配队列
    hash<int> hasher;
    return hasher(linkId) % task_queues.size();
}

// 线程池工作函数
void WorkerThread(int queueIndex) {
    TaskQueue& queue = task_queues[queueIndex];
    while (!stop_threads) {
        Task task;
        {
            unique_lock<mutex> lock(queue.mtx);
            queue.cv.wait(lock, [&] { return !queue.task_queue.empty() || stop_threads; });
            if (stop_threads && queue.task_queue.empty()) {
                return;
            }
            task = move(queue.task_queue.front());
            queue.task_queue.pop();
        }
        task();
    }
}

void CacheAlarmNode(const AlarmNode& stAlarmNode) {
    {
        unique_lock<mutex> lock(mtx_port_alarm);
        if (m_mapPortNode2AlarmLevel.find(stAlarmNode.m_stPortNode) == m_mapPortNode2AlarmLevel.end()) {
            m_mapPortNode2AlarmLevel[stAlarmNode.m_stPortNode] = set<AlarmLevel>();
        } else {
            if (m_mapPortNode2AlarmLevel[stAlarmNode.m_stPortNode].count(stAlarmNode.m_eAlarmLevel) > 0) {
                return;
            }
        }
        m_mapPortNode2AlarmLevel[stAlarmNode.m_stPortNode].insert(stAlarmNode.m_eAlarmLevel);
    }
}

void ClearAlarmNode(const AlarmNode& stAlarmNode) {
    {
        unique_lock<mutex> lock(mtx_port_alarm);
        if (m_mapPortNode2AlarmLevel.find(stAlarmNode.m_stPortNode) != m_mapPortNode2AlarmLevel.end()) {
            m_mapPortNode2AlarmLevel[stAlarmNode.m_stPortNode].erase(stAlarmNode.m_eAlarmLevel);
        }
    }
}

int GetMaxAlarmLevel(const PortNode& stPortNode) {
    int iMaxAlarmLevel = -1;
    {
        unique_lock<mutex> lock(mtx_port_alarm);
        if (m_mapPortNode2AlarmLevel.find(stPortNode) != m_mapPortNode2AlarmLevel.end()) {
            for (auto& eAlarmLevel : m_mapPortNode2AlarmLevel[stPortNode]) {
                if (eAlarmLevel > iMaxAlarmLevel) {
                    iMaxAlarmLevel = eAlarmLevel;
                }
            }
        }
    }
    return iMaxAlarmLevel;
}

void GetAlarmLevel(const PortNode& stPortNode, set<AlarmLevel>& vecAlarmLevel) {
    unique_lock<mutex> lock(mtx_port_alarm);
    if (m_mapPortNode2AlarmLevel.find(stPortNode) != m_mapPortNode2AlarmLevel.end()) {
        vecAlarmLevel = m_mapPortNode2AlarmLevel[stPortNode]; 
    } else {
        vecAlarmLevel.clear();
    }
}

bool GetPairPortNode(const PortNode& stPortNode, PortNode& stPairPortNode) {
    unique_lock<mutex> lock(mtx_port_fiber);
    if (m_mapPortNode2Fiber.find(stPortNode) != m_mapPortNode2Fiber.end()) {
        PortNode stSrcPort = m_mapPortNode2Fiber[stPortNode].m_SrcPortNode;
        BoardNode boardNode;
        BoardHandle::GetInstance()->GetBoardInfo(stSrcPort.m_iBoardId, boardNode);
        if (boardNode.m_strBoardType == "OSCAD")
        {
            PortNode stOscadPortNode = stSrcPort;
            if (stSrcPort.m_strPortKey == "MAIN") {
                stOscadPortNode.m_strPortKey = "OSC";
            }
            else if (stSrcPort.m_strPortKey == "OSC") {
                stOscadPortNode.m_strPortKey = "MAIN";
            }

            if (m_mapPortNode2Fiber.find(stOscadPortNode) != m_mapPortNode2Fiber.end())
            {
                stPairPortNode = m_mapPortNode2Fiber[stOscadPortNode].m_DstPortNode;
            }
            else{
                return false;
            }
        }
        
        return true;
    }
    return false;
}

void ReportMsg(const int& iLnkId, const FiberColor &eFiberColor) {
    unique_lock<mutex> lock(mtx_link_color);
    if (m_mapLinkId2Color.find(iLnkId) == m_mapLinkId2Color.end()) {
        if (eFiberColor != FC_GREEN) {
            m_mapLinkId2Color[iLnkId] = eFiberColor;
            cout << "Link ID: " << iLnkId << ", Fiber Color set to: " << strFiberColor[eFiberColor] << endl;
        }
    } else {
        if (m_mapLinkId2Color[iLnkId] != eFiberColor) {
            FiberColor eOldFiberColor = m_mapLinkId2Color[iLnkId];
            if (eFiberColor != FC_GREEN) {
                m_mapLinkId2Color[iLnkId] = eFiberColor;
            } else {
                m_mapLinkId2Color.erase(iLnkId);
            }
            cout << "Link ID: " << iLnkId << ", Fiber Color changed from: " << strFiberColor[eOldFiberColor] << " to: " << strFiberColor[eFiberColor] << endl;
        } else {
            cout << "Link ID: " << iLnkId << ", Fiber Color remains: " << strFiberColor[eFiberColor] << endl;
        }
    }
}

void ProcessAlarmNode(const AlarmNode& stAlarmNode) {
    int iLnkId = -1;
    {
        unique_lock<mutex> lock(mtx_port_fiber);
        if (m_mapPortNode2Fiber.find(stAlarmNode.m_stPortNode) != m_mapPortNode2Fiber.end()) {
            iLnkId = m_mapPortNode2Fiber[stAlarmNode.m_stPortNode].m_iLinkId;
        } else {
            cout << "Error: PortNode not found in Fiber map." << endl;
            return;
        }
    }

    FiberColor eFiberColor = FC_GREEN;
    
    // 获取连纤上所有端口的告警信息
    vector<PortNode> allPortNodes;
    { 
        unique_lock<mutex> lock(mtx_port_fiber);
        if (m_mapPortNode2Fiber.find(stAlarmNode.m_stPortNode) != m_mapPortNode2Fiber.end()) {
            allPortNodes = m_mapPortNode2Fiber[stAlarmNode.m_stPortNode].m_vPortNode;
        }
    }
    
    // 检查是否所有端口都有红色告警
    bool allRedAlarm = true;
    // 检查是否有任何端口有黄色告警
    bool anyYellowAlarm = false;
    
    for (const auto& portNode : allPortNodes) {
        set<AlarmLevel> vecAlarmLevel;
        GetAlarmLevel(portNode, vecAlarmLevel);
        
        if (vecAlarmLevel.count(AT_RED_ALARM) == 0) {
            allRedAlarm = false;
        }
        if (vecAlarmLevel.count(AT_YELLOW_ALARM) > 0) {
            anyYellowAlarm = true;
        }
    }
    
    // 根据告警情况设置连纤颜色
    if (allPortNodes.size() > 1) { // 多端口连纤
        if (allRedAlarm) {
            eFiberColor = FC_RED;
        } else if (anyYellowAlarm) {
            eFiberColor = FC_YELLOW;
        } else {
            eFiberColor = FC_GREEN;
        }
    } else { // 单端口连纤
        int iMaxAlarmLevel = GetMaxAlarmLevel(stAlarmNode.m_stPortNode);
        if (iMaxAlarmLevel > 0) {
            eFiberColor = FC_RED;
        } else if (iMaxAlarmLevel == 0) {
            eFiberColor = FC_YELLOW;
        } else {
            eFiberColor = FC_GREEN;
        }
    }

    ReportMsg(iLnkId, eFiberColor);
}

void ReceiveAlarmNode(const AlarmNode& stAlarmNode) {
    // 首先获取连纤ID
    int iLnkId = -1;
    {
        unique_lock<mutex> lock(mtx_port_fiber);
        if (m_mapPortNode2Fiber.find(stAlarmNode.m_stPortNode) != m_mapPortNode2Fiber.end()) {
            iLnkId = m_mapPortNode2Fiber[stAlarmNode.m_stPortNode].m_iLinkId;
        } else {
            cout << "Error: PortNode not found in Fiber map when allocating queue." << endl;
            return;
        }
    }
    
    // 将告警处理任务分配到固定队列（基于连纤ID）
    int queueIndex = GetQueueIndex(iLnkId);
    TaskQueue& queue = task_queues[queueIndex];
    
    {
        unique_lock<mutex> lock(queue.mtx);
        queue.task_queue.emplace([stAlarmNode]() {
            if (stAlarmNode.m_eAlarmType == AT_CLEAR_ALARM) {
                ClearAlarmNode(stAlarmNode);
            } else {
                CacheAlarmNode(stAlarmNode);
            }
            ProcessAlarmNode(stAlarmNode);
        });
    }
    queue.cv.notify_one();
}

void initFiberMap() {
    vector<Fiber> vFiberInfo;
    LinkHandle::GetInstance()->GetAllLinkInfo(vFiberInfo);
    
    unique_lock<mutex> lock(mtx_port_fiber);
    for (const auto& fiber : vFiberInfo) {
        m_mapPortNode2Fiber[fiber.m_DstPortNode] = fiber;
    }
}

void MyAlarmCallback(const AlarmNode& alarm) {

    cout << "Port Node: (" << alarm.m_stPortNode.m_iBoardId << ", " << alarm.m_stPortNode.m_strPortKey << ") " 
        << "Alarm Type: " << strAlarmType[alarm.m_eAlarmType] << ", Alarm Level: " << strAlarmLevel[alarm.m_eAlarmLevel] << endl;
            
    ReceiveAlarmNode(alarm);
}

int main() {
    LinkHandle::GetInstance()->initializeLinkHandle();

    initFiberMap();
    
    // 初始化线程池
    int threadCount = 4;
    task_queues.resize(threadCount);
    for (int i = 0; i < threadCount; ++i) {
        thread_pool.emplace_back(WorkerThread, i);
    }

    thread alarmThread(&AlarmHandle::AlarmThreadFunc, AlarmHandle::GetInstance(), MyAlarmCallback);
    alarmThread.join();
    
    // 停止线程池
    stop_threads = true;
    for (auto& queue : task_queues) {
        queue.cv.notify_all();
    }
    for (auto& t : thread_pool) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    return 0;
}