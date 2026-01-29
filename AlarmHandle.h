#ifndef __ALARM_HANDLE_H__
#define __ALARM_HANDLE_H__

#include <vector>
#include <string>
#include <functional>
#include "CommonStruck.h"
#include "LinkHandle.h"
using namespace std;

// 定义告警回调函数类型
typedef function<void(const AlarmNode&)> AlarmCallback;

class AlarmHandle {
private:
    static AlarmHandle* m_pInstance;
    static mutex m_mtx;

    // 私有构造函数和析构函数，防止外部创建实例
    AlarmHandle();
    ~AlarmHandle();

    // 禁用拷贝构造和赋值运算符
    AlarmHandle(const AlarmHandle&) = delete;
    AlarmHandle& operator=(const AlarmHandle&) = delete;
public:
    // 获取唯一实例的静态方法
    static AlarmHandle* GetInstance();
    
    // 声明带回调函数参数的告警线程函数
    void AlarmThreadFunc(AlarmCallback callback = nullptr);
};

#endif // __ALARM_HANDLE_H__