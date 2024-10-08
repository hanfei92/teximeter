/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "pcf2129.h"
#include "EEPROM.h"
#include "math.h"
#include "stdbool.h"
#include "hmi.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FALSE 				0
#define TRUE 				1

#define S_VACANT			0
#define S_HIRED				1
#define S_STOP				2
#define S_PRINT				3

#define BUFFER_SIZE  		64

#define ROW_INIT 			0
#define ROW1				1
#define ROW2				2
#define ROW3				3
#define ROW4				4
#define ROW5				5
#define ROW6				6
#define ROW7				7
#define ROW8				8
#define ROW_FINISH			9

#define ADDR_EEPROM_Write 	0xA0
#define ADDR_EEPROM_Read 	0xA1
#define BufferSize 			0x30 //48 bytes

#define RTC_WRITE_ADDR  	0xA2
#define RTC_READ_ADDR   	0xA3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart1_rx;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
uint8_t RxFlag = FALSE;
uint8_t StateChg = FALSE;
uint8_t RxBuff[12] = {0};
uint8_t TestBuff[12] = {0};

uint32_t Final_price = 0;
uint32_t Last_Final_price = 0;
uint32_t Last_Final_price_x10 = 0;
uint32_t ex_Addition = 0;
uint8_t Flag_Display = FALSE;
uint8_t display_wait = 0;
//0712
uint8_t Flag_added = FALSE;
uint8_t added_wait = 0;

uint32_t Dis = 0;
uint32_t Last_Dis = 0;
uint32_t Last_Addition = 0;
uint32_t Addition = 0;
uint8_t Flag_Addition_Add = FALSE;
uint8_t Flag_Addition_F = FALSE;

uint8_t Drv_state = S_VACANT;
float Distance_km = 0;
uint16_t Wait_sec = 0;
uint16_t Plus_10 = 0;
uint16_t Plus_1 = 0;
uint8_t Dur_Hr = 0;
uint8_t Dur_Min = 0;
uint8_t Dur_Sec = 0;
uint8_t rec_ch = 0;

uint8_t value = 0;
uint8_t txflag = 1;

//status changed flag
uint8_t Drv_state_flag;

uint8_t Flag_V = FALSE;
uint8_t Flag_H = FALSE;
uint8_t Flag_S = FALSE;

uint8_t Flag_V2H = FALSE;
uint8_t Flag_H2S = FALSE;
uint8_t Flag_S2V = FALSE;
uint8_t Flag_S2H = FALSE;

uint8_t receive_buff[64];

uint8_t P_flag = TRUE;
uint8_t S_flag = FALSE;

uint8_t sec = 0;
uint8_t min = 0;
uint8_t hou = 0;

uint16_t sec_1;
uint16_t sec_10;
uint16_t min_1;
uint16_t min_10;
uint16_t hou_1;
uint16_t hou_10;




