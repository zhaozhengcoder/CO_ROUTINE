#include <iostream>
#include <string>
#include <unistd.h>
#include "co_schedule.h"

struct CO1_DATA
{
    std::string req_data;
    std::string cb_data;
};

struct CO2_DATA
{
    double example_val1;
    float  example_val2;
    int    example_val3;
};

void demo1_co_task_func(void * user_data)
{
    std::cout<<"send req to db ... "<<std::endl;
    sleep(1);

    // db callback to call resume
    CO_SCHEDULE::get_instance()->co_yeild();

    // get db result
    CO1_DATA * co_data = static_cast<CO1_DATA *>(user_data);
    std::cout<<"handle db result "<< co_data->cb_data <<std::endl;
}

void default_co_task_func(void * user_data)
{
    std::cout<<"first enter co task func"<<std::endl;

    // db callback to call resume
    CO_SCHEDULE::get_instance()->co_yeild();

    // get db result
    std::cout<<"scond enter co task func"<<std::endl;
}

int demo1()
{
    CO1_DATA co1_data;
    co1_data.req_data = std::string("req sql to db");
    int co_id = CO_SCHEDULE::get_instance()->co_new(demo1_co_task_func, &co1_data);
    // 第一次执行协程
    CO_SCHEDULE::get_instance()->co_resume(co_id);

    // 第二次进入协程，从yield地方继续执行
    co1_data.cb_data = std::string("rsp data from db");
    CO_SCHEDULE::get_instance()->co_resume(co_id);
    return 0;
}

int demo2()
{
    CO1_DATA co1_data;
    CO2_DATA co2_data;

    int co_id1 = CO_SCHEDULE::get_instance()->co_new(default_co_task_func, &co1_data);   // 指定协程的任务函数
    int co_id2 = CO_SCHEDULE::get_instance()->co_new(default_co_task_func, &co2_data);

    // 第一次执行协程（执行的函数是new的时候注册的）
    CO_SCHEDULE::get_instance()->co_resume(co_id1);
    CO_SCHEDULE::get_instance()->co_resume(co_id2);

    // 第二次进入协程，从yield地方继续执行
    CO_SCHEDULE::get_instance()->co_resume(co_id1);
    CO_SCHEDULE::get_instance()->co_resume(co_id2);
    return 0;
}

int main()
{
    demo1();
    // demo2();

    // co_schedule.co_free();
}