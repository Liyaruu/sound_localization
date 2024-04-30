#pragma section all "cpu1_dsram"

#include "includes.h"

float Value_RV;//VCC电压值——充电电压
float Value_VB;//超级电容电压值
float Value_RI;//充电电流

PID_t Charge_PID;//充电PID
float Charge_P=34.0;//目标恒功率值
float Now_P=0.0;//当前功率值
int32 Charge_Time=0;//充电持续时间——单位 ms
int32 second=0;//充电持续时间——单位 s
int32 time=0;//充电持续时间——单位 s
int32 time_flag=0;
int32 ci=0;

int32 Charge_PWM=5000;//buck电路初始PWM值

int Charge_flag=1;//充电标志位
int Charge_Flag_start=0;//真正开始充电标志位
int Charge_Protect=0;//充电保护

extern int16 center_flag;

void Charge_init(void)
{
    adc_init(RV, ADC_12BIT); //RV
    adc_init(VB, ADC_12BIT); //VB
    adc_init(RI, ADC_12BIT); //RI

    pwm_init(Buck_PWM, 100000, 0);//Buck电路开关频率250KHz
    gpio_init(SD, GPO, 0, GPO_PUSH_PULL);


    //初始化PID
    Charge_PID.LastData = 0.0;
    Charge_PID.LastError = 0.0;
    Charge_PID.PrevError = 0.0;
    Charge_PID.SumError = 0.0;
    Charge_PID.Kp = 2.0;
    Charge_PID.Ki = 0.2;
    Charge_PID.Kd = 0;

}

//位置式PID
float PID_Realize(PID_t  p, float NowData, float SetData)
{
    float iError,   // 当前误差
          Realize;  // 最后得出的实际增量

    iError = SetData - NowData; // 计算当前误差
    p.SumError += p.Ki * iError;

    if(p.SumError >= 20 || p.SumError <= -20)
    {
        if(p.SumError >= 20)
        {
             p.SumError = 20;
        }
        if(p.SumError <= -20)
        {
             p.SumError = -20;
        }
    }

    // 消除在启动时，积分过大导致超调，误差大于 CHARGE_P * 0.8时I为0。例如50w时，50 * 0.8=40，超过40以上，I不起作用
    if(fabs(iError)>SetData*0.7)
    {
        Realize =  (p.Kp) * iError + p.Kd * (iError - p.LastError) ;
        p.SumError=0;
    }
    else
    {
        Realize =  (p.Kp) * iError + p.SumError + p.Kd * (iError - p.LastError) ;
    }

    p.PrevError = p.LastError;    // 更新前次误差
    p.LastError = iError;          // 更新上次误差
    p.LastData  = NowData;         // 更新上次数据


    return Realize; // 返回实际值
}

//Buck电路输入PWM
void Charge_out(float pwm)
{
    pwm=184*pwm+80;//电压和占空比呈线性关系

    //充电保护
    if(pwm>9000)
    {
        pwm=9000;
        //达不到功率时，功率降低10W以保护充电电路
        if(Charge_Protect>50)
        {
            Charge_Protect=0;
            Charge_P-=2;
        }
        else
        {
            Charge_Protect++;
        }
    }
    if(pwm<4000)
    {
        pwm=4000;
    }
    pwm_set_duty(Buck_PWM, (int)pwm);
}

//恒功率充电控制
void Charge_Ctrl(float Goal,float Actual)
{
    float Charge_Err=0.0;
    float Charge_Value=0.0;

    Charge_Err=PID_Realize(Charge_PID, Now_P, Charge_P);//PID计算误差
    Charge_Value=Charge_Err+Actual;//功率

    Charge_Value=sqrt(Charge_Value*(float)time);//电压

    Charge_out(Charge_Value);//输出PWM

}

//功率检测电路
void Charge_Check(void)
{

    if(Value_RV<2.0)//具体VCC值需要测量
    {
        Charge_Flag_start=0;
    }
    else
    {
        Charge_Flag_start=1;
    }

    //当电压充到11.5V时，充电结束。
    if(Value_VB > 12.0)
    {
        ci++;
        if(ci==3)
        {

//            flash_buffer_clear();                                                       // 清空缓冲区
//            flash_union_buffer[0].int8_type  = 1;
//            flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);        // 向指定 Flash 扇区的页码写入缓冲区数据

            gpio_set_level(SD, 0);
            pwm_set_duty(Buck_PWM,0);

            //标志位改变状态
            Charge_flag=0;
            Charge_Flag_start=0;
            center_flag=1;

            //充电时间复位
            Charge_Time=0;
            second=0;
            time=0;
        }

    }


}

void Charge_Regular(void)
{
    if(Charge_Time==200)
    {
        Charge_Time=0;
        time++;
        second++;
    }

    if(second==1)
    {
        second=0;
        if(time_flag==1)
        {
            Charge_PWM+=100;
            if(Charge_PWM>=8000)
            {
                Charge_PWM=8000;
            }
        }
    }
    if(time==20)
    {
        time_flag=1;
    }

    pwm_set_duty(Buck_PWM,Charge_PWM);
}

//充电函数
void Charge_Start(void)
{
    //获取VCC（充电电压）、充电电流、超级电容电压
    Value_RV = adc_mean_filter_convert(RV,5)*0.0195;//*3.3*24.45/4096
    //Value_RI = adc_mean_filter_convert(RI,5)*0.002;//*3.3*2.5/4096
    Value_VB = adc_mean_filter_convert(VB,5)*0.0195;//*3.3*24.45/4096
    //Now_P=Value_RI*Value_RV;

    Charge_Check();

    //检查RV、RI、VB的ADC测量
//    ips114_show_float(0, 0, Value_RV, 4, 3);
//    ips114_show_float(0, 20,Value_RI, 4, 3);
//    ips114_show_float(0, 40,Value_VB, 4, 3);


    //充电核心代码
    if(Charge_Flag_start==1)
    {
        gpio_set_level(SD, 1);

        //人为控制充电
        Charge_Regular();

        //PID+前馈控制充电
        //Charge_Ctrl(Charge_P,Now_P);

        Charge_Time+=1;
    }
}




#pragma section all restore