unsigned char msg01[] = {0xdc, 0x87, 0xcc, 0x96, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char msg03[] = {0xc9, 0xcf, 0xdc, 0x87, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char msg05[] = {0xcf, 0xc2, 0xdc, 0x87, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char msg07[] = {0xbf, 0x82, 0xb9, 0xab, 0xc0, 0xef, 0x20, 0x20, 0x20};
unsigned char msg09[] = {0xca, 0xd5, 0xd9, 0x4d, 0xb9, 0xab, 0xc0, 0xef, 0x20};
unsigned char msg11[] = {0xca, 0xd5, 0xd9, 0x4d, 0xb7, 0xd6, 0xe7, 0x8a, 0x20};
unsigned char msg13[] = {0xb8, 0xbd, 0xbc, 0xd3, 0xd9, 0x4d, 0x20, 0x20, 0x20};
unsigned char msg15[] = {0xbf, 0x82, 0xdc, 0x87, 0xd9, 0x4d, 0x20, 0x20, 0x20};

unsigned char msg02[] = {0x54, 0x41, 0x58, 0x49, 0x20, 0x4e, 0x4f, 0x2e, 0x20};
unsigned char msg04[] = {0x53, 0x54, 0x41, 0x52, 0x54, 0x20, 0x20, 0x20, 0x20};
unsigned char msg06[] = {0x45, 0x4e, 0x44, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char msg08[] = {0x54, 0x4f, 0x54, 0x41, 0x4c, 0x20, 0x4b, 0x4d, 0x20};
unsigned char msg10[] = {0x50, 0x41, 0x49, 0x44, 0x20, 0x4b, 0x4d, 0x20, 0x20};
unsigned char msg12[] = {0x50, 0x41, 0x49, 0x44, 0x20, 0x4d, 0x49, 0x4e, 0x20};
unsigned char msg14[] = {0x53, 0x55, 0x52, 0x43, 0x48, 0x41, 0x52, 0x47, 0x45};
unsigned char msg16[] = {0x54, 0x4f, 0x54, 0x41, 0x4c, 0x20, 0x46, 0x41, 0x52, 0x45};

unsigned char msg17[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xd6, 0x78, 0xd6, 0x78, 0xbb, 0xdd, 0xee, 0x99, 0x20};
unsigned char msg18[] = {0X54, 0x48, 0x41, 0x4e, 0x4b, 0x20, 0x59, 0x4f, 0x55, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};

//printer cmd
unsigned char enter[] = {0x0d, 0x0a};
unsigned char init[] = {0x1b, 0x40};
unsigned char selftest[] = {0x1b, 0x40, 0x12, 0x54};
unsigned char space[] = {0x1b, 0x33, 0x20};
unsigned char font[] = {0x1b, 0x4d, 0x00};
unsigned char uTx_Data[] = {0x0d, 0x0a, 0x0d, 0x0a, 0x0d, 0x0a};
unsigned char cmd[] = {0x1b, 0x33, 0x20, 0x1b, 0x4d, 0x00};

//define struct
struct print1 receipt1;
struct print2 receipt2;

unsigned char plate[14] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
//plate[] = plate_no[] -> eeprom_r()
unsigned char start[] = {0x00, 0x00, 0x2f, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x20, 0x00, 0x00, 0x3a, 0x30, 0x30};				//d-m-y h:m
unsigned char end[] = {0x00, 0x00, 0x2f, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x20, 0x00, 0x00, 0x3a, 0x30, 0x30};					//d-m-y h:m
unsigned char re_stop[] = {0x00, 0x00, 0x2f, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x20, 0x00, 0x00, 0x3a, 0x30, 0x30};				//d-m-y h:m
//unsigned char total_km[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x2e, 0x00};			//123.4km
//unsigned char paid_km[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x2e, 0x00};				//123.4km
//unsigned char paid_min[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x2e, 0x00};			//123.4 minutes
//unsigned char sub[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x2e, 0x30};					//999.0
//unsigned char t_fare[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x2e, 0x30};					//9999.9

unsigned char total_km[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x2e, 0x30, 0x30};			//123.45km
unsigned char paid_km[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x2e, 0x30, 0x30};				//123.45km
unsigned char paid_min[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x2e, 0x30, 0x30};			//123.45 minutes
unsigned char sub[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x2e, 0x30, 0x30};					//999.00
unsigned char t_fare[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x2e, 0x30, 0x30};					//9999.99

uint8_t Flag_Printer_Start = TRUE;
uint8_t Flag_Printer_End = FALSE;
uint8_t Flag_Printed = FALSE;
uint8_t Flag_End_Record = TRUE;
uint8_t flag_vs = FALSE;
uint8_t flag_vs_first = FALSE;

//==============================tbox==============================
uint16_t serial = 0;
uint32_t Dis_x10 = 0;
uint32_t Final_price_x10 = 0;
uint32_t fare_km_x100 = 0;
uint32_t fare_min_x100 = 0;
uint32_t Addition_x10 = 0;

uint8_t Flag_Tbox_Start = TRUE;
uint8_t Flag_Tbox_Finish = FALSE;

//header
unsigned char zone[] = {0xaa, 0x55};
unsigned char hd1[] = {0x00, 0x00, 0x10, 0x01, 0x00, 0x06}; 		//6 byte
unsigned char hd2[] = {0x00, 0x00, 0x10, 0x01, 0x00, 0x1a};			//26 byte

//tbox_finish
unsigned char data1[6];
unsigned char data2[26];

unsigned char t_start[16];
unsigned char t_finish[36];

//==============================eeprom==============================
uint8_t ReadBuffer[BufferSize];
uint8_t WriteBuffer[BufferSize];
uint16_t i = 0;

uint8_t Flag_Para_W = FALSE;
uint8_t Flag_Head = FALSE;
uint8_t Receiving = FALSE;

unsigned char ReplyBuffer[16] = {0};
unsigned char Type[2] = {0};
unsigned char plate_no[10] = {0};

uint8_t Flag_test1 = 0;
uint8_t Flag_test2 = 0;
uint8_t Flag_test3 = 0;

//==============================rtc==============================
PCF2129_t RTC_date;
uint16_t year = 0;
uint16_t month = 0;
uint16_t day = 0;
uint16_t weekday = 0;
uint16_t hour = 0;
uint16_t minute = 0;
uint16_t second = 0;

utc_t UtcTime;
uint32_t u32UnixTimeStamp = 0;
time_tt stCurrentTime;

uint8_t ctrl_1 = 0;
uint8_t ctrl_2 = 0;
uint8_t ctrl_3 = 0;
uint8_t flag_alarm_clear = 0;

uint8_t hh = 0;
uint8_t mm = 0;
uint8_t ss = 0;

//==============================flag==============================
uint8_t Flag_up = TRUE;
uint8_t Flag_down = FALSE;
uint8_t Flag_set = FALSE;

//==============================fare==============================
#define CK_S 	 1000

uint32_t IC_Val1  = 0;
uint32_t IC_Val2  = 0;
uint32_t IC_Val3  = 0;
uint32_t IC_Val4  = 0;
uint32_t Last_Difference  = 0;
uint32_t Difference  = 0;
uint32_t Width  = 0;

uint32_t Is_First_Captured  = 0;
uint32_t Is_First_Captured_x  = 0;

float Fare_reached = 0;
float Fare_2km = 0;
float Fare_200m0 = 0;					//T0 = 2.7
float Fare_200m1 = 0;					//T1 = 1.7
float Fare_200m2 = 0;					//T2 = 1.2
uint32_t Fare_min = 0;					//2700, 2350, 2200
//uint16_t inverse_k; 				//uint16_t ik = 833;
uint16_t ck = 1000;					//init value, avoid x/ck = 0
uint16_t ck_r = 0;						//read from eeprom, car ck
uint16_t Speed_co = 12;				//Speed_co = 12km/h
uint16_t Speed_max = 150;			//Speed_max = 150km/h

float Speed = 0;
//float Speed_x;
float Distance = 0;
float distance = 0;
float Last_Distance = 0;

//uint32_t Last_counter;
uint32_t counter = 0;
uint32_t ex_counter = 0;
uint32_t last_counter = 0;
uint32_t Pulse_Record = 0;
uint8_t cnt = 0;

float Fi = 0; // = 2.05;

uint32_t Dt = 0;
uint32_t DtX = 0;
uint32_t Df = 0;

//uint32_t Fu;
float Fu = 0;
uint32_t Ft = 0;
uint32_t Fd = 0;

uint8_t Flag_2km = 0;
//uint8_t Flag_22km;
uint8_t Flag_2km_c = 0;

float fare = 0;
float fare_d = 0;
float Last_fare_d = 0;
//float fare_d_x10;
uint32_t fare_d_x10 = 0;
uint32_t fare_d_x100 = 0;
uint32_t Last_fare_d_x100 = 0;

float fare_km = 0;
float fare_min = 0;

uint32_t fare_km_x = 0;
uint32_t fare_min_x = 0;

int8_t flag_fare_r = 0;

//==============================anti-tampering==============================
uint8_t Flag_Protect = FALSE;
uint8_t Flag_alarm = TRUE;
uint8_t Flag_set_alarm_rtc = FALSE;

unsigned char cal_min = {0};
unsigned char cal_sec = {0};
unsigned char alarm_min = {0};
unsigned char alarm_sec = {0};
unsigned char alarm_min_set = {0};
unsigned char alarm_sec_set = {0};



uint8_t Tamper_Counter = 0;
uint8_t last_pulse = 0;

int8_t cds = 0;
int8_t cdm = 0;
int8_t cdh = 1;
uint8_t cds_1 = 0;
uint8_t cdm_1 = 0;
uint8_t cdh_1 = 0;
uint8_t cds_10 = 0;
uint8_t cdm_10 = 0;
uint8_t cdh_10 = 0;

uint32_t speed_d;
uint8_t speed_a[8] = {0};
uint8_t width_a[8] = {0};

//==============================total data==============================
uint8_t dataRead[100] = {0};
uint8_t dataWrite[100] = {0};
uint8_t dataDisplay[40] = {0};

uint32_t tkm_d = 0;
uint32_t pkm_d = 0;
uint32_t tflag_d = 0;
uint32_t cflag_d = 0;
uint32_t tpulse_d = 0;
uint32_t tfare_d = 0;
uint32_t tsub_d = 0;
uint32_t cfare_d = 0;
uint32_t csub_d = 0;
//new added
uint32_t tfare_d_x10 = 0;
uint32_t tsub_d_x10 = 0;
uint32_t cfare_d_x10 = 0;
uint32_t csub_d_x10 = 0;
uint32_t ttime_d = 0;
uint32_t ttime_x = 0;
uint32_t nbtime_d = 0;
uint32_t jtime_d = 0;
uint32_t pulses_d = 0;

uint8_t flag_count = 0;
uint8_t change_count = 0;
uint16_t ttime_counter = 0;
uint32_t nbtime_counter = 0;

uint8_t tkm[8] = {0};
uint8_t pkm[8] = {0};
uint8_t tflag[8] = {0};
uint8_t cflag[8] = {0};
uint8_t tpulse[8] = {0};
uint8_t tfare[8] = {0};
uint8_t tsub[8] = {0};
uint8_t cfare[8] = {0};
uint8_t csub[8] = {0};
uint8_t ttime[8] = {0};
//also used by SD
uint8_t nbtime[8] = {0};
uint8_t jtime[8] = {0};
uint8_t pulses[8] = {0};

uint8_t dt[12] = {0};

unsigned char tmsg01[] = {0x31, 0x2e, 0x20, 0xc0, 0xdb, 0xb7, 0x65, 0xbf, 0x82, 0xdf, 0xe4, 0xb9, 0xab, 0xc0, 0xef, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg02[] = {0x32, 0x2e, 0x20, 0xc0, 0xdb, 0xb7, 0x65, 0xdd, 0x64, 0xbf, 0xcd, 0xb9, 0xab, 0xc0, 0xef, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg03[] = {0x33, 0x2e, 0x20, 0xC0, 0xDB, 0xB7, 0x65, 0xC2, 0xE4, 0xC6, 0xEC, 0xB4, 0xCE, 0x94, 0xB5, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg04[] = {0x34, 0x2e, 0x20, 0xAE, 0x94, 0xCC, 0xEC, 0xC0, 0xDB, 0xB7, 0x65, 0xC2, 0xE4, 0xC6, 0xEC, 0xB4, 0xCE, 0x94, 0xB5, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg05[] = {0x35, 0x2e, 0x20, 0xC0, 0xDB, 0xB7, 0x65, 0xD0, 0xA1, 0xCC, 0xF8, 0xB4, 0xCE, 0x94, 0xB5, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg06[] = {0x36, 0x2e, 0x20, 0xC0, 0xDB, 0xB7, 0x65, 0xDC, 0x87, 0xD9, 0x4D, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg07[] = {0x37, 0x2e, 0x20, 0xC0, 0xDB, 0xB7, 0x65, 0xB8, 0xBD, 0xBC, 0xD3, 0xD9, 0x4D, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg08[] = {0x38, 0x2e, 0x20, 0xAE, 0x94, 0xCC, 0xEC, 0xC0, 0xDB, 0xB7, 0x65, 0xDC, 0x87, 0xD9, 0x4D, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg09[] = {0x39, 0x2e, 0x20, 0xAE, 0x94, 0xCC, 0xEC, 0xC0, 0xDB, 0xB7, 0x65, 0xB8, 0xBD, 0xBC, 0xD3, 0xD9, 0x4D, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg10[] = {0x31, 0x30, 0x2e, 0xC0, 0xDB, 0xB7, 0x65, 0xDD, 0x64, 0xBF, 0xCD, 0x95, 0x72, 0xE9, 0x67, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
unsigned char tmsg11[] = {0x31, 0x31, 0x2e, 0xC3, 0x7D, 0xD0, 0x6E, 0x94, 0xB5, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};

//unsigned char data01[] = {0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
//unsigned char data02[] = {0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
//unsigned char data06[] = {0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
//unsigned char data07[] = {0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
//unsigned char data08[] = {0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
//unsigned char data09[] = {0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
//unsigned char data10[] = {0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30};
unsigned char data01[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data02[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data03[] = {0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
unsigned char data04[] = {0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
unsigned char data05[] = {0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
unsigned char data06[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data07[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data08[] = {0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data09[] = {0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data10[] = {0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30};
unsigned char data11[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30};

unsigned char dtdata[] = {0xB4, 0xF2, 0xD3, 0xA1, 0x95, 0x72, 0xE9, 0x67, 0x3A, 0x20, 0x30, 0x30, 0x2f, 0x30, 0x30, 0x2f, 0x30, 0x30, 0x20, 0x30, 0x30, 0x3a, 0x30, 0x30};

//==============================spi sd-card==============================
FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

char buffer[1024] = {0};

unsigned char SD_buffer[114] = {0}; //SD_buffer[106];

/**** capacity related *****/
//FATFS *pfs;
//DWORD fre_clust;
uint32_t total = 0;
uint32_t free_space = 0;

void send_uart (char *string)
{
	uint8_t len = strlen (string);
	HAL_UART_Transmit(&huart2, (uint8_t *) string, len, 2000);// transmit in blocking mode
}

int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void clear_buffer (void)
{
	for (int i=0; i<1024; i++)
	{
		buffer[i] = '\0';
	}
}

//==============================audio==============================
unsigned char v_value[9]  = {0};
unsigned char result[26] = {0};

unsigned char v_can_start[] = {0xAA, 0x1B, 0x10, 0x36, 0x38, 0x30, 0x00, 0x30, 0x00, 0x35, 0x00, 0x35, 0x00, 0x35, 0x00, 0x35, 0x00, 0x36, 0x37, 0x00};
unsigned char v_cn_start[] = {0xAA, 0x1B, 0x10, 0x39, 0x33, 0x30, 0x00, 0x30, 0x00, 0x4A, 0x00, 0x4A, 0x00, 0x4A, 0x00, 0x4A, 0x00, 0x39, 0x32, 0x00};
//testing - only letters of plate no.
//unsigned char v_can_start[] = {0xAA, 0x1B, 0x10, 0x36, 0x38, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x36, 0x37, 0x00};
//unsigned char v_cn_start[] = {0xAA, 0x1B, 0x10, 0x39, 0x33, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x39, 0x32, 0x00};
unsigned char v_en_start[] = {0xAA, 0x1B, 0x10, 0x33, 0x36, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x33, 0x35, 0x00};

unsigned char v_can_end[32] = {0xAA, 0x1B, 0x00, 0x36, 0x39};
unsigned char v_cn_end[32] = {0xAA, 0x1B, 0x00, 0x39, 0x34};
unsigned char v_en_end[32] = {0xAA, 0x1B, 0x00, 0x33, 0x37};

uint8_t Flag_Audio_start = 1; //TRUE;
uint8_t Flag_Audio_end = 0;
uint8_t Flag_btn_changed = 0;

uint8_t btn_value;
uint8_t last_btn_value;

//==============================timer==============================
uint8_t test_f;

uint8_t flag_tester;
uint8_t flag_encoder;
uint8_t flag_sp = 1;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart1_rx;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART4_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	// 5A A5 05 82 5420 0064
	// 5A A5帧头 05 数据长度 82写指令 83读指令
    HAL_UART_DMAStop(&huart1);
    uint8_t data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

    memcpy(RxBuff,receive_buff,12);
    //test
    if (RxBuff[0] == 0 && RxBuff[1] == 0)
    {
    	HAL_UART_ErrorCallback(huart);
    }
    if (RxBuff[3] == 0x83)
    	RxFlag = TRUE;
    memset(receive_buff,0,data_length);
    data_length = 0;
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)receive_buff, 64);
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);
            USER_UART_IDLECallback(huart);
        }
    }
}

//test
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
//	if(huart->ErrorCode&HAL_UART_ERROR_ORE)
//	{
//		__HAL_UART_CLEAR_OREFLAG(huart);
//	}
	if(huart->Instance == USART1)
	{
		HAL_UART_AbortReceive(&huart1);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t RxBuffer[48];
	static uint16_t RxCount = 0;

	memcpy(ReplyBuffer, Type, sizeof(unsigned char)*2);
	memcpy(ReplyBuffer+2, plate_no, sizeof(unsigned char)*10);

	if(huart->Instance == USART3)
	{
		if(Flag_Head == TRUE)
		{
			if(Receiving == TRUE)
			{
				if((RxCount >= 46) && (rec_ch == 0xFF))
				{
					RxCount = 0;
					memset(WriteBuffer,0,48);
					memcpy(WriteBuffer,RxBuffer,48);
					memset(RxBuffer,0,48);
					Flag_Para_W = TRUE;
					Flag_Head = FALSE;
					Receiving = FALSE;
				}
				else
				{
					RxBuffer[RxCount] = rec_ch;
					RxCount++;
				}
			}
			else
			{
				if(rec_ch == 0xBB)
				{
					//header - 0xAA 0xBB: start receiving parameters
					Receiving = TRUE;
				}
				else if(rec_ch == 0xCC)
				{
					//header - 0xAA 0xCC: start reply taxi type & plate no.
					Receiving = FALSE;
					HAL_UART_Transmit(&huart3, ReplyBuffer, 16, 0xff);
				}
				else
				{
					Receiving = FALSE;
				}
			}
		}
		else
		{
			if(rec_ch == 0xAA)
				Flag_Head = TRUE;
			else
				Flag_Head = FALSE;
		}
	}
	HAL_UART_Receive_IT(&huart3, &rec_ch, 1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(Is_First_Captured == 0)
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			Is_First_Captured = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		}
		else if(Is_First_Captured == 1)
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(htim, 0);

			if(IC_Val2 > IC_Val1)
			{
				Width = IC_Val2 - IC_Val1;
			}

			Is_First_Captured = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
	}

	if(htim->Instance== TIM1)
	{
		if(Is_First_Captured_x == 0)
		{
			IC_Val3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			Is_First_Captured_x = 1;
			counter++;
		}
		else if(Is_First_Captured_x == 1)
		{
			IC_Val4 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			counter++;

			if(IC_Val4 > IC_Val3)
			{
				Difference = IC_Val4 - IC_Val3;
			}
			else if(IC_Val4 < IC_Val3)
			{
				Difference = ((0xFFFF - IC_Val3) + IC_Val4) + 1;
			}
			Is_First_Captured_x = 0;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance== TIM3)
	{
		if (test_f == 0)
		{
			test_f = 1;
		}
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
//  	HAL_Delay(2000); //Proposed to test the electricity.
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_LPUART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART4_UART_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  if (MX_FATFS_Init() != APP_OK) {
    Error_Handler();
  }
  MX_TIM3_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

  /*eric*/
  //HAL_UART_Receive_DMA(&huart1, (uint8_t*)receive_buff, 64);
  HAL_UART_Receive_DMA(&huart1, receive_buff, 64);
  /*eric*/

  HAL_UART_Receive_IT(&huart3, &rec_ch, 1);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim3);

  HAL_Delay(200); //changed by Edwin from 200 to 10000

//  EEPROM_PageErase(3);
//  eeprom_w();
//  eeprom_check();
  eeprom_r();
  PCF2129_Init(hi2c1, RTC_WRITE_ADDR);
//  rtc_set();


  //Mounting SD card
  fresult = f_mount(&fs, "/", 1);
  //Create the file with read write access and open it
//  fresult = f_open(&fil, "tripDATA.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
  fresult = f_open(&fil, "tripDATA.txt", FA_CREATE_NEW | FA_WRITE);
  //Writing text
  strcpy (buffer, "vehicleNo meterNo startDate startTime endDate endTime waitTime journeyTime paidKM totalKM nonBusiness amount surCharge totalCharge\n");
  fresult = f_write(&fil, buffer, bufsize(buffer), &bw);
  //Close file
  f_close(&fil);
  // clearing buffer to show that result obtained is from the file
  clear_buffer();

  Voice_init();

//  E_read();

//0x20 space
//0x41 A
//0X30 '0'
// 0X31 '1'

  SD_buffer[0] = plate_qr[6];
  SD_buffer[1] = plate_qr[7];
  SD_buffer[2] = plate_qr[8];
  SD_buffer[3] = plate_qr[9];
  SD_buffer[4] = plate_qr[10];
  SD_buffer[5] = plate_qr[11];
  SD_buffer[6] = 0x20;

  SD_buffer[7] = 0x41;
  SD_buffer[8] = 0x30;
  SD_buffer[9] = 0x30;
  SD_buffer[10] = 0x30;
  SD_buffer[11] = 0x30;
  SD_buffer[12] = 0x31;
  SD_buffer[13] = 0x20;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (test_f)
	  {
		  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
		  {
			  if(Flag_Para_W)
			  {
				  Flag_Para_W = FALSE;
				  eeprom_w();
				  eeprom_check();
				  eeprom_r();
				  rtc_set();
				  memset(ReplyBuffer, 0, 16);
				  unsigned char ok[1] = {0xDD};
				  HAL_UART_Transmit(&huart3, ok, 1, 0x02);
			  }
			  time_update();
			  HAL_UART_Transmit(&huart1, (uint8_t *) plate_qr, 14, 0x1C);
		  }
		  else if(Flag_Protect)
		  {
			  DebugLog(); // Debug record every 3 sec
			  //anti-tamper process
			  Anti_Tamper_Handle();
		  }
		  else
		  {
			  DebugLog(); // Debug record every 3 sec
			  //normal process
			  Status_Check();
		  }

		  test_f = 0;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00303D5B;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT|UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart1.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  huart1.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 15999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pin : PF2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 PB3
                           PB4 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void Status_Check(void)
{
	time_update();

	HAL_UART_Transmit(&huart1, (uint8_t *) plate_qr, 14, 0x1C);
	HAL_Delay(1);

	HAL_UART_Transmit(&huart1, unlock_clear, 12, 0x20);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, C_Fare_clear, 7, 0x20);
	HAL_Delay(1);

	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Hired, 7, 0x0E);
	HAL_Delay(2);
	Status_Change();
	if(Drv_state_flag)
	{
		Flag_H = TRUE;
		Drv_state_flag = FALSE;
	}
	else
	{
		Flag_H = FALSE;
	}
	memset(RxBuff,0,12);

	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Stop, 7, 0x0E);
	HAL_Delay(2);
	Status_Change();
	if(Drv_state_flag)
	{
		Flag_S = TRUE;
		Drv_state_flag = FALSE;
	}
	else
	{
		Flag_S = FALSE;
	}
	memset(RxBuff,0,12);

	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Vacant, 7, 0x0E);
	HAL_Delay(2);
	Status_Change();
	if(Drv_state_flag)
	{
		Flag_V = TRUE;
		Drv_state_flag = FALSE;
	}
	else
	{
		Flag_V = FALSE;
	}
	memset(RxBuff,0,12);

	last_btn_value = btn_value;
	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Audio, 7, 0x0E);
	HAL_Delay(2);
	if(RxFlag)
	{
		RxFlag = FALSE;
		btn_value = RxBuff[8];
		if (btn_value != last_btn_value)
			Flag_btn_changed = true; //TRUE;

		memset(RxBuff,0,12);
	}

	if (Flag_V && Flag_H)
	{
		Flag_V2H = TRUE;
		Flag_H2S = FALSE;
		Flag_S2V = FALSE;
	}
	else if (Flag_H && Flag_S)
	{
		Flag_V2H = FALSE;
		Flag_H2S = TRUE;
		Flag_S2V = FALSE;
	}
	else if (Flag_S && Flag_V)
	{
		Flag_V2H = FALSE;
		Flag_H2S = FALSE;
		Flag_S2V = TRUE;
	}

	ex_counter = counter;
//	cnt += 1;

	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 1)
	{
		//pc9 = 0, encoder
		//pc9 = 1, tester/not connect
		flag_tester = 1;
		flag_encoder = 0;
		cnt += 1;
	}
	else
	{
		flag_tester = 0;
		flag_encoder = 1;
		cnt = 0;
	}

	if(ex_counter >= 10 && flag_tester && flag_sp)
	{
//		flag_tester = 1;
		Flag_V2H = FALSE;
		Flag_H2S = FALSE;
		Flag_S2V = TRUE;
	}

	if (Flag_V2H && (Drv_state == S_VACANT))
	{
		Drv_state = S_HIRED;
		Flag_V2H = FALSE;
	}
	else if (Flag_H2S && (Drv_state == S_HIRED))
	{
		Drv_state = S_STOP;
		Flag_H2S = FALSE;
	}
	else if (Flag_H2S && (Drv_state == S_STOP))
	{
		Drv_state = S_HIRED;
		Flag_H2S = FALSE;
	}
	else if (Flag_S2V && (Drv_state == S_STOP))
	{
		Drv_state = S_VACANT;
		Flag_S2V = FALSE;
		counter = 0;
		ex_counter = 0;
	}
	else if (Flag_S2V && (Drv_state == S_VACANT))
	{
		Drv_state = S_STOP;
		Flag_S2V = FALSE;
	}

	if (Drv_state == S_HIRED)
	{
		HMI_Hired_W();
		ttime_counter++;
	}
	else if (Drv_state == S_STOP)
	{
		HMI_Stop_W();
		ttime_counter++;
	}
	else if (Drv_state == S_VACANT)
	{
		if (cnt != 0)
		{
			if (cnt > 3 && counter == ex_counter && ex_counter < 10)
			{
				cnt = 0;
				ex_counter = 0;
				counter = 0;
			}
		}
		else
		{
			counter = 0;
		}
		HMI_Vacant_W();
		nbtime_counter++;
	}
}

void Status_Change(void)
{
	if (RxFlag)
	{
		if(RxBuff[8] == 0x01)
		{
			Drv_state_flag = TRUE;
		}
		RxFlag = FALSE;
	}
}

void HMI_Hired_W(void)
{
	uint8_t temp_array[4];

	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Hired_On, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Vacant_Off, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Stop_Off, 8, 0x10);
	HAL_Delay(1);

	Last_Final_price = 0;
	Last_Final_price_x10 = 0;
	ex_Addition = 0;
	Last_Dis = 0;
	Last_fare_d_x100 = 0;

	HAL_UART_Transmit(&huart1, (uint8_t *) Print_clear, 8, 0x10);
	HAL_UART_Transmit(&huart1,CLR_extra_btn,8,0x0E);

	Fare_Calculation();

	fare_d_x10 = (int)(fare_d * 10 + 0.5);
	//display limit - total fare 9999.9, sub 999.0, distance 999.9, duration 99:59:59
	if(fare_d_x10 >= 99999)
	{
		fare_d_x10 = 99999;
	}
	fare_d_x100 = fare_d_x10 * 10;
//	Long_to_byte(fare_d_x10, temp_array);
	Long_to_byte(fare_d_x100, temp_array);
	HMI_Command(Fee,temp_array,10);
	//send fare_d_x100 to Fee_dup & Fee_total, no extra add on
//	HMI_Command(Fee_dup, temp_array, 10);
//	HMI_Command(Fee_total, temp_array, 10);

	extras_rd();

	//distance display
	Dis = (int)(Distance*100); // + 0.5);
	if(Dis >= 99999)
	{
		Dis = 99999;
	}
	Long_to_byte(Dis, temp_array);
	HMI_Command(Dis_A,temp_array,10);

	//duration display
	Dur_hms_cal();

	//audio start
	if ((Flag_Audio_start==1) || (Flag_btn_changed==1))
	{
		Flag_Audio_start = 0; //false; //FALSE;
		Flag_Audio_end = 1; //true; //TRUE;
		Flag_btn_changed = 0; //false; //FALSE;

		switch (btn_value)
		{
			case 1:
				Voice_start(v_can_start);
				break;
			case 2:
				Voice_start(v_cn_start);
				break;
			case 3:
				Voice_start(v_en_start);
				break;

			default:
				break;
		}
	}

	//record printer start time
	if(Flag_Printer_Start)
	{
		Flag_Printer_Start = FALSE;
		Flag_Printer_End = TRUE;

		start[0] = hmi_rtc[14];
		start[1] = hmi_rtc[15];
		start[3] = hmi_rtc[11];
		start[4] = hmi_rtc[12];
		start[6] = hmi_rtc[8];
		start[7] = hmi_rtc[9];
		start[9] = hmi_rtc[17];
		start[10] = hmi_rtc[18];
		start[12] = hmi_rtc[20];
		start[13] = hmi_rtc[21];
	}

	//transfer pick-up time (transfer only one time, Flag_Tbox_Start changed in S_to_V status)
	if(Flag_Tbox_Start)
	{
		Flag_Tbox_Start = FALSE;
		Flag_Tbox_Finish = TRUE;
		tbox_start();
		flag_count = 1;
		E_read();
		//non business time
		nbtime_d = ((nbtime_counter / 60.0)* 1000) / 10; //changed
		nbtime[7] = (nbtime_d /1) % 10 + 0x30;
		nbtime[6] = (nbtime_d /10) % 10 + 0x30;
		nbtime[5] = (nbtime_d /100) % 10 + 0x30;
		nbtime[4] = (nbtime_d /1000) % 10 + 0x30;
		nbtime[3] = (nbtime_d /10000) % 10 + 0x30;
		nbtime[2] = (nbtime_d /10000) % 10 + 0x30;

		if (nbtime[2] == 0x30)
			SD_buffer[82] = 0x20;
		else
			SD_buffer[82] = nbtime[2];
		if (nbtime[3] == 0x30)
			SD_buffer[83] = 0x20;
		else
			SD_buffer[83] = nbtime[3];
		if (nbtime[4] == 0x30)
			SD_buffer[84] = 0x20;
		else
			SD_buffer[84] = nbtime[4];

		SD_buffer[85] = nbtime[5];
		SD_buffer[86] = 0x2E;
		SD_buffer[87] = nbtime[6];
		SD_buffer[88] = nbtime[7];
		SD_buffer[89] = 0x20;
		nbtime_counter = 0;
		//start date time
		SD_buffer[14] = 0x32;
		SD_buffer[15] = 0x30;
		SD_buffer[16] = hmi_rtc[8];
		SD_buffer[17] = hmi_rtc[9];
		SD_buffer[18] = 0x2D;
		SD_buffer[19] = hmi_rtc[11];
		SD_buffer[20] = hmi_rtc[12];
		SD_buffer[21] = 0x2D;
		SD_buffer[22] = hmi_rtc[14];
		SD_buffer[23] = hmi_rtc[15];
		SD_buffer[24] = 0x20;

		SD_buffer[25] = hmi_rtc[17];
		SD_buffer[26] = hmi_rtc[18];
		SD_buffer[27] = 0x3A;
		SD_buffer[28] = hmi_rtc[20];
		SD_buffer[29] = hmi_rtc[21];
		SD_buffer[30] = 0x3A;
		SD_buffer[31] = hmi_rtc[23];
		SD_buffer[32] = hmi_rtc[24];
		SD_buffer[33] = 0x20;
	}
	Flag_Control_Down();
	flag_sp = 0;
	flag_vs = 1;
	Flag_End_Record = TRUE;
	flag_vs_first = 0;
	cnt = 0;
}

void HMI_Stop_W(void)
{
	uint8_t temp_array[4];

	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Hired_Off, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Stop_On, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Vacant_Off, 8, 0x10);
	HAL_Delay(1);

	Fare_Calculation();

	fare_d_x10 = (int)(fare_d * 10 + 0.5);
	//display limit - total fare 9999.9, sub 999.0, distance 999.9, duration 99:59:59
	if(fare_d_x10 >= 99999)
	{
		fare_d_x10 = 99999;
	}
	fare_d_x100 = fare_d_x10 * 10;
	Long_to_byte(fare_d_x100, temp_array);
	//bug: if addition != 0, fare will be not updated
//	if (Addition == 0)
//	{
//		HMI_Command(Fee,temp_array,10);
//	}
	//0702 !!! test !!!
//	HMI_Command(Fee,temp_array,10);

	//update when fare changed
//	if (Last_fare_d != fare_d)
//	{
//		HMI_Command(Fee,temp_array,10);
//		HMI_Command(Fee_dup,temp_array,10);
//	}

	Dis = (int)(Distance*100); // + 0.5);
	if(Dis >= 99999)
	{
		Dis = 99999;
	}
	Long_to_byte(Dis, temp_array);
	HMI_Command(Dis_A,temp_array,10);

	Dur_hms_cal();

	//record printer start time
	if(Flag_Printer_Start)
	{
		Flag_Printer_Start = FALSE;
		Flag_Printer_End = TRUE;

		start[0] = hmi_rtc[14];
		start[1] = hmi_rtc[15];
		start[3] = hmi_rtc[11];
		start[4] = hmi_rtc[12];
		start[6] = hmi_rtc[8];
		start[7] = hmi_rtc[9];
		start[9] = hmi_rtc[17];
		start[10] = hmi_rtc[18];
		start[12] = hmi_rtc[20];
		start[13] = hmi_rtc[21];
	}

	if(flag_vs)
	{
		flag_vs = 0;
		re_stop[0] = hmi_rtc[14];
		re_stop[1] = hmi_rtc[15];
		re_stop[3] = hmi_rtc[11];
		re_stop[4] = hmi_rtc[12];
		re_stop[6] = hmi_rtc[8];
		re_stop[7] = hmi_rtc[9];
		re_stop[9] = hmi_rtc[17];
		re_stop[10] = hmi_rtc[18];
		re_stop[12] = hmi_rtc[20];
		re_stop[13] = hmi_rtc[21];
	}

	//sub read and add on, prepare receipt message
	if (!Flag_added)
	{
		extras_rd();
	}

	if(Flag_Addition_Add)
	{
		Final_price = fare_d_x10 + Addition;
		uint8_t temp_array[4];
		Final_price_x10 = Final_price * 10; //for display (4 decimal+2 digits)
		Long_to_byte(Final_price_x10,temp_array);
//		HMI_Command(Fee,temp_array,10);
		//send Fee (Final_price_x100) to 0x4000 for holding preview
//		HMI_Command(Fee_total,temp_array,10);
		Flag_Addition_Add = FALSE;
	}

	if(Flag_Addition_F)
	{
		Final_price = fare_d_x10 + Addition;
		uint8_t temp_array[4];
		Final_price_x10 = Final_price * 10; //for display (4 decimal+2 digits)
		Long_to_byte(Final_price_x10,temp_array);
//		HMI_Command(Fee,temp_array,10);
		//send Fee (Final_price_x100) to 0x4000 for holding preview
//		HMI_Command(Fee_total,temp_array,10);
		Flag_Addition_F = FALSE;
	}

	//Flag_Display check:
	HAL_UART_Transmit(&huart1, (uint8_t *) RD_extra_btn, 7, 0x0E);
	HAL_Delay(2);
	if(RxFlag)
	{
		RxFlag = FALSE;
		if (RxBuff[8] == 0x01 && Flag_added == TRUE)
		{
			added_wait = 0;
			Flag_added = FALSE;
			HAL_UART_Transmit(&huart1,CLR_extra_btn,8,0x0E);
		}
		else if(RxBuff[8] == 0x01)
		{
			added_wait++;
			Flag_added = TRUE;
			HAL_UART_Transmit(&huart1,CLR_extra_btn,8,0x0E);
		}
	}
	else
	{
		Flag_added = FALSE;
	}

	if(Flag_added)
	{
		uint32_t add_clr = 0;

		Long_to_byte(Final_price_x10, temp_array);
		HMI_Command(Fee, temp_array, 10);

		Long_to_byte(add_clr, temp_array);
		HMI_Command(Add_Charge, temp_array, 10);

		if(added_wait != 0)
		{
			added_wait++;
			//added 10s
			if(added_wait >= 5)
			{
				Flag_added = FALSE;
				added_wait = 0;
			}
		}
	}
	else
	{
		Long_to_byte(fare_d_x100, temp_array);
		HMI_Command(Fee,temp_array,10);

		Long_to_byte(Addition,temp_array);
		HMI_Command(Add_Charge,temp_array,10);
	}

	//cpy total_km
	//123.45 - default 0.00
	total_km[13] = (Dis / 1) % 10 + 0x30;
	total_km[12] = (Dis / 10) % 10 + 0x30;
	total_km[10] = (Dis / 100) % 10 + 0x30;

	if ((Dis / 10000) % 10 == 0 && (Dis / 1000) % 10 == 0)
	{
		total_km[9] = 0x20;
		total_km[8] = 0x20;
	}
	else if ((Dis / 10000) % 10 == 0)
	{
		total_km[9] = (Dis / 1000 % 10) + 0x30;
		total_km[8] = 0x20;
	}
	else
	{
		total_km[9] =  (Dis / 1000 % 10) + 0x30;
		total_km[8] =  (Dis / 10000 % 10) + 0x30;
	}

	//cpy paid_km 9, 10, 11, 13
	//default: paid_km 123.45 (8,9,10,12,13)
	fare_km_x = (int)(fare_km*100);
	paid_km[13] = (fare_km_x /1) % 10 + 0x30;
	paid_km[12] = (fare_km_x /10) % 10 + 0x30;
	paid_km[10] = (fare_km_x /100) % 10 + 0x30;

	if((fare_km_x / 10000) % 10 == 0 && (fare_km_x / 1000) % 10 == 0)
	{
		paid_km[9] = 0x20;
		paid_km[8] = 0x20;
	}
	else if((fare_km_x / 10000) % 10 == 0)
	{
		paid_km[9] =  (fare_km_x / 1000) % 10 + 0x30;
		paid_km[8] = 0x20;
	}
	else
	{
		paid_km[9] =  (fare_km_x / 1000) % 10 + 0x30;
		paid_km[8] =  (fare_km_x / 10000) % 10 + 0x30;
	}

	//cpy paid_min 9, 10, 11, 13
	//default: paid_min 123.45 (8,9,10,12,13)
	fare_min_x = (fare_min * 1000) / 10;
	paid_min[13] = (fare_min_x /1) % 10 + 0x30;
	paid_min[12] = (fare_min_x /10) % 10 + 0x30;
	paid_min[10] = (fare_min_x /100) % 10 + 0x30;

	if((fare_min_x / 10000) % 10 == 0 && (fare_min_x / 1000) % 10 == 0)
	{
		paid_min[9] = 0x20;
		paid_min[8] = 0x20;
	}
	else if((fare_min_x / 10000) % 10 == 0)
	{
		paid_min[9] =  (fare_min_x / 1000) % 10 + 0x30;
		paid_min[8] = 0x20;
	}
	else
	{
		paid_min[9] =  (fare_min_x / 1000) % 10 + 0x30;
		paid_min[8] =  (fare_min_x / 10000) % 10 + 0x30;
	}

	if (Addition == 0)
	{
		//8,9,10-HK$0.0; 11,12,13 default 0.0
		//7,8,9-HK$, 10,11,12,13-0.00, default HK$0.00
		SD_buffer[98] = 0x20;
		SD_buffer[99] = 0x20;
		SD_buffer[100] = 0x30;
		sub[13] = 0x30;
		sub[12] = 0x30;
		sub[11] = 0x2e;
		sub[10] = 0x30;
		sub[9] = 0x24;
		sub[8] = 0x4b;
		sub[7] = 0x48;
		sub[6] = 0x20;
		sub[5] = 0x20;
	}
	else
	{
		if(((Addition/10) / 100 % 10) == 0 && ((Addition/10) / 10 % 10) == 0)
		{
			sub[10] = ((Addition/10) / 1 % 10) + 0x30;
			sub[9] = 0x24;
			sub[8] = 0x4b;
			sub[7] = 0x48;
			sub[6] = 0x20;
			sub[5] = 0x20;
			SD_buffer[98] = 0x20;
			SD_buffer[99] = 0x20;
			SD_buffer[100] = sub[10]; //sub[11];
		}
		else if(((Addition/10) / 100 % 10) == 0)
		{
			sub[10] = ((Addition/10) / 1 % 10) + 0x30;
			sub[9]= ((Addition/10) / 10 % 10) + 0x30;
			sub[8] = 0x24;
			sub[7] = 0x4b;
			sub[6] = 0x48;
			sub[5] = 0x20;
			SD_buffer[98] = 0x20;
			SD_buffer[99] = sub[9]; //sub[10];
			SD_buffer[100] = sub[10]; //sub[11];
		}
		else
		{
			sub[10] = ((Addition/10) / 1 % 10) + 0x30;
			sub[9] = ((Addition/10) / 10 % 10) + 0x30;
			sub[8] = ((Addition/10) / 100 % 10) + 0x30;
			sub[7] = 0x24;
			sub[6] = 0x4b;
			sub[5] = 0x48;
			SD_buffer[98] = sub[8]; //sub[9];
			SD_buffer[99] = sub[9]; //sub[10];
			SD_buffer[100] = sub[10]; //sub[11];
		}
	}
	SD_buffer[101] = 0x2E;
	SD_buffer[102] = 0x30;
	SD_buffer[103] = 0x30;
	SD_buffer[104] = 0x20;

	//cpy fare(without sub) //new add for sd_card
	if ((fare_d_x10 /10000) % 10 == 0 && (fare_d_x10 /1000) % 10 == 0)
	{
		SD_buffer[90] = 0x20;
		SD_buffer[91] = 0x20;
	}
	else if((fare_d_x10 /10000) % 10 == 0)
	{
		SD_buffer[90] = 0x20;
		SD_buffer[91] = (fare_d_x10 /1000) % 10 + 0x30;
	}
	else
	{
		SD_buffer[90] = (fare_d_x10 /10000) % 10 + 0x30;
		SD_buffer[91] = (fare_d_x10 /1000) % 10 + 0x30;
	}
	SD_buffer[92] = (fare_d_x10 /100) % 10 + 0x30;
	SD_buffer[93] = (fare_d_x10 /10) % 10 + 0x30;
	SD_buffer[94] = 0x2E;
	SD_buffer[95] = (fare_d_x10 /1) % 10 + 0x30;
	SD_buffer[96] = 0x30;
	SD_buffer[97] = 0x20;

	//cpy total_fare
	//default 00.0 - [9][10][11][12] - 0x30 0x30 0x2e 0x30
	//default 00.00 - [8][9][10][11][12]
	if(Addition == 0)
	{
		Final_price = fare_d_x10;
	}

	t_fare[12] = 0x30;
	t_fare[11] = (Final_price /1) % 10 + 0x30;
	t_fare[9] = (Final_price /10) % 10 + 0x30;
	t_fare[8] = (Final_price /100) % 10 + 0x30;

	if ((Final_price /10000) % 10 == 0 && (Final_price /1000) % 10 == 0)
	{
//		t_fare[8] = 0x24;
//		t_fare[7] = 0x4b;
//		t_fare[6] = 0x48;
		t_fare[7] = 0x24;
		t_fare[6] = 0x4b;
		t_fare[5] = 0x48;
		t_fare[4] = 0x20;
		t_fare[3] = 0x20;
		SD_buffer[105] = 0x20;
		SD_buffer[106] = 0x20;
	}
	else if((Final_price /10000) % 10 == 0)
	{
//		t_fare[8] = (Final_price /1000 % 10) + 0x30;
//		t_fare[7] = 0x24;
//		t_fare[6] = 0x4b;
//		t_fare[5] = 0x48;
		t_fare[7] = (Final_price /1000) % 10 + 0x30;
		t_fare[6] = 0x24;
		t_fare[5] = 0x4b;
		t_fare[4] = 0x48;
		t_fare[3] = 0x20;
		SD_buffer[105] = 0x20;
		SD_buffer[106] = t_fare[7]; //t_fare[8];
	}
	else
	{
//		t_fare[8] = (Final_price /1000 % 10) + 0x30;
//		t_fare[7] = (Final_price /10000 % 10) + 0x30;
//		t_fare[6] = 0x24;
//		t_fare[5] = 0x4b;
//		t_fare[4] = 0x48;
		t_fare[7] = (Final_price /1000) % 10 + 0x30;
		t_fare[6] = (Final_price /10000) % 10 + 0x30;
		t_fare[5] = 0x24;
		t_fare[4] = 0x4b;
		t_fare[3] = 0x48;
		SD_buffer[105] = t_fare[6]; //t_fare[7];
		SD_buffer[106] = t_fare[7]; //t_fare[8];
	}
	SD_buffer[107] = t_fare[8]; //t_fare[9];
	SD_buffer[108] = t_fare[9]; //t_fare[10];
	SD_buffer[109] = 0x2E;
	SD_buffer[110] = t_fare[11];
	SD_buffer[111] = 0x30;
	SD_buffer[112] = 0x20;

	//v_can_end
	v_value[1] = (Final_price /10000) % 10 + 0x30;
	v_value[2] = (Final_price /1000) % 10 + 0x30;
	v_value[3] = (Final_price /100) % 10 + 0x30;
	v_value[4] = (Final_price /10) % 10 + 0x30;
	v_value[5] = (Final_price /1) % 10 + 0x30;
	if(Addition == 0)
	{
		v_value[0] = 0;
		v_value[6] = 0;
		v_value[7] = 0;
		v_value[8] = 0;
	}
	else
	{
		v_value[0] = 1;
		v_value[6] = (Addition /1000) % 10 + 0x30;
		v_value[7] = (Addition /100) % 10 + 0x30;
		v_value[8] = (Addition /10) % 10 + 0x30;
	}
	//audio end
	if (((Flag_Audio_end==1) || (Flag_btn_changed==1)) && (Flag_Audio_end!=48))
	{
		Flag_Audio_end = 0; //false; //FALSE;
		Flag_Audio_start = 1; //true; //TRUE;
		Flag_btn_changed = 0; //false; //FALSE;

		if (btn_value == 1)
		{
			cal_vcan(v_value, result);
			v_can_end[2] = result[0] + 2;
			for (i=0; i<result[0]; i++)
			{
				v_can_end[i+5] = result[i+1];
			}
			v_can_end[v_can_end[2] + 3] = checksum_8bit(v_can_end, (v_can_end[2] + 3));
			Voice_end(v_can_end);
			memset(v_can_end+5,0,sizeof(unsigned char)*27);
		}
		else if (btn_value == 2)
		{
			cal_vcn(v_value, result);
			v_cn_end[2] = result[0] + 2;
			for (i=0; i<result[0]; i++)
			{
				v_cn_end[i+5] = result[i+1];
			}
			v_cn_end[v_cn_end[2] + 3] = checksum_8bit(v_cn_end, (v_cn_end[2] + 3));
			Voice_end(v_cn_end);
			memset(v_cn_end+5,0,sizeof(unsigned char)*27);
		}
		else if (btn_value == 3)
		{
			cal_ven(v_value, result);
			v_en_end[2] = result[0] + 2;
			for (i=0; i<result[0]; i++)
			{
				v_en_end[i+5] = result[i+1];
			}
			v_en_end[v_en_end[2] + 3] = checksum_8bit(v_en_end, (v_en_end[2] + 3));
			Voice_end(v_en_end);
			memset(v_en_end+5,0,sizeof(unsigned char)*27);
		}
		memset(v_value, 0, sizeof(unsigned char)*9);
		memset(result, 0, sizeof(unsigned char)*26);
		Flag_Audio_end = 0;
	}

	//record last final price
	Last_Final_price = Final_price;
	Last_Final_price_x10 = Final_price_x10;
	//change
	Last_fare_d_x100 = fare_d_x100;
	ex_Addition = Addition;
	Last_Dis = Dis;

	//transfer pick-up time (transfer only one time, Flag_Tbox_Start changed in S_to_V status)
	if(Flag_Tbox_Start)
	{
		Flag_Tbox_Start = FALSE;
		Flag_Tbox_Finish = TRUE;
		tbox_start();
		flag_count = 1;
		E_read();
		//non business time
		nbtime_d = ((nbtime_counter / 60.0)* 1000) / 10; //changed
		Dec2ASC(nbtime_d, nbtime);
		if (nbtime[2] == 0x30)
			SD_buffer[82] = 0x20;
		else
			SD_buffer[82] = nbtime[2];
		if (nbtime[3] == 0x30)
			SD_buffer[83] = 0x20;
		else
			SD_buffer[83] = nbtime[3];
		if (nbtime[4] == 0x30)
			SD_buffer[84] = 0x20;
		else
			SD_buffer[84] = nbtime[4];

		SD_buffer[85] = nbtime[5];
		SD_buffer[86] = 0x2E;
		SD_buffer[87] = nbtime[6];
		SD_buffer[88] = nbtime[7];
		SD_buffer[89] = 0x20;
		nbtime_counter = 0;
		//start date time
		SD_buffer[14] = 0x32;
		SD_buffer[15] = 0x30;
		SD_buffer[16] = hmi_rtc[8];
		SD_buffer[17] = hmi_rtc[9];
		SD_buffer[18] = 0x2D;
		SD_buffer[19] = hmi_rtc[11];
		SD_buffer[20] = hmi_rtc[12];
		SD_buffer[21] = 0x2D;
		SD_buffer[22] = hmi_rtc[14];
		SD_buffer[23] = hmi_rtc[15];
		SD_buffer[24] = 0x20;
		SD_buffer[25] = hmi_rtc[17];
		SD_buffer[26] = hmi_rtc[18];
		SD_buffer[27] = 0x3A;
		SD_buffer[28] = hmi_rtc[20];
		SD_buffer[29] = hmi_rtc[21];
		SD_buffer[30] = 0x3A;
		SD_buffer[31] = hmi_rtc[23];
		SD_buffer[32] = hmi_rtc[24];
		SD_buffer[33] = 0x20;
	}

	printer_rd();

	unsigned char Dis_x10_A[4] = {0};
	Dis_x10 = Dis * 10;
	Long_to_byte(Dis_x10, Dis_x10_A);
	data2[0] = Dis_x10_A[0];
	data2[1] = Dis_x10_A[1];
	data2[2] = Dis_x10_A[2];
	data2[3] = Dis_x10_A[3];
	//total_fare - 4 bytes
	unsigned char Final_price_x10_A[4] = {0};
	Final_price_x10 = Final_price * 10;
	Long_to_byte(Final_price_x10, Final_price_x10_A);
	data2[4] = Final_price_x10_A[0];
	data2[5] = Final_price_x10_A[1];
	data2[6] = Final_price_x10_A[2];
	data2[7] = Final_price_x10_A[3];
	//end - 6 bytes
	RTC_date = PCF2129_getDate();
	year = RTC_date.date.y;
	data2[8] = (uint8_t)(year & 0xFF);
	month = RTC_date.date.m;
	data2[9] = (uint8_t)(month & 0xFF);
	day = RTC_date.date.d;
	data2[10] = (uint8_t)(day & 0xFF);
	hour = RTC_date.date.hh;
	data2[11] = (uint8_t)(hour & 0xFF);
	minute = RTC_date.date.mm;
	data2[12] = (uint8_t)(minute & 0xFF);
	second = RTC_date.date.ss;
	data2[13] = (uint8_t)(second & 0xFF);

	//paid_km - 4 bytes
	fare_km_x100 = (int)(fare_km * 100);
	unsigned char temp[4] = {0};
	Long_to_byte(fare_km_x100, temp);
	data2[14] = temp[0];
	data2[15] = temp[1];
	data2[16] = temp[2];
	data2[17] = temp[3];
	//wait_time - 4 bytes
	fare_min_x100 = (int)(fare_min * 100);
	unsigned char temp2[4] = {0};
	Long_to_byte(fare_min_x100, temp2);
	data2[18] = temp2[0];
	data2[19] = temp2[1];
	data2[20] = temp2[2];
	data2[21] = temp2[3];
	//extra - 4 bytes
	Addition_x10 = Addition * 10;
	unsigned char temp3[4] = {0};
	Long_to_byte(Addition_x10, temp3);
	data2[22] = temp3[0];
	data2[23] = temp3[1];
	data2[24] = temp3[2];
	data2[25] = temp3[3];
	//reserved - 4 bytes
	Flag_Control_Down();
	flag_sp = 0;
	cnt = 0;
}

void HMI_Vacant_W(void)
{
	uint8_t temp_array[4];

	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Hired_Off, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Stop_Off, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) LED_Vacant_On, 8, 0x10);
	HAL_Delay(1);

	//record eeprom total
	tkm_d += Dis;
	pkm_d += fare_km_x;
	tfare_d += Final_price;
	tsub_d += Addition;
	cfare_d += Final_price;
	csub_d += Addition;
	tflag_d += flag_count;
	cflag_d += flag_count;
	tpulse_d += change_count;
	ttime_d += ((ttime_counter / 60.0)* 1000) / 10; //changed

	Dec2ASC(tkm_d, tkm);
	Dec2ASC(pkm_d, pkm);
	Dec2ASC(tfare_d, tfare);
	Dec2ASC(tsub_d, tsub);
	Dec2ASC(tflag_d, tflag);
	Dec2ASC(cflag_d, cflag);
	Dec2ASC(cfare_d, cfare);
	Dec2ASC(csub_d, csub);
	Dec2ASC(tpulse_d, tpulse);
	Dec2ASC(ttime_d, ttime);

//	Fu = 0;
//	Ft = 0;
//	Fd = 0;
//	Dt = 0;
//	DtX = 0;
//	Fi = 0; //2.35;
//	fare_d_x10 = 0;
//	fare_d_x100 = 0;
//	fare_d = 0;
//	Last_fare_d = 0;
//	fare = 0;
//	fare_km = 0;
//	fare_min = 0;
//	Flag_2km = FALSE;
//	flag_fare_r = FALSE;
//	Final_price = 0;
//	Final_price_x10 = 0;
//	Addition = 0;
//	flag_count = 0;
//	change_count = 0;
//	fare_km_x = 0;
////	ttime_counter = 0; //move to end
//	last_counter = 0; //pull to start of the function as T-box has no signal
//	Dis = 0;
//	Distance = 0;
//	distance = 0;

	//Flag_Display check:
	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Clear, 7, 0x0E);
	HAL_Delay(2);
	if(RxFlag)
	{
		RxFlag = FALSE;
		if (RxBuff[8] == 0x01 && Flag_Display == TRUE)
		{
			display_wait = 0;
			Flag_Display = FALSE;
			HAL_UART_Transmit(&huart1,Clear_clear,8,0x0E);
		}
		else if(RxBuff[8] == 0x01)
		{
			display_wait++;
			Flag_Display = TRUE;
			HAL_UART_Transmit(&huart1,Clear_clear,8,0x0E);
		}
	}
	else
	{
		Flag_Display = FALSE;
	}

	if(Flag_Display)
	{
		//change
//		Long_to_byte(Last_Final_price_x10,temp_array);
		Long_to_byte(Last_fare_d_x100, temp_array);
		HMI_Command(Fee,temp_array,10);

		Long_to_byte(ex_Addition,temp_array);
		HMI_Command(Add_Charge,temp_array,10);

		Long_to_byte(Last_Dis,temp_array);
		HMI_Command(Dis_A,temp_array,10);

		HAL_UART_Transmit(&huart1, Dur_HMS, 14, 0x1C);

		printer_rd();
		check_rd();
		clear_rd();

		if(display_wait != 0)
		{
			display_wait++;
			//display 5s
			if(display_wait >= 6)
			{
				Flag_Display = FALSE;
				display_wait = 0;
			}
		}
	}
	else
	{
//		Long_to_byte(Final_price_x10,temp_array);
		Long_to_byte(fare_d_x100, temp_array);
		HMI_Command(Fee,temp_array,10);

		Long_to_byte(Addition,temp_array);
		HMI_Command(Add_Charge,temp_array,10);

//		counter = 0;
//		ex_counter = 0;

//		last_counter = 0; //pull to start of the function as T-box has no signal
//		Dis = 0;
//		Distance = 0;
//		distance = 0;

		Long_to_byte(Dis,temp_array);
		HMI_Command(Dis_A,temp_array,10);

		sec = 0;
		min = 0;
		hou = 0;
		HAL_UART_Transmit(&huart1, (uint8_t *) Dur_HMS_clear, 14, 0x1C);

		HAL_Delay(2);
	}

	HAL_UART_Transmit(&huart1, (uint8_t *) Plus10_clear, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) Plus1_clear, 8, 0x10);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t *) Print_clear, 8, 0x10);
	HAL_UART_Transmit(&huart1,CLR_extra_btn,8,0x0E);

	IC_Val1 = 0;
	IC_Val2 = 0;
	IC_Val3 = 0;
	IC_Val4 = 0;
	Width = 0;
	Difference = 0;
	//counter = 0;
	//last_counter = 0;
	//Dis = 0;
	//Distance = 0;

	if (Flag_Tbox_Finish)
	{
		Flag_Tbox_Start = TRUE;
		Flag_Tbox_Finish = FALSE;

		Flag_Printer_Start = TRUE;
		Flag_Printer_End = FALSE;

		if(!Flag_End_Record)
		{
			//printed on stop mode,already record the end time
			tbox_finish();
		}
		else
		{
			//get end time
			//end - dd/mm/yy hh:mm
			//hmi_rtc - yy-mm-dd hh:mm:ss
			end[0] = hmi_rtc[14];
			end[1] = hmi_rtc[15];
			end[3] = hmi_rtc[11];
			end[4] = hmi_rtc[12];
			end[6] = hmi_rtc[8];
			end[7] = hmi_rtc[9];
			end[9] = hmi_rtc[17];
			end[10] = hmi_rtc[18];
			end[12] = hmi_rtc[20];
			end[13] = hmi_rtc[21];
			tbox_finish();
		}
		Flag_End_Record = TRUE;
		//calculate trip time (end time - start time)
		E_write();
		//trip data write to sd card
		SD_buffer[34] = 0x32;
		SD_buffer[35] = 0x30;
		SD_buffer[36] = hmi_rtc[8];
		SD_buffer[37] = hmi_rtc[9];
		SD_buffer[38] = 0x2D;
		SD_buffer[39] = hmi_rtc[11];
		SD_buffer[40] = hmi_rtc[12];
		SD_buffer[41] = 0x2D;
		SD_buffer[42] = hmi_rtc[14];
		SD_buffer[43] = hmi_rtc[15];
		SD_buffer[44] = 0x20;
		SD_buffer[45] = hmi_rtc[17];
		SD_buffer[46] = hmi_rtc[18];
		SD_buffer[47] = 0x3A;
		SD_buffer[48] = hmi_rtc[20];
		SD_buffer[49] = hmi_rtc[21];
		SD_buffer[50] = 0x3A;
		SD_buffer[51] = hmi_rtc[23];
		SD_buffer[52] = hmi_rtc[24];
		SD_buffer[53] = 0x20;

		SD_buffer[54] = paid_min[8];
		SD_buffer[55] = paid_min[9];
		SD_buffer[56] = paid_min[10];
		SD_buffer[57] = 0x2E;
		SD_buffer[58] = paid_min[12];
		SD_buffer[59] = paid_min[13];
		SD_buffer[60] = 0x20;

		//journey time
		jtime_d = ((ttime_counter / 60.0)* 1000) / 10; //changed
		jtime[7] = (jtime_d /1 % 10) + 0x30;
		jtime[6] = (jtime_d /10 % 10) + 0x30;
		jtime[5] = (jtime_d /100 % 10) + 0x30;
		jtime[4] = (jtime_d /1000 % 10) + 0x30;
		jtime[3] = (jtime_d /10000 % 10) + 0x30;

		if (jtime[3] == 0x30)
			SD_buffer[61] = 0x20;
		else
			SD_buffer[61] = jtime[3];
		if (jtime[4] == 0x30)
			SD_buffer[62] = 0x20;
		else
			SD_buffer[62] = jtime[4];

		SD_buffer[63] = jtime[5];
		SD_buffer[64] = 0x2E;
		SD_buffer[65] = jtime[6];
		SD_buffer[66] = jtime[7];
		SD_buffer[67] = 0x20;
		ttime_counter = 0;

		SD_buffer[68] = paid_km[8];
		SD_buffer[69] = paid_km[9];
		SD_buffer[70] = paid_km[10];
		SD_buffer[71] = 0x2E;
		SD_buffer[72] = paid_km[12];
		SD_buffer[73] = paid_km[13];
		SD_buffer[74] = 0x20;

		SD_buffer[75] = total_km[8];
		SD_buffer[76] = total_km[9];
		SD_buffer[77] = total_km[10];
		SD_buffer[78] = 0x2E;
		SD_buffer[79] = total_km[12];
		SD_buffer[80] = total_km[13];
		SD_buffer[81] = 0x20;

		SD_buffer[113] = 0x0A;
		fresult = f_open(&fil, "tripDATA.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		fresult = f_lseek(&fil, f_size(&fil));
//		fresult = f_puts("AB1234 A000001 2023-07-18 18:18:30 2023-07-18 18:35:54 4.3 17.4 10.3 10.8 5.6 107.8 5 112.8\n", &fil);
		char const * ch = (const char*)&SD_buffer;
		fresult = f_puts(ch, &fil);
		f_close (&fil);
		clear_buffer();
	}
	Flag_Control_Up();
	flag_sp = 1;
	flag_vs_first = 1;
	Fu = 0;                       //moved to end start to solve T-box clear problem
	Ft = 0;
	Fd = 0;
	Dt = 0;
	DtX = 0;
	Fi = 0; //2.35;
	fare_d_x10 = 0;
	fare_d_x100 = 0;
	fare_d = 0;
	Last_fare_d = 0;
	fare = 0;
	fare_km = 0;
	fare_min = 0;
	Flag_2km = FALSE;
	flag_fare_r = FALSE;
	Final_price = 0;
	Final_price_x10 = 0;
	Addition = 0;
	flag_count = 0;
	change_count = 0;
	fare_km_x = 0;
//	ttime_counter = 0; //move to end
	last_counter = 0;
	Dis = 0;
	Distance = 0;
	distance = 0;				//moved to end start to solve T-box clear problem
}

void extras_rd(void)
{
	Last_Addition = Addition;

	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Plus10, 7, 0x0E);
	HAL_Delay(2);
//	HAL_UART_Transmit(&huart3,RxBuff,12,0x20);
	if(RxFlag)
	{
		RxFlag = FALSE;
		unsigned char buffer1[2] = {0};
		buffer1[0] = RxBuff[7];
		buffer1[1] = RxBuff[8];
		Plus_10 = byte_to_long(buffer1);
		memset(RxBuff,0,12);
		memset(buffer1,0,2);
	}

	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Plus1, 7, 0x0E);
	HAL_Delay(2);
//	HAL_UART_Transmit(&huart3,RxBuff,12,0x20);
	if(RxFlag)
	{
		RxFlag = FALSE;
		unsigned char buffer2[2] = {0};
		buffer2[0] = RxBuff[7];
		buffer2[1] = RxBuff[8];
		Plus_1 = byte_to_long(buffer2);
		memset(RxBuff,0,12);
		memset(buffer2,0,2);
	}

	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Clear, 7, 0x0E);
	HAL_Delay(2);
//	HAL_UART_Transmit(&huart3,RxBuff,12,0x20);
	if(RxFlag)
	{
		RxFlag = FALSE;
		if(RxBuff[8] == 0x01)
		{
//			HAL_UART_Transmit(&huart3,RxBuff,12,0x20);
			HAL_UART_Transmit(&huart1,Clear_clear,8,0x10);
			HAL_UART_Transmit(&huart1, Plus10_clear, 8, 0x10);
			HAL_UART_Transmit(&huart1, Plus1_clear, 8, 0x10);
			Addition = 0;
			Plus_10 = 0;
			Plus_1 = 0;
			//clear print sub[6]-[11]
			sub[6] = 0x20;
			sub[7] = 0x20;
			sub[8] = 0x20;
			sub[9] = 0x20;
			sub[10] = 0x20;
			sub[11] = 0x30;
			//clear print total[4]-[8]
			t_fare[4] = 0x20;
			t_fare[5] = 0x20;
			t_fare[6] = 0x20;
			t_fare[7] = 0x20;
			t_fare[8] = 0x20;
			uint8_t temp_array[4];
			Long_to_byte(Addition,temp_array);
			HMI_Command(Add_Charge,temp_array,10);
		}
	}

	Addition = 10 * (10 * Plus_10 + Plus_1);
	uint8_t temp_array[4];
	Long_to_byte(Addition,temp_array);
	HMI_Command(Add_Charge,temp_array,10);

	if(Last_Addition != Addition)
	{
		Flag_Addition_Add = TRUE;
	}

	if((Addition != 0) && (Flag_Addition_Add == FALSE))
	{
		Flag_Addition_F = TRUE;
	}
}

void printer_rd(void)
{
	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Print, 7, 0x0E);
	HAL_Delay(2);
	if(RxFlag)
	{
		RxFlag = FALSE;
		if(RxBuff[8] == 0x01)
		{
//			HAL_UART_Transmit(&huart3,RxBuff,12,0x20);
			HAL_UART_Transmit(&huart1, (uint8_t *) Print_clear, 8, 0x10);
			Flag_Printed = TRUE;

			//record printer end time
			if((Drv_state == S_STOP) & Flag_Printed & Flag_End_Record)
			{
				Flag_End_Record = FALSE;
				//get end time
				//end - dd/mm/yy hh:mm
				//hmi_rtc - yy-mm-dd hh:mm:ss
				if (flag_vs_first == 1)
				{
					flag_vs_first = 0;
					end[0] = start[0];
					end[1] = start[1];
					end[3] = start[3];
					end[4] = start[4];
					end[6] = start[6];
					end[7] = start[7];
					end[9] = start[9];
					end[10] = start[10];
					end[12] = start[12];
					end[13] = start[13];
				}
				else if (flag_vs == 0)
				{
					//record re-stop start moment
					end[0] = re_stop[0];
					end[1] = re_stop[1];
					end[3] = re_stop[3];
					end[4] = re_stop[4];
					end[6] = re_stop[6];
					end[7] = re_stop[7];
					end[9] = re_stop[9];
					end[10] = re_stop[10];
					end[12] = re_stop[12];
					end[13] = re_stop[13];
				}
				else
				{
					end[0] = hmi_rtc[14];
					end[1] = hmi_rtc[15];
					end[3] = hmi_rtc[11];
					end[4] = hmi_rtc[12];
					end[6] = hmi_rtc[8];
					end[7] = hmi_rtc[9];
					end[9] = hmi_rtc[17];
					end[10] = hmi_rtc[18];
					end[12] = hmi_rtc[20];
					end[13] = hmi_rtc[21];
				}
			}

			printer_process();
			//printer_e(); //test
			Flag_Printed = FALSE;
		}
	}
}

void check_rd(void)
{
	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Plus10, 7, 0x0E);
	HAL_Delay(2);
	if(RxFlag)
	{
		RxFlag = FALSE;
		if(RxBuff[8] == 0x01)
		{
			printer_e(); //test
		}
	}
}

void clear_rd(void)
{
	// clear everyday info
	HAL_UART_Transmit(&huart1, (uint8_t *) RD_Plus1, 7, 0x0E);
	HAL_Delay(2);
	if(RxFlag)
	{
		RxFlag = FALSE;
		if(RxBuff[8] == 0x01)
		{
			cflag_d = 0;
			cfare_d = 0;
			csub_d = 0;

			cflag[0] = 0x30;
			cflag[1] = 0x30;
			cflag[2] = 0x30;
			cflag[3] = 0x30;
			cflag[4] = 0x30;
			cflag[5] = 0x30;
			cflag[6] = 0x30;
			cflag[7] = 0x30;

			cfare[0] = 0x30;
			cfare[1] = 0x30;
			cfare[2] = 0x30;
			cfare[3] = 0x30;
			cfare[4] = 0x30;
			cfare[5] = 0x30;
			cfare[6] = 0x30;
			cfare[7] = 0x30;

			csub[0] = 0x30;
			csub[1] = 0x30;
			csub[2] = 0x30;
			csub[3] = 0x30;
			csub[4] = 0x30;
			csub[5] = 0x30;
			csub[6] = 0x30;
			csub[7] = 0x30;

			E_write();
		}
	}
}

void Long_to_byte(uint32_t longInt, uint8_t* byteArray)
{
	byteArray[0] = (uint8_t)((longInt & 0xFF000000) >> 24 );
	byteArray[1] = (uint8_t)((longInt & 0x00FF0000) >> 16 );
	byteArray[2] = (uint8_t)((longInt & 0x0000FF00) >> 8 );
	byteArray[3] = (uint8_t)((longInt & 0X000000FF));
}

uint32_t byte_to_Long(uint8_t* byteArray)
{
	long LongInt = 0;

	LongInt  = ((unsigned int) byteArray[0]) << 24;
	LongInt |= ((unsigned int) byteArray[1]) << 16;
	LongInt |= ((unsigned int) byteArray[2]) << 8;
	LongInt |= ((unsigned int) byteArray[3]);

	return LongInt;
}

void HMI_Command(const uint8_t* command, uint8_t* data, uint8_t NOB)
{
	char string[15] = {0};

	memset(string,0,15);
	memcpy(string, command, NOB);	// Fee 10 byte
	memcpy(string+6,data,4);
	HMI_Transmit(string,NOB);
}

void HMI_Transmit(char out[], uint8_t nob)
{
	uint8_t temp[20]  = {0};
//	uint16_t length = 0;

	memcpy(temp,out,11);	// debug brake point
//	length = strlen(out);

	HAL_UART_Transmit(&huart1, (uint8_t *) out, nob, 10);
}

uint16_t CRC16(unsigned char *cal, uint16_t len)
{
    int i;
    unsigned short crc = 0xFFFF;
    if (len == 0)
        len = 1;

    while (len--)
    {
        crc ^= *cal;
        for (i = 0; i<8; i++)
        {
            if (crc & 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
        cal++;
    }
    return(crc);
}

unsigned char checksum_8bit(unsigned char *data, int len)
{
	int i;
	unsigned short sum = 0x00;
	for (i=0; i<len; i++)
	{
		sum += data[i];
	}
//	sum &= 0xFF
	return (unsigned char)sum;
}

void long_to_byte(uint16_t longInt, unsigned char* byteArray)
{
	byteArray[0] = (uint8_t)((longInt & 0xFF00) >> 8 );
	byteArray[1] = (uint8_t)((longInt & 0X00FF));
}

uint32_t byte_to_long(uint8_t* byteArray)
{
	long LongInt = 0;

	LongInt  = ((unsigned int) byteArray[0]) << 8;
	LongInt |= ((unsigned int) byteArray[1]);

	return LongInt;
}

void long_to_byte_crc(uint16_t longInt, unsigned char* byteArray)
{
	byteArray[0] = (uint8_t)(longInt & 0x00FF);
	byteArray[1] = (uint8_t)((longInt & 0XFF00) >> 8 );
}

void tbox_start(void)
{
	unsigned char sn[2] = {0};
//	unsigned char data1[6];
	unsigned char cal_s[12] = {0};
	uint16_t crc1_value = 0;
	unsigned char crc1[2] = {0};
//	unsigned char t_start[16];

	//sn
	serial = serial + 1;
	long_to_byte(serial, sn);
	memcpy(hd1, sn, sizeof(unsigned char)*2);

	//data
	RTC_date = PCF2129_getDate();
	year = RTC_date.date.y;
	data1[0] = (uint8_t)(year & 0xFF);
	month = RTC_date.date.m;
	data1[1] = (uint8_t)(month & 0xFF);
	day = RTC_date.date.d;
	data1[2] = (uint8_t)(day & 0xFF);
	hour = RTC_date.date.hh;
	data1[3] = (uint8_t)(hour & 0xFF);
	minute = RTC_date.date.mm;
	data1[4] = (uint8_t)(minute & 0xFF);
	second = RTC_date.date.ss;
	data1[5] = (uint8_t)(second & 0xFF);
	//for crc calculate, cal_s = (header + data)
	memcpy(cal_s, hd1, sizeof(unsigned char)*6);
	memcpy(cal_s+6, data1, sizeof(unsigned char)*6);
	crc1_value = CRC16(cal_s, 12);
	long_to_byte_crc(crc1_value, crc1);

	//update msg
	memcpy(t_start, zone, sizeof(unsigned char)*2);
	memcpy(t_start+2, crc1, sizeof(unsigned char)*2);
	memcpy(t_start+4, hd1, sizeof(unsigned char)*6);
	memcpy(t_start+10, data1, sizeof(unsigned char)*6);

	//transmit
	unsigned char buffer[sizeof(t_start)];
	memset(buffer, 0, sizeof(t_start));
	memcpy(buffer, &t_start, sizeof(t_start));
	HAL_UART_Transmit(&huart4, (uint8_t *)buffer, sizeof(buffer),0xffff);

	//clear
//	memset(t_start, 0, 16);
}

void tbox_finish(void)
{
	unsigned char sn[2] = {0};
//	unsigned char data2[26];
	unsigned char cal_f[32] = {0};
	uint16_t crc2_value = 0;
	unsigned char crc2[2] = {0};
//	unsigned char t_finish[36];

	//sn
	serial = serial + 1;
	long_to_byte(serial, sn);
	memcpy(hd2, sn, sizeof(unsigned char)*2);

	//data
	RTC_date = PCF2129_getDate();
	year = RTC_date.date.y;
	data2[8] = (uint8_t)(year & 0xFF);
	month = RTC_date.date.m;
	data2[9] = (uint8_t)(month & 0xFF);
	day = RTC_date.date.d;
	data2[10] = (uint8_t)(day & 0xFF);
	hour = RTC_date.date.hh;
	data2[11] = (uint8_t)(hour & 0xFF);
	minute = RTC_date.date.mm;
	data2[12] = (uint8_t)(minute & 0xFF);
	second = RTC_date.date.ss;
	data2[13] = (uint8_t)(second & 0xFF);

	//for crc calculate, cal_f = (header + data)
	memcpy(cal_f, hd2, sizeof(unsigned char)*6);
	memcpy(cal_f+6, data2, sizeof(unsigned char)*26);
	crc2_value = CRC16(cal_f, 32);
	long_to_byte_crc(crc2_value, crc2);

	//update msg
	memcpy(t_finish, zone, sizeof(unsigned char)*2);
	memcpy(t_finish+2, crc2, sizeof(unsigned char)*2);
	memcpy(t_finish+4, hd2, sizeof(unsigned char)*6);
	memcpy(t_finish+10, data2, sizeof(unsigned char)*26);

	//transmit
	unsigned char buffer[sizeof(t_finish)];
	memset(buffer, 0, sizeof(t_finish));
	memcpy(buffer, &t_finish, sizeof(t_finish));
	HAL_UART_Transmit(&huart4, (uint8_t *)buffer, sizeof(buffer),0xffff);

	//clear
//	memset(t_finish, 0, 36);
}

void printer_process(void)
{
	//row_init: space + enter
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) uTx_Data, sizeof(uTx_Data), 0xffff);
	//row1: taxi no.
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg01, 9);
	memcpy(receipt1.en, msg02, 9);
	memcpy(receipt1.va, plate, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row2: start
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg03, 9);
	memcpy(receipt1.en, msg04, 9);
	memcpy(receipt1.va, start, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row3: end
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg05, 9);
	memcpy(receipt1.en, msg06, 9);
	memcpy(receipt1.va, end, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row4: total_km
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg07, 9);
	memcpy(receipt1.en, msg08, 9);
	memcpy(receipt1.va, total_km, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row5: paid_km
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg09, 9);
	memcpy(receipt1.en, msg10, 9);
	memcpy(receipt1.va, paid_km, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row6: paid_min
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg11, 9);
	memcpy(receipt1.en, msg12, 9);
	memcpy(receipt1.va, paid_min, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row7: surcharge
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg13, 9);
	memcpy(receipt1.en, msg14, 9);
	memcpy(receipt1.va, sub, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row8: total_fare
	memcpy(receipt2.sp, cmd, 6);
	memcpy(receipt2.ch, msg15, 9);
	memcpy(receipt2.en, msg16, 10);
	memcpy(receipt2.va, t_fare, 13);
	memcpy(receipt2.cmd, enter,2);
	printer_transmit2(&receipt2, &hlpuart1);
	//row_end: thank you
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) enter, sizeof(enter), 0xffff);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) msg17, sizeof(msg17), 0xffff);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) msg18, sizeof(msg18), 0xffff);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) uTx_Data, sizeof(uTx_Data), 0xffff);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) uTx_Data, sizeof(uTx_Data), 0xffff);
}

