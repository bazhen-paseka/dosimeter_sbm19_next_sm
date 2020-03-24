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

#ifndef DOSIMETER_SBM19_SM_H_INCLUDED
#define DOSIMETER_SBM19_SM_H_INCLUDED

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
/*
**************************************************************************
*								    DEFINES
**************************************************************************
*/
	#define SOFT_VERSION 			110
	#define	START_RADIATION_VALUE	95
	#define	VALUE_ARRAY_CNT			100
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
	void Dozimeter_Init(void);
	void Dozimeter_Main(void);
	void Dozimeter_set_TIM3_flag(uint8_t _flag);
	void Dozimeter_set_time_between_electrons(void);

#endif /* DOSIMETER_SBM19_SM_H_INCLUDED */
