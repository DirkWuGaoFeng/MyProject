#ifndef BOARD_HANDLE_H
#define BOARD_HANDLE_H

#include <vector>
#include <string>
#include <map>
#include <mutex>
using namespace std;

struct BoardNode
{
    string m_strBoardType;
    vector<string> m_vecPortKey;
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

    void CreateBoardInfo(const int &iBoardId, const BoardNode& boardNode);

    void GetBoardInfo(const int &iBoardId, BoardNode& boardNode);
};

#endif