void printer_e(void)
{
	E_read();
	//row_init: space + enter
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) uTx_Data, sizeof(uTx_Data), 0xffff);
	//row1: taxi no.
	memcpy(receipt1.sp, cmd, 6);
	memcpy(receipt1.ch, msg01, 9);
	memcpy(receipt1.en, msg02, 9);
	memcpy(receipt1.va, plate, 14);
	memcpy(receipt1.cmd, enter,2);
	printer_transmit(&receipt1, &hlpuart1);
	//row2: total km
	if (tkm[0] == 0x30)
		data01[0] = 0x20;
	else
		data01[0] = tkm[0];

	for (int i=1; i<=4; i++)
	{
		if (tkm[i] == 0x30 && tkm[i-1] == 0x30)
			data01[i] = 0x20;
		else
			data01[i] = tkm[i];
	}
	data01[5] = tkm[5];
	data01[7] = tkm[6];
	data01[8] = tkm[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg01, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data01, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row3: paid km
	if (pkm[0] == 0x30)
		data02[0] = 0x20;
	else
		data02[0] = pkm[0];

	for (int i=1; i<=4; i++)
	{
		if (pkm[i] == 0x30 && pkm[i-1] == 0x30)
			data02[i] = 0x20;
		else
			data02[i] = pkm[i];
	}
	data02[5] = pkm[5];
	data02[7] = pkm[6];
	data02[8] = pkm[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg02, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data02, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row4: total flags
	if (tflag[2] == 0x30)
		data03[3] = 0x20;
	else
		data03[3] = tflag[2];

	for (int i=3; i<=6; i++)
	{
		if (tflag[i] == 0x30 && tflag[i-1] == 0x30)
			data03[i+1] = 0x20;
		else
			data03[i+1] = tflag[i];
	}
	data03[8] = tflag[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg03, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data03, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row5: current flags
	if (cflag[2] == 0x30)
		data04[3] = 0x20;
	else
		data04[3] = cflag[2];

	for (int i=3; i<=6; i++)
	{
		if (cflag[i] == 0x30 && cflag[i-1] == 0x30)
			data04[i+1] = 0x20;
		else
			data04[i+1] = cflag[i];
	}
	data04[8] = cflag[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg04, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data04, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row6: fare changes
	if (tpulse[2] == 0x30)
		data05[3] = 0x20;
	else
		data05[3] = tpulse[2];

	for (int i=3; i<=6; i++)
	{
		if (tpulse[i] == 0x30 && tpulse[i-1] == 0x30)
			data05[i+1] = 0x20;
		else
			data05[i+1] = tpulse[i];
	}
	data05[8] = tpulse[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg05, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data05, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row7: total fare
	if (tfare[1] == 0x30)
		data06[0] = 0x20;
	else
		data06[0] = tfare[1];

	if (tfare[2] == 0x30 && tfare[1] == 0x30)
		data06[1] = 0x20;
	else
		data06[1] = tfare[2];

	if (tfare[3] == 0x30 && tfare[2] == 0x30)
		data06[2] = 0x20;
	else
		data06[2] = tfare[3];

	if (tfare[4] == 0x30 && tfare[3] == 0x30)
		data06[3] = 0x20;
	else
		data06[3] = tfare[4];

	data06[4] = tfare[5];
	data06[5] = tfare[6];
	data06[7] = tfare[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg06, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data06, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row8: total sub
	if (tsub[1] == 0x30)
		data07[0] = 0x20;
	else
		data07[0] = tsub[1];

	if (tsub[2] == 0x30 && tsub[1] == 0x30)
		data07[1] = 0x20;
	else
		data07[1] = tsub[2];

	if (tsub[3] == 0x30 && tsub[2] == 0x30)
		data07[2] = 0x20;
	else
		data07[2] = tsub[3];

	if (tsub[4] == 0x30 && tsub[3] == 0x30)
		data07[3] = 0x20;
	else
		data07[3] = tsub[4];

	data07[4] = tsub[5];
	data07[5] = tsub[6];
	data07[7] = tsub[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg07, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data07, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row9: current total fare
	if (cfare[3] == 0x30)
		data08[2] = 0x20;
	else
		data08[2] = cfare[3];

	if (cfare[4] == 0x30 && cfare[3] == 0x30)
		data08[3] = 0x20;
	else
		data08[3] = cfare[4];

	data08[4] = cfare[5];
	data08[5] = cfare[6];
	data08[7] = cfare[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg08, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data08, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row10: current total sub
	if (csub[3] == 0x30)
		data09[2] = 0x20;
	else
		data09[2] = csub[3];

	if (csub[4] == 0x30 && csub[3] == 0x30)
		data09[3] = 0x20;
	else
		data09[3] = csub[4];

	data09[4] = csub[5];
	data09[5] = csub[6];
	data09[7] = csub[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg09, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data09, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row11: total time
	if (ttime[2] == 0x30)
		data10[2] = 0x20;
	else
		data10[2] = ttime[2];

	if (ttime[3] == 0x30 && ttime[2] == 0x30)
		data10[3] = 0x20;
	else
		data10[3] = ttime[3];

	if (ttime[4] == 0x30 && ttime[3] == 0x30)
		data10[4] = 0x20;
	else
		data10[4] = ttime[4];

	data10[5] = ttime[5];
	data10[7] = ttime[6];
	data10[8] = ttime[7];
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, tmsg10, 23, 0xffff);
	HAL_UART_Transmit(&hlpuart1, data10, 9, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row12: pulses
//	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
//	HAL_UART_Transmit(&hlpuart1, tmsg11, 23, 0xffff);
//	HAL_UART_Transmit(&hlpuart1, data11, 9, 0xffff);
//	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	//row13: (chi)print time: 04/05/23 11:15
	dtdata[10] = hmi_rtc[14];
	dtdata[11] = hmi_rtc[15];
	dtdata[13] = hmi_rtc[11];
	dtdata[14] = hmi_rtc[12];
	dtdata[16] = hmi_rtc[8];
	dtdata[17] = hmi_rtc[9];
	dtdata[19] = hmi_rtc[17];
	dtdata[20] = hmi_rtc[18];
	dtdata[22] = hmi_rtc[20];
	dtdata[23] = hmi_rtc[21];

	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);
	HAL_UART_Transmit(&hlpuart1, cmd, 6, 0xffff);
	HAL_UART_Transmit(&hlpuart1, dtdata, 24, 0xffff);
	HAL_UART_Transmit(&hlpuart1, enter, 2, 0xffff);

	HAL_UART_Transmit(&hlpuart1, (uint8_t *) uTx_Data, sizeof(uTx_Data), 0xffff);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) uTx_Data, sizeof(uTx_Data), 0xffff);
}

void printer_transmit(struct print1 *data, UART_HandleTypeDef *huart)
{
	unsigned char buffer[sizeof(struct print1)];
	memset(buffer, 0, sizeof(struct print1));
	memcpy(buffer, data, sizeof(struct print1));

//	memcpy(buffer, &data, sizeof data);

	HAL_UART_Transmit(&hlpuart1, (uint8_t *) buffer, sizeof(buffer), 0xffff);
}

void printer_transmit2(struct print2 *data, UART_HandleTypeDef *huart)
{
	unsigned char buffer[sizeof(struct print2)];
	memset(buffer, 0, sizeof(struct print2));
	memcpy(buffer, data, sizeof(struct print2));

//	memcpy(buffer, &data, sizeof data);

	HAL_UART_Transmit(&hlpuart1, (uint8_t *) buffer, sizeof(buffer), 0xffff);
}

//write data to EEPROM

void eeprom_w(void)
{
	if(HAL_I2C_Mem_Write(&hi2c2, ADDR_EEPROM_Write, 0, I2C_MEMADD_SIZE_16BIT,&(WriteBuffer[0]),BufferSize, 1000) == HAL_OK)
	{
		HAL_Delay(10);
	}
	else
	{
		//test - return eeprom write data
		//HAL_UART_Transmit(&huart2,WriteBuffer,BufferSize,0x200);
		Flag_test1 = TRUE;
		HAL_Delay(10);
	}
}

//read data from EEPROM & check
void eeprom_check(void)
{
	HAL_I2C_Mem_Read(&hi2c2, ADDR_EEPROM_Read, 0, I2C_MEMADD_SIZE_16BIT,ReadBuffer,BufferSize, 1000);
	if(memcmp(WriteBuffer,ReadBuffer,BufferSize) == 0 )
	{
		HAL_Delay(10);
	}
	else
	{
		//test - return read buffer
		//HAL_UART_Transmit(&huart2,ReadBuffer,BufferSize,0x200);
		Flag_test2 = TRUE;
		HAL_Delay(10);
	}
}

void eeprom_r(void)
{
	HAL_I2C_Mem_Read(&hi2c2, ADDR_EEPROM_Read, 0, I2C_MEMADD_SIZE_16BIT,ReadBuffer,BufferSize, 1000);

	unsigned char Dist_Fkm[4] = {0};
	unsigned char Fa_amt[2] = {0};
	unsigned char Fa_2km[2] = {0};
	unsigned char Fa_200m1[2] = {0};
	unsigned char Fa_200m2[2] = {0};
//	unsigned char i_constant_k[2];
	unsigned char constant_k[2] = {0};
	unsigned char Sp_Co[2] = {0};
	unsigned char Sp_Max[2] = {0};
	//unsigned char Type[2]; - move to global
	//unsigned char plate_no[10]; - move to global

	//rtc set time [6] - rtc_set()
	//HW ver. [2]
	//FW ver. [2]
	//total = 64 (AABB + 59 + reserve[2] + FF)

	//testing - check first few bytes (2 or 4 bytes?), if they are all 0xff, then the EEPROM is empty
	/*
	if (ReadBuffer[0] == 0xFF && ReadBuffer[1] == 0xFF)
	{
		memcpy(ReadBuffer, WriteBuffer, BufferSize);
	}
	*/

	//0 - header AA BB

	//1 - ReadBuffer[0][1][2][3] - 2km
	Dist_Fkm[0] = ReadBuffer[0];
	Dist_Fkm[1] = ReadBuffer[1];
	Dist_Fkm[2] = ReadBuffer[2];
	Dist_Fkm[3] = ReadBuffer[3];
	//first 2km, not used yet
	memset(Dist_Fkm, 0, 4);

	//2 - ReadBuffer[4][5] - Fa_amt, 935, 745, 1750
	Fa_amt[0] = ReadBuffer[4];
	Fa_amt[1] = ReadBuffer[5];
	Fare_reached = (float) byte_to_long(Fa_amt) / 10;
	memset(Fa_amt, 0, 2);

	//3 - ReadBuffer[6][7] - Fa_2km, 270, 235, 220
	Fa_2km[0] = ReadBuffer[6];
	Fa_2km[1] = ReadBuffer[7];
	Fare_2km = (float) byte_to_long(Fa_2km) / 10;
	//Fare_200m0 = Fare_2km / 10 (2.7, 2.35, 2.2), C_Fare_Min = Fare_2km * 10 (2700, 2350, 2200)
	Fare_200m0 = (float) byte_to_long(Fa_2km) / 100;
	Fare_min = byte_to_long(Fa_2km) * 10;
	memset(Fa_2km, 0, 2);

	//4 - ReadBuffer[8][9] - Fa_200m1, 19, 17, 17
	Fa_200m1[0] = ReadBuffer[8];
	Fa_200m1[1] = ReadBuffer[9];
	Fare_200m1 = (float) byte_to_long(Fa_200m1) / 10;
	//Fare_200m1 -> T1
	memset(Fa_200m1, 0, 2);

	//5 - ReadBuffer[10][11] - Fa_200m2, 13, 13, 15
	Fa_200m2[0] = ReadBuffer[10];
	Fa_200m2[1] = ReadBuffer[11];
	Fare_200m2 = (float) byte_to_long(Fa_200m2) / 10;
	//Fare_200m2 -> T2
	memset(Fa_200m2, 0, 2);

	//6 - ReadBuffer[12][13] - k, 1000
	constant_k[0] = ReadBuffer[12];
	constant_k[1] = ReadBuffer[13];
	ck_r = byte_to_long(constant_k);
	memset(constant_k, 0, 2);

	//7 - ReadBuffer[14][15] - Sp_Co
	Sp_Co[0] = ReadBuffer[14];
	Sp_Co[1] = ReadBuffer[15];
	Speed_co = byte_to_long(Sp_Co);
	//Speed_co -> 12km/h
	memset(Sp_Co, 0, 2);

	//8 - ReadBuffer[16][17] - Sp_Max
	Sp_Max[0] = ReadBuffer[16];
	Sp_Max[1] = ReadBuffer[17];
	Speed_max = byte_to_long(Sp_Max);
	//Speed_max -> 150km/h
	memset(Sp_Max, 0, 2);

	//9 - ReadBuffer[18][19] - Type of Taxi
	Type[0] = ReadBuffer[18];
	Type[1] = ReadBuffer[19];
	//taxiType -> 00 01 02 03 -> init urban nt lantau
	//no need to transfer, will use in parameter reply
	//memset(Type, 0, 2);

	//10 - ReadBuffer[20][21][22][23]24][25][26][27][28][29] - plate no.
	plate_no[0] = ReadBuffer[20];
	plate_no[1] = ReadBuffer[21];
	plate_no[2] = ReadBuffer[22];
	plate_no[3] = ReadBuffer[23];
	plate_no[4] = ReadBuffer[24];
	plate_no[5] = ReadBuffer[25];
	plate_no[6] = ReadBuffer[26];
	plate_no[7] = ReadBuffer[27];
	plate_no[8] = ReadBuffer[28];
	plate_no[9] = ReadBuffer[29];
	//QR code
	//need to remove space - 0x20 for QR CODE use
	//new variable plate_qr[] = {0x5A, 0xA5, length, 0x82, 0x10, 0x80, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF};
	//check if 0x20
	//then calculate length
	plate_qr[6] = ReadBuffer[24];
	plate_qr[7] = ReadBuffer[25];
	plate_qr[8] = ReadBuffer[26];
	plate_qr[9] = ReadBuffer[27];
	plate_qr[10] = ReadBuffer[28];
	plate_qr[11] = ReadBuffer[29];

	//printer -> print the receipt / tbox will use set plate no. in web config
	plate[13] = plate_no[9];
	plate[12] = plate_no[8];
	plate[11] = plate_no[7];
	plate[10] = plate_no[6];
	plate[9] = plate_no[5];
	plate[8] = plate_no[4];
	plate[7] = plate_no[3];
	plate[6] = plate_no[2];
	plate[5] = plate_no[1];
	plate[4] = plate_no[0];
	//parameter reply -> directly reply plate_no[] in Replybuffer[]
	//memset(plate_no, 0, 10);

	//16 - stop bytes FF
}

//set time & date
void rtc_set(void)
{
	//read from eeprom for first time set

	PCF2129_Init(hi2c1, RTC_WRITE_ADDR);
	PCF2129_configure();                         ///Set 24hr Format

	PCF2129_setDate(ReadBuffer[30], ReadBuffer[31], ReadBuffer[32], ReadBuffer[33], ReadBuffer[34], ReadBuffer[35]);
}

//read time & date
//transfer to uart
void time_update(void)
{
	unsigned char r_buffer[9] = {0};
	unsigned char u_buffer[6] = {0};

	RTC_date = PCF2129_getDate();

	year = RTC_date.date.y;
	r_buffer[0] = (uint8_t)(year & 0xFF);

	month = RTC_date.date.m;
	r_buffer[1] = (uint8_t)(month & 0xFF);

	day = RTC_date.date.d;
	r_buffer[2] = (uint8_t)(day & 0xFF);

//	weekday = RTC_date.date.w;
//	buffer[3] = (uint8_t)(weekday & 0xFF);

	hour = RTC_date.date.hh;
	r_buffer[3] = (uint8_t)(hour & 0xFF);

	minute = RTC_date.date.mm;
	r_buffer[4] = (uint8_t)(minute & 0xFF);

	second = RTC_date.date.ss;
	r_buffer[5] = (uint8_t)(second & 0xFF);

	ctrl_1 = PCF2129_read(PCF2129_REG_CONTROL1);
	ctrl_2 = PCF2129_read(PCF2129_REG_CONTROL2);
	ctrl_3 = PCF2129_read(PCF2129_REG_CONTROL3);

	hh = PCF2129_read(PCF2129_REG_ALARM_HOUR);
	mm = PCF2129_read(PCF2129_REG_ALARM_MINUTE);
	ss = PCF2129_read(PCF2129_REG_ALARM_SECOND);

	PCF2129_write(PCF2129_REG_CONTROL1, 0x00);
//	PCF2129_write(PCF2129_REG_CONTROL2, 0x00);  //to unlock
	PCF2129_write(PCF2129_REG_CONTROL3, 0x00);

	//check AIE if power off reset all flags
	if ((ctrl_2 & 0x02) == 0x02)
	{
		//if AIE set, continue anti-tamper mode, only show unlock time
		flag_alarm_clear = TRUE;
		EventDebugLog("AIE if 0x02 Before Flag_Protect");
		Flag_Protect = TRUE;
		EventDebugLog("AIE if 0x02 After Flag_Protect");
	}

	//Flag_set_alarm_rtc - this flag will set after 30s alarm, this if will calculate 1 hour count down
	if(Flag_set_alarm_rtc)
	{
		Flag_set_alarm_rtc = FALSE;
		//current time to unix TimeStamp
		stCurrentTime.year = bcdToDec(RTC_date.date.y) + 2000;
		stCurrentTime.month = bcdToDec(RTC_date.date.m);
		stCurrentTime.date = bcdToDec(RTC_date.date.d);
		stCurrentTime.hour = bcdToDec(RTC_date.date.hh);
		stCurrentTime.minute = bcdToDec(RTC_date.date.mm);
		stCurrentTime.second = bcdToDec(RTC_date.date.ss);

		u32UnixTimeStamp = utcToUnix(&stCurrentTime);

		//calculate alarm time - TimeStamp + alarm time(1 hour)
		u32UnixTimeStamp += 3600; //test 1 min (60); //5 mins (300) //set 3600 for 1 hour

		//write to pcf2129
		unixToUtc(u32UnixTimeStamp);

		//change to bcd format
		u_buffer[0] = decToBcd((UtcTime.year-2000));
		u_buffer[1] = decToBcd(UtcTime.month);
		u_buffer[2] = decToBcd(UtcTime.date);
		u_buffer[3] = decToBcd(UtcTime.hour);
		u_buffer[4] = decToBcd(UtcTime.minute);
		u_buffer[5] = decToBcd(UtcTime.second);

		//Leo&Edwin: Add log to SD card to record down the things to write in RCF2129.
				//log==> current time+setalarm+ u_buffer[3]:u_buffer[4]:u_buffer[5]
		fresult = f_open(&fil, "LockLog.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		if (fresult == FR_NO_FILE) {
		    // Create the file
		    fresult = f_open(&fil, "LockLog.txt", FA_CREATE_ALWAYS | FA_WRITE);

		    // Check if the file was created successfully
		    if (fresult == FR_OK) {
		        // Optionally write initial data to the file
		        const char *initialData = "Smart Taximeter Lock Log \n";
		        UINT bytesWritten;
		        f_write(&fil, initialData, strlen(initialData), &bytesWritten);
		    }
		}
		fresult = f_lseek(&fil, f_size(&fil));
		char buffer2[200]  = {0};
		snprintf(buffer2, sizeof(buffer2), "\nBefore writing Alarm :\n Locking Time(ymdhms):%d-%d-%d %d:%d:%d,\n Flag_set_alarm_rtc: %d,\n u_buffer[3](dec): %d,\n u_buffer[4](dec): %d,\n u_buffer[5](dec): %d,\n ctrl_2_before: %d,\n",stCurrentTime.year ,stCurrentTime.month,stCurrentTime.date, stCurrentTime.hour, stCurrentTime.minute, stCurrentTime.second, Flag_set_alarm_rtc, bcdToDec(u_buffer[3]), bcdToDec(u_buffer[4]),bcdToDec(u_buffer[5]),PCF2129_read(PCF2129_REG_CONTROL2));
		fresult = f_puts(buffer2, &fil);
		f_close (&fil);
		clear_buffer();

		//write to alarm registers
		PCF2129_write(PCF2129_REG_ALARM_SECOND, u_buffer[5]);
		PCF2129_write(PCF2129_REG_ALARM_MINUTE, u_buffer[4]);
		PCF2129_write(PCF2129_REG_ALARM_HOUR, u_buffer[3]);

		//AIE
		PCF2129_write(PCF2129_REG_CONTROL2, 0x02);

		//PCF2129_read    // Add by Leo & Edwin
		//log==> current time+ reread alarm+ hh_read[3]:mm_read[4]:ss_read[5]
		fresult = f_open(&fil, "LockLog.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);//open
		fresult = f_lseek(&fil, f_size(&fil));
		char buffer1[200]  = {0};
		snprintf(buffer1, sizeof(buffer1), "\nAfter writing Alarm :\n Written Time(ymdhms):%d-%d-%d %d:%d:%d,\n Flag_set_alarm_rtc: %d,\n Alarm_hr: %d,\n Alarm_min: %d,\n Alarm_sec: %d,\n ctrl_2_after: %d,\n ",stCurrentTime.year ,stCurrentTime.month,stCurrentTime.date, stCurrentTime.hour, stCurrentTime.minute, stCurrentTime.second, Flag_set_alarm_rtc, bcdToDec(PCF2129_read(PCF2129_REG_ALARM_HOUR)), bcdToDec(PCF2129_read(PCF2129_REG_ALARM_MINUTE)),bcdToDec(PCF2129_read(PCF2129_REG_ALARM_SECOND)),PCF2129_read(PCF2129_REG_CONTROL2));
		fresult = f_puts(buffer1, &fil);
		f_close (&fil);
		clear_buffer();
	}
	//trigger the alarm - ~INT = 0 (PA11)
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0)
	{
		flag_alarm_clear = FALSE;

		EventDebugLog("Unlock Meter Before Flag_Protect modified"); //Added by Keven Recommand

		Flag_Protect = FALSE;

		EventDebugLog("Unlock Meter After Flag_Protect modified");

		Flag_alarm = TRUE;
		//PCF2129_write(PCF2129_REG_CONTROL1, 0x00);
		PCF2129_write(PCF2129_REG_CONTROL2, 0x00);

//		unsigned char unlock_clear[] = {0x5A, 0xA5, 0x09, 0x82, 0x10, 0x32, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
		HAL_UART_Transmit(&huart1, unlock_clear, 12, 0x20);
//		unsigned char C_Fare_clear[] = {0x5A, 0xA5, 0x04, 0x82, 0x10, 0x30, 0x20};
		HAL_UART_Transmit(&huart1, C_Fare_clear, 7, 0x20);

		//Drv_state = S_VACANT;
		//HMI_Vacant_W();
		//Flag_H = FALSE;
		//Flag_S = FALSE;
		//Flag_V = TRUE;
		HAL_UART_Transmit(&huart1, (uint8_t *) LED_Hired_Off, 8, 0x10);
		HAL_UART_Transmit(&huart1, (uint8_t *) LED_Stop_Off, 8, 0x10);
		HAL_UART_Transmit(&huart1, (uint8_t *) LED_Vacant_On, 8, 0x10);
		Drv_state = S_VACANT;
		Width = 0;
		Speed = 0;
//		Speed_x = 0;
	}
//	else
//	{
//		flag_alarm_clear = FALSE;
//	}
	//test code - rtc alarm function

	unsigned char rtc_buffer[18] = {0};
	BCD2ASC(rtc_buffer, r_buffer, 18);

//	HAL_UART_Transmit(&huart2, rtc_buffer, 12, 0x20);
//	HAL_UART_Transmit(&huart2, rtc_buffer, 14, 0x20);

	hmi_rtc[8] = rtc_buffer[0];
	hmi_rtc[9] = rtc_buffer[1];

	hmi_rtc[11] = rtc_buffer[2];
	hmi_rtc[12] = rtc_buffer[3];

	hmi_rtc[14] = rtc_buffer[4];
	hmi_rtc[15] = rtc_buffer[5];

	hmi_rtc[17] = rtc_buffer[6];
	hmi_rtc[18] = rtc_buffer[7];

	hmi_rtc[20] = rtc_buffer[8];
	hmi_rtc[21] = rtc_buffer[9];

	hmi_rtc[23] = rtc_buffer[10];
	hmi_rtc[24] = rtc_buffer[11];

	HAL_UART_Transmit(&huart1, hmi_rtc, 25, 0x20);
}



void DebugLog(void)
{
	#ifdef DEBUG
	stCurrentTime.year = bcdToDec(RTC_date.date.y) + 2000;
	stCurrentTime.month = bcdToDec(RTC_date.date.m);
	stCurrentTime.date = bcdToDec(RTC_date.date.d);
	stCurrentTime.hour = bcdToDec(RTC_date.date.hh);
	stCurrentTime.minute = bcdToDec(RTC_date.date.mm);
	stCurrentTime.second = bcdToDec(RTC_date.date.ss);
	if (stCurrentTime.second%3==0){
	fresult = f_open(&fil, "DebugLog.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
	if (fresult == FR_NO_FILE) {
	    // Create the file
	    fresult = f_open(&fil, "DebugLog.txt", FA_CREATE_ALWAYS | FA_WRITE);

	    // Check if the file was created successfully
	    if (fresult == FR_OK) {
	        // Optionally write initial data to the file
	        const char *initialData = "Smart Taximeter Debug Log \n";
	        UINT bytesWritten;
	        f_write(&fil, initialData, strlen(initialData), &bytesWritten);
	    }
	}

	fresult = f_lseek(&fil, f_size(&fil));
	char buffer3[300];
	snprintf(buffer3, sizeof(buffer3), "\nLog Time(ymdhms):%d-%d-%d %d:%d:%d  Flag_set_alarm_rtc: %d  hh:mm:ss: %d:%d:%d  ctrl_2: %d  Flag_Protect: %d  Flag_Alarm: %d  Tamper_Counter: %d  counter: %d  last_counter: %d  Speed: %d  Width: %d",stCurrentTime.year ,stCurrentTime.month,stCurrentTime.date, stCurrentTime.hour, stCurrentTime.minute, stCurrentTime.second, Flag_set_alarm_rtc, bcdToDec(hh),bcdToDec(mm),bcdToDec(ss),ctrl_2,Flag_Protect,Flag_alarm,Tamper_Counter,counter,last_counter,Speed,Width);
	fresult = f_puts(buffer3, &fil);
	f_close (&fil);
	clear_buffer();
	}
	#endif
}

void EventDebugLog(char *str)
{
//	#ifdef DEBUG
	stCurrentTime.year = bcdToDec(RTC_date.date.y) + 2000;
	stCurrentTime.month = bcdToDec(RTC_date.date.m);
	stCurrentTime.date = bcdToDec(RTC_date.date.d);
	stCurrentTime.hour = bcdToDec(RTC_date.date.hh);
	stCurrentTime.minute = bcdToDec(RTC_date.date.mm);
	stCurrentTime.second = bcdToDec(RTC_date.date.ss);
//	if (stCurrentTime.second%3==0){
	fresult = f_open(&fil, "DebugLog.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
	if (fresult == FR_NO_FILE) {
	    // Create the file
	    fresult = f_open(&fil, "DebugLog.txt", FA_CREATE_ALWAYS | FA_WRITE);

	    // Check if the file was created successfully
	    if (fresult == FR_OK) {
	        // Optionally write initial data to the file
	        const char *initialData = "Smart Taximeter Debug Log \n";
	        UINT bytesWritten;
	        f_write(&fil, initialData, strlen(initialData), &bytesWritten);
	    }
	}

	fresult = f_lseek(&fil, f_size(&fil));
	char buffer3[300]  = {0};
	snprintf(buffer3, sizeof(buffer3), "\nEvent: %s  Log Time(ymdhms):%d-%d-%d %d:%d:%d  Flag_set_alarm_rtc: %d  hh:mm:ss: %d:%d:%d  ctrl_2: %d  Flag_Protect: %d  Flag_Alarm: %d  Tamper_Counter: %d  counter: %d  last_counter: %d  Speed: %d  Width: %d",str,stCurrentTime.year ,stCurrentTime.month,stCurrentTime.date, stCurrentTime.hour, stCurrentTime.minute, stCurrentTime.second, Flag_set_alarm_rtc, bcdToDec(hh),bcdToDec(mm),bcdToDec(ss),ctrl_2,Flag_Protect,Flag_alarm,Tamper_Counter,counter,last_counter,Speed,Width);
	fresult = f_puts(buffer3, &fil);
	f_close (&fil);
	clear_buffer();
//	}
//	#endif
}


void BCD2ASC(uint8_t *asc, const uint8_t *bcd, uint32_t len)
{
	uint8_t c = 0;
	uint8_t i;
	static uint8_t bcd2ascii[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	for (i = 0; i < len; i++)
	{
		//first bcd
		c = *bcd >> 4;
		*asc++ = bcd2ascii[c];

		//second bcd
		c = *bcd & 0x0F;
		*asc++ = bcd2ascii[c];
		bcd++;
	}
}

unsigned char BCD2Dec(unsigned char data)
{
	unsigned char temp  = {0};
	temp = ((data >> 4)*10 + (data & 0x0F));
	return temp;
}

unsigned char Dec2BCD(unsigned char data)
{
	unsigned char temp = {0};
	temp = (((data/10) << 4) + (data%10));
	return temp;
}

void Dur_hms_cal()
{
	sec++;
	if(sec == 60)
	{
		min++;
		sec = 0;
	}
	if(min == 60)
	{
		hou++;
		min = 0;
	}
//	if(hou == 24)
//	{
//		hou = 0;
//	}
	//display limit - duration 99:59:59
	if(hou >= 99)
	{
		hou = 99;
		min = 59;
		sec = 59;
	}

	sec_1 = (sec / 1) % 10 + 0x30;
	sec_10 = (sec / 10) % 10 + 0x30;
	min_1 = (min / 1) % 10 + 0x30;
	min_10 = (min / 10) % 10 + 0x30;
	hou_1 = (hou / 1) % 10 + 0x30;
	hou_10 = (hou / 10) % 10 + 0x30;

	Dur_HMS[13] = sec_1;
	Dur_HMS[12] = sec_10;
	Dur_HMS[10] = min_1;
	Dur_HMS[9] = min_10;
	Dur_HMS[7] = hou_1;
	Dur_HMS[6] = hou_10;

	HAL_UART_Transmit(&huart1, Dur_HMS, 14, 0x20);
}

void Read_Pulse(void)
{
	//change Width_min from 6000 to 2000
	if ((Width != 0) && (Width >= 10000 || Width <= 2000))
	{
		last_pulse = 1;
	}

	if ((Width != 0) && (Width >= 10000 || Width <= 2000) && (last_pulse == 1))
	{
		Tamper_Counter++;
	}
	else if (Speed >= Speed_max) //150
	{
		Tamper_Counter++;
	}
	else if (flag_tester && (Speed >= 120))
	{
		Tamper_Counter++;
	}
	else
	{
		last_pulse = 0;
		Tamper_Counter = 0;
	}
}

void Read_Distance(void)
{
	Distance = (float) (counter * 1.0 / ck);
	distance = (float) (counter * 1000.0 / ck);
}

void Read_Speed(void)
{
	if(Difference != 0)
	{
//		Speed_x = ((float) (1000000 / ck) / Difference) * 3.6;
//		Difference = 0;
		Speed = ((float) (counter - last_counter) * 3.6);
		Difference = 0;
	}
	else
	{
		Speed = 0;
//		Speed_x = 0;
	}
}

void Fare_Calculation(void)
{
	//anti-tampering check
	Read_Pulse();
	Read_Distance();
	Read_Speed();

	if (Tamper_Counter >= 5)
	{
		//record tamper data to SD card
		//TAMPER
		SD_buffer[34] = 0x54;
		SD_buffer[35] = 0x41;
		SD_buffer[36] = 0x4D;
		SD_buffer[37] = 0x50;
		SD_buffer[38] = 0x45;
		SD_buffer[39] = 0x52;
		SD_buffer[40] = 0x20;
		//speed & width
		speed_d = (Speed * 100) / 10;
		Dec2ASC(speed_d, speed_a);
		if (speed_a[4] == 0)
			SD_buffer[41] = 0x20;
		else
			SD_buffer[41] = speed_a[4];
		if (speed_a[5] == 0)
			SD_buffer[42] = 0x20;
		else
			SD_buffer[42] = speed_a[5];
		if (speed_a[6] == 0)
			SD_buffer[43] = 0x30;
		else
			SD_buffer[43] = speed_a[6];
		SD_buffer[44] = 0x2E;
		SD_buffer[45] = speed_a[7];
		SD_buffer[46] = 0x20;

		Dec2ASC(Width, width_a);
		if (width_a[3] == 0)
			SD_buffer[47] = 0x20;
		else
			SD_buffer[47] = width_a[3];
		SD_buffer[48] = width_a[4];
		SD_buffer[49] = width_a[5];
		SD_buffer[50] = width_a[6];
		SD_buffer[51] = width_a[7];
		SD_buffer[52] = 0x20;

		SD_buffer[53] = 0x0A;
		fresult = f_open(&fil, "tripDATA.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		fresult = f_lseek(&fil, f_size(&fil));
//		fresult = f_puts("AB1234 A000001 2023-07-18 18:18:30 TAMPER 180.0 15000\n", &fil);
		char const * tp = (const char*)&SD_buffer;
		fresult = f_puts(tp, &fil);
		f_close (&fil);
		clear_buffer();

		Width = 0;
		Speed = 0;
//		Speed_x = 0;
		Tamper_Counter = 0;

		EventDebugLog("Before Normal Lock in Fare Calculation");
		Flag_Protect = TRUE;
		EventDebugLog("After Normal Lock in Fare Calculation");
	}

	if (fare > Fare_reached || fabs(fare - Fare_reached) <= 1e-3)
		Fi = Fare_200m2; //T2
	else if (fare > Fare_2km || fabs(fare - Fare_2km) <= 1e-3)
		Fi = Fare_200m1; //T1;
	else
		Fi = Fare_200m0; //2.70; //2.35;

	if (Speed <= Speed_co) //12
	{
		if(Drv_state == S_HIRED)
		{
			last_counter = counter;
//			Fu+=4;
			Fu+=(float)ck/300;
			Ft++;
		}
		else
		{
			Dt = counter - last_counter;
			DtX = Dt + DtX;
			last_counter = counter;
			for (; DtX>0; DtX--)
			{
				Fd++;
				Fu++;
			}
		}
	}
	else
	{
		Dt = counter - last_counter;
		DtX = Dt + DtX;
		last_counter = counter;
		for (; DtX>0; DtX--)
		{
			Fd++;
			Fu++;
		}
	}

	//check Fu
//	if (fabs(Fu - ((float)ck/5 + (float)ck/1000)) <= 1e-3 || Fu > ((float)ck/5 + (float)ck/1000))
	if (fabs(Fu - (float)ck/5) <= 1e-6 || Fu > (float)ck/5)
	{
		fare = fare + Fi;
		Fu = Fu - ck/5;
	}

	fare_km = ((float)Fd / ck);
	fare_min = ((float)Ft / 60.0);

	if((Flag_2km == FALSE) && (((distance > 2000) || (fabs(distance-2000) <= 1e-3)) || ((fare>Fare_2km) || (fabs(fare-Fare_2km) <= 1e-3))))
	{
		Flag_2km = TRUE;
		Fi = Fare_200m1; //T1
		fare = Fare_2km;
		fare = fare + Fi;
	}

	//check fare for HMI fare display - fare_d
	if ((fare > Fare_2km) || fabs(fare-Fare_2km) <= 1e-3)
	{
		fare_d = fare;
	}
	else
	{
		fare_d = Fare_2km; //20.5 //24.0;
	}

	if ((Last_fare_d != fare_d) && (fare_d != Fare_2km))
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, RESET);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, SET);
		HAL_Delay(20);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, RESET);
		change_count += 1;
	}

	Last_fare_d = fare_d;
}

void Flag_Control_Up()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, SET);
}

void Flag_Control_Down()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, SET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, RESET);
}

void Anti_Tamper_Handle()
{
	//x read eeprom status
	//fake pulse
	//detect pin

	//set alarm in rtc & ctrl_2

	//1. alarm 30s (first time enter will alarm)
	//2. 'C' in HMI fare
	unsigned char C_Fare[] = {0x5A, 0xA5, 0x04, 0x82, 0x10, 0x30, 0x43};				// 7 byte  //min fare
//	unsigned char C_Fare_Min[] = {0x5A, 0xA5, 0x07, 0x82, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
	//c_fare_min -> fare_reached
//	unsigned char C_Fare_Min[] = {0x5A, 0xA5, 0x07, 0x82, 0x10, 0x00, 0x00, 0x00, 0x09, 0x2E}; //2350 - 23.50
	//DONE：read from parameter, move to global define
//	unsigned char C_Fare_Min[] = {0x5A, 0xA5, 0x07, 0x82, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x8C}; //2700 - 27.00

	uint8_t temp_array[4];
	Long_to_byte(Fare_min, temp_array);
	C_Fare_Min[6] = temp_array[0];
	C_Fare_Min[7] = temp_array[1];
	C_Fare_Min[8] = temp_array[2];
	C_Fare_Min[9] = temp_array[3];
	HAL_UART_Transmit(&huart1, C_Fare, 7, 0x20);
	HAL_UART_Transmit(&huart1, C_Fare_Min, 10, 0x20);

	unsigned char alarm[] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0xA0, 0x00, 0x7d};
	//flag protect set again (maybe power off), no need alarm, only show unlock
	if(flag_alarm_clear)
	{
		Flag_alarm = FALSE;
	}

	if(Flag_alarm)
	{
		Flag_alarm = FALSE;
		Flag_set_alarm_rtc = TRUE;
		for(i=0; i<30; i++) //test 5s - set 30s
		{
			time_update();
			HAL_UART_Transmit(&huart1, alarm, 8, 0xffff);
			HAL_Delay(1000);
		}
	}
	else
	{
		//3. lock the meter
//		Lock_Count_Down();
		unsigned char a_buffer[3] = {0};
		unsigned char d_buffer[6] = {0};

		unsigned char unlock[] = {0x5A, 0xA5, 0x09, 0x82, 0x10, 0x32, 0x55, 0x4E, 0x4C, 0x4F, 0x43, 0x4B};
		HAL_UART_Transmit(&huart1, unlock, 12, 0x20);

		a_buffer[0] = hh;
		a_buffer[1] = mm;
		a_buffer[2] = ss;
		BCD2ASC(d_buffer, a_buffer, 6);

		Dur_HMS[13] = d_buffer[5];
		Dur_HMS[12] = d_buffer[4];
		Dur_HMS[10] = d_buffer[3];
		Dur_HMS[9] = d_buffer[2];
		Dur_HMS[7] = d_buffer[1];
		Dur_HMS[6] = d_buffer[0];

		HAL_UART_Transmit(&huart1, Dur_HMS, 14, 0x20);

		time_update();
//		HAL_Delay(1000);
	}
}

