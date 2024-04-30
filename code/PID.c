#pragma section all "cpu1_dsram"

#include "includes.h"

float Steer_Error[3] = {0.0};// 外环-角度环
float Speed_Error[4] = {0.0};// 内环-角速度环

extern float ANGLE[3];//目标角度
extern uint16 Direction_Flag;
extern int16 center_flag ;
extern float gyro_x;
int32 Differ_PWM = 0;

PID_t Steer_PID;//外环
PID_t Speed_PID;//内环

//PID参数初始化
void PID_Init(void)
{

    Steer_PID.LastData = Speed_PID.LastData = 0.0;
    Steer_PID.LastError = Speed_PID.LastError = 0.0;
    Steer_PID.PrevError = Speed_PID.PrevError = 0.0;
    Steer_PID.SumError = Speed_PID.SumError = 0.0;
    //外环
    Steer_PID.Kp = 1.5;
    Steer_PID.Ki = 2.5;
    Steer_PID.Kd = 0.5;

    //内环

    //速度PWM=4000时 2.0  3.5  0.3

    Speed_PID.Kp = 2.0;//2.5
    Speed_PID.Ki = 3.5;//4.0
    Speed_PID.Kd = 0.3;//0.3
}

void PID_Refresh(PID_t p)
{
    p.LastData=0;
    p.LastError=0;
    p.PrevError=0;
    p.SumError=0;
}

//在charge.c中使用
////位置式PID
//float PID_Realize(PID_t  p, float NowData, float SetData)
//{
//    float iError,   // 当前误差
//          Realize;  // 最后得出的实际增量
//
//    iError = SetData - NowData; // 计算当前误差
//    p.SumError += p.Ki * iError;   // 误差积分
////    p->SumError = RANGE_PROTECT(p->SumError, p->T, -(p->T));
//
//    if(p.SumError >= 40 || p.SumError <= -40)
//    {
//        if(p.SumError >= 40)
//        {
//            p.SumError = 40;
//        }
//        if(p.SumError <= -40)
//        {
//            p.SumError = -40;
//        }
//    }
//    Realize =  (p.Kp) * iError + p.SumError + p.Kd * (iError - p.LastError) ;
//
//    p.PrevError = p.LastError;    // 更新前次误差
//    p.LastError = iError;          // 更新上次误差
//    p.LastData  = NowData;         // 更新上次数据
//
//    return Realize; // 返回实际值
//}

//增量式PID
int32 PID_Increase(PID_t  p, int32 NowData, int32 SetData)
{
    int32 iError, //当前误差
        Increase; //最后得出的实际增量

    iError = SetData - NowData; // 计算当前误差

    Increase = (int32)(p.Kp * (iError - p.LastError) + p.Ki * iError + p.Kd * (iError - 2 * p.LastError + p.PrevError));

    p.PrevError = p.LastError; // 更新前次误差
    p.LastError = (float)iError;       // 更新上次误差

    return Increase; // 返回增量
}



void DifferControl(void)
{
    if(ANGLE[0] <= 0.0)
    {
        Direction_Flag = 0;
    }
    else if(ANGLE[0] >= 0.0)
    {
        Direction_Flag = 1;
    }
    if(Direction_Flag == 1)
    {

       DirectError_Get();//获取偏差


       Direction_out(); //偏差PWM获取
    }

}


void DirectError_Get(void)  //外环,角度环
{



      Steer_Error[0] = (float)PID_Increase(Steer_PID, ANGLE[0], 90.0);

//    b = Steer_Error[0] * 3.8 - 0.25/150;
//    Differ_PWM = (int32) b ;
//
//    if(Differ_PWM>=650)
//    {
//        Differ_PWM=650;
//    }
//    if(Differ_PWM<=-650)
//    {
//        Differ_PWM=-650;
//    }
//    Steer_Error[0]=0; //开启这句话，表转向环只有内环，外环输出为0
}


float error[3] = {0.0};

void Direction_out(void)//内环,角速度环
{

//   float a;
//  error[0] = Steer_Error[0]*1.0 - gyro_x*1.0; //声音信号偏差+陀螺仪10.3  0.19

  Speed_Error[0] = (float)PID_Increase(Speed_PID, gyro_x*1.0, Steer_Error[0]);
//  error[1] =  error[0];

//  Speed_Error[3] = Speed_Error[2];
//  Speed_Error[2] = Speed_Error[1];
//  Speed_Error[1] = Speed_Error[0];

  Differ_PWM=Speed_Error[0];
//  a = Speed_Error[0]*0.8+Speed_Error[1]*0.1+Speed_Error[2]*0.06+Speed_Error[3]*0.04;//滤波
//  Differ_PWM = (int32)a;
  //ips114_show_int(0, 0, Differ_PWM, 5);

  if(Differ_PWM>=1200)
  {
      Differ_PWM=1200;
  }
  if(Differ_PWM<=-1200)
  {
      Differ_PWM=-1200;
  }
}
#pragma section all restore

