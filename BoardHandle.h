#ifndef BOARD_HANDLE_H
#define BOARD_HANDLE_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <map>

using namespace std;

struct BoardNode
{
    string m_strBoardType;         //板卡类型
    vector<string> m_vecPortKey;  //单盘关联的端口
};

class BoardHandle
{
private:
    map<int, BoardNode> m_mapBoardInfo;
    static BoardHandle* m_pInstance;
    static mutex m_mtx;

    // 私有构造函数和析构函数，防止外部创建实例
    BoardHandle();
    ~BoardHandle();

    // 禁用拷贝构造和赋值运算符
    BoardHandle(const BoardHandle&) = delete;
    BoardHandle& operator=(const BoardHandle&) = delete;

public:
    // 获取唯一实例的静态方法
    static BoardHandle* GetInstance();

    void initializeBoardHandle();

    void GetAllBoardInfo(const int &iBoardId, BoardNode& boardNode);
};

#endif 