//void Lock_Count_Down(void)
//{
//	//get current time from rtc - ymd hms
//	//set end time
//	unsigned char lock[] = {0x5A, 0xA5, 0x07, 0x82, 0x10, 0x32, 0x4C, 0x4F, 0x43, 0x4B};
//	HAL_UART_Transmit(&huart3, lock, 10, 0x20);
//
//	cds_1 = (cds / 1 % 10) + 0x30;
//	cds_10 = (cds / 10 % 10) + 0x30;
//	cdm_1 = (cdm / 1 % 10) + 0x30;
//	cdm_10 = (cdm / 10 % 10) + 0x30;
//	cdh_1 = (cdh / 1 % 10) + 0x30;
//	cdh_10 = (cdh / 10 % 10) + 0x30;
//
//	Dur_HMS[13] = cds_1;
//	Dur_HMS[12] = cds_10;
//	Dur_HMS[10] = cdm_1;
//	Dur_HMS[9] = cdm_10;
//	Dur_HMS[7] = cdh_1;
//	Dur_HMS[6] = cdh_10;
//
//	HAL_UART_Transmit(&huart3, Dur_HMS, 14, 0x20);
//	cds--;
//	if(cds < 0)
//	{
//		cdm--;
//		cds = 59;
//	}
//	if(cdm < 0)
//	{
//		cdh--;
//		cdm = 59;
//	}
//	if(cdh == 1)
//	{
//		cdh = 0;
//	}
//
//	if (cds == 0 && cdm == 0 && cdh == 0)
//	{
//		Flag_Protect = FALSE;
//		unsigned char lock_clear[] = {0x5A, 0xA5, 0x07, 0x82, 0x10, 0x32, 0x20, 0x20, 0x20, 0x20};
//		HAL_UART_Transmit(&huart3, lock_clear, 10, 0x20);
//		unsigned char C_Fare_clear[] = {0x5A, 0xA5, 0x04, 0x82, 0x10, 0x30, 0x20};
//		HAL_UART_Transmit(&huart3, C_Fare_clear, 7, 0x20);
//		cds = 0;
//		cdm = 0;
//		cdh = 1;
//		Drv_state = S_VACANT;
//	}
//}

