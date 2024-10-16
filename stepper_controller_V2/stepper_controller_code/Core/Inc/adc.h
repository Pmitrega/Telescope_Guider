/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN Private defines */
#define ADC_TOTAL_CHANNELS 8U
#define BATT_RAW_TO_mV(RAW) (int)((RAW * 10000) / 1562)
#define BUCK1_RAW_TO_mV(RAW) (int)((RAW * 10000) / 1562)
#define BUCK2_RAW_TO_mV(RAW) (int)((RAW * 10000) / 1562)
/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */

int getBatteryVoltagemV();
int getBuck1VoltagemV();
int getBuck2VoltagemV();
void startAdc();

typedef enum ADC_CHANNELS{
  BATT_V_CH = 0,
  BUCK1_V_CH,
  MOT1_C1_SENS_CH,
  MOT1_C2_SENS_CH,
  MOT2_C1_SENS_CH,
  MOT2_C2_SENS_CH,
  BUCK2_V_CH,
  TEMP_CH,
  ADC_ALL_CHANNELS
}ADC_CHANNELS;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

