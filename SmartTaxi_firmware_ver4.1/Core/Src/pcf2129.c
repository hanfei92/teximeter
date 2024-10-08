/*
 * pcf2129.c
 *
 *  Created on: Oct 16, 2020
 *      Author: apas
 */

#include "pcf2129.h"
///#include "i2c.h"
#include "stm32g0xx_hal_i2c.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define I2C_TIMEOUT   30             //Timeout

PCF2129_t pcf2129;
extern utc_t UtcTime;
extern time_tt stCurrentTime;

uint16_t LeapYearNumber;
uint16_t FlatYearNumber;
uint16_t LastMonDays;

/*
=============================================================================
 *  @brief :         初始化函数 (Initialization function)
 *  @note :
 *  @param :         使用HAL庫 I2C接口 (Use HAL library I2C interface)
 *  @retva l: None
=============================================================================
*/
void PCF2129_Init(I2C_HandleTypeDef i2c_h,uint8_t addr) {
    pcf2129.i2c_h = i2c_h;
    pcf2129.i2caddr = addr;
}

/*
=============================================================================
 *  @brief :        查找設備 (Find device)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
bool PCF2129_searchDevice(void) {
	return !(PCF2129_read(pcf2129.i2caddr) >>7);
}

/*
=============================================================================
 *  @brief :             設置24小時計時格式 (Set 24-hour timing format)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
void PCF2129_configure(void) {
    PCF2129_set24mode();
}


/*
=============================================================================
 *  @brief :           獲取PCF2129時間 日期 (Get PCF2129 time and date)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
PCF2129_t PCF2129_getDate(void)
{
	uint8_t datecontent;

	datecontent = PCF2129_REG_SECONDS;
	HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
    	pcf2129.date.ss = datecontent;

    datecontent = PCF2129_REG_MINUTES;
    HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
    	pcf2129.date.mm= datecontent;

    datecontent = PCF2129_REG_HOURS;
    HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
    	pcf2129.date.hh = datecontent;

//    datecontent = PCF2129_WEEKDAYS;
//    HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
//    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
//        pcf2129.date.w= datecontent;

    datecontent = PCF2129_REG_DAYS;
    HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
    	pcf2129.date.d= datecontent;

    datecontent = PCF2129_REG_MONTHS;
    HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
        pcf2129.date.m= datecontent;

    datecontent = PCF2129_REG_YEARS;
    HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &datecontent, 1, I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &datecontent, 1, I2C_TIMEOUT) == 0)
        pcf2129.date.y= datecontent;

    /*
	pcf2129.date.ss = PCF2129_read(PCF2129_REG_SECONDS);
	pcf2129.date.mm = PCF2129_read(PCF2129_REG_MINUTES);
	pcf2129.date.hh = PCF2129_read(PCF2129_REG_HOURS);
	pcf2129.date.d = PCF2129_read(PCF2129_REG_DAYS);
	pcf2129.date.m = PCF2129_read(PCF2129_REG_MONTHS);
	pcf2129.date.y = PCF2129_read(PCF2129_REG_YEARS);
	*/
    return pcf2129;
}

/*
=============================================================================
 *  @brief :          設置PCF2129時間 日期 (Set PCF2129 time and date)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
void PCF2129_setDate(uint16_t years, uint8_t months, uint8_t days,
                     uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    PCF2129_write(PCF2129_REG_SECONDS,seconds);
    PCF2129_write(PCF2129_REG_MINUTES,minutes);
    PCF2129_write(PCF2129_REG_HOURS,hours);
    PCF2129_write(PCF2129_REG_DAYS,days);
    PCF2129_write(PCF2129_REG_WEEKDAYS,0x00);
    PCF2129_write(PCF2129_REG_MONTHS,months);
    PCF2129_write(PCF2129_REG_YEARS,years);
}

/*
=============================================================================
 *  @brief :        設置12小時計時格式 (Set 12-hour timing format)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
void PCF2129_set12mode(void) {
	uint8_t ctrl;
	ctrl = PCF2129_readCtrl(PCF2129_REG_CONTROL1);
	ctrl |= PCF2129_CONTROL_12_24;
	PCF2129_writeCtrl(PCF2129_REG_CONTROL1, ctrl);
}

/*
=============================================================================
 *  @brief :     設置24小時計時格式 (Set 24-hour timing format)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
void PCF2129_set24mode(void) {
	uint8_t ctrl;
	ctrl = PCF2129_readCtrl(PCF2129_REG_CONTROL1);
	ctrl &= ~(PCF2129_CONTROL_12_24);
	PCF2129_writeCtrl(PCF2129_REG_CONTROL1, ctrl);
}

/*
=============================================================================
 *  @brief :       讀取一個寄存器內容 (Read the contents of a register)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
uint8_t PCF2129_read(uint8_t address)
{
	HAL_I2C_Master_Transmit(&(pcf2129.i2c_h), 0xA2,  &address, 1, I2C_TIMEOUT);
	if (HAL_I2C_Master_Receive(&(pcf2129.i2c_h), pcf2129.i2caddr, &address, 1, I2C_TIMEOUT) == 0)
	    	pcf2129.cdata.data = address;

	return address;
	/*
	uint8_t data;
	HAL_I2C_Mem_Read(&(pcf2129.i2c_h), pcf2129.i2caddr, address-1, 1, &data, 1, I2C_TIMEOUT);
	return data;
	*/
}