void Dec2ASC(uint32_t decimal, uint8_t* asc)
{
	uint8_t len = 0;
	uint32_t tmp = decimal;

	do {
		len++;
		tmp /= 10;
	} while (tmp > 0);

	for (int i=0; i<len; i++)
	{
		asc[7-i] = decimal % 10 + 0x30;
		decimal /= 10;
	}
}

uint32_t ASC2Dec(uint8_t* asc)
{
	printf("[debug] asc to dec : \n");
	uint32_t decimal = 0;
	for (int i=0; i<8; i++)
	{

		if (asc[i]>=0x30 && asc[i]<=0x39)
			decimal = 10*decimal + (asc[i] - 0x30);
	}
	return decimal;
}

void E_read(void)
{
	HAL_I2C_Mem_Read(&hi2c2, 0xA1, (3<<7), 2, dataRead, 100, 1000);
	tkm[0] = dataRead[0];
	tkm[1] = dataRead[1];
	tkm[2] = dataRead[2];
	tkm[3] = dataRead[3];
	tkm[4] = dataRead[4];
	tkm[5] = dataRead[5];
	tkm[6] = dataRead[6];
	tkm[7] = dataRead[7];

	pkm[0] = dataRead[8];
	pkm[1] = dataRead[9];
	pkm[2] = dataRead[10];
	pkm[3] = dataRead[11];
	pkm[4] = dataRead[12];
	pkm[5] = dataRead[13];
	pkm[6] = dataRead[14];
	pkm[7] = dataRead[15];

	tflag[0] = dataRead[16];
	tflag[1] = dataRead[17];
	tflag[2] = dataRead[18];
	tflag[3] = dataRead[19];
	tflag[4] = dataRead[20];
	tflag[5] = dataRead[21];
	tflag[6] = dataRead[22];
	tflag[7] = dataRead[23];

	cflag[0] = dataRead[24];
	cflag[1] = dataRead[25];
	cflag[2] = dataRead[26];
	cflag[3] = dataRead[27];
	cflag[4] = dataRead[28];
	cflag[5] = dataRead[29];
	cflag[6] = dataRead[30];
	cflag[7] = dataRead[31];

	tpulse[0] = dataRead[32];
	tpulse[1] = dataRead[33];
	tpulse[2] = dataRead[34];
	tpulse[3] = dataRead[35];
	tpulse[4] = dataRead[36];
	tpulse[5] = dataRead[37];
	tpulse[6] = dataRead[38];
	tpulse[7] = dataRead[39];

	tfare[0] = dataRead[40];
	tfare[1] = dataRead[41];
	tfare[2] = dataRead[42];
	tfare[3] = dataRead[43];
	tfare[4] = dataRead[44];
	tfare[5] = dataRead[45];
	tfare[6] = dataRead[46];
	tfare[7] = dataRead[47];

	tsub[0] = dataRead[48];
	tsub[1] = dataRead[49];
	tsub[2] = dataRead[50];
	tsub[3] = dataRead[51];
	tsub[4] = dataRead[52];
	tsub[5] = dataRead[53];
	tsub[6] = dataRead[54];
	tsub[7] = dataRead[55];

	cfare[0] = dataRead[56];
	cfare[1] = dataRead[57];
	cfare[2] = dataRead[58];
	cfare[3] = dataRead[59];
	cfare[4] = dataRead[60];
	cfare[5] = dataRead[61];
	cfare[6] = dataRead[62];
	cfare[7] = dataRead[63];

	csub[0] = dataRead[64];
	csub[1] = dataRead[65];
	csub[2] = dataRead[66];
	csub[3] = dataRead[67];
	csub[4] = dataRead[68];
	csub[5] = dataRead[69];
	csub[6] = dataRead[70];
	csub[7] = dataRead[71];

	ttime[0] = dataRead[72];
	ttime[1] = dataRead[73];
	ttime[2] = dataRead[74];
	ttime[3] = dataRead[75];
	ttime[4] = dataRead[76];
	ttime[5] = dataRead[77];
	ttime[6] = dataRead[78];
	ttime[7] = dataRead[79];

	pulses[0] = dataRead[80];
	pulses[1] = dataRead[81];
	pulses[2] = dataRead[82];
	pulses[3] = dataRead[83];
	pulses[4] = dataRead[84];
	pulses[5] = dataRead[85];
	pulses[6] = dataRead[86];
	pulses[7] = dataRead[87];

	tkm_d = ASC2Dec(tkm);
	pkm_d = ASC2Dec(pkm);
	tflag_d = ASC2Dec(tflag);
	cflag_d = ASC2Dec(cflag);
	tpulse_d = ASC2Dec(tpulse);
	tfare_d = ASC2Dec(tfare);
	tsub_d = ASC2Dec(tsub);
	cfare_d = ASC2Dec(cfare);
	csub_d = ASC2Dec(csub);
//	ttime_d = ASC2Dec(ttime);
	ttime_x = ASC2Dec(ttime);

//Done: transfer eeprom data to display
//	uint8_t temp_array[4];

//	Long_to_byte(tkm_d, temp_array);
//	memcpy(dataDisplay, temp_array, sizeof(uint8_t)*4);
//	Long_to_byte(pkm_d, temp_array);
//	memcpy(dataDisplay+4, temp_array, sizeof(uint8_t)*4);
//	Long_to_byte(tflag_d, temp_array);
//	memcpy(dataDisplay+8, temp_array, sizeof(uint8_t)*4);
//	Long_to_byte(tpulse_d, temp_array);
//	memcpy(dataDisplay+12, temp_array, sizeof(uint8_t)*4);
//
//	tfare_d_x10 = tfare_d * 10;
//	Long_to_byte(tfare_d_x10, temp_array);
//	memcpy(dataDisplay+16, temp_array, sizeof(uint8_t)*4);
//	tsub_d_x10 = tsub_d * 10;
//	Long_to_byte(tsub_d_x10, temp_array);
//	memcpy(dataDisplay+20, temp_array, sizeof(uint8_t)*4);
//
//	Long_to_byte(ttime_x, temp_array);
//	memcpy(dataDisplay+24, temp_array, sizeof(uint8_t)*4);
//	Long_to_byte(cflag_d, temp_array);
//	memcpy(dataDisplay+28, temp_array, sizeof(uint8_t)*4);
//
//	cfare_d_x10 = cfare_d * 10;
//	Long_to_byte(cfare_d_x10, temp_array);
//	memcpy(dataDisplay+32, temp_array, sizeof(uint8_t)*4);
//	csub_d_x10 = csub_d * 10;
//	Long_to_byte(csub_d_x10, temp_array);
//	memcpy(dataDisplay+36, temp_array, sizeof(uint8_t)*4);
//
//	memcpy(total_hmi+6, dataDisplay, sizeof(uint8_t)*40);
//	HAL_UART_Transmit(&huart1, total_hmi, 46, 0x0E);
}

