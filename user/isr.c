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
* 文件名称          isr
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

#include "isr_config.h"
#include "isr.h"
#include "includes.h"

#define Sample_MAX 1024           // 定义采集麦克风点数
#define MIC_NUM 3072

uint16  mic_flag=0;                // 麦克风目前可采集标志位
uint16 mic_sample_num=0;          // 当前采集点数
float icm_add[2]={0};//angle[0]为陀螺仪积分值

cfloat32 mic_1[MIC_NUM]; // 保存麦克风数据的缓存
cfloat32 mic_2[MIC_NUM];
cfloat32 mic_3[MIC_NUM];
cfloat32 mic_4[MIC_NUM];

uint16 L_flag=0;
uint16 L_count;
uint16 run_flag=0;
extern uint16 L1,L2,L3,L4;//电感采集

float first_flag=0;//第一次采集标志位
extern int num;//在xcorr.c中声明

extern int Charge_flag;//充电标志位，用于中途没电时进行充电
extern int16 center_flag;
extern float Value_VB;
extern uint8 key1_state;
extern PID_t Steer_PID;//串级pid外环
extern PID_t Speed_PID;//串级pid内环


// **************************** PIT中断函数 ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, CCU6_0_CH0_INT_VECTAB_NUM, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);
//    tsl1401_collect_pit_handler();                  // 线阵CCD采集

    // 采集麦克风数据
    // 因为在做FFT计算的时候我们需要减去直流偏量，因此这里固定减2000，实际应该自己通过测量没有声音时的数值来确定这里应该减去多少
    // 当没有声音的时候，减去偏置后得到的数值应该是在0附近，说明减的偏置就是正确的
    mic_1[mic_sample_num].real = adc_convert(ADC1_CH2_A10)-2000;
    mic_1[mic_sample_num].imag = 0;

    mic_2[mic_sample_num].real = adc_convert(ADC1_CH3_A11) - 2000;
    mic_2[mic_sample_num].imag = 0;

    mic_3[mic_sample_num].real = adc_convert(ADC1_CH4_A12) - 2000;
    mic_3[mic_sample_num].imag = 0;

    mic_4[mic_sample_num].real = adc_convert(ADC1_CH5_A13)-2000;
    mic_4[mic_sample_num].imag = 0;

    mic_sample_num++;

    //第一次采集数据时需要特殊处理
    if(mic_sample_num==2048&&first_flag==0)//对第一次采集特殊处理
    {
        first_flag=1;
        mic_flag=1;
    }

    //num=0时，不进行处理
    //num=1时，处理1、2
    //num=2时，处理2、3
    //num=3时，处理3、1
    if(mic_sample_num==2048&&first_flag==1)
    {
        num=1;
    }
    if(mic_sample_num==3072&&first_flag==1)
    {
        num=2;
        mic_sample_num = 0;
    }
    if(mic_sample_num==1024&&first_flag==1)
    {
       num=3;
    }


}

float gyro_x = 0.0;
extern int16 icm20602_gyro_x;

IFX_INTERRUPT(cc60_pit_ch1_isr, CCU6_0_CH1_INT_VECTAB_NUM, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH1);

    icm20602_get_gyro();
    gyro_x = icm20602_gyro_transition(icm20602_gyro_x);
    //icm_add[0] += gyro_x * (-0.005)+0.00122;
    //Get_Speed();


    if(Charge_flag==1)
    {
        gpio_set_level(SD, 1);

        Charge_Start();
    }

//电感测试
//        if(L_flag==1)
//        {
//            L_count++;
//        }
//        else
//        {
//            L1=adc_mean_filter_convert (ADC0_CH0_A0,4);
//            L2=adc_mean_filter_convert (ADC0_CH1_A1,4);
//            if(L1>500||L2>500)
//            {
//                 Motor_stop();//小车停止
//                 PID_Refresh(Steer_PID);//更新pid数据
//                 PID_Refresh(Steer_PID);
//                 run_flag=1;
//                 L_flag=1;
//             }
//        }
//
//        if(L_count==600)//电感停止检测的时间为 600*5ms
//        {
//            L_flag=0;
//            L_count=0;
//        }

}

