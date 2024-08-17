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
#include "adc.h"
#include "power_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IsLetter(letter) ((letter >='a' && letter <= 'z') || (letter >='A' && letter <= 'Z'))
#define MAX_CMD_LENGTH     5U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int USB_CDC_RxHandler(uint8_t* Buf, uint32_t *Len);
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern uint16_t adc_readings[];
uint8_t uart_transmit_buffer[100];
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void startPowerManager(void *argument);

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
  const uint16_t LED_BRIGHTNESS = 30;
  CDC_Transmit_FS("STAR GUIDER CONTROLLER!\r\n"
                  "  Send -h for more info!\r\n"
                                      , 52);
  /* Infinite loop */
  for(;;)
  {
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

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int USB_CDC_RxHandler(uint8_t* Buf, uint32_t *Len){
  if(*Len >= 2){
    if(Buf[0]== '-' && Buf[1] == 'h'){
      CDC_Transmit_FS("STAR GUIDER HELP!\r\n"
                      "  --buck[x] - voltage of buck x\r\n"
                      "  --batt    - voltage of battery\r\n"
                      "  --RS[x]   - x is 1 or -1 - step for Ra\r\n"
                      "  --DS[x]   - x is 1 or -1 - step for Dec\r\n"
                      , 172);
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
      if(strcmp(cmd_buff, "batt") == 0){
        int length = sprintf(uart_transmit_buffer,"Battery voltage is %d mV!\r\n", BATT_RAW_TO_mV(adc_readings[BATT_V_CH]));
        CDC_Transmit_FS(uart_transmit_buffer, length);
      }
      else if(cmd_buff, "buck"){
        int length = sprintf(uart_transmit_buffer,"Buck1_V is %d mV, Buck2_V is %d mV!\r\n",BATT_RAW_TO_mV(adc_readings[BUCK1_V_CH]),BATT_RAW_TO_mV(adc_readings[BUCK2_V_CH]));
        CDC_Transmit_FS(uart_transmit_buffer, length);
      }
      }
      else{
              CDC_Transmit_FS("STAR GUIDER HELP!\r\n"
                        "  --buck[x] - voltage of buck x\r\n"
                        "  --batt    - voltage of battery\r\n"
                        "  --RS[x]   - x is 1 or -1 - step for Ra\r\n"
                        "  --DS[x]   - x is 1 or -1 - step for Dec\r\n"
                        , 172);
      }
  }
}
/* USER CODE END Application */

