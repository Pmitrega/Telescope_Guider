#include "power_manager.h"
#include "main.h"
#include "adc.h"
#include "buzzer.h"
#include "trace.h"

/* ------------ LOCAL DEFINES ------------ */
#define BATTERY_STATUS_DEBOUNCE_CNT 12
#define BUCK_ERROR_DEBOUNCE_CNT 5
#define BATT_STATE_HIST 50
#define BATTERY_STATE_LEVELS 4

#define BUCK_ERROR_RETRIGGER_CNT ((uint16_t)(25 / 0.25)) /*time between buzzer error can be retriggered*/

#define CRITICAL_BATTERY_RETRIGGER_CNT ((uint16_t)(30 / 0.25)) /*time between buzzer error can be retriggered*/
#define LOW_BATTERY_RETRIGGER_CNT ((uint16_t)(120 / 0.25)) /*time between buzzer error can be retriggered*/

#define CHECK_BUCK1_PIN_STATUS() HAL_GPIO_ReadPin(BUCK1_EN_GPIO_Port, BUCK1_EN_Pin)
#define CHECK_BUCK2_PIN_STATUS() HAL_GPIO_ReadPin(BUCK2_EN_GPIO_Port, BUCK2_EN_Pin)

/* ------------ DECLARATION OF GLOBAL VARIABLES ------------ */

/* ------------ DECLARATION OF STATIC VARIABLES ------------ */
static BATTERY_STATUS battery_level;

/*
------- Following voltage levels are connected to BATTERY_STATUS states: ------
12.50V - 15.00V -- HIGH
11.81V - 12.50V -- NORMAL
11.51V - 11.81V -- LOW
10.00V - 11.51V -- CRITICAL
     ELSE       -- UNKNOWN
*/

typedef struct BATTERY_STATE_BOUNDARIES{
    uint16_t lower_boundary;
    uint16_t higher_boundary;
}BATTERY_STATE_BOUNDARIES;

static const BATTERY_STATE_BOUNDARIES battery_level_boundaries[4]  = {
                                                                    {12500U, 15000U},
                                                                    {11810U, 12500U + BATT_STATE_HIST},
                                                                    {11510U, 11810U + BATT_STATE_HIST},
                                                                    {5000U    , 11510U + BATT_STATE_HIST}
                                                                    };

static const uint16_t buck_proper_voltage[2]  = {7000, 10500};

/*Power manager task will be run every 250ms*/

/*STATIC FUNCTION DECLARATION*/
static void handleStateChange(BATTERY_STATUS current_state, BATTERY_STATUS previous_state);
static void getInitialBatteryState(uint16_t battery_voltage);
static void handleState(BATTERY_STATUS current_state);

static void getInitialBatteryState(uint16_t battery_voltage){
    /*
     * iterate from highest to lowest battery level - so
     * in case of finding lower boundary we stop and set
     * global battery level
    */
    for(int i = 0; i < BATTERY_STATE_LEVELS; i++){
        if(battery_voltage > battery_level_boundaries[i].lower_boundary){
            battery_level = (BATTERY_STATUS)i;
            break;
        }
    }
    if(battery_level == BATTERY_LOW){
        handleStateChange(BATTERY_LOW, BATTERY_NORMAL);
    }
    else if(battery_level == BATTERY_CRITICAL){
        handleStateChange(BATTERY_LOW, BATTERY_NORMAL);
        handleStateChange(BATTERY_CRITICAL, BATTERY_LOW);
    }
}

static void handleStateChange(BATTERY_STATUS current_state, BATTERY_STATUS previous_state){
    if(current_state == BATTERY_LOW && previous_state == BATTERY_NORMAL){
        LOG_INFO("WARNING: LOW BATTERY LEVEL, %d mV!\r\n", getBatteryVoltagemV());
        queueBuzzerSignal(BUZZER_SIGNAL_LOW_BATTERY);
    }
    else if(current_state == BATTERY_CRITICAL && previous_state == BATTERY_LOW){
        LOG_INFO("WARNING: CRITICAL BATTERY LEVEL %d mV, disabling buck converters!\r\n", getBatteryVoltagemV());
        queueBuzzerSignal(BUZZER_SIGNAL_CRITICAL_BATTERY);
        SetBuck1(BUCK_DISABLE);
        SetBuck2(BUCK_DISABLE);
    }
    else if(current_state == BATTERY_LOW && previous_state == BATTERY_CRITICAL){
        LOG_INFO("INFO: RECOVERED FROM  CRITICAL BATTERY LEVEL, enabling bucks!\r\n", getBatteryVoltagemV());
        SetBuck1(BUCK_ENABLE);
        SetBuck2(BUCK_ENABLE);
    }
}

static void handleState(BATTERY_STATUS current_state){
    static uint16_t low_battery_retrigger;
    static uint16_t critical_battery_retrigger;
    if(current_state == BATTERY_LOW){
        low_battery_retrigger +=1;
        critical_battery_retrigger = 0;
        if(low_battery_retrigger == LOW_BATTERY_RETRIGGER_CNT){
            low_battery_retrigger = 0;
            LOG_INFO("WARNING: LOW BATTERY LEVEL, %d mV!\r\n", getBatteryVoltagemV());
            queueBuzzerSignal(BUZZER_SIGNAL_LOW_BATTERY);
        }
    }
    if(current_state == BATTERY_CRITICAL){
        critical_battery_retrigger +=1;
        low_battery_retrigger = 0;
        if(critical_battery_retrigger == CRITICAL_BATTERY_RETRIGGER_CNT){
            critical_battery_retrigger = 0;
            LOG_INFO("WARNING: CRITICAL BATTERY LEVEL %d mV, charge battery imidietly!\r\n", getBatteryVoltagemV());
            queueBuzzerSignal(BUZZER_SIGNAL_CRITICAL_BATTERY);
        }
    }
}

int checkBatteryLevel(uint16_t battery_voltage){
    static uint16_t battery_debounce_cnt = 0;
    BATTERY_STATUS requested_battery_level = battery_level;
    if((int)battery_level < (int)BATTERY_UNKNOWN){
        if(battery_voltage < battery_level_boundaries[(int)battery_level].lower_boundary){
            /*
             * Go to lower battery voltage state, if battery voltage is
             * lower than lower critical - battery goes to UNKNOWN STATE
            */
            requested_battery_level = (BATTERY_STATUS)((int)battery_level + 1);
        }
    }


    if(battery_voltage > battery_level_boundaries[(int)battery_level].higher_boundary){
            /*
             * Go to higher battery voltage state, if
             * voltage is higher than HIGH higher boundary
             * battery goes to UNKNOWN
            */
        if(battery_level == BATTERY_HIGH){
            requested_battery_level = BATTERY_UNKNOWN;
        }
        else{
            requested_battery_level = (BATTERY_STATUS)((int)battery_level - 1);
        }
    }

    if(battery_level != requested_battery_level){
        battery_debounce_cnt +=1;
        if(battery_debounce_cnt >= BATTERY_STATUS_DEBOUNCE_CNT){
            handleStateChange(requested_battery_level, battery_level);
            battery_level = requested_battery_level;
            battery_debounce_cnt = 0;
        }
    }
    else{
        battery_debounce_cnt = 0;
    }

    return battery_level;
}

void checkBuckIntegrity(uint16_t buck1_voltage_mV, uint16_t buck2_voltage_mV){
    static uint16_t buck1_error_debounce = 0;
    static uint16_t buck2_error_debounce = 0;
    if(CHECK_BUCK1_PIN_STATUS() == BUCK_ENABLE){
        if(buck1_voltage_mV > buck_proper_voltage[1] || buck1_voltage_mV < buck_proper_voltage[0]){
            buck1_error_debounce +=1;
        }
        else{
            buck1_error_debounce = 0;
        }
        if(buck1_error_debounce == BUCK_ERROR_DEBOUNCE_CNT){
            queueBuzzerSignal(BUZZER_SIGNAL_BUCK1_ERROR);
        }
    }

    if(CHECK_BUCK1_PIN_STATUS() == BUCK_ENABLE){
        if(buck2_voltage_mV > buck_proper_voltage[1] || buck2_voltage_mV < buck_proper_voltage[0]){
            buck2_error_debounce +=1;
        }
        else{
            buck2_error_debounce = 0;
        }
        if(buck2_error_debounce == BUCK_ERROR_DEBOUNCE_CNT){
            queueBuzzerSignal(BUZZER_SIGNAL_BUCK2_ERROR);
        }
    }
    buck1_error_debounce = buck1_error_debounce % BUCK_ERROR_RETRIGGER_CNT;
    buck2_error_debounce = buck2_error_debounce % BUCK_ERROR_RETRIGGER_CNT;
}

void initPowerManager(){
    uint16_t battery_voltage_mV = (uint16_t)getBatteryVoltagemV();
    getInitialBatteryState(battery_voltage_mV);
}

void updatePowerManager(){
    volatile uint16_t battery_voltage_mV = (uint16_t)getBatteryVoltagemV();
    volatile uint16_t buck1_voltage_mV = (uint16_t)getBuck1VoltagemV();
    volatile uint16_t buck2_voltage_mV = (uint16_t)getBuck2VoltagemV();
    checkBatteryLevel(battery_voltage_mV);
    checkBuckIntegrity(buck1_voltage_mV, buck2_voltage_mV);
    handleState(battery_level);
}


void SetBuck1(BUCK_POWER pwr_status){
    HAL_GPIO_WritePin(BUCK1_EN_GPIO_Port, BUCK1_EN_Pin, (int)pwr_status);
}

void SetBuck2(BUCK_POWER pwr_status){
    HAL_GPIO_WritePin(BUCK2_EN_GPIO_Port, BUCK2_EN_Pin, (int)pwr_status);
}