void E_write(void)
{
	dataWrite[0] = tkm[0];
	dataWrite[1] = tkm[1];
	dataWrite[2] = tkm[2];
	dataWrite[3] = tkm[3];
	dataWrite[4] = tkm[4];
	dataWrite[5] = tkm[5];
	dataWrite[6] = tkm[6];
	dataWrite[7] = tkm[7];

	dataWrite[8] = pkm[0];
	dataWrite[9] = pkm[1];
	dataWrite[10] = pkm[2];
	dataWrite[11] = pkm[3];
	dataWrite[12] = pkm[4];
	dataWrite[13] = pkm[5];
	dataWrite[14] = pkm[6];
	dataWrite[15] = pkm[7];

	dataWrite[16] = tflag[0];
	dataWrite[17] = tflag[1];
	dataWrite[18] = tflag[2];
	dataWrite[19] = tflag[3];
	dataWrite[20] = tflag[4];
	dataWrite[21] = tflag[5];
	dataWrite[22] = tflag[6];
	dataWrite[23] = tflag[7];

	dataWrite[24] = cflag[0];
	dataWrite[25] = cflag[1];
	dataWrite[26] = cflag[2];
	dataWrite[27] = cflag[3];
	dataWrite[28] = cflag[4];
	dataWrite[29] = cflag[5];
	dataWrite[30] = cflag[6];
	dataWrite[31] = cflag[7];

	dataWrite[32] = tpulse[0];
	dataWrite[33] = tpulse[1];
	dataWrite[34] = tpulse[2];
	dataWrite[35] = tpulse[3];
	dataWrite[36] = tpulse[4];
	dataWrite[37] = tpulse[5];
	dataWrite[38] = tpulse[6];
	dataWrite[39] = tpulse[7];

	dataWrite[40] = tfare[0];
	dataWrite[41] = tfare[1];
	dataWrite[42] = tfare[2];
	dataWrite[43] = tfare[3];
	dataWrite[44] = tfare[4];
	dataWrite[45] = tfare[5];
	dataWrite[46] = tfare[6];
	dataWrite[47] = tfare[7];

	dataWrite[48] = tsub[0];
	dataWrite[49] = tsub[1];
	dataWrite[50] = tsub[2];
	dataWrite[51] = tsub[3];
	dataWrite[52] = tsub[4];
	dataWrite[53] = tsub[5];
	dataWrite[54] = tsub[6];
	dataWrite[55] = tsub[7];

	dataWrite[56] = cfare[0];
	dataWrite[57] = cfare[1];
	dataWrite[58] = cfare[2];
	dataWrite[59] = cfare[3];
	dataWrite[60] = cfare[4];
	dataWrite[61] = cfare[5];
	dataWrite[62] = cfare[6];
	dataWrite[63] = cfare[7];

	dataWrite[64] = csub[0];
	dataWrite[65] = csub[1];
	dataWrite[66] = csub[2];
	dataWrite[67] = csub[3];
	dataWrite[68] = csub[4];
	dataWrite[69] = csub[5];
	dataWrite[70] = csub[6];
	dataWrite[71] = csub[7];

	dataWrite[72] = ttime[0];
	dataWrite[73] = ttime[1];
	dataWrite[74] = ttime[2];
	dataWrite[75] = ttime[3];
	dataWrite[76] = ttime[4];
	dataWrite[77] = ttime[5];
	dataWrite[78] = ttime[6];
	dataWrite[79] = ttime[7];

	dataWrite[80] = pulses[0];
	dataWrite[81] = pulses[1];
	dataWrite[82] = pulses[2];
	dataWrite[83] = pulses[3];
	dataWrite[84] = pulses[4];
	dataWrite[85] = pulses[5];
	dataWrite[86] = pulses[6];
	dataWrite[87] = pulses[7];

	dataWrite[88] = dt[0];
	dataWrite[89] = dt[1];
	dataWrite[90] = dt[2];
	dataWrite[91] = dt[3];
	dataWrite[92] = dt[4];
	dataWrite[93] = dt[5];
	dataWrite[94] = dt[6];
	dataWrite[95] = dt[7];
	dataWrite[96] = dt[8];
	dataWrite[97] = dt[9];
	dataWrite[98] = dt[10];
	dataWrite[99] = dt[11];

	HAL_I2C_Mem_Write(&hi2c2, 0xA0, (3<<7), 2, dataWrite, 100, 1000);
	HAL_Delay(5);
}