/*
=============================================================================
 *  @brief :       寫入一個結存器內容 (Write the contents of a register)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
void PCF2129_write(uint8_t address, uint8_t data)
{
    HAL_I2C_Mem_Write(&(pcf2129.i2c_h), pcf2129.i2caddr, address, 1, &data, 1, I2C_TIMEOUT);
}

/*
=============================================================================
 *  @brief :        讀取控制結存器內容 (Read control register content)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
uint8_t PCF2129_readCtrl(uint8_t address) {
    return PCF2129_read(PCF2129_REG_CONTROL1);
}
/*
=============================================================================
 *  @brief :       寫入編輯寄存器內容 (Write the contents of the edit register)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
void PCF2129_writeCtrl(uint8_t address, uint8_t data) {
    PCF2129_write(PCF2129_REG_CONTROL1, data);
}

/*
=============================================================================
 *  @brief :        BCD編碼 轉 十進制 (BCD code to decimal)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
uint8_t bcdToDec(uint8_t value) {
    return (uint8_t) ( (value/16*10) + (value%16) );
}

/*
=============================================================================
 *  @brief :       十進制 轉 BCD編碼 (Decimal to BCD encoding)
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
uint8_t decToBcd(uint8_t value) {
    return (uint8_t) ( (value/10*16) + (value%10) );
}

/*
=============================================================================
 *  @brief :       UNIX 转 UTC
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
utc_t unixToUtc(uint32_t u32UnixTimeStamp)
{
	uint8_t flat_year_month_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint8_t leap_year_month_day[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	int32_t year_temp = 0;
	int32_t day_temp = 0;
	int32_t cnt_temp = 0;
	uint8_t is_leap_or_flat_year;
	uint8_t i;

	cnt_temp = u32UnixTimeStamp;

	while(cnt_temp >= 126230400)
	{
		year_temp ++;
		cnt_temp = cnt_temp - 126230400;
	}

	UtcTime.year = UNIX_TIME_YEAR + (4 * year_temp);

	if(cnt_temp >= 31536000)
	{
		UtcTime.year ++;
		cnt_temp -= 31536000;

		if(cnt_temp >= 31536000)
		{
			UtcTime.year ++;
			cnt_temp -= 31536000;

			if(cnt_temp >= 31622400)
			{
				UtcTime.year ++;
				cnt_temp -= 31622400;

				if(cnt_temp >= 31536000)
				{
					UtcTime.year ++;
					cnt_temp -= 31536000;
				}
			}
		}
	}

	if((((UtcTime.year % 4) == 0) && ((UtcTime.year % 100) != 0)) || ((UtcTime.year % 400) == 0))
	{
		is_leap_or_flat_year = LEAP_YEAR;
	}
	else
	{
		is_leap_or_flat_year = FLAT_YEAR;
	}

	day_temp = cnt_temp / 86400;

	UtcTime.hour = (cnt_temp - day_temp * 86400) / 3600; //Calculate hours
	UtcTime.minute = (cnt_temp - day_temp * 86400 - UtcTime.hour * 3600) / 60; //Calculate minutes
	UtcTime.second = cnt_temp % 60;

	UtcTime.month = 1;
	for(i = 0; i < 12; i ++)
	{
		if(is_leap_or_flat_year == FLAT_YEAR)
		{
			if(day_temp >= flat_year_month_day[i + 1])
			{
				UtcTime.month ++; day_temp -= flat_year_month_day[i + 1];
			}
		}
		else if(is_leap_or_flat_year == LEAP_YEAR)
		{
			if(day_temp >= leap_year_month_day[i + 1])
			{
				UtcTime.month ++;
				day_temp -= leap_year_month_day[i + 1];
			}
		}
	}

	UtcTime.date = day_temp + 1;

	return UtcTime;
}

/*
=============================================================================
 *  @brief :       UTC 转 UNIX
 *  @note :
 *  @param :None
 *  @retva l: None
=============================================================================
*/
uint32_t utcToUnix(time_tt *time)
{
	uint8_t FlatYearMonthDay[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint8_t LeapYearMonthDay[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	uint16_t i;
	uint16_t ThisYear;

	uint16_t LeapYearNumber = 0;
	uint16_t FlatYearNumber = 0;
	uint16_t LastMonDays =  0;

	uint32_t TimeStamp = 0;

	for (i = 1970; i < stCurrentTime.year; i ++)
	{
		if((((i % 4) == 0) && ((i % 100) != 0)) || ((i % 400) == 0))
		{
			LeapYearNumber ++;
		}
		else
		{
			FlatYearNumber ++;
		}
	}
	TimeStamp = LeapYearNumber * 31622400 + FlatYearNumber * 31536000;

	if((((stCurrentTime.year % 4) == 0) && ((stCurrentTime.year % 100) != 0)) || ((stCurrentTime.year % 400) == 0))
	{
		ThisYear = LEAP_YEAR;
	}
	else
	{
		ThisYear = FLAT_YEAR;
	}

	for ( i = 1; i < stCurrentTime.month; i ++)
	{
		if(ThisYear == LEAP_YEAR)
		{
			LastMonDays += LeapYearMonthDay[i];
		}
		else if(ThisYear == FLAT_YEAR)
		{
			LastMonDays += FlatYearMonthDay[i];
		}
	}
	LastMonDays = LastMonDays + stCurrentTime.date - 1;
	TimeStamp += LastMonDays * 86400;

	TimeStamp += stCurrentTime.hour * 3600;
	TimeStamp += stCurrentTime.minute * 60;
	TimeStamp += stCurrentTime.second;

	return TimeStamp;
}