IFX_INTERRUPT(cc61_pit_ch0_isr, CCU6_1_CH0_INT_VECTAB_NUM, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU61_CH0);




}

IFX_INTERRUPT(cc61_pit_ch1_isr, CCU6_1_CH1_INT_VECTAB_NUM, CCU6_1_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU61_CH1);





}
// **************************** PIT中断函数 ****************************


// **************************** 外部中断函数 ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, EXTI_CH0_CH4_INT_VECTAB_NUM, EXTI_CH0_CH4_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    if(exti_flag_get(ERU_CH0_REQ0_P15_4))           // 通道0中断
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);
        wireless_module_uart_handler();                 // 无线模块统一回调函数



    }

    if(exti_flag_get(ERU_CH4_REQ13_P15_5))          // 通道4中断
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);




    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, EXTI_CH1_CH5_INT_VECTAB_NUM, EXTI_CH1_CH5_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    if(exti_flag_get(ERU_CH1_REQ10_P14_3))          // 通道1中断
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);



    }

    if(exti_flag_get(ERU_CH5_REQ1_P15_8))           // 通道5中断
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);




    }
}

// 由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
// IFX_INTERRUPT(exti_ch2_ch6_isr, EXTI_CH2_CH6_INT_VECTAB_NUM, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // 开启中断嵌套
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // 通道2中断
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // 通道6中断
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, EXTI_CH3_CH7_INT_VECTAB_NUM, EXTI_CH3_CH7_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    if(exti_flag_get(ERU_CH3_REQ6_P02_0))           // 通道3中断
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();                     // 摄像头触发采集统一回调函数
    }
    if(exti_flag_get(ERU_CH7_REQ16_P15_1))          // 通道7中断
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);




    }
}
// **************************** 外部中断函数 ****************************


// **************************** DMA中断函数 ****************************
IFX_INTERRUPT(dma_ch5_isr, DMA_INT_VECTAB_NUM, DMA_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    camera_dma_handler();                           // 摄像头采集完成统一回调函数
}
// **************************** DMA中断函数 ****************************


// **************************** 串口中断函数 ****************************
IFX_INTERRUPT(uart0_tx_isr, UART0_INT_VECTAB_NUM, UART0_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart0_handle);


}
IFX_INTERRUPT(uart0_rx_isr, UART0_INT_VECTAB_NUM, UART0_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart0_handle);

#if DEBUG_UART_USE_INTERRUPT                        // 如果开启 debug 串口中断
        debug_interrupr_handler();                  // 调用 debug 串口接收处理函数 数据会被 debug 环形缓冲区读取
#endif                                              // 如果修改了 DEBUG_UART_INDEX 那这段代码需要放到对应的串口中断去

}
IFX_INTERRUPT(uart0_er_isr, UART0_INT_VECTAB_NUM, UART0_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart0_handle);



}

// 串口1默认连接到摄像头配置串口
IFX_INTERRUPT(uart1_tx_isr, UART1_INT_VECTAB_NUM, UART1_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart1_handle);




}
IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    camera_uart_handler();                          // 摄像头参数配置统一回调函数
}
IFX_INTERRUPT(uart1_er_isr, UART1_INT_VECTAB_NUM, UART1_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart1_handle);




}


// 串口2默认连接到无线转串口模块
IFX_INTERRUPT(uart2_tx_isr, UART2_INT_VECTAB_NUM, UART2_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart2_handle);





}
IFX_INTERRUPT(uart2_rx_isr, UART2_INT_VECTAB_NUM, UART2_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart2_handle);
    wireless_module_uart_handler();                 // 无线模块统一回调函数

}
IFX_INTERRUPT(uart2_er_isr, UART2_INT_VECTAB_NUM, UART2_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart2_handle);

}



IFX_INTERRUPT(uart3_tx_isr, UART3_INT_VECTAB_NUM, UART3_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart3_handle);




}
IFX_INTERRUPT(uart3_rx_isr, UART3_INT_VECTAB_NUM, UART3_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart3_handle);





}
IFX_INTERRUPT(uart3_er_isr, UART3_INT_VECTAB_NUM, UART3_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart3_handle);





}
// **************************** 串口中断函数 ****************************
