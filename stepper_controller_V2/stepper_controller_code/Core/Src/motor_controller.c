#include "motor_controller.h"
#include "tim.h"

#define SECONDS_IN_MINUTE (60U)

const uint16_t PWM1_microsteps_array[] = {1000 ,902  ,805  ,710  ,618  ,529  ,445  ,366  ,293  ,227  ,169  ,119  ,77   ,44   ,20   ,5    ,
                                          0    ,5    ,20   ,44   ,77   ,119  ,169  ,227  ,293  ,366  ,445  ,529  ,618  ,710  ,805  ,902  ,
                                          1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,
                                          1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 };

const uint16_t PWM2_microsteps_array[] = {1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,
                                          1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,
                                          1000 ,902  ,805  ,710  ,618  ,529  ,445  ,366  ,293  ,227  ,169  ,119  ,77   ,44   ,20   ,5    ,
                                          0    ,5    ,20   ,44   ,77   ,119  ,169  ,227  ,293  ,366  ,445  ,529  ,618  ,710  ,805  ,902  };

const uint16_t PWM3_microsteps_array[] = {0    ,5    ,20   ,44   ,77   ,119  ,169  ,227  ,293  ,366  ,445  ,529  ,618  ,710  ,805  ,902  ,
                                          1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,
                                          1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,
                                          1000 ,902  ,805  ,710  ,618  ,529  ,445  ,366  ,293  ,227  ,169  ,119  ,77   ,44   ,20   ,5    };

const uint16_t PWM4_microsteps_array[] = {1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,
                                          1000 ,902  ,805  ,710  ,618  ,529  ,445  ,366  ,293  ,227  ,169  ,119  ,77   ,44   ,20   ,5    ,
                                          0    ,5    ,20   ,44   ,77   ,119  ,169  ,227  ,293  ,366  ,445  ,529  ,618  ,710  ,805  ,902  ,
                                          1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 ,1000 };

typedef enum NUMBER_OF_MICROSTEPS{
    MICROSTEPS_16 = 1,
    MICROSTEPS_8  = 2,
    MICROSTEPS_4  = 4,
    HALF_STEP     = 8,
    FULL_STEP     = 16,
}NUMBER_OF_MICROSTEPS;

/*division of frequency of */
static uint32_t frequency_division;

static NUMBER_OF_MICROSTEPS microsteps = MICROSTEPS_16;

extern uint32_t SystemCoreClock;

void initializeMotors(){

    /*Start PWM for motor 1 (Ra)*/
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    /*Start PWM for motor 2 (Dec)*/
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    /*Start Ra motor timer*/
    HAL_TIM_Base_Start_IT(&htim6);
    /*Start Dec motor timer*/
    HAL_TIM_Base_Start_IT(&htim7);
}

/*Requested speed in Full_steps per minute for Ra motor*/
void setRaMotorSpeed(uint32_t requested_speed){
    uint32_t PSC_value = (72U*(uint32_t)FULL_STEP)/((uint32_t)microsteps) - 1;
    uint32_t CCR_value = (SystemCoreClock/(uint32_t)(requested_speed * SECONDS_IN_MINUTE))/(PSC_value * 4);
    __HAL_TIM_SET_COMPARE(&htim6, TIM_CHANNEL_1, CCR_value);
    __HAL_TIM_SET_PRESCALER(&htim6, PSC_value);
}


/*Requested speed in Full_steps per minute for Dec motor*/
void setDecMotorSpeed(uint32_t requested_speed){
    uint32_t PSC_value = (72U*(uint32_t)FULL_STEP)/((uint32_t)microsteps) - 1;
    uint32_t CCR_value = (SystemCoreClock/(uint32_t)(requested_speed * SECONDS_IN_MINUTE))/(PSC_value * 4);
    __HAL_TIM_SET_COMPARE(&htim7, TIM_CHANNEL_1, CCR_value);
    __HAL_TIM_SET_PRESCALER(&htim7, PSC_value);
}

