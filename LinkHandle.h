#ifndef __LINK_HANDLE_H__
#define __LINK_HANDLE_H__

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include "CommonStruck.h"

using namespace std;

class LinkHandle
{
private:
    vector<Fiber> m_vFiberInfo;
    static LinkHandle* m_pInstance;
    static mutex m_mtx;

    // 私有构造函数和析构函数，防止外部创建实例
    LinkHandle();
    ~LinkHandle();

    // 禁用拷贝构造和赋值运算符
    LinkHandle(const LinkHandle&) = delete;
    LinkHandle& operator=(const LinkHandle&) = delete;

public:
    // 获取唯一实例的静态方法
    static LinkHandle* GetInstance();

    void initializeLinkHandle();

    void GetAllLinkInfo(vector<Fiber>& vFiberInfo);
};

#endif // __LINK_HANDLE_H__