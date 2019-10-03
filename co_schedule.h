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

const int STACK_SIZE = 1024 * 1024;
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
    coroutine_func task_func;
    ucontext_t ctx;             // 协程上下文
    CO_CROUTINE_STATUS status;
    char* stack;                // 协程栈

    ptrdiff_t cap;
    ptrdiff_t size; 
};

class CO_SCHEDULE
{
public:
    int co_new(coroutine_func task_func);

    int co_resume(int id);

    int co_yeild(int co_id);
    
    void _save_stack(CO_ROUTINE *C, char * top);

private:
    static void _co_entry(void *data);
    int _generate_co_id();
    std::map<int, CO_ROUTINE> co_pool;

    char stack[STACK_SIZE];
    ucontext_t main;
};

#endif