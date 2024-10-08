/*
 * pcf2129.h
 *
 *  Created on: Oct 16, 2020
 *      Author: apas
 */

#ifndef INC_PCF2129_H_
#define INC_PCF2129_H_

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
* Module Name      :  PCF2129.h
* Copyright        :
* Description      :  PCF2129  I2C接口驅動 (I2C interface driver)
* Revision History :
* Date          Author        Version        Notes
                apas          1.0            創建
------------------------------------------------------------------------------*/

#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stm32g0xx_hal.h"
#include "stdbool.h"

#define PCF2129_SLAVE_ADDRESS 			0x51

#define UNIX_TIME_YEAR       			1970
#define UNIX_TIME_MONTH   				1
#define UNIX_TIME_DATE  				1
#define UNIX_TIME_HOUR  				0
#define UNIX_TIME_MINUTE  				0
#define UNIX_TIME_SECOND				0

#define LEAP_YEAR						1
#define FLAT_YEAR						0

#define PCF2129_CONTROL_12_24 			0x04
#define RTC_WRITE_ADDR  				0xA2
#define RTC_READ_ADDR   				0xA3

#define PCF2129_REG_CONTROL1      		0x00
#define PCF2129_REG_CONTROL2      		0x01
#define PCF2129_REG_CONTROL3        	0x02
#define PCF2129_REG_SECONDS         	0x03
#define PCF2129_REG_MINUTES         	0x04
#define PCF2129_REG_HOURS           	0x05
#define PCF2129_REG_DAYS            	0x06
#define PCF2129_REG_WEEKDAYS        	0x07
#define PCF2129_REG_MONTHS          	0x08
#define PCF2129_REG_YEARS           	0x09

#define PCF2129_REG_ALARM_SECOND        0x0A
#define PCF2129_REG_ALARM_MINUTE        0x0B
#define PCF2129_REG_ALARM_HOUR          0x0C
#define PCF2129_REG_ALARM_DAY           0x0D
#define PCF2129_REG_ALARM_WEEKDAY       0x0E
#define PCF2129_REG_CLOCKOUT_CTRL       0x0F

/// @}

/*
=============================================================================
 *  @brief :            時間日期結構體 (Time and date structure)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
typedef struct{

    uint8_t y,m,d,w,hh,mm,ss;
}DateTime_t;

typedef struct
{
    uint8_t data;
}CtrlData;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}utc_t;

typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}time_tt;

utc_t unixToUtc(uint32_t u32UnixTimeStamp);

//typedef uint8_t UnixTimeStamp_t;
uint32_t utcToUnix(time_tt *time);

/*
=============================================================================
 *  @brief :            PCF2129 結構體 (Structure)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
typedef struct{
  I2C_HandleTypeDef i2c_h;
  uint8_t i2caddr;
  DateTime_t date;
  CtrlData cdata;
}PCF2129_t;

void PCF2129_Init(I2C_HandleTypeDef i2c_h,uint8_t addr);
///bool PCF2129_searchDevice(void);
void PCF2129_configure(void);
PCF2129_t PCF2129_getDate(void);
void PCF2129_setDate(uint16_t years, uint8_t months, uint8_t days,
                              uint8_t hours, uint8_t minutes, uint8_t seconds);
void PCF2129_set12mode(void);
void PCF2129_set24mode(void);
uint8_t PCF2129_read(uint8_t address);
void PCF2129_write(uint8_t address, uint8_t data);
void PCF2129_writeCtrl(uint8_t address, uint8_t data);
uint8_t PCF2129_readCtrl(uint8_t address);

uint8_t bcdToDec(uint8_t value);
uint8_t decToBcd(uint8_t value);

#endif /* INC_PCF2129_H_ */
