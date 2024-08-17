#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

typedef enum BATTERY_STATUS{
    BATTERY_HIGH = 0,
    BATTERY_NORMAL,
    BATTERY_LOW,
    BATTERY_CRITICAL,
    BATTERY_UNKNOWN
}BATTERY_STATUS;


typedef enum BUCK_POWER{
    BUCK_DISABLE = 0,
    BUCK_ENABLE
}BUCK_POWER;


typedef enum BUCK_FAULT_STATUS{
    BUCK_OK = 0,
    BUCK_FAULT
}BUCK_FAULT_STATUS;


void initPowerManager();
void updatePowerManager();

void SetBuck1(BUCK_POWER pwr_status);
void SetBuck2(BUCK_POWER pwr_status);

BATTERY_STATUS getBatteryStatus();
BUCK_FAULT_STATUS getBuck1Status();
BUCK_FAULT_STATUS getBuck2Status();




#endif /*POWER_MANAGER_H*/
