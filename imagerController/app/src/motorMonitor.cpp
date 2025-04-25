#include "motorMonitor.hpp"
#include "logger.hpp"
void motorMonitor::resetMotorsPosition(){
    dec_motor_steps = 0;
    ra_motor_speed = 0;
}

double motorMonitor::getPositionRa(){
    return ra_motor_steps * steps_to_deg;
}

double motorMonitor::getPositionDec(){
    //LOG_INFO("stepstodeg: %f\r\n", steps_to_deg);
    return dec_motor_steps * steps_to_deg;
}

void motorMonitor::updateRaSpeed(double speed){
    ra_motor_speed = speed;
}

void motorMonitor::updateDecSpeed(double speed){
    dec_motor_speed = speed;
}
void motorMonitor::updateRaPosition(double t_diff){
    
    ra_motor_steps += t_diff/60 * ra_motor_speed;
}

void motorMonitor::updateDecPosition(double t_diff){
    dec_motor_steps += t_diff/60 * dec_motor_speed;
}
