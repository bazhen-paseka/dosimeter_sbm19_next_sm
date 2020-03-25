/**
* \file
* \version 1.0
* \author bazhen.levkovets
** \date 2018
*
*************************************************************************************
* \copyright	Bazhen Levkovets
* \copyright	Brovary, Kyiv region
* \copyright	Ukraine
*
*************************************************************************************
*
* \brief
*
*/

/*
**************************************************************************
*							INCLUDE FILES
**************************************************************************
*/
	#include "dosimeter_sbm19_next_sm.h"
/*
**************************************************************************
*							LOCAL DEFINES
**************************************************************************
*/

/*
**************************************************************************
*							LOCAL CONSTANTS
**************************************************************************
*/


/*
**************************************************************************
*						    LOCAL DATA TYPES
**************************************************************************
*/


/*
**************************************************************************
*							  LOCAL TABLES
**************************************************************************
*/

/*
**************************************************************************
*								 MACRO'S
**************************************************************************
*/

/*
**************************************************************************
*						    GLOBAL VARIABLES
**************************************************************************
*/

	uint8_t 	tim3_flag_u8 			= 0;
	uint8_t		update_flag_u8 			= 0;
	uint8_t		electron_array_count_u8	= 0;
	uint8_t		led_count_u8			= 0;
	uint32_t	electron_hard_count_u32	= 0;
	uint32_t 	radiation_u32_arr[VALUE_ARRAY_CNT];

/*
**************************************************************************
*                        LOCAL FUNCTION PROTOTYPES
**************************************************************************
*/
	void LED_Blink(uint8_t _position_u8);
	void Print_radiation(uint32_t _radiation_u32, tm1637_struct * _h_tm1637);

/*
**************************************************************************
*                           GLOBAL FUNCTIONS
**************************************************************************
*/

void Dozimeter_set_TIM3_flag(uint8_t _flag) {
	tim3_flag_u8 = _flag;
}
//************************************************************************

void Dozimeter_set_time_between_electrons(void) {
	HAL_GPIO_TogglePin(LED_BOARD_GPIO_Port, LED_BOARD_Pin);
	electron_array_count_u8++;
	if (electron_array_count_u8 >= VALUE_ARRAY_CNT) {
		electron_array_count_u8 = 0;
	}
	radiation_u32_arr[electron_array_count_u8] = TIM4->CNT;
	TIM4->CNT = 0;
	electron_hard_count_u32++;
	update_flag_u8 = 1;
}
//************************************************************************

void Dozimeter_Init(UART_HandleTypeDef * _uart_debug) {
	int soft_version_arr_int[3];
	soft_version_arr_int[0] = ((SOFT_VERSION) / 100) %10 ;
	soft_version_arr_int[1] = ((SOFT_VERSION) /  10) %10 ;
	soft_version_arr_int[2] = ((SOFT_VERSION)      ) %10 ;

	char DataChar[100];
	sprintf(DataChar,"\r\n\r\n\tDosimeter SBM-19 Next 2020-march-24 v%d.%d.%d \r\n\tUART1 for debug on speed 115200/8-N-1\r\n\r\n",
			soft_version_arr_int[0], soft_version_arr_int[1], soft_version_arr_int[2]);
	HAL_UART_Transmit(_uart_debug, (uint8_t *)DataChar, strlen(DataChar), 100);

	for (int i=0; i<VALUE_ARRAY_CNT; i++) {
	  radiation_u32_arr[i] = 60000 / START_RADIATION_VALUE;
	}

	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start(&htim4);
}
//************************************************************************

void Dozimeter_Main(UART_HandleTypeDef * _uart_debug, tm1637_struct * _h_tm1637) {
	char DataChar[100];

	if (tim3_flag_u8 == 1) {
		sprintf(DataChar,"\t\t\t\tTIM3 60Sec. Hard_CNT= %d imp;\r\n", (int)electron_hard_count_u32);
		HAL_UART_Transmit(_uart_debug, (uint8_t *)DataChar, strlen(DataChar), 100);
		electron_hard_count_u32 = 0;
		tim3_flag_u8 = 0;
	}
	//************************************************************************

	if (update_flag_u8 > 0) {
		uint32_t summa_of_all_array_u32 = 0;
		for (int i=0; i<VALUE_ARRAY_CNT; i++) {
			summa_of_all_array_u32 = summa_of_all_array_u32 + radiation_u32_arr[i];
		}
		uint32_t qnt_electrons_per_60sec_u32 = ( 60000 * VALUE_ARRAY_CNT ) / summa_of_all_array_u32;

		sprintf (DataChar, "%d) \t%04d \t%d \t CNT: %03d \r\n",
				(int)electron_array_count_u8,
				(int)radiation_u32_arr[electron_array_count_u8],
				(int)summa_of_all_array_u32,
				(int)qnt_electrons_per_60sec_u32 );
		HAL_UART_Transmit(_uart_debug, (uint8_t *)DataChar, strlen(DataChar), 100);

		Print_radiation(qnt_electrons_per_60sec_u32, _h_tm1637);

		led_count_u8++;
		if (led_count_u8>=6) {
			led_count_u8 = 0;
		}
		LED_Blink(led_count_u8);
		update_flag_u8 = 0;
	}
}

/*
**************************************************************************
*                           LOCAL FUNCTIONS
**************************************************************************
*/

void LED_Blink(uint8_t _position_u8) {

	HAL_GPIO_WritePin(LED_BLUE_1_GPIO_Port,	LED_BLUE_1_Pin, RESET);
	HAL_GPIO_WritePin(LED_BLUE_2_GPIO_Port,	LED_BLUE_2_Pin, RESET);
	HAL_GPIO_WritePin(LED_BLUE_3_GPIO_Port,	LED_BLUE_3_Pin, RESET);
	HAL_GPIO_WritePin(LED_BLUE_4_GPIO_Port,	LED_BLUE_4_Pin, RESET);

	switch (_position_u8) {
		case 0: HAL_GPIO_WritePin(LED_BLUE_1_GPIO_Port, LED_BLUE_1_Pin, SET); break;
		case 1: HAL_GPIO_WritePin(LED_BLUE_2_GPIO_Port,	LED_BLUE_2_Pin, SET); break;
		case 2: HAL_GPIO_WritePin(LED_BLUE_3_GPIO_Port,	LED_BLUE_3_Pin, SET); break;
		case 3: HAL_GPIO_WritePin(LED_BLUE_4_GPIO_Port,	LED_BLUE_4_Pin, SET); break;
		case 4: HAL_GPIO_WritePin(LED_BLUE_3_GPIO_Port,	LED_BLUE_3_Pin, SET); break;
		case 5: HAL_GPIO_WritePin(LED_BLUE_2_GPIO_Port,	LED_BLUE_2_Pin, SET); break;
		default: break;
	}
}
//************************************************************************

void Print_radiation(uint32_t _radiation_u32, tm1637_struct * _h_tm1637)	{
	if (_radiation_u32 > 9999) {
		_radiation_u32 = 9999;
	}
	tm1637_Display_Decimal(_h_tm1637, _radiation_u32, no_double_dot);
}
//************************************************************************
