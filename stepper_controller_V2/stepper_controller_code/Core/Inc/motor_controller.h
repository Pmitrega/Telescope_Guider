#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H
#include "main.h"
#define MOTOR_AUTO_MODE     (0U)
#define MOTOR_MANUAL_MODE   (1U)

int getStepsRa();
int getStepsDec();
void setRaMotorSpeed(int requested_speed);
void setDecMotorSpeed(int requested_speed);

void startMotorRa();
void startMotorDec();
void stopMotorRa();
void stopMotorDec();


void setCoilM1C1(int val);
void setCoilM1C2(int val);
void setCoilM2C1(int val);
void setCoilM2C2(int val);

void estimateResistance();
void startMotorAutoMode();
void startMotorManualMode();
uint8_t getMotorMode();


void startStepsAutoMode();
void moveDecSteps(int steps);
void moveRaSteps(int steps);

void shutdownMotors();
void startMotors();
#endif