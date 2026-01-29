#include "LinkHandle.h"

// 初始化静态成员变量
LinkHandle* LinkHandle::m_pInstance = nullptr;
mutex LinkHandle::m_mtx;

LinkHandle::LinkHandle()
{
    // 可以在构造函数中进行一些初始化
}

LinkHandle::~LinkHandle()
{
    // 可以在析构函数中进行资源释放
}

// 获取唯一实例的静态方法（线程安全）
LinkHandle* LinkHandle::GetInstance()
{
    if (m_pInstance == nullptr)
    {
        // 使用双重检查锁定模式确保线程安全
        unique_lock<mutex> lock(m_mtx);
        if (m_pInstance == nullptr)
        {
            m_pInstance = new LinkHandle();
        }
    }
    return m_pInstance;
}

void LinkHandle::initializeLinkHandle()
{
    // 创建一个连纤，关联3个端口（测试多端口连纤场景）
    Fiber stFiber1;
    stFiber1.m_iLinkId = 1;
    stFiber1.m_vPortNode.push_back({1, "Port1"});
    stFiber1.m_vPortNode.push_back({2, "Port2"});
    stFiber1.m_vPortNode.push_back({3, "Port3"}); // 添加第三个端口

    m_vFiberInfo.push_back(stFiber1);

    // 创建一个连纤，关联2个端口（普通场景）
    Fiber stFiber2;
    stFiber2.m_iLinkId = 2;
    stFiber2.m_vPortNode.push_back({4, "Port1"});
    stFiber2.m_vPortNode.push_back({5, "Port2"});

    m_vFiberInfo.push_back(stFiber2);

    // 创建一个连纤，只关联1个端口（特殊场景）
    Fiber stFiber3;
    stFiber3.m_iLinkId = 3;
    stFiber3.m_vPortNode.push_back({6, "Port1"});

    m_vFiberInfo.push_back(stFiber3);
}

void LinkHandle::GetAllLinkInfo(vector<Fiber>& vFiberInfo)
{
    vFiberInfo = m_vFiberInfo;
}