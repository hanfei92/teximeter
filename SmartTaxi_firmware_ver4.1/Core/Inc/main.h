/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
struct print1 {
	unsigned char sp[6];
	unsigned char ch[9];
	unsigned char en[9];
	unsigned char va[14];
	unsigned char cmd[2];
};

struct print2 {
	unsigned char sp[6];
	unsigned char ch[9];
	unsigned char en[10];
	unsigned char va[13];
	unsigned char cmd[2];
};

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void HMI_Hired_W(void);
void HMI_Stop_W(void);
void HMI_Vacant_W(void);
void Status_Check(void);
void Status_Change(void);
void Long_to_byte(uint32_t longInt, uint8_t* byteArray);
uint32_t byte_to_Long(uint8_t* byteArray);
void long_to_byte(uint16_t longInt, unsigned char* byteArray);
uint32_t byte_to_long(uint8_t* byteArray);
void long_to_byte_crc(uint16_t longInt, unsigned char* byteArray);
void HMI_Command(const uint8_t* command, uint8_t* data, uint8_t NOB);
void HMI_Transmit(char out[], uint8_t nob);
uint16_t CRC16(unsigned char *cal, uint16_t len);
unsigned char checksum_8bit(unsigned char *data, int len);
void extras_rd(void);
void tbox_start(void);
void tbox_finish(void);
void printer_rd(void);
void check_rd(void);
void clear_rd(void);
void printer_process(void);
void printer_e(void);
void printer_transmit(struct print1 *data, UART_HandleTypeDef *huart);
void printer_transmit2(struct print2 *data, UART_HandleTypeDef *huart);
void eeprom_w(void);
void eeprom_r(void);
void eeprom_check(void);
void rtc_set(void);
void time_update(void);
void BCD2ASC(uint8_t *asc, const uint8_t *bcd, uint32_t len);
unsigned char BCD2Dec(unsigned char data);
unsigned char Dec2BCD(unsigned char data);
void Dur_hms_cal();
void Read_Pulse(void);
void Read_Speed(void);
void Read_Distance(void);
void Fare_Calculation(void);
//void Fare_Cal_Stop(void);
void Flag_Control_Up();
void Flag_Control_Down();
void Anti_Tamper_Handle();
void Lock_Count_Down(void);
void Dec2ASC(uint32_t decimal, uint8_t* asc);
uint32_t ASC2Dec(uint8_t* asc);
void E_read(void);
void E_write(void);
void Voice_init(void);
void Voice_start(unsigned char data[]);
void Voice_end(unsigned char data[]);
void cal_vcan(unsigned char data[], unsigned char result[]);
void cal_vcn(unsigned char data[], unsigned char result[]);
void cal_ven(unsigned char data[], unsigned char result[]);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
