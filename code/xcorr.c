#pragma section all "cpu0_dsram"
#include "xcorr.h"
#include<stdio.h>
#include<math.h>
#include<complex.h>

//#define MIC_NUM 3072
//#define Sample_MAX 1024
//信号为250Hz~2000Hz

#define FFT_MAX     410//频率上限
#define FFT_MIN     51//频率下限
#define FFT_NUM     2048//互相关处理数据长度

float X1,X2;//对角线上的硅麦与声源距离之差
float angle_i=45/34.6;//45/34.6 //角度系数，//最小角度间隔为4.5°
float xcorr_angle=0.0;//互相关所得角度值
int angle_flag=0;
int num=0;//判断处理声音信号的哪两段数据
int MAXi_14,MAXi_23,flag=0;//mic14和mic23互相关数组最大下标值

extern float ANGLE[3];//cpu1-互相关角度值
extern int16 center_flag;

/*复数求模*/
float32 value(cfloat32 a)
{
    float32 c;
    c =sqrt( a.real * a.real+ a.imag * a.imag);
    return c;
}


/*互相关复数相乘*/
cfloat32 xcorr_mul(cfloat32 a, cfloat32 b)
{
    cfloat32 c;
    c.real = a.real * b.real + a.imag * b.imag;
    c.imag = a.imag * b.real-a.real * b.imag;
    return c;
}

/*复数相乘*/
cfloat32 mul(cfloat32 a, cfloat32 b)
{
    cfloat32 c;
    c.real = a.real * b.real - a.imag * b.imag;
    c.imag = a.real * b.imag + a.imag * b.real;
    return c;
}

/*复数相加*/
cfloat32 add(cfloat32 a, cfloat32 b)
{
    cfloat32 c;
    c.real = a.real + b.real;
    c.imag = a.imag + b.imag;
    return c;
}

/*复数相减*/
cfloat32 minus(cfloat32 a, cfloat32 b)
{
    cfloat32 c;
    c.real = a.real - b.real;
    c.imag = a.imag - b.imag;
    return c;
}

/*复数求出幅值*/
float Absolute_Value(cfloat32 a)
{
    float i,j;
    float value;
    i=a.real*a.real;
    j=a.imag*a.imag;
    value=i+j;
    return value;
}

/*快速傅里叶变换*/
void FFT(cfloat32* output, cfloat32* input, int n)
{
    int i, j, k=0,f=11, l = 0,m=n;
    cfloat32 wn, w, x1, x2;

    for (i = 0; i < n; i++)
    {
        k = f;
        for (m = i, j = 0; k > 0; m >>= 1)
        {
            j = (j << 1) | (m & 1);
            k--;
        }
        if(num==3)
        {
           if(j>=1024)
           {
               output[i].real = input[j-Sample_MAX].real;
               output[i].imag = 0.0;
           }
           else
           {
               output[i].real = input[j+2*Sample_MAX].real;
               output[i].imag = 0.0;
            }//if

         }
         else
         {
             output[i].real = input[j].real;
             output[i].imag = 0.0;
         }//if
    }//for
    for (l = 1; l < n; l <<= 1)
    {

        wn.real = (float)cos(PI / l);
        wn.imag = (float)-sin(PI / l);
        for (j = 0; j < n; j += l << 1)
        {
            w.real = 1.0;
            w.imag = 0.0;
            for (k = 0; k < l; k++, w = mul(w, wn))
            {
                 x1 = output[j + k];
                 x2 = mul(w, output[j + k + l]);
                 output[j + k] = add(x1, x2);
                 output[j + k + l] = minus(x1, x2);
             }
         }
     }

}

/*快速傅里叶逆变换*/
void IFFT(cfloat32* output, cfloat32* input, int n)
{
    int i, j, k = 0, f =11, l = 0,m=n;
    cfloat32 wn, w, x1, x2;

    for (i = 0; i < n; i++)
    {
        k = f;
        for (m = i, j = 0; k > 0; m >>= 1)
        {
            j = (j << 1) | (m & 1);
            k--;
        }
        output[i] = input[j];//与FFT不同之处
    }
    for (l = 1; l < n; l <<= 1)
    {
        wn.real = (float)cos(PI / l);
        wn.imag = (float)sin(PI / l);
        for (j = 0; j < n; j += l << 1)
        {
            w.real = 1.0;
            w.imag = 0.0;
            for (k = 0; k < l; k++, w = mul(w, wn))
            {
                x1 = output[j + k];
                x2 = mul(w, output[j + k + l]);
                output[j + k] = add(x1, x2);
                output[j + k + l] = minus(x1, x2);
            }
        }
    }

}


