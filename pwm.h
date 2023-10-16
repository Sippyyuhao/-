#ifndef _PWM_H_
#define _PWM_H_
void ServoSetPluseAndTime(uint8 id,uint16 pwm,uint16 time);
void ServoPwmDutyCompare(void);
void ServoSetPluseAndTime(uint8 id,uint16 p,uint16 time);
void InitPwm(void);
#endif


