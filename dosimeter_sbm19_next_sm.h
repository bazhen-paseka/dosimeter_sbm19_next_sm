/**
* \file
* \version 1.0
* \author bazhen.levkovets
* \date 2018 
* \mail bazhen.info(at)gmail.com
*************************************************************************************
* \copyright	Bazhen Levkovets
* \copyright	Brovary, Kyiv region
* \copyright	Ukraine
*
*
*************************************************************************************
*
* \brief
*
*/

#ifndef DOSIMETER_SBM19_NEXT_SM_H_INCLUDED
#define DOSIMETER_SBM19_NEXT_SM_H_INCLUDED

/*
**************************************************************************
*								INCLUDE FILES
**************************************************************************
*/

	#include "stm32f1xx_hal.h"
	#include "gpio.h"
	#include "tim.h"
	#include "usart.h"
	#include "stdio.h"
	#include <string.h>
	#include "dosimeter_sbm19_next_local_config.h"
	#include "tm1637_sm.h"
/*
**************************************************************************
*								    DEFINES
**************************************************************************
*/

/*
**************************************************************************
*								   DATA TYPES
**************************************************************************
*/

/*
**************************************************************************
*								GLOBAL VARIABLES
**************************************************************************
*/

/*
**************************************************************************
*									 MACRO'S
**************************************************************************
*/

/*
**************************************************************************
*                              FUNCTION PROTOTYPES
**************************************************************************
*/
	void Dozimeter_Init(UART_HandleTypeDef * _uart_debug);
	void Dozimeter_Main(UART_HandleTypeDef * _uart_debug, tm1637_struct * _h_tm1637);
	void Dozimeter_set_TIM3_flag(uint8_t _flag);
	void Dozimeter_set_time_between_electrons(void);
	void LED_Blink(uint8_t _position_u8);

#endif /* DOSIMETER_SBM19_NEXT_SM_H_INCLUDED */