/*两个数组互相关*/
float xcorr(cfloat32 *mic_1,cfloat32 *mic_2)
{
    cfloat32 mic1_out[FFT_NUM];
    cfloat32 mic2_out[FFT_NUM];
    cfloat32 xcorr_out[FFT_NUM];

    int32 MAX_i=0;//互相关最大值的坐标
    float32 Delay_12=0.0;//距离差
    float32 MIC_MAX=0.0;//最大幅值
    float32 MAX_value=0.0;


    //fft

     FFT(mic1_out,mic_1,FFT_NUM);
     FFT(mic2_out,mic_2,FFT_NUM);


    //频域互相关
    for(int i=0; i<FFT_NUM; i++)
    {
        if(i>=FFT_MIN && i<=FFT_MAX)
        {
            xcorr_out[i]=xcorr_mul(mic1_out[i],mic2_out[i]);//两个信号频域的乘积
            MAX_value = value(xcorr_out[i]);
            xcorr_out[i].real = xcorr_out[i].real /MAX_value;
            xcorr_out[i].imag = xcorr_out[i].imag /MAX_value;
        }
        else
        {
            xcorr_out[i].real=xcorr_out[i].imag=0.0;
        }
    }

    IFFT(mic2_out,xcorr_out,FFT_NUM);//ifft

    //取最大值（只看实部）
    MIC_MAX=mic2_out[0].real;
    MAX_i=0;
    for(int i=1; i<FFT_NUM; i++)
    {
        if(mic2_out[i].real>MIC_MAX)
         {
           MIC_MAX=mic2_out[i].real;
           MAX_i=i;
         }
    }

//查看14、23硅麦互相关最大值坐标
    if(flag==0)
    {
        MAXi_14=MAX_i;
        flag=1;
    }
    else
    {
        MAXi_23=MAX_i;
        flag=0;
    }

    //找到最大值对应下标MAX_i，Delay_12单位为cm
    if(MAX_i>1024)
    {
        Delay_12=(float32)(FFT_NUM-MAX_i)*(-3.46);
    }
    else
    {
        Delay_12=MAX_i*(3.46);
    }
    return Delay_12;
}

/*角度为-180~180，角度与err近似为线性关系*/
float Get_Angle(float X14,float X23,float err)
{
    float angle;
    if(X14<0&&X23<=0)//角度范围(45,135]
    {
        angle=90.0+angle_i*err;
    }
    else if(X14<=0&&X23>0)//角度范围(135,180]U[-180,-135]
    {
        if(err>=0)
        {
            angle=180.0-angle_i*err;
        }
        else
        {
            angle=-180.0-angle_i*err;
        }
    }
    else if(X14>0&&X23>=0)//角度范围(-135,-45]
    {
        angle=-90.0+angle_i*err;
    }
    else//角度范围(-45,45]
    {
        angle=-angle_i*err;
    }
    return angle;
}

/*互相关处理*/
void XCORR(void)
{
    float err=0.0;
    //处理需要50ms左右，而采集1024个数据需要100ms左右

    if(mic_flag == 1&&num!=0)
    {
       if(num==2)
       {
           X1=xcorr(mic_1+Sample_MAX,mic_4+Sample_MAX);//x1比x4离声源距离远多少
           X2=xcorr(mic_2+Sample_MAX,mic_3+Sample_MAX);
       }
       else
       {
           X1=xcorr(mic_1,mic_4);//x1比x4离声源距离远多少
           X2=xcorr(mic_2,mic_3);
       }

       err=fabs(X1)-fabs(X2);

       if(fabs(err)<50&&fabs(X1)<50&&fabs(X2)<50)//暴力防噪
       {
           angle_flag=1;
           xcorr_angle=Get_Angle(X1,X2,err);
           angle_flag=0;
       }
//       gpio_set_level(P21_5, 0);
//       system_delay_ms(500);
//       gpio_set_level(P21_5, 1);

        //center_flag=1;//电机使能

        num=0;


    }
}

#pragma section all restore
