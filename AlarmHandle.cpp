#include "AlarmHandle.h"
#include <random>
#include <chrono>
#include <thread>

// 初始化静态成员变量
AlarmHandle* AlarmHandle::m_pInstance = nullptr;
mutex AlarmHandle::m_mtx;

AlarmHandle::AlarmHandle()
{
    // 可以在构造函数中进行一些初始化
}

AlarmHandle::~AlarmHandle()
{
    // 可以在析构函数中进行资源释放
}

// 获取唯一实例的静态方法（线程安全）
AlarmHandle* AlarmHandle::GetInstance()
{
    if (m_pInstance == nullptr)
    {
        // 使用双重检查锁定模式确保线程安全
        unique_lock<mutex> lock(m_mtx);
        if (m_pInstance == nullptr)
        {
            m_pInstance = new AlarmHandle();
        }
    }
    return m_pInstance;
}

void AlarmHandle::AlarmThreadFunc(AlarmCallback callback) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> alarmLevelDist(0, 1);
    uniform_int_distribution<> boardIdDist(1, 6); // 1-6个板卡，对应3个连纤
    uniform_int_distribution<> portIdDist(1, 1);
    uniform_int_distribution<> sleepDist(1, 5);   // 随机休眠1-5秒，加快测试
    uniform_int_distribution<> actionDist(0, 100);

    vector<PortNode> portNodes = {
        {1, "Port1"}, {2, "Port2"}, {3, "Port3"}, // 连纤1的3个端口
        {4, "Port1"}, {5, "Port2"},                // 连纤2的2个端口
        {6, "Port1"}                                // 连纤3的1个端口
    };

    while (true) {
        for (const auto& portNode : portNodes) {
            int action = actionDist(gen);
            AlarmType alarmType;
            AlarmLevel alarmLevel;

            if (action < 70) { // 70%概率产生告警
                alarmType = AT_SEND_ALARM;
                int level = alarmLevelDist(gen);
                alarmLevel = (level < 50) ? AT_RED_ALARM : AT_YELLOW_ALARM;
            } else { // 30%概率清除告警
                alarmType = AT_CLEAR_ALARM;
                int level = alarmLevelDist(gen);
                alarmLevel = (level < 50) ? AT_RED_ALARM : AT_YELLOW_ALARM;
            }
            
            AlarmNode alarmNode = {portNode, alarmLevel, alarmType};
            
            // 调用回调函数（如果提供）
            if (callback) {
                callback(alarmNode);
            }
        }

        int sleepTime = sleepDist(gen);
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }
}