//voice
void Voice_init(void)
{
	//v_can_start[5][6] ~ [15][16], odd-0x35(number)/0x30(letter), even = plate_qr[6]-[11]
	//v_cn_start[5][6] ~ [15][16], odd-0x4A(number)/0x30(letter), even = plate_qr[6]-[11]
	for (int i=6; i<=11; i++)
	{
		if (plate_qr[i] < 0x41)
		{
			v_can_start[2*i-7] = 0x35;
			v_can_start[2*i-6] = plate_qr[i];
			v_cn_start[2*i-7] = 0x4A;
			v_cn_start[2*i-6] = plate_qr[i];
		}
		else
		{
			v_can_start[2*i-7] = 0x30;
			v_can_start[2*i-6] = plate_qr[i];
			v_cn_start[2*i-7] = 0x30;
			v_cn_start[2*i-6] = plate_qr[i];
		}
	}

	v_en_start[6] = plate_qr[6];
	v_en_start[8] = plate_qr[7];
	v_en_start[10] = plate_qr[8];
	v_en_start[12] = plate_qr[9];
	v_en_start[14] = plate_qr[10];
	v_en_start[16] = plate_qr[11];

	v_can_start[19] = checksum_8bit(v_can_start,20);
	v_cn_start[19] = checksum_8bit(v_cn_start,20);
	v_en_start[19] = checksum_8bit(v_en_start,20);
}

