#include "motor_controller.h"
#include "tim.h"


#define SECONDS_IN_MINUTE   (60U)

#define MOTOR_AUTO_MODE     (0U)
#define MOTOR_MANUAL_MODE   (1U)


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
static uint8_t  motor_mode = MOTOR_AUTO_MODE;

static int RaDir = 1;
static int DecDir = 1;

static int fullStepsRa = 0;
static int fullStepsDec = 0;

static uint8_t is_dec_running = 0;
static uint8_t is_ra_running = 0;

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
}

void startMotorAutoMode(){
    /*Start Ra motor timer*/
    HAL_TIM_Base_Start_IT(&htim6);
    /*Start Dec motor timer*/
    HAL_TIM_Base_Start_IT(&htim7);
    motor_mode = MOTOR_AUTO_MODE;
    is_dec_running = 1;
    is_ra_running = 1;
}
void startMotorManualMode(){
    HAL_TIM_Base_Stop_IT(&htim6);
    HAL_TIM_Base_Stop_IT(&htim7);
    is_dec_running = 0;
    is_ra_running = 0;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0U);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0U);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0U);
    motor_mode = MOTOR_MANUAL_MODE;
}

uint8_t getMotorMode(){return motor_mode;}

void startMotorRa(){
    HAL_TIM_Base_Start_IT(&htim6);
    is_ra_running = 1;
}

void startMotorDec(){
    HAL_TIM_Base_Start_IT(&htim7);
    is_dec_running = 1;
}

void stopMotorRa(){
    HAL_TIM_Base_Stop_IT(&htim6);
    is_ra_running = 0;
}

void stopMotorDec(){
    HAL_TIM_Base_Stop_IT(&htim7);
    is_dec_running = 0;
}

void shutdownMotors(){
}

int getStepsRa(){
    return fullStepsRa;
}
int getStepsDec(){
    return fullStepsDec;
}

/*Set PWM for coil 1 from -1000 to 1000*/
void setCoilM1C1(int val){
    if(val >= 0){
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000U);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1000 - (uint32_t)val);
    }
    else{
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000 - (uint32_t)(-val));
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1000U);
    }
}

/*Set PWM for coil 2 from -1000 to 1000*/
void setCoilM1C2(int val){
    if(val >= 0){
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000U);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1000 - (uint32_t)val);
    }
    else{
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000 - (uint32_t)(-val));
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1000U);
    }
}

void setCoilM2C1(int val){
    if(val >= 0){
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 1000U);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 1000 - (uint32_t)val);
    }
    else{
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 1000 - (uint32_t)(-val));
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 1000U);
    }
}

void setCoilM2C2(int val){
    if(val >= 0){
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 1000U);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 1000 - (uint32_t)val);
    }
    else{
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 1000  - (uint32_t)(-val));
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 1000U);
    }
}

/*Requested speed in Full_steps per minute for Ra motor*/
void setRaMotorSpeed(volatile int requested_speed){
    if(is_ra_running == 0 && requested_speed !=0){
        startMotorRa();
    }
    if(requested_speed < 0){
        requested_speed = -requested_speed;
        RaDir = -1;
    }
    else if(requested_speed > 0){
        RaDir = 1;
    }
    else{
        stopMotorRa();
    }
    uint32_t PSC_value = ((uint32_t)FULL_STEP)/((uint32_t)microsteps) - 1;
    uint32_t CCR_value = (uint32_t)((float)SystemCoreClock/(float)(requested_speed))/(((float)PSC_value+1.f) * 64/(float)microsteps) * SECONDS_IN_MINUTE - 1;
    while(CCR_value >= UINT16_MAX){
        PSC_value = (PSC_value+1)*2-1;
        CCR_value = (uint32_t)((float)SystemCoreClock/(float)(requested_speed))/(((float)PSC_value+1.f) * 64/(float)microsteps) * SECONDS_IN_MINUTE - 1;
    }
    __HAL_TIM_SET_AUTORELOAD(&htim6,  CCR_value);
    __HAL_TIM_SET_PRESCALER(&htim6, PSC_value);
}


/*Requested speed in Full_steps per minute for Dec motor*/
void setDecMotorSpeed(int requested_speed){
    if(is_dec_running == 0 && requested_speed !=0){
        startMotorRa();
    }
    if(requested_speed < 0){
        requested_speed = -requested_speed;
        DecDir = -1;
    }
    else if(requested_speed > 0){
        DecDir = 1;
    }
    else{
        stopMotorDec();
    }
    uint32_t PSC_value = ((uint32_t)FULL_STEP)/((uint32_t)microsteps) - 1;
    uint32_t CCR_value = (uint32_t)((float)SystemCoreClock/(float)(requested_speed))/(((float)PSC_value+1.f) * 64/(float)microsteps) * SECONDS_IN_MINUTE - 1;
    while(CCR_value >= UINT16_MAX){
        PSC_value = (PSC_value+1)*2-1;
        CCR_value = (uint32_t)((float)SystemCoreClock/(float)(requested_speed))/(((float)PSC_value+1.f) * 64/(float)microsteps) * SECONDS_IN_MINUTE - 1;
    }
    __HAL_TIM_SET_AUTORELOAD(&htim7,  CCR_value);
    __HAL_TIM_SET_PRESCALER(&htim7, PSC_value);
}

void updateRaPWM(){
    static int counter;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PWM1_microsteps_array[counter]);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, PWM2_microsteps_array[counter]);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, PWM3_microsteps_array[counter]);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, PWM4_microsteps_array[counter]);
    counter += (int)microsteps*RaDir;

    htim8.Instance->CCR2 = PWM1_microsteps_array[counter];

    if(counter >= 64){
        counter = 0;
        fullStepsRa +=1;
    }
    else if(counter < 0){
        counter = 63;
        fullStepsRa -=1;
    }
}

void updateDecPWM(){
    static int counter;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM1_microsteps_array[counter]);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, PWM2_microsteps_array[counter]);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, PWM3_microsteps_array[counter]);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, PWM4_microsteps_array[counter]);
    counter += (int)microsteps*DecDir;

    htim8.Instance->CCR1 = PWM1_microsteps_array[counter];
    //   osDelay(10);
    // }
    // for(uint16_t i = 0; i < LED_BRIGHTNESS; i++){
    //   htim8.Instance->CCR2 = LED_BRIGHTNESS * 10U - i * 10U - 8U;
    if(counter >= 64){
        counter = 0;
        fullStepsDec +=1;
    }
    else if(counter < 0){
        counter = 63;
        fullStepsDec -=1;
    }
}

