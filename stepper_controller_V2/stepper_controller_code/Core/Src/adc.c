/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "FreeRTOS.h"
#define RAW_TO_mA(raw)            ((int)((float)raw * 0.67f))
#define BUCK_TO_BATT_CURR(buck)   ((float)buck * 0.75f)
#define STANDBY_CURRENT           77 /*mA*/


#define FILTER_COEFF 0.05
#define BATT_CURR_FILT_COEFF 0.001
#define BATT_VOL_FILT_COEFF 0.001

uint16_t adc_readings[ADC_TOTAL_CHANNELS];
uint16_t adc_readings_filtered[ADC_TOTAL_CHANNELS];
uint16_t curr_callib_offset[4];
float battery_curr = 0;
float battery_volt = 0;
float batt_conn_resistance = 0;

float M1C1_res = 0.f;
float M1C2_res = 0.f;
float M2C1_res = 0.f;
float M2C2_res = 0.f;

static const float batt_volatage_table[] = {13500, 13000, 12850, 12800, 12750, 12500, 12300,
                                            12150, 12050, 11950, 11810, 11660, 11510, 10500};

static const int batt_perc_table []= {100, 97, 95, 93, 90, 80, 70,
                                        60 , 50, 40, 30, 20, 10, 0};

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 8;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    PC2     ------> ADC1_IN12
    PC3     ------> ADC1_IN13
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PC4     ------> ADC1_IN14
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    PC2     ------> ADC1_IN12
    PC3     ------> ADC1_IN13
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PC4     ------> ADC1_IN14
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int getBatteryVoltagemV(){
  return (int)(battery_volt);
}

int getBuck1VoltagemV(){
  return BUCK1_RAW_TO_mV(adc_readings_filtered[BUCK1_V_CH]);
}
int getBuck2VoltagemV(){
  return BUCK2_RAW_TO_mV(adc_readings_filtered[BUCK2_V_CH]);
};

int getCurrentM1C1mA(){
  return RAW_TO_mA(adc_readings[MOT1_C1_SENS_CH] - curr_callib_offset[0]);
};

int getCurrentM1C2mA(){
  return RAW_TO_mA(adc_readings[MOT1_C2_SENS_CH] - curr_callib_offset[1]);
};

int getCurrentM2C1mA(){
  return RAW_TO_mA(adc_readings[MOT2_C1_SENS_CH] - curr_callib_offset[2]);
};

int getEstimatedBattCurrmA(){
  return (int)battery_curr;
}

int getCurrentM2C2mA(){
  return RAW_TO_mA(adc_readings[MOT2_C2_SENS_CH] - curr_callib_offset[3]);
};

float getM1C1Res(){
  return M1C1_res;
}

float getM1C2Res(){
  return M1C2_res;
}
float getM2C1Res(){
  return M2C1_res;
}
float getM2C2Res(){
  return M2C2_res;
}

/*shall be called before running current through load*/
void callibrateCurrent(){
  curr_callib_offset[0] = 0U;
  curr_callib_offset[1] = 0U;
  curr_callib_offset[2] = 0U;
  curr_callib_offset[3] = 0U;
  for(int i = 0; i < 5; i++){
    osDelay(10);
    curr_callib_offset[0] += adc_readings[MOT1_C1_SENS_CH];
    curr_callib_offset[1] += adc_readings[MOT1_C2_SENS_CH];
    curr_callib_offset[2] += adc_readings[MOT2_C1_SENS_CH];
    curr_callib_offset[3] += adc_readings[MOT2_C2_SENS_CH];
  }
  curr_callib_offset[0] /= 5U;
  curr_callib_offset[1] /= 5U;
  curr_callib_offset[2] /= 5U;
  curr_callib_offset[3] /= 5U;
}


int getBatteryPercentage(){
  int i = sizeof(batt_volatage_table)/sizeof(int) - 1;
  while(i >= 0){
    if(battery_volt > batt_volatage_table[i]){
      i-=1;
    }
    else{
      break;
    }
  }
  return batt_perc_table[i];
}



void startAdc(){
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(adc_readings), ADC_TOTAL_CHANNELS);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  float calc_bat_v = BATT_RAW_TO_mV(adc_readings[BATT_V_CH]) + battery_curr * batt_conn_resistance;
  battery_volt = ((float)battery_volt * (1.f-BATT_CURR_FILT_COEFF) + (float)calc_bat_v * BATT_CURR_FILT_COEFF);
  adc_readings_filtered[BUCK1_V_CH] = (uint16_t)((float)adc_readings_filtered[BUCK1_V_CH] * (1.f-FILTER_COEFF) + (float)adc_readings[BUCK1_V_CH] * FILTER_COEFF);
  adc_readings_filtered[BUCK2_V_CH] = (uint16_t)((float)adc_readings_filtered[BUCK2_V_CH] * (1.f-FILTER_COEFF) + (float)adc_readings[BUCK2_V_CH] * FILTER_COEFF);
  float tot_buck_curr = (float)RAW_TO_mA(adc_readings[MOT1_C1_SENS_CH] - curr_callib_offset[0])+
                        (float)RAW_TO_mA(adc_readings[MOT1_C2_SENS_CH] - curr_callib_offset[1])+
                        (float)RAW_TO_mA(adc_readings[MOT2_C1_SENS_CH] - curr_callib_offset[2])+
                        (float)RAW_TO_mA(adc_readings[MOT2_C2_SENS_CH] - curr_callib_offset[3]);
  battery_curr = battery_curr*(1.f- BATT_CURR_FILT_COEFF) + BATT_CURR_FILT_COEFF *( BUCK_TO_BATT_CURR(tot_buck_curr) + STANDBY_CURRENT);
}

void estimateResistance(){
    /*Set PWM for coil 1 from -1000 to 1000*/
    setCoilM1C1(0);
    setCoilM1C2(0);
    setCoilM2C1(0);
    setCoilM2C2(0);
    osDelay(3000);
    volatile int start_voltage = getBatteryVoltagemV();
    int Buck1_volt = getBuck1VoltagemV();
    int Buck2_volt = getBuck1VoltagemV();

    volatile int start_current = getEstimatedBattCurrmA();
    osDelay(10);
    setCoilM1C1(1000);
    setCoilM1C2(1000);
    setCoilM2C1(1000);
    setCoilM2C2(1000);
    osDelay(3000);



    M1C1_res = (float)Buck1_volt/(float)(getCurrentM1C1mA());
    M1C2_res = (float)Buck1_volt/(float)(getCurrentM1C2mA());
    M2C1_res = (float)Buck2_volt/(float)(getCurrentM2C1mA());
    M2C2_res = (float)Buck2_volt/(float)(getCurrentM2C2mA());
    volatile int end_voltage = getBatteryVoltagemV();
    volatile int end_current = getEstimatedBattCurrmA();
    osDelay(200);
    setCoilM1C1(0);
    setCoilM1C2(0);
    setCoilM2C1(0);
    setCoilM2C2(0);
    if(end_current - start_current != 0){
      float res = (float)(start_voltage - end_voltage)/(float)(end_current - start_current);
      if(res > 10){
        res = 10;
      }
      batt_conn_resistance = res;
    }
}

/* USER CODE END 1 */
