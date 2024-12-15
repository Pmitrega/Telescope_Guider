/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "power_manager.h"
#include "motor_controller.h"
#include "buzzer.h"s
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IsLetter(letter) ((letter >='a' && letter <= 'z') || (letter >='A' && letter <= 'Z'))
#define IsNumber(number) ((number >='0' && number <= '9') || (number == '-'))
#define MAX_CMD_LENGTH     (5U)
#define MAX_NUM_ARG_LENGTH (10U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim4;
extern uint8_t uart_recieve_buff[1];
extern UART_HandleTypeDef huart3;
int suspendVoltageInfo = 0;
// int runBuzzerSignal = 0;
// int runBuzzerSignalQueue[BUZZER_QUEUE_SIZE] = {0};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for pwrManagerTask */
osThreadId_t pwrManagerTaskHandle;
uint32_t powerManagerTasBuffer[ 128 ];
osStaticThreadDef_t powerManagerTasControlBlock;
const osThreadAttr_t pwrManagerTask_attributes = {
  .name = "pwrManagerTask",
  .cb_mem = &powerManagerTasControlBlock,
  .cb_size = sizeof(powerManagerTasControlBlock),
  .stack_mem = &powerManagerTasBuffer[0],
  .stack_size = sizeof(powerManagerTasBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BuzzerTask */
osThreadId_t BuzzerTaskHandle;
uint32_t BuzzerBuffer[ 128 ];
osStaticThreadDef_t BuzzerControlBlock;
const osThreadAttr_t BuzzerTask_attributes = {
  .name = "BuzzerTask",
  .cb_mem = &BuzzerControlBlock,
  .cb_size = sizeof(BuzzerControlBlock),
  .stack_mem = &BuzzerBuffer[0],
  .stack_size = sizeof(BuzzerBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int USB_CDC_RxHandler(uint8_t* Buf, uint32_t *Len);
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern uint16_t adc_readings[];
uint8_t uart_transmit_buffer[100];

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void startPowerManager(void *argument);
void StartBuzzer(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  static int signalType = 0;
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of pwrManagerTask */
  pwrManagerTaskHandle = osThreadNew(startPowerManager, NULL, &pwrManagerTask_attributes);

  /* creation of BuzzerTask */
  BuzzerTaskHandle = osThreadNew(StartBuzzer, (void*) signalType, &BuzzerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  static int signalType = 0;
  startAdc();
  queueBuzzerSignal(BUZZER_SIGNAL_STARTUP);
  osDelay(200);
  callibrateCurrent();
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
  HAL_UART_Receive_IT(&huart3, uart_recieve_buff, 1);
  SetBuck1(ENABLE);
  SetBuck2(ENABLE);
  initializeMotors();
  estimateResistance();
  startMotorAutoMode();
  // startStepsAutoMode();
  // moveRaSteps(54000);
  setDecMotorSpeed(0);
  setRaMotorSpeed(0);
  const uint16_t LED_BRIGHTNESS = 30;

  /* Infinite loop */
  for(;;)
  {
    for(uint16_t i = 0; i < LED_BRIGHTNESS; i++){
      htim8.Instance->CCR3 = LED_BRIGHTNESS * 10U - i * 10U - 8U;
      osDelay(10);
    }

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_startPowerManager */
/**
* @brief Function implementing the pwrManagerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startPowerManager */
void startPowerManager(void *argument)
{
  /* USER CODE BEGIN startPowerManager */

  osDelay(1250);
  initPowerManager();
  /* Infinite loop */
  for(;;)
  {
    updatePowerManager();
    osDelay(250);
  }
  /* USER CODE END startPowerManager */
}

/* USER CODE BEGIN Header_StartBuzzer */
/*ARR 1000 - 500Hz
  frq = 1/ARR * 500 000
  ARR = 500 000 / frq
*/



/**
* @brief Function implementing the BuzzerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBuzzer */
void StartBuzzer(void *argument)
{
  /* USER CODE BEGIN StartBuzzer */
  /* Infinite loop */
  for(;;){
    osDelay(250);
    handleBuzzer();
  }
  /* USER CODE END StartBuzzer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

