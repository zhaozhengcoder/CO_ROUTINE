#ifndef __CO_SCHEDULE_H__
#define __CO_SCHEDULE_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include "ucontext.h"
#include <map>

typedef void (*coroutine_func)(void *ud);

enum CO_CROUTINE_STATUS
{
    CO_READY = 1,
    CO_SUSPEND,
    CO_RUNING,
};

class CO_ROUTINE
{
public:
    coroutine_func task_func;  // 每个协程执行的回调函数
    ucontext_t ctx;            // 协程的ctx
    CO_CROUTINE_STATUS status; // 当前协程的执行状态
    void * user_data;

    char *stack;    // 协程栈，用来存放协程被切换出去的时候的上下文
    ptrdiff_t cap;  // char * stack 的最大容量
    ptrdiff_t size; // char * stack 的大小
};

/*
协程管理器，负责调度和管理多个协程；
协程存放在map中
*/
class CO_SCHEDULE
{
public:
    int co_new(coroutine_func task_func, void *user_data);
    int co_resume(int co_id);
    int co_yeild();

private:
    static void _co_entry(void *data);              // 每一个协程执行前的入口函数
    void _save_stack(CO_ROUTINE *C, char *top);     // 协程被切换出去的时候，保存当前的协程栈

    int _generate_co_id(); // 分配协程id
    int _get_running_coid();
    int _set_running_coid(int run_coid);

    std::map<int, CO_ROUTINE> co_pool; // map，存放协程id，以及对应的协程数据结构
    int running_coid;                  // 正在运行的协程id

    static const int STACK_SIZE = 1024 * 1024;
    char stack[STACK_SIZE];
    ucontext_t main; // 非对称协程的框架，协程1切换的时候，先切换的main，然后在切换到切成2
};

#endif