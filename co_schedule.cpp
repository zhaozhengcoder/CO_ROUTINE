#include "co_schedule.h"
#include "ucontext.h"
#include <iostream>
#include <stdlib.h>

CO_SCHEDULE * CO_SCHEDULE::instance = NULL;
CO_SCHEDULE::CO_SCHEDULE(){}
CO_SCHEDULE::~CO_SCHEDULE(){}

int CO_SCHEDULE::_generate_co_id()
{
    static int co_id = 1;
    return co_id++;
}

int CO_SCHEDULE::_get_running_coid()
{
    return running_coid;
}

int CO_SCHEDULE::_set_running_coid(int run_coid)
{
    running_coid = run_coid;
    return 0;
}

int CO_SCHEDULE::co_new(coroutine_func task_func, void * user_data)
{
    int co_id = _generate_co_id();
    co_pool[co_id].status = CO_READY;
    co_pool[co_id].task_func = task_func;
    co_pool[co_id].user_data = user_data;
    return co_id;
}

void CO_SCHEDULE::_co_entry(void * data)
{
    std::cout<<"entry"<<std::endl;
    CO_ROUTINE *co_rountine = (CO_ROUTINE *)data;
    co_rountine->task_func(co_rountine->user_data);
}

// 协程开始，或者协程resume回来
int CO_SCHEDULE::co_resume(int co_id)
{
    CO_ROUTINE & co_rountine = co_pool[co_id];
    switch (co_rountine.status)
    {
        case CO_READY:
            getcontext(&co_rountine.ctx);
            co_rountine.ctx.uc_stack.ss_sp = stack;        // 设置上下文C->ctx的栈
            co_rountine.ctx.uc_stack.ss_size = STACK_SIZE; // 设置上下文C->ctx的栈容量
            co_rountine.ctx.uc_link = &main;               //
            co_rountine.status = CO_RUNING;                // 修改状态
            _set_running_coid(co_id);
            makecontext(&co_rountine.ctx, (void(*)(void))_co_entry, 1, (void *)&co_rountine);
            swapcontext(co_rountine.ctx.uc_link, &co_rountine.ctx);
            break;
        case CO_SUSPEND:
            memcpy(stack + STACK_SIZE - co_rountine.size, co_rountine.stack, co_rountine.size);
            co_rountine.status = CO_RUNING;
            _set_running_coid(co_id);
            swapcontext(&main, &co_rountine.ctx);
            break;
        default:
            std::cout << "co status error, co_id"<< co_id << std::endl;
            break;
    }
}

// 把当前运行的协程yield出去
int CO_SCHEDULE::co_yeild()
{
    int co_id = _get_running_coid();
    co_pool[co_id].status = CO_SUSPEND;
    _save_stack(&co_pool[co_id], stack + STACK_SIZE);
    co_pool[co_id].status = CO_SUSPEND;
    swapcontext(&co_pool[co_id].ctx, &main);
}

// 保存协程栈
void CO_SCHEDULE::_save_stack(CO_ROUTINE *C, char * top)
{
    char dummy = 0;
    assert(top - &dummy <= STACK_SIZE);
    if (C->cap < top - &dummy)
    {
        // 为协程栈分配内存空间
        free(C->stack);
        C->cap = top - &dummy;
        C->stack = (char *)malloc(C->cap);
    }

    C->size = top - &dummy;
    memcpy(C->stack, &dummy, C->size); // TODO - 不是很明白为什么这种方式可以保存协程栈
}

int CO_SCHEDULE::co_free()
{
    // 遍历map
    for(auto iter = co_pool.begin(); iter != co_pool.end(); iter++)
    {
        int co_id = iter->first;
        free(co_pool[co_id].stack);
    }
    return 0;
}

CO_SCHEDULE * CO_SCHEDULE::get_instance()
{
    if (instance == NULL)
    {
        instance = new CO_SCHEDULE();
    }
    return instance;
}

// TODO
// 1. 封装一个单例 (ok)
// 2. 协程 class CO_ROUTINE; 里面的user_data; 