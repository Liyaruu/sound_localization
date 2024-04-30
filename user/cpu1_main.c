/*********************************************************************************************************************
* TC377 Opensourec Library 即（TC377 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC377 开源库的一部分
*
* TC377 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu1_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC377TP
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-11-03       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
#include "includes.h"

float ANGLE[3]={0.0};//cpu1-互相关角度值
extern float xcorr_angle;//cpu0-硅麦阵列角度结果
extern int MAXi_14,MAXi_23;//mic14和mic23互相关数组最大下标值
extern int angle_flag;//ANGLE[0]是否在被cpu1使用

uint16 L1,L2,L3,L4;//ADC采集电感值
extern uint16 L_flag;//打开电感检测标志位
extern uint16 run_flag;

extern uint8   bluetooth_ch9141_data;//蓝牙发送数据

extern int16 center_flag;//打开电机标志位

extern float gyro_x;
extern int32 Differ_PWM;//串级pid输出值


extern uint8 key1_state;//key1状态
extern int Charge_flag;//充电标志位
//extern int16 N_L, N_R;     //编码器读取值
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中


// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 enableInterrupts(); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 disableInterrupts(); 来拒绝响应任何的中断，因此需要我们自己手动调用 enableInterrupts(); 来开启中断的响应。


// **************************** 代码区域 ****************************
void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    // 此处编写用户代码 例如外设初始化代码等
    //char txt[256];

//    //第一次使用时
//    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
//    {
//        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);                // 擦除这一页
//    }

//    //读取Flash区的数据
//    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);           // 将数据从 flash 读取到缓冲区
//    if(flash_union_buffer[0].int8_type==1)
//    {
//
//        Charge_flag=0;//开始即采集声音寻找信标
//    }
//    flash_buffer_clear();
//
    init_cpu1();


    // 此处编写用户代码 例如外设初始化代码等
    //cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码
        //编码器测试
        //        Motor_PWM();
//                ips114_show_int(0, 0,MAXi_14, 5);
//                ips114_show_int(0, 20,MAXi_23, 5);


//        //按键1进入Flash修改
//        //按键2按下——小车直接启动
//        //按键3按下——再按复位键，超级电容重新充电至12V
//            key1_state=gpio_get_level(P22_0);
//            while(!gpio_get_level(P22_0));
//            if(key1_state==0)
//            {
//                 ips114_show_string(0, 0, "test");
//                 test();
//                 ips114_show_string(0, 0, "exit");
//            }

        if(angle_flag==0)
        {
            ANGLE[0]=xcorr_angle;

        }

        //ips114_show_float(20,0,ANGLE[0], 4, 3);

        if(center_flag==1)
        {
            DifferControl();

            Motor_PWM();

        }


//增加电感检测
//            if(run_flag==1)
//            {
//                system_delay_ms(250);
//                run_flag=0;
//            }

//蓝牙测试
//                sprintf(txt, "%d,%f\n",Differ_PWM,ANGLE[0]);
//                uart_write_string(UART_2,txt);
//                system_delay_ms(100);


        // 此处编写需要循环执行的代码
    }
}
#pragma section all restore