void Voice_start(unsigned char data[])
{
	HAL_UART_Transmit(&huart2, (uint8_t *)data, 20, 1000);
}

void Voice_end(unsigned char data[])
{
	HAL_UART_Transmit(&huart2, (uint8_t *)data, 32, 1000);
}

void cal_vcan(unsigned char data[], unsigned char result[])
{
	if (data[1] != 0x30)
	{
		result[0] += 2;
		//x000 - (1000 ~ 9000)
		result[1] = 0x46;
		result[2] = data[1];
		if (data[2] == 0x30)
		{
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//0x - (1001 ~ 9009)
					result[3] = 0x35;
					result[4] = 0x30;
					result[5] = 0x35;
					result[6] = data[4];
				}
			}
			else
			{
				result[0] += 4;
				//0x0 - (x0x0 1010 ~ 9090)
				result[3] = 0x35;
				result[4] = 0x30;
				result[5] = 0x44;
				result[6] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (x0xx 1011 ~ 9099)
					result[7] = 0x35;
					result[8] = data[4];
				}
			}
		}
		else
		{
			result[0] += 2;
			//xx00 (1100 ~ 9900)
			result[3] = 0x45;
			result[4] = data[2];
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//0x (1101 ~ 9909)
					result[5] = 0x35;
					result[6] = 0x30;
					result[7] = 0x35;
					result[8] = data[4];
				}
			}
			else
			{
				result[0] += 2;
				//x0 - (1110 ~ 9990)
				result[5] = 0x44;
				result[6] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (1111 ~ 9999)
					result[7] = 0x35;
					result[8] = data[4];
				}
			}
		}
	}
	else
	{
		if (data[2] != 0x30)
		{
			result[0] += 2;
			//x?�� - (100 ~ 900)
			result[1] = 0x45;
			result[2] = data[2];
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//0x - (101 ~ 909)
					result[3] = 0x35;
					result[4] = 0x30;
					result[5] = 0x35;
					result[6] = data[4];
				}
			}
			else
			{
				result[0] += 2;
				//x0 - (110 ~ 990)
				result[3] = 0x44;
				result[4] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (111 ~ 999)
					result[5] = 0x35;
					result[6] = data[4];
				}
			}
		}
		else
		{
			result[0] += 2;
			//x0 - (30 ~ 90)
			result[1] = 0x44;
			result[2] = data[3];
			if (data[4] != 0x30)
			{
				result[0] += 2;
				//x - (23 ~ 99)
				result[3] = 0x35;
				result[4] = data[4];
			}
		}
	}
	//yuan
	result[0] += 2;
	result[result[0] - 1] = 0x36;
	result[result[0]] = 0x34;

	if (data[5] != 0x30)
	{
		result[0] += 4;
		result[result[0] - 3] = 0x35;
		result[result[0] - 2] = data[5];
		//hau
		result[result[0] - 1] = 0x36;
		result[result[0]] = 0x33;
	}

	if (data[0] != 0)
	{
		//surcharge
		result[0] += 2;
		result[result[0] - 1] = 0x37;
		result[result[0]] = 0x32;
		if (data[6] != 0x30)
		{
			result[0] += 2;
			//x?�� - (x?��)
			result[result[0] - 1] = 0x45;
			result[result[0]] = data[6];
			if (data[7] == 0x30)
			{
				if (data[8] != 0x30)
				{
					result[0] += 4;
					//0x - (101 ~ 909)
					result[result[0] - 3] = 0x35;
					result[result[0] - 2] = 0x30;
					result[result[0] - 1] = 0x35;
					result[result[0]] = data[8];
				}
			}
			else
			{
				result[0] += 2;
				//x0 - (110 ~ 990)
				result[result[0] - 1] = 0x44;
				result[result[0]] = data[7];
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (111 ~ 999)
					result[result[0] - 1] = 0x35;
					result[result[0]] = data[8];
				}
			}
		}
		else
		{
			if (data[7] == 0x31)
			{
				result[0] += 2;
				//10 - (10)
				result[result[0] - 1] = 0x36;
				result[result[0]] = 0x30;
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (11 ~ 19)
					result[result[0] - 1] = 0x35;
					result[result[0]] = data[8];
				}
			}
			else if (data[7] != 0x30)
			{
				result[0] += 2;
				//x0 - (20 ~ 90)
				result[result[0] - 1] = 0x44;
				result[result[0]] = data[7];
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (21 ~ 99)
					result[result[0] - 1] = 0x35;
					result[result[0]] = data[8];
				}
			}
			else
			{
				result[0] += 2;
				//x - (1 ~ 9)
				result[result[0] - 1] = 0x35;
				result[result[0]] = data[8];
			}
		}
		//yuan
		result[0] += 2;
		result[result[0] - 1] = 0x36;
		result[result[0]] = 0x34;
	}
	//thx, bye
	result[0] += 2;
	result[result[0] - 1] = 0x37;
	result[result[0]] = 0x31;
}

void cal_vcn(unsigned char data[], unsigned char result[])
{
	if (data[1] != 0x30)
	{
		result[0] += 2;
		//x000 - (1000 ~ 9000)
		result[1] = 0x49;
		result[2] = data[1];
		if (data[2] == 0x30)
		{
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//0x - (1001 ~ 9009)
					result[3] = 0x4A;
					result[4] = 0x30;
					result[5] = 0x4A;
					result[6] = data[4];
				}
			}
			else
			{
				result[0] += 4;
				//0x0 - (1010 ~ 9090)
				result[3] = 0x4A;
				result[4] = 0x30;
				result[5] = 0x47;
				result[6] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (1011 ~ 9099)
					result[7] = 0x4A;
					result[8] = data[4];
				}
			}
		}
		else
		{
			result[0] += 2;
			//x00 - (100 ~ 900)
			result[3] = 0x48;
			result[4] = data[2];
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//0x - (1101 ~ 9909)
					result[5] = 0x4A;
					result[6] = 0x30;
					result[7] = 0x4A;
					result[8] = data[4];
				}
			}
			else
			{
				result[0] += 2;
				//x0 - (1110 ~ 9990)
				result[5] = 0x47;
				result[6] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (1111 ~ 9999)
					result[7] = 0x4A;
					result[8] = data[4];
				}
			}
		}
	}
	else
	{
		if (data[2] != 0x30)
		{
			result[0] += 2;
			//x00 - (100 ~ 900)
			result[1] = 0x48;
			result[2] = data[2];
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//0x - (101 ~ 909)
					result[3] = 0x4A;
					result[4] = 0x30;
					result[5] = 0x4A;
					result[6] = data[4];
				}
			}
			else
			{
				result[0] += 2;
				//x0 - (110 ~ 990)
				result[3] = 0x47;
				result[4] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (111 ~ 999)
					result[5] = 0x4A;
					result[6] = data[4];
				}
			}
		}
		else
		{
			result[0] += 2;
			//x0 - (30 ~ 90)
			result[1] = 0x47;
			result[2] = data[3];
			if (data[4] != 0x30)
			{
				result[0] += 2;
				//x - (23 ~ 99)
				result[3] = 0x4A;
				result[4] = data[4];
			}
		}
	}

	if (data[5] != 0x30)
	{
		result[0] += 4;
		//dot
		result[result[0] - 3] = 0x38;
		result[result[0] - 2] = 0x38;
		result[result[0] - 1] = 0x4A;
		result[result[0]] = data[5];
	}
	//yuan
	result[0] += 2;
	result[result[0] - 1] = 0x38;
	result[result[0]] = 0x39;

	if (data[0] != 0)
	{
		//surcharge
		result[0] += 2;
		result[result[0] - 1] = 0x39;
		result[result[0]] = 0x37;
		if (data[6] != 0x30)
		{
			result[0] += 2;
			//x00 - (100 ~ 900)
			result[result[0] - 1] = 0x48;
			result[result[0]] = data[6];
			if (data[7] == 0x30)
			{
				if (data[8] != 0x30)
				{
					result[0] += 4;
					//0x - (101 ~ 909)
					result[result[0] - 3] = 0x4A;
					result[result[0] - 2] = 0x30;
					result[result[0] - 1] = 0x4A;
					result[result[0]] = data[8];
				}
			}
			else
			{
				result[0] += 2;
				//x0 - (110 ~ 990)
				result[result[0] - 1] = 0x47;
				result[result[0]] = data[7];
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (111 ~ 999)
					result[result[0] - 1] = 0x4A;
					result[result[0]] = data[8];
				}
			}
		}
		else
		{
			if (data[7] == 0x31)
			{
				result[0] += 2;
				//10 - (10)
				result[result[0] - 1] = 0x38;
				result[result[0]] = 0x35;
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (11 ~ 19)
					result[result[0] - 1] = 0x4A;
					result[result[0]] = data[8];
				}
			}
			else if (data[7] != 0x30)
			{
				result[0] += 2;
				//x0 - (20 ~ 90)
				result[result[0] - 1] = 0x47;
				result[result[0]] = data[7];
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (21 ~ 99)
					result[result[0] - 1] = 0x4A;
					result[result[0]] = data[8];
				}
			}
			else
			{
				result[0] += 2;
				//x - (1 ~ 9)
				result[result[0] - 1] = 0x4A;
				result[result[0]] = data[8];
			}
		}
		//yuan
		result[0] += 2;
		result[result[0] - 1] = 0x38;
		result[result[0]] = 0x39;
	}
	//thx, bye
	result[0] += 2;
	result[result[0] - 1] = 0x39;
	result[result[0]] = 0x36;
}

void cal_ven(unsigned char data[], unsigned char result[])
{
	if (data[1] != 0x30)
	{
		result[0] += 2;
		//x thousand - (1000 ~ 9000)
		result[1] = 0x42;
		result[2] = data[1];
		if (data[2] == 0x30)
		{
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (1001 ~ 9009)
					result[3] = 0x30;
					result[4] = data[4];
				}
			}
			else if (data[3] == 0x31)
			{
				result[0] += 2;
				//ten, eleven, ..., nineteen (x thousand ten/eleven/.../nineteen)
				result[3] = 0x31;
				result[4] = data[4];
			}
			else
			{
				result[0] += 2;
				//twenty, ..., ninety (x thousand twenty/.../ninety)
				result[3] = 0x32;
				result[4] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (x thousand twenty/.../ninety x)
					result[5] = 0x30;
					result[6] = data[4];
				}
			}
		}
		else
		{
			result[0] += 2;
			//x hundred (x thousand x hundred)
			result[3] = 0x41;
			result[4] = data[2];
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (x thousand x hundred x)
					result[5] = 0x30;
					result[6] = data[4];
				}
			}
			else if (data[3] == 0x31)
			{
				result[0] += 2;
				//ten, eleven, ..., nineteen (x thousand x hundred ten/eleven/.../nineteen)
				result[5] = 0x31;
				result[6] = data[4];
			}
			else
			{
				result[0] += 2;
				//twenty, ..., ninety (x thousand x hundred twenty/.../ninety)
				result[5] = 0x32;
				result[6] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x (x thousand x hundred twenty/.../ninety x)
					result[7] = 0x30;
					result[8] = data[4];
				}
			}
		}
	}
	else
	{
		if (data[2] != 0x30)
		{
			result[0] += 2;
			//x hundred - (x hundred)
			result[1] = 0x41;
			result[2] = data[2];
			if (data[3] == 0x30)
			{
				if (data[4] != 0x30)
				{
					result[0] += 4;
					//x - (x hundred x)
					result[3] = 0x30;
					result[4] = data[4];
				}
			}
			else if (data[3] == 0x31)
			{
				result[0] += 2;
				//ten, eleven, ..., nineteen (x hundred ten/eleven/.../nineteen)
				result[3] = 0x31;
				result[4] = data[4];
			}
			else
			{
				result[0] += 2;
				//twenty, ..., ninety - (x hundred twenty/.../ninety)
				result[3] = 0x32;
				result[4] = data[3];
				if (data[4] != 0x30)
				{
					result[0] += 2;
					//x - (x hundred twenty/.../ninety x)
					result[5] = 0x30;
					result[6] = data[4];
				}
			}
		}
		else
		{
			result[0] += 2;
			//twenty, ..., ninety - (twenty/.../ninety)
			result[1] = 0x32;
			result[2] = data[3];
			if (data[4] != 0x30)
			{
				result[0] += 2;
				//x - (twenty/.../ninety x)
				result[3] = 0x30;
				result[4] = data[4];
			}
		}
	}

	//dollars
	result[0] += 2;
	result[result[0] - 1] = 0x33;
	result[result[0]] = 0x32;

	if (data[5] != 0x30)
	{
		result[0] += 6;
		//and
		result[result[0] - 5] = 0x33;
		result[result[0] - 4] = 0x30;
		if (data[5] == 0x31)
		{
			//ten
			result[result[0] - 3] = 0x31;
			result[result[0] - 2] = 0x30;
		}
		else
		{
			//twenty/.../ninety
			result[result[0] - 3] = 0x32;
			result[result[0] - 2] = data[5];
		}
		//cents
		result[result[0] - 1] = 0x33;
		result[result[0]] = 0x31;
	}

	if (data[0] != 0)
	{
		//including the surcharge
		result[0] += 2;
		result[result[0] - 1] = 0x33;
		result[result[0]] = 0x39;
		if (data[6] != 0x30)
		{
			result[0] += 2;
			//x hundred - (x hundred)
			result[result[0] - 1] = 0x41;
			result[result[0]] = data[6];
			if (data[7] == 0x30)
			{
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (x hundred x)
					result[result[0] - 1] = 0x30;
					result[result[0]] = data[8];
				}
			}
			else
			{
				if (data[7] == 0x31)
				{
					result[0] += 2;
					//ten, eleven, ..., nineteen (x hundred ten/eleven/.../nineteen)
					result[result[0] - 1] = 0x30;
					result[result[0]] = data[8];
				}
				else
				{
					result[0] += 2;
					//twenty, ..., ninety - (x hundred twenty/.../ninety)
					result[result[0] - 1] = 0x32;
					result[result[0]] = data[7];
					if (data[8] != 0x30)
					{
						result[0] += 2;
						//x - (x hundred twenty/.../ninety x)
						result[result[0] - 1] = 0x30;
						result[result[0]] = data[8];
					}
				}
			}
		}
		else
		{
			if (data[7] == 0x31)
			{
				result[0] += 2;
				//ten, eleven, ..., nineteen - (ten/eleven/.../nineteen)
				result[result[0] - 1] = 0x31;
				result[result[0]] = data[8];
			}
			else if (data[7] != 0x30)
			{
				result[0] += 2;
				//twenty, ..., ninety - (twenty/.../ninety)
				result[result[0] - 1] = 0x32;
				result[result[0]] = data[7];
				if (data[8] != 0x30)
				{
					result[0] += 2;
					//x - (twenty/.../ninety x)
					result[result[0] - 1] = 0x30;
					result[result[0]] = data[8];
				}
			}
			else
			{
				result[0] += 2;
				//x - (x)
				result[result[0] - 1] = 0x30;
				result[result[0]] = data[8];
			}
		}
		//dollars
		result[0] += 2;
		result[result[0] - 1] = 0x33;
		result[result[0]] = 0x32;
	}
	//thank you, have a nice day
	result[0] += 2;
	result[result[0] - 1] = 0x34;
	result[result[0]] = 0x30;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
