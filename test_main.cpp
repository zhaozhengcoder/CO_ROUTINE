#include <iostream>
#include "co_schedule.h"

CO_SCHEDULE co_schedule;

void co_task_func(void *user_data)
{
    std::cout<<"co task func"<<std::endl;
    co_schedule.co_yeild();
    std::cout<<"co task func end"<<std::endl;
}

int demo1()
{
    int co_id = co_schedule.co_new(co_task_func);
    // 第一次执行协程
    co_schedule.co_resume(co_id);
    // 第二次进入协程，从yield地方继续执行
    co_schedule.co_resume(co_id);    
}

int demo2()
{
    int co_id1 = co_schedule.co_new(co_task_func);
    int co_id2 = co_schedule.co_new(co_task_func);

    // 第一次执行协程
    co_schedule.co_resume(co_id1);
    co_schedule.co_resume(co_id2);

    // 第二次进入协程，从yield地方继续执行
    co_schedule.co_resume(co_id1);
    co_schedule.co_resume(co_id2);
}

int main()
{
    // demo1();
    demo2();
}