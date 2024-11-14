#include "power_manager.h"
#include "main.h"
#include "adc.h"

/* ------------ LOCAL DEFINES ------------ */
#define BATTERY_STATUS_DEBOUNCE_CNT 12
#define BATT_STATE_HIST 50
#define BATTERY_STATE_LEVELS 4

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
                                                                    {10000U    , 11510U + BATT_STATE_HIST}
                                                                    };

/*Power manager task will be run every 250ms*/

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
            battery_level = requested_battery_level;
            battery_debounce_cnt = 0;
        }
    }
    else{
        battery_debounce_cnt = 0;
    }

    return battery_level;
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
}

void SetBuck1(BUCK_POWER pwr_status){
    HAL_GPIO_WritePin(BUCK1_EN_GPIO_Port, BUCK1_EN_Pin, (int)pwr_status);
}

void SetBuck2(BUCK_POWER pwr_status){
    HAL_GPIO_WritePin(BUCK2_EN_GPIO_Port, BUCK2_EN_Pin, (int)pwr_status);
}

void signalLowBattery(){

    
}