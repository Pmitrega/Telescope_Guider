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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
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
int suspendVoltageInfo = 0;
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
/* Definitions for voltageInfo */
osThreadId_t voltageInfoHandle;
uint32_t voltageInfoBuffer[ 128 ];
osStaticThreadDef_t voltageInfoControlBlock;
const osThreadAttr_t voltageInfo_attributes = {
  .name = "voltageInfo",
  .cb_mem = &voltageInfoControlBlock,
  .cb_size = sizeof(voltageInfoControlBlock),
  .stack_mem = &voltageInfoBuffer[0],
  .stack_size = sizeof(voltageInfoBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for usbTransmitMutex */
osMutexId_t usbTransmitMutexHandle;
osStaticMutexDef_t usbTransmitMutexControlBlock;
const osMutexAttr_t usbTransmitMutex_attributes = {
  .name = "usbTransmitMutex",
  .cb_mem = &usbTransmitMutexControlBlock,
  .cb_size = sizeof(usbTransmitMutexControlBlock),
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
void voltageInfoTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of usbTransmitMutex */
  usbTransmitMutexHandle = osMutexNew(&usbTransmitMutex_attributes);

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

  /* creation of voltageInfo */
  voltageInfoHandle = osThreadNew(voltageInfoTask, NULL, &voltageInfo_attributes);

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
  startAdc();
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
  SetBuck1(ENABLE);
  SetBuck2(ENABLE);
  osDelay(10);
  initializeMotors();
  setDecMotorSpeed(-147);
  setRaMotorSpeed(-1470);
  const uint16_t LED_BRIGHTNESS = 30;
  osMutexAcquire(usbTransmitMutexHandle, 0U);
  CDC_Transmit_FS("STAR GUIDER CONTROLLER!\r\n"
                  "  Send -h for more info!\r\n"
                                      , 52);
  osMutexRelease(usbTransmitMutexHandle);
  /* Infinite loop */
  for(;;)
  {
    if(!suspendVoltageInfo){
      osThreadResume(voltageInfoHandle);
    }
    for(uint16_t i = 0; i < LED_BRIGHTNESS; i++){
      htim8.Instance->CCR1 = LED_BRIGHTNESS * 10U - i*10U - 8U;
      osDelay(10);
    }
    for(uint16_t i = 0; i < LED_BRIGHTNESS; i++){
      htim8.Instance->CCR2 = LED_BRIGHTNESS * 10U - i * 10U - 8U;
      osDelay(10);
    }

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

  osDelay(250);
  initPowerManager();
  /* Infinite loop */
  for(;;)
  {
    updatePowerManager();
    osDelay(250);
  }
  /* USER CODE END startPowerManager */
}

/* USER CODE BEGIN Header_voltageInfoTask */
/**
* @brief Function implementing the voltageInfo thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_voltageInfoTask */
void voltageInfoTask(void *argument)
{
  /* USER CODE BEGIN voltageInfoTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
    osMutexAcquire(usbTransmitMutexHandle, 0U);
    int length = sprintf(uart_transmit_buffer,"%d, %d, %d\r\n", getBatteryVoltagemV(), getBuck1VoltagemV(), getBuck2VoltagemV());
    CDC_Transmit_FS(uart_transmit_buffer, length);
    osMutexRelease(usbTransmitMutexHandle);
    if(suspendVoltageInfo == 1){
      osThreadSuspend(voltageInfoHandle);
    }
  }
  /* USER CODE END voltageInfoTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int USB_CDC_RxHandler(uint8_t* Buf, uint32_t *Len){
  if(*Len >= 2){
    if(Buf[0]== '-' && Buf[1] == 'h'){
      osMutexAcquire(usbTransmitMutexHandle, 0U);
      CDC_Transmit_FS("STAR GUIDER HELP!\r\n"
                      "  --buck[x] - voltage of buck x\r\n"
                      "  --batt    - voltage of battery\r\n"
                      "  --RS[x]   - x is speed [fullsteps/minute] of Ra\r\n"
                      "  --DS[x]   - x is speed [fullsteps/minute] of Dec\r\n"
                      "  --VI[x]   - x is 0 or 1 if 0:Transmit voltage info of batt and buck"
                      , 259);
      osMutexRelease(usbTransmitMutexHandle);
    }
    else if(Buf[0] =='-'&& Buf[1] == '-'){
      uint32_t st_index = 2;
      char cmd_buff[MAX_CMD_LENGTH + 1];
      while(st_index < *Len){
        if(IsLetter(Buf[st_index])){
          cmd_buff[st_index - 2] = Buf[st_index];
          st_index = st_index + 1;
        }
        else{
          /*put zero at the end and leave loop*/
          cmd_buff[st_index -2] = '\0';
          break;
        }
      }
      if(strcmp(cmd_buff, "RS") == 0){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 4;
        while(Buf[indx] && indx < (MAX_NUM_ARG_LENGTH + 4)){
          num_buff[indx - 4] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 4] = '\0';
        int numb = atoi(num_buff);
        setRaMotorSpeed(numb);
      }
      else if(strcmp(cmd_buff, "DS") == 0){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 4;
        while(Buf[indx] && indx < (MAX_NUM_ARG_LENGTH + 4)){
          num_buff[indx - 4] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 4] = '\0';
        int numb = atoi(num_buff);
        setDecMotorSpeed(numb);
      }
      else if(strcmp(cmd_buff, "batt") == 0){
        osMutexAcquire(usbTransmitMutexHandle, 0U);
        int length = sprintf(uart_transmit_buffer,"Battery voltage is %d mV!\r\n", getBatteryVoltagemV());
        CDC_Transmit_FS(uart_transmit_buffer, length);
        osMutexRelease(usbTransmitMutexHandle);
      }
      else if(strcmp(cmd_buff, "buck") == 0){
        osMutexAcquire(usbTransmitMutexHandle, 0U);
        int length = sprintf(uart_transmit_buffer,"Buck1_V is %d mV, Buck2_V is %d mV!\r\n",getBuck1VoltagemV(), getBuck2VoltagemV());
        CDC_Transmit_FS(uart_transmit_buffer, length);
        osMutexRelease(usbTransmitMutexHandle);
      }
      else if(strcmp(cmd_buff, "VI") == 0){
        if(Buf[4] == '1'){
          suspendVoltageInfo = 1;
        }
        else if(Buf[4] == '0'){
          suspendVoltageInfo = 0;
        }
      }
      }
      else{
        osMutexAcquire(usbTransmitMutexHandle, 0U);
        CDC_Transmit_FS("STAR GUIDER CONTROLLER!\r\n"
                        "  Send -h for more info!\r\n"
                                                , 52);
        osMutexRelease(usbTransmitMutexHandle);
      }
  }
}
/* USER CODE END Application */

