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
    BoardNode OscadBoard;
    OscadBoard.m_strBoardType = "OSCAD";
    OscadBoard.m_vecPortKey.push_back("LINE");
    OscadBoard.m_vecPortKey.push_back("MAIN");
    OscadBoard.m_vecPortKey.push_back("OSC");

    int iSrcOscadBoardId = 1;
    int iDesOscadBoardId = 2;
    BoardHandle::GetInstance()->CreateBoardInfo(iSrcOscadBoardId, OscadBoard);
    BoardHandle::GetInstance()->CreateBoardInfo(iDesOscadBoardId, OscadBoard);

    BoardNode stOaBoard;
    stOaBoard.m_strBoardType = "OA";
    stOaBoard.m_vecPortKey.push_back("OUT");
    stOaBoard.m_vecPortKey.push_back("IN");
    int iSrcOaBoardId = 3;
    int iDesOaBoardId = 4;
    BoardHandle::GetInstance()->CreateBoardInfo(iSrcOaBoardId, stOaBoard);
    BoardHandle::GetInstance()->CreateBoardInfo(iDesOaBoardId, stOaBoard);

    BoardNode stOscBoard;
    stOscBoard.m_strBoardType = "OSC";
    stOscBoard.m_vecPortKey.push_back("OSC_IN");
    stOscBoard.m_vecPortKey.push_back("OSC_OUT");
    int iSrcOscBoardId = 5;
    int iDesOscBoardId = 6;
    BoardHandle::GetInstance()->CreateBoardInfo(iSrcOscBoardId, stOscBoard);
    BoardHandle::GetInstance()->CreateBoardInfo(iDesOscBoardId, stOscBoard);

    Fiber stFiber1;
    stFiber1.m_iLinkId = 1;
    stFiber1.m_DstPortNode = {iDesOscadBoardId, "LINE"};
    stFiber1.m_SrcPortNode = {iSrcOscadBoardId, "LINE"};

    m_vFiberInfo.push_back(stFiber1);

    Fiber stFiber2;
    stFiber2.m_iLinkId = 2;
    stFiber2.m_DstPortNode = {iDesOaBoardId, "IN"};
    stFiber2.m_SrcPortNode = {iDesOscadBoardId, "MAIN"};

    m_vFiberInfo.push_back(stFiber2);

    Fiber stFiber3;
    stFiber3.m_iLinkId = 3;
    stFiber3.m_DstPortNode = {iDesOscBoardId, "OSC_IN"};
    stFiber3.m_SrcPortNode = {iDesOscadBoardId, "OSC"};

    m_vFiberInfo.push_back(stFiber3);

    Fiber stFiber4;
    stFiber4.m_iLinkId = 4;
    stFiber4.m_DstPortNode = {iSrcOscadBoardId, "MAIN"};
    stFiber4.m_SrcPortNode = {iSrcOaBoardId, "MAIN"};

    m_vFiberInfo.push_back(stFiber4);

    Fiber stFiber5;
    stFiber5.m_iLinkId = 5;
    stFiber5.m_DstPortNode = {iSrcOscadBoardId, "OSC"};
    stFiber5.m_SrcPortNode = {iDesOscBoardId, "OSC_OUT"};

    m_vFiberInfo.push_back(stFiber5);
}

void LinkHandle::GetAllLinkInfo(vector<Fiber>& vFiberInfo)
{
    vFiberInfo = m_vFiberInfo;
}