#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

#include "stm32f401xc.h"

#define RA_MOTOR_TIMER TIM4
#define DEC_MOTOR_TIMER TIM2

enum DIRECTION{
  FORWARD,
  BACKWARD
}typedef DIRECTION;

void MotorRaStep(DIRECTION dir);
void MotorDecStep(DIRECTION dir);

#endif

