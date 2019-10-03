#include <iostream>
#include "co_schedule.h"

CO_SCHEDULE co_schedule;

void co_task_func(void *user_data)
{
    std::cout<<"co task func"<<std::endl;
    co_schedule.co_yeild(1);

    std::cout<<"co task func end"<<std::endl;
}

int main()
{

    int co_id = co_schedule.co_new(co_task_func);

    co_schedule.co_resume(co_id);

    co_schedule.co_resume(co_id);
}