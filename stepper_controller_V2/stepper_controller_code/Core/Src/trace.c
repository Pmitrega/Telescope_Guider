#include "trace.h"
#include "motor_controller.h"
#include "power_manager.h"
#include "adc.h"

uint8_t log_buffer[LOG_BUFFER_SIZE];
uint8_t uart_recieve_buff[1];

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;


#define IsLetter(letter) ((letter >='a' && letter <= 'z') || (letter >='A' && letter <= 'Z'))
#define IsNumber(number) ((number >='0' && number <= '9') || (number == '-'))
#define MAX_CMD_LENGTH     (5U)
#define MAX_NUM_ARG_LENGTH (10U)

int CmdProcessor(uint8_t* Buf, uint32_t *Len){
  if(*Len >= 2){
    if(Buf[0]== '-' && Buf[1] == 'h'){
        LOG_INFO("STAR GUIDER HELP!\r\n"
                      "  -V[x]   - voltage  0 - battery | 1 - buck1 | 2 - buck2\r\n"
                      "  -C[x]   - current  0 - M1C1 | 1 - M1C2 | 2 - M2C1 | 3 - M2C2 | 4 - BC\r\n"
                      "  -P[x]   - batt percentage\r\n"
                      "  -R[x]   - x is speed [fullsteps/minute] of Ra\r\n"
                      "  -D[x]   - x is speed [fullsteps/minute] of Dec\r\n"
                      "  -A[x]   - go speed mode\r\n"
                      "  -M[x]   - go manual control of coils\r\n"
                      "  -u,i,o,p[x]   - raw ctrl coil1/2/3/4\r\n");
    }
    else if(Buf[0] =='-'&& Buf[1] == 'R'){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 2;
        while(Buf[indx] &&  indx < *Len){
          num_buff[indx - 2] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 2] = '\0';
        int numb = atoi(num_buff);
        setRaMotorSpeed(numb);
      }
    else if(Buf[0] =='-'&& Buf[1] == 'D'){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 2;
        while(Buf[indx] && indx < *Len){
          num_buff[indx - 2] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 2] = '\0';
        int numb = atoi(num_buff);
        setDecMotorSpeed(numb);
      }
    else if(Buf[0] =='-'&& Buf[1] == 'R'){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 2;
        while(Buf[indx] && indx < *Len){
          num_buff[indx - 2] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 2] = '\0';
        int numb = atoi(num_buff);
        setRaMotorSpeed(numb);
      }
    else if(Buf[0] =='-'&& Buf[1] == 'V'){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 2;
        while(Buf[indx] &&  indx < *Len){
          num_buff[indx - 2] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 2] = '\0';
        int numb = atoi(num_buff);
        if(numb == 0){
            LOG_INFO("B0 %d\r\n", getBatteryVoltagemV());
        }
        else if(numb == 1){
            LOG_INFO("B1 %d\r\n", getBuck1VoltagemV());
        }
        else if(numb == 2){
            LOG_INFO("B2 %d\r\n", getBuck2VoltagemV());
        }
      }
    else if(Buf[0] =='-'&& Buf[1] == 'P'){
        LOG_INFO("PP %d\r\n", getBatteryPercentage());
      }
    else if(Buf[0] =='-'&& Buf[1] == 'C'){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH];
        uint8_t indx = 2;
        while(Buf[indx] && indx < *Len){
          num_buff[indx - 2] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 2] = '\0';
        int numb = atoi(num_buff);
        if(numb == 0){
            LOG_INFO("C0 %d\r\n", getCurrentM1C1mA());
        }
        else if(numb == 1){
            LOG_INFO("C1 %d\r\n", getCurrentM1C2mA());
        }
        else if(numb == 2){
            LOG_INFO("C2 %d\r\n", getCurrentM2C1mA());
        }
        else if(numb == 3){
            LOG_INFO("C3 %d\r\n", getCurrentM2C2mA());
        }
        else if(numb == 4){
            LOG_INFO("BC %d\r\n",getEstimatedBattCurrmA());
        }
      }
    else if(Buf[0] =='-'&& Buf[1] == 'A'){
      uint8_t mode = getMotorMode();
      if(mode == MOTOR_MANUAL_MODE){
        startMotorAutoMode();
        LOG_INFO("ENTERING AUTO MODE\r\n");
      }
      }
    else if(Buf[0] =='-'&& Buf[1] == 'M'){
      uint8_t mode = getMotorMode();
      if(mode == MOTOR_AUTO_MODE){
        startMotorManualMode();
        LOG_INFO("ENTERING MANUAL MODE\r\n");
      }
      }
    else if(Buf[0] =='-'&& (Buf[1] == 'u' || Buf[1] == 'i' || Buf[1] == 'o' || Buf[1] == 'p')){
        uint8_t num_buff[MAX_NUM_ARG_LENGTH] = {0};
        uint8_t indx = 2;
        while(Buf[indx] && indx < *Len){
          num_buff[indx - 2] = Buf[indx];
          indx +=1;
        }
        num_buff[indx - 2] = '\0';
        int numb = atoi(num_buff);
        if(numb > 1000 || numb < -1000){
          LOG_INFO("Control must be from -1000 to 1000 \r\n");
          return;
        }
        switch (Buf[1])
        {
        case 'u':
          setCoilM1C1(numb);
          break;
        case 'i':
          setCoilM1C2(numb);
          break;
        case 'o':
          setCoilM2C1(numb);
          break;
        case 'p':
          setCoilM2C2(numb);
          break;
        default:
          break;
        }
    }
  }
}


int USB_CDC_RxHandler(uint8_t* Buf, uint32_t *Len){
    CmdProcessor(Buf, Len);
}
uint32_t cmd_idx = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    static uint8_t uart_CMD[MAX_CMD_LENGTH + MAX_NUM_ARG_LENGTH];

    if(uart_recieve_buff[0] == '-' && cmd_idx == 0){
        uart_CMD[0] = '-';
        cmd_idx = 1;
    }
    else if(IsLetter(uart_recieve_buff[0]) && cmd_idx == 1){
        uart_CMD[1] = uart_recieve_buff[0];
        cmd_idx = 2;
    }
    else if(cmd_idx >=2 && cmd_idx < MAX_NUM_ARG_LENGTH && IsNumber(uart_recieve_buff[0])){
        uart_CMD[cmd_idx] = uart_recieve_buff[0];
        cmd_idx +=1;
    }
    else if(cmd_idx >=2 && uart_recieve_buff[0] == '\n' || uart_recieve_buff[0] == '\r' || uart_recieve_buff[0] == '#'){
        cmd_idx = cmd_idx;
        CmdProcessor(uart_CMD, &cmd_idx);
        cmd_idx = 0;
    }
    else{
        cmd_idx = 0;
    }
	HAL_UART_Receive_IT(&UART_HANDLE, uart_recieve_buff, 1); // Ponowne włączenie nasłuchiwania
}