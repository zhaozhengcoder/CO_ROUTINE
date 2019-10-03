#include "co_schedule.h"
#include "ucontext.h"
#include <iostream>
#include <stdlib.h>

int CO_SCHEDULE::_generate_co_id()
{
    static int co_id = 1;
    return co_id++;
}

int CO_SCHEDULE::co_new(coroutine_func task_func)
{
    int co_id = _generate_co_id();
    co_pool[co_id].status = CO_READY;
    co_pool[co_id].task_func = task_func;
    return co_id;
}

void CO_SCHEDULE::_co_entry(void *data)
{
    std::cout<<"entry"<<std::endl;
    CO_ROUTINE *co_rountine = (CO_ROUTINE *)data;
    co_rountine->task_func(NULL);
}

int CO_SCHEDULE::co_resume(int id)
{
    CO_ROUTINE & co_rountine = co_pool[id];
    switch (co_rountine.status)
    {
        case CO_READY:
            getcontext(&co_rountine.ctx);
            co_rountine.ctx.uc_stack.ss_sp = stack;        // 设置上下文C->ctx的栈
            co_rountine.ctx.uc_stack.ss_size = STACK_SIZE; // 设置上下文C->ctx的栈容量
            co_rountine.ctx.uc_link = &main;               //
            
            co_rountine.status = CO_RUNING;
            makecontext(&co_rountine.ctx, (void(*)(void))_co_entry, 1, (void *)&co_rountine);
            swapcontext(co_rountine.ctx.uc_link, &co_rountine.ctx);
            break;
        
        case CO_SUSPEND:
            memcpy(stack + STACK_SIZE - co_rountine.size, co_rountine.stack, co_rountine.size);
            co_rountine.status = CO_RUNING;
            swapcontext(&main, &co_rountine.ctx);
            break;

        default:
            std::cout << "default" << std::endl;
            break;
    }
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

// 协程yield出去
int CO_SCHEDULE::co_yeild(int co_id)
{
    // 修改状态
    co_pool[co_id].status = CO_SUSPEND;

    _save_stack(&co_pool[co_id], stack + STACK_SIZE);
    co_pool[co_id].status = CO_SUSPEND;

    swapcontext(&co_pool[co_id].ctx, &main);
}