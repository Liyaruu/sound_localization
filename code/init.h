#ifndef CODE_INIT_H_
#define CODE_INIT_H_

#include "zf_common_headfile.h"

#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (10)                                // 存储数据用的页码 倒数第一个页码

#define MOTOR3_A   ATOM0_CH4_P02_4  //定义3电机正转PWM引脚
#define MOTOR3_B   ATOM0_CH5_P02_5  //定义3电机反转PWM引脚

#define MOTOR4_A   ATOM0_CH6_P02_6  //定义4电机正转PWM引脚
#define MOTOR4_B   ATOM0_CH7_P02_7  //定义4电机反转PWM引脚

#define RV ADC2_CH0_A16 //VCC谐振电压——充电电压
#define RI ADC2_CH1_A17 //充电电流
#define VB ADC2_CH4_A20 //超级电容电压

#define SD P10_3 //Buck电路使能端
#define Buck_PWM ATOM0_CH1_P21_3 //Buck输出PWM的端口

typedef struct
{
    float SumError;  //误差累计
    float LastError;  //上一次误差
    float PrevError;  //上上次误差
    float LastData;  //上次数据
    float Kp;
    float Ki;
    float Kd;
}PID_t;

void ADC_init(void);
void GPIO_init(void);
void PWM_init(void);
void Key_count(void);

void init_cpu0(void);
void init_cpu1(void);
#endif /* CODE_INIT_H_ */
