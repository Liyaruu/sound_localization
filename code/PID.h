#pragma section all "cpu1_dsram"

#ifndef CODE_PID_H_
#define CODE_PID_H_


void PID_Init(void);
void PID_Refresh(PID_t p);
//float PID_Realize(PID_t  p, float NowData, float SetData);
int32 PID_Increase(PID_t  p, int32 NowData, int32 SetData);
void DifferControl(void);
void DirectError_Get(void);
void Direction_out(void);

#endif /* CODE_PID_H_ */

#pragma section all restore
