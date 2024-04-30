#pragma section all "cpu1_dsram"
#ifndef _CHARGE_H
#define _CHARGE_H

#include "includes.h"
#include "math.h"

void Charge_init(void);
void Charge_Start(void);
void Charge_Check(void);
void Charge_Ctrl(float Goal,float Actual);
void Charge_out(float pwm);
void Charge_Regular(void);


#endif
#pragma section all restore
