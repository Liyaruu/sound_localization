#pragma section all "cpu0_dsram"

#ifndef XCORR_H
#define XCORR_H

#include "SysSe/Math/Ifx_FftF32.h"
#include "isr.h"

#define MIC_NUM 3072    // 定义采集麦克风点数
#define Sample_MAX 1024


extern float X1,X2;
extern int MAXi_14,MAXi_23;//互相关最大值位置
extern uint16  mic_flag;   //第一次麦克风采集完成标志位


extern cfloat32 mic_1[MIC_NUM]; // 保存麦克风数据的缓存
extern cfloat32 mic_2[MIC_NUM];
extern cfloat32 mic_3[MIC_NUM];
extern cfloat32 mic_4[MIC_NUM];

float xcorr(cfloat32 *demo_1,cfloat32 *demo_2);
float Get_Angle(float X14,float X23,float err);
void XCORR(void);

#endif

#pragma section all restore
