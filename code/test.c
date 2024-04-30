#pragma section all "cpu1_dsram"
#include "includes.h"

uint8 key1_state=1,key2_state=1,key3_state=1,key4_state=1;//按键状态读取
uint8 parameter=1;//具体调节哪个参数
uint8 send_flag=0;//蓝牙发送数据只赋值一次
extern uint8   bluetooth_ch9141_data;
extern PID_t Steer_PID;//外环
extern PID_t Speed_PID;//内环

extern int Charge_flag;//充电标志位
extern int Charge_Flag_start;//真正开始充电标志位
extern int32 Charge_Time;//充电持续时间——单位 ms
extern int32 second;//充电持续时间——单位 s
extern int32 time;//充电持续时间——单位 s

float p1,i1,d1,p2,i2,d2;



void test(void)
{
    while(1)
    {
        //显示当前调参值
       // ips114_show_uint(0, 20, parameter, 3);

        //选择需要调节的参数
        key2_state=gpio_get_level(P22_1);
        while(!gpio_get_level(P22_1));
        if(key2_state==0)
        {
            ips114_show_uint(0, 20, 1 , 3);


//            gpio_set_level(SD, 0);
//            pwm_set_duty(Buck_PWM,0);
//
//            //标志位改变状态
//            Charge_flag=0;
//            //center_flag=1;
//            Charge_Flag_start=0;
//
//            //充电时间复位
//            Charge_Time=0;
//            second=0;
//            time=0;
//
//            parameter++;
//            if(parameter==7)
//            {
//                parameter=1;
//            }
//            ips114_show_uint(0, 20, parameter, 3);
//            send_flag=0;
        }

        //确认选择参数
        key3_state=gpio_get_level(P22_2);
        while(!gpio_get_level(P22_2));

        if(key3_state==0)
        {

            ips114_show_uint(0, 20, 2, 3);
//          switch(parameter)
//          {
//            case 1:
//                if(send_flag==0)
//                {
//                    Speed_PID.Kp=(float)bluetooth_ch9141_data*0.1;
//                    ips114_show_uint(0, 40, bluetooth_ch9141_data, 3);
//                    send_flag=1;
//                }
//                ips114_show_uint(0, 60, 22, 3);
//                break;
//            case 2:
//                if(send_flag==0)
//                {
//                    Speed_PID.Ki=(float)bluetooth_ch9141_data*0.1;
//                    ips114_show_uint(0, 40, bluetooth_ch9141_data, 3);
//                    send_flag=1;
//                }
//                 break;
//            case 3:
//                if(send_flag==0)
//                {
//                    Speed_PID.Kd=(float)bluetooth_ch9141_data*0.1;
//                    ips114_show_uint(0, 40, bluetooth_ch9141_data, 3);
//                    send_flag=1;
//                }
//                 break;
//            case 4:
//                if(send_flag==0)
//                {
//                    Steer_PID.Kp=(float)bluetooth_ch9141_data*0.1;
//                    ips114_show_uint(0, 40, bluetooth_ch9141_data, 3);
//                    send_flag=1;
//                }
//                 break;
//            case 5:
//                if(send_flag==0)
//                {
//                    Steer_PID.Ki=(float)bluetooth_ch9141_data*0.1;
//                    ips114_show_uint(0, 40, bluetooth_ch9141_data, 3);
//                    send_flag=1;
//                }
//                 break;
//            case 6:
//                if(send_flag==0)
//                {
//                    Steer_PID.Kd=(float)bluetooth_ch9141_data*0.1;
//                    ips114_show_uint(0, 40, bluetooth_ch9141_data, 3);
//                    send_flag=1;
//                }
//                 break;
//          }//switch
        }//if



        //退出调参
        key4_state=gpio_get_level(P22_3);
        while(!gpio_get_level(P22_3));
        if(key4_state==0)
        {
            break;
        }
    }
}
#pragma section all restore
