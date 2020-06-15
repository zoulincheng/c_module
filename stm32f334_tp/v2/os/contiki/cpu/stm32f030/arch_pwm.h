#ifndef _ARCH_PWM_H
#define _ARCH_PWM_H


#define PWM_DEFAULT_FEQ				70	//hz
#define PWM_SMALL_FEQ				10	//hz
#define PWM_MID_FEQ					20	//hz
#define PWM_HIGH_FEQ				30	//hz
#define PWM_MIN_FREQHZ				50
#define PWM_MAX_FREQHZ				20000

void timPwmOut(void);
void timPwmUpdateParam(u_short feqhz, u_short dutyCycle);
void timPwmClose(void);
void timPwmInit(void);
void timPWMOn(void);
void timPWMOff(void);
u_short timPwmGetDuty(void);
void timPwmUpdateDutyCycle(u_short dutyCycle);
void timPwmSetFreq(u_short freq);

#endif
