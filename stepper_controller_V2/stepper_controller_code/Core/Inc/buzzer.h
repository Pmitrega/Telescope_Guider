#ifndef BUZZER_H
#define BUZZER_H

#include "main.h"

#define NO_BUZZER_SIGNAL 0
#define BUZZER_SIGNAL_STARTUP 1
#define BUZZER_SIGNAL_LOW_BATTERY 2
#define BUZZER_SIGNAL_CRITICAL_BATTERY 3
#define BUZZER_SIGNAL_BUCK1_ERROR 4
#define BUZZER_SIGNAL_BUCK2_ERROR 5
#define BUZZER_SIGNAL_NO_RA_MOTOR 6
#define BUZZER_SIGNAL_NO_DEC_MOTOR 7
#define BUZZER_QUEUE_SIZE 10

void handleBuzzer();
void queueBuzzerSignal(int signal_type);
void testBuzzer();
#endif