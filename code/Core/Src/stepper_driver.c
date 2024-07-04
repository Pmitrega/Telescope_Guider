#include "stepper_driver.h"

#define TOTAL_STEPS 32

struct StepperCoilState{
  int coil1;
  int coil2;
}typedef StepperCoilState;

//const StepperCoilState step_array[4] = {{1000, 0},{0, 1000},{-1000, 0},{0, -1000}};
const StepperCoilState step_array[32] = {{1000,0},{980,195},{923,382},{831,555},{707,707},{555,831},{382,923},{195,980},{0,1000},{-195,980},{-382,923},{-555,831},{-707,707},{-831,555},{-923,382},{-980,195},{-1000,0},{-980,-195},{-923,-382},{-831,-555},{-707,-707},{-555,-831},{-382,-923},{-195,-980},{0,-1000},{195,-980},{382,-923},{555,-831},{707,-707},{831,-555},{923,-382},{980,-195}};

void MotorRaStep(DIRECTION dir){
    static int step_iter = 0;
  if(dir == BACKWARD){
    step_iter -=1;
  }
  else{
    step_iter+=1;
  }
  if(step_iter < 0){
    step_iter = TOTAL_STEPS - 1;
  }
  else if(step_iter == TOTAL_STEPS){
    step_iter = 0;
  }

  StepperCoilState next_step = step_array[step_iter];

  /*Set coil 1*/
  if(next_step.coil1 < 0){
    next_step.coil1 = -next_step.coil1;
    RA_MOTOR_TIMER->CCR1 = (uint32_t)next_step.coil1;
    RA_MOTOR_TIMER->CCR2 = 0U;
  }
  else{
    RA_MOTOR_TIMER->CCR1 = 0U;
    RA_MOTOR_TIMER->CCR2 = (uint32_t)next_step.coil1;
  }

  if(next_step.coil2 < 0){
    next_step.coil2 = -next_step.coil2;
    RA_MOTOR_TIMER->CCR3 = (uint32_t)next_step.coil2;
    RA_MOTOR_TIMER->CCR4 = 0U;
  }
  else{
    RA_MOTOR_TIMER->CCR3 = 0U;
    RA_MOTOR_TIMER->CCR4 = (uint32_t)next_step.coil2;
  }

}

void MotorDecStep(DIRECTION dir){
  static int step_iter = 0;
  if(dir == BACKWARD){
    step_iter -=1;
  }
  else{
    step_iter+=1;
  }
  if(step_iter < 0){
    step_iter = TOTAL_STEPS - 1;
  }
  else if(step_iter == TOTAL_STEPS){
    step_iter = 0;
  }

  StepperCoilState next_step = step_array[step_iter];

  /*Set coil 1*/
  if(next_step.coil1 < 0){
    next_step.coil1 = -next_step.coil1;
    DEC_MOTOR_TIMER->CCR1 = (uint32_t)next_step.coil1;
    DEC_MOTOR_TIMER->CCR2 = 0U;
  }
  else{
    DEC_MOTOR_TIMER->CCR1 = 0U;
    DEC_MOTOR_TIMER->CCR2 = (uint32_t)next_step.coil1;
  }

  if(next_step.coil2 < 0){
    next_step.coil2 = -next_step.coil2;
    DEC_MOTOR_TIMER->CCR3 = (uint32_t)next_step.coil2;
    DEC_MOTOR_TIMER->CCR4 = 0U;
  }
  else{
    DEC_MOTOR_TIMER->CCR3 = 0U;
    DEC_MOTOR_TIMER->CCR4 = (uint32_t)next_step.coil2;
  }
}