#include "BoardHandle.h"

// 初始化静态成员变量
BoardHandle* BoardHandle::m_pInstance = nullptr;
mutex BoardHandle::m_mtx;

BoardHandle::BoardHandle()
{
    // 可以在构造函数中进行一些初始化
}

BoardHandle::~BoardHandle()
{
    // 可以在析构函数中进行资源释放
}

// 获取唯一实例的静态方法（线程安全）
BoardHandle* BoardHandle::GetInstance()
{
    if (m_pInstance == nullptr)
    {
        // 使用双重检查锁定模式确保线程安全
        unique_lock<mutex> lock(m_mtx);
        if (m_pInstance == nullptr)
        {
            m_pInstance = new BoardHandle();
        }
    }
    return m_pInstance;
}

void BoardHandle::initializeBoardHandle()
{
    // 创建一个板卡，关联3个端口（测试多端口板卡场景）
    BoardNode stOscadBoard;
    stOscadBoard.m_strBoardType = "OSCAD";
    stOscadBoard.m_vecPortKey.push_back("LINE");
    stOscadBoard.m_vecPortKey.push_back("MAIN");
    stOscadBoard.m_vecPortKey.push_back("OSC"); // 添加第三个端口 

    m_mapBoardInfo[1] = stOscadBoard;

    // 创建一个板卡，关联2个端口（普通场景）
    BoardNode stOaBoard;
    stOaBoard.m_strBoardType = "OA";
    stOaBoard.m_vecPortKey.push_back("OUT");
    stOaBoard.m_vecPortKey.push_back("IN");

    m_mapBoardInfo[2] = stOaBoard;
    
    BoardNode stOscBoard;
    stOscBoard.m_strBoardType = "OSC";
    stOscBoard.m_vecPortKey.push_back("OSC_IN");
    stOscBoard.m_vecPortKey.push_back("OSC_OUT");

    m_mapBoardInfo[3] = stOscBoard;
}

void BoardHandle::CreateBoardInfo(const int &iBoardId, const BoardNode& boardNode)
{
    m_mapBoardInfo[iBoardId] = boardNode;
}

void BoardHandle::GetBoardInfo(const int &iBoardId, BoardNode& boardNode)
{
    boardNode = m_mapBoardInfo[iBoardId];
}