#include "buzzer.h"
extern TIM_HandleTypeDef htim4;
int BuzzerQueue[BUZZER_QUEUE_SIZE] = {0};
int buzzerQueuePointer = 0;
int buzzerDeQueuePointer = 0;
int buzzerQueueElements = 0;


void queueBuzzerSignal(int signal_type){
  BuzzerQueue[buzzerQueuePointer] = signal_type;
  buzzerQueuePointer +=1;
  if(buzzerQueuePointer >= BUZZER_QUEUE_SIZE){
    buzzerQueuePointer %= BUZZER_QUEUE_SIZE;
  }
}
static int dequeueBuzzerSignal(){
  int ret_val = NO_BUZZER_SIGNAL;

  if(buzzerDeQueuePointer !=  buzzerQueuePointer){
    ret_val = BuzzerQueue[buzzerDeQueuePointer];
    buzzerDeQueuePointer +=1;
  }
  if(buzzerDeQueuePointer >= BUZZER_QUEUE_SIZE){
    buzzerDeQueuePointer %= BUZZER_QUEUE_SIZE;
  }
  return ret_val;
}

static void BuzzerBeep(int frq, int on_time, int off_time){
    int req_ARR = 500000/frq;
    if(req_ARR > 100 && req_ARR < 65000 ){
      htim4.Instance->ARR = req_ARR;
      HAL_TIM_Base_Start_IT(&htim4);
      osDelay(on_time);
      HAL_TIM_Base_Stop_IT(&htim4);
      if(off_time > 0){
        osDelay(off_time);
      }
    }
}

void handleBuzzer(){
    int req_signal = dequeueBuzzerSignal();
    if( req_signal == BUZZER_SIGNAL_STARTUP){
      BuzzerBeep(300, 100, 100);
      BuzzerBeep(500, 100, 100);
      BuzzerBeep(700, 100, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(700, 100, 100);
    }
    else if(req_signal == BUZZER_SIGNAL_LOW_BATTERY){
      BuzzerBeep(1000, 100, 100);
      BuzzerBeep(1000, 100, 100);
      BuzzerBeep(1000, 100, 100);
      BuzzerBeep(1000, 100, 100);
      BuzzerBeep(1000, 100, 100);
    }
    else if(req_signal == BUZZER_SIGNAL_CRITICAL_BATTERY){
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
      BuzzerBeep(1000, 300, 100);
    }
    else if(req_signal == BUZZER_SIGNAL_BUCK1_ERROR){
      BuzzerBeep(1000, 50, 100);
      BuzzerBeep(1000, 50, 100);
      BuzzerBeep(500, 50, 100);
      BuzzerBeep(1000, 50, 100);
      BuzzerBeep(1000, 50, 100);
    }
    else if(req_signal == BUZZER_SIGNAL_BUCK2_ERROR){
      BuzzerBeep(1000, 50, 100);
      BuzzerBeep(1000, 50, 100);
      BuzzerBeep(1500, 50, 100);
      BuzzerBeep(1000, 50, 100);
      BuzzerBeep(1000, 50, 100);
    }
    else if(req_signal == BUZZER_SIGNAL_NO_RA_MOTOR){
      BuzzerBeep(1500, 100, 100);
      BuzzerBeep(1500, 100, 100);
      BuzzerBeep(1500, 100, 100);
    }
    else if(req_signal == BUZZER_SIGNAL_NO_DEC_MOTOR){
      BuzzerBeep(2000, 100, 100);
      BuzzerBeep(2000, 100, 100);
      BuzzerBeep(2000, 100, 100);
    }
    if(req_signal != NO_BUZZER_SIGNAL){
        osDelay(500);
    }
}

void testBuzzer(){
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_STARTUP);
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_LOW_BATTERY);
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_CRITICAL_BATTERY);
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_BUCK1_ERROR);
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_BUCK2_ERROR);
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_NO_RA_MOTOR);
    osDelay(200);
    queueBuzzerSignal(BUZZER_SIGNAL_NO_DEC_MOTOR